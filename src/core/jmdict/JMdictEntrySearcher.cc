/*
 *  Copyright (C) 2008/2009/2010  Alexandre Courbot
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "core/TextTools.h"
#include "core/jmdict/JMdictEntrySearcher.h"
#include "core/jmdict/JMdictEntry.h"
#include "core/jmdict/JMdictPlugin.h"
#include "sqlite/SQLite.h"

PreferenceItem<QString> JMdictEntrySearcher::miscPropertiesFilter("jmdict", "miscPropertiesFilter", "arch,obs");
QSet<QString> JMdictEntrySearcher::_miscFilterMask;
QSet<QString> JMdictEntrySearcher::_explicitlyRequestedMiscs;

JMdictEntrySearcher::JMdictEntrySearcher() : EntrySearcher(JMDICTENTRY_GLOBALID)
{
	connect(&JMdictEntrySearcher::miscPropertiesFilter, SIGNAL(valueChanged(QVariant)), this, SLOT(updateMiscFilterMask()));

	QueryBuilder::Join::addTablePriority("jmdict.entries", 50);
	QueryBuilder::Join::addTablePriority("jmdict.kanjiChar", 45);
	QueryBuilder::Join::addTablePriority("jmdict.kanji", 40);
	QueryBuilder::Join::addTablePriority("jmdict.kanjiText", 35);
	QueryBuilder::Join::addTablePriority("jmdict.kana", 30);
	QueryBuilder::Join::addTablePriority("jmdict.kanaText", 25);
	foreach (const QString &lang, JMdictPlugin::instance()->attachedDBs().keys()) {
		if (lang.isEmpty()) continue;
		QueryBuilder::Join::addTablePriority("jmdict_" + lang + ".gloss", 20);
		QueryBuilder::Join::addTablePriority("jmdict_" + lang + ".glossText", 15);
	}
	QueryBuilder::Join::addTablePriority("jmdict.senses", 10);
	QueryBuilder::Join::addTablePriority("jmdict.jlpt", 8);

	QueryBuilder::Order::orderingWay["freq"] = QueryBuilder::Order::DESC;

	// Register text search commands
	validCommands << "romaji" << "mean" << "kana" << "kanji" << "jmdict" << "haskanji" << "jlpt" << "withstudiedkanjis" << "hascomponent" << "withkanaonly";
	// Also register commands that are sense properties
	validCommands << "pos" << "misc" << "dial" << "field";

	updateMiscFilterMask();
}

SearchCommand JMdictEntrySearcher::commandFromWord(const QString &word) const
{
	// First see what the parent has to propose
	SearchCommand fromParent = EntrySearcher::commandFromWord(word);
	if (fromParent.isValid()) return fromParent;

	QString checkString(word);
	// Remove all wild cards
	checkString.remove(QRegExp("[\\?\\*]"));
	// Only wildcards? Lets assume romaji
	if (checkString.size() == 0) return SearchCommand::fromString(QString(":mean=\"%1\"").arg(word));
	// Else check if the string is pure kana
	else if (TextTools::isKana(checkString)) return SearchCommand::fromString(QString(":kana=\"%1\"").arg(word));
	// Otherwise check if it is kanji or kanji/kana mixed Japanese
	else if (TextTools::isJapanese(checkString)) return SearchCommand::fromString(QString(":kanji=\"%1\"").arg(word));
	// Then we are probably looking for romaji
	else {
		if (allowRomajiSearch()) {
			QString kn(TextTools::romajiToKana(word));
			if (!kn.isEmpty()) return SearchCommand::fromString(QString(":romaji=\"%1\"").arg(word));
		}
		return SearchCommand::fromString(QString(":mean=\"%1\"").arg(word));
	}
	// Nothing? Return an invalid command
	//return SearchCommand::invalid();
}

static QString buildTextSearchCondition(const QStringList &words, const QString &table)
{
	static QRegExp regExpChars = QRegExp("[\\?\\*]");
	static QString ftsMatch("jmdict%3.%2Text.reading MATCH '%1'");
	static QString regexpMatch("jmdict%3.%2Text.reading REGEXP '%1'");
	static QString glossRegexpMatch("{{leftcolumn}} in (select id from jmdict_%2.glosses where FTSUNCOMPRESS(glosses) REGEXP '%1')");
	static QString globalMatch("{{leftcolumn}} IN (SELECT id FROM jmdict%3.%2 JOIN jmdict%3.%2Text ON jmdict%3.%2.docid = jmdict%3.%2Text.docid WHERE %1)");

	QStringList globalMatches;
	QStringList langs(JMdictPlugin::instance()->attachedDBs().keys());
	langs.removeAll("");
	foreach (const QString &lang, langs) {
		QStringList fts;
		QStringList conds;
		QStringList condsGloss;
		foreach (const QString &w, words) {
			if (w.contains(regExpChars)) {
				// First check if we can optimize by using the FTS index (i.e. the first character is not a wildcard)
				int wildcardIdx = 0;
				while (!regExpChars.exactMatch(w[wildcardIdx])) wildcardIdx++;
				if (wildcardIdx != 0) fts << "\"" + w.mid(0, wildcardIdx) + "*\"";
				// If the wildcard we found is the last character and a star, there is no need for a regexp search
				if (wildcardIdx == w.size() - 1 && w.size() > 1 && w[wildcardIdx] == '*') continue;
				// Otherwise insert the regular expression search
				QString regExp(TextTools::escapeForRegexp(w));
				if (table != "gloss")
					conds << regexpMatch.arg(regExp);
				else
					condsGloss << glossRegexpMatch.arg(regExp).arg(lang);
			} else fts << "\"" + w + "\"";
		}
		if (!fts.isEmpty()) conds.insert(0, ftsMatch.arg(fts.join(" ")));
		if (!conds.isEmpty()) globalMatches << globalMatch.arg(conds.join(" AND ")).arg(table).arg(table == "gloss" ? "_" + lang : "");
		globalMatches += condsGloss;
	}
	return globalMatches.join(" OR ");
}

void JMdictEntrySearcher::buildStatement(QList<SearchCommand> &commands, QueryBuilder::Statement &statement)
{
	// First delegate to the parent
	EntrySearcher::buildStatement(commands, statement);

	// Then process remaining commands
	QStringList kanjiReadingsMatch;
	QStringList kanaReadingsMatch;
	QStringList transReadingsMatch;
	QStringList romajiSearch;
	QStringList hasKanjiSearch;
	QStringList hasComponentSearch;
	quint64 miscFilter(0), dialectFilter(0), fieldFilter(0);
	QSet<quint16> posFilter;

	QSet<QString> allCommands;
	// First build the global list of all commands
	foreach(const SearchCommand &command, commands) allCommands << command.command();

	_explicitlyRequestedMiscs.clear();

	foreach(const SearchCommand &command, commands) {
		const QString &commandLabel = command.command();

		// Search for text search commands
		if (commandLabel == "mean" || commandLabel == "kana" || commandLabel == "kanji") {
			foreach(const QString &arg, command.args()) {
				// TODO Protect multi-words arguments within quotes
				if (command.command() == "mean")
					transReadingsMatch << arg;
				if (command.command() == "kanji")
					kanjiReadingsMatch << arg;
				if (command.command() == "kana")
					kanaReadingsMatch << arg;
			}
			commands.removeOne(command);
		}
		else if (command.command() == "romaji") {
			foreach(const QString &arg, command.args()) romajiSearch << arg;
			commands.removeOne(command);
		}
		else if (commandLabel == "jmdict") {
			statement.addJoin(QueryBuilder::Column("jmdict.entries", "id"));
			if (command.args().size() >= 1) {
				QStringList entriesId;
				bool ok;
				foreach (const QString &arg, command.args()) {
					arg.toInt(&ok);
					if (!ok) continue;
					entriesId << arg;
				}
				statement.addWhere(QString("jmdict.entries.id in (%1)").arg(entriesId.join(", ")));
			}
			commands.removeOne(command);
		}
		else if (commandLabel == "haskanji") {
			if (command.args().size() == 0) continue;
			bool valid = true;
			foreach (const QString &arg, command.args()) {
				if (arg.size() != 1 || !TextTools::isKanjiChar(arg)) {
					valid = false;
					break;
				}
				else {
					QString kNumber = QString::number(TextTools::singleCharToUnicode(arg));
					if (!hasKanjiSearch.contains(kNumber)) hasKanjiSearch << kNumber;
				}
			}
			// Break command if one of the arguments were invalid
			if (!valid) continue;
			statement.addJoin(QueryBuilder::Column("jmdict.senses", "id"));
			commands.removeOne(command);
		}
		else if (commandLabel == "hascomponent") {
			if (command.args().size() == 0) continue;
			bool valid = true;
			foreach (const QString &arg, command.args()) {
				if (arg.size() != 1 || !TextTools::isKanjiChar(arg)) {
					valid = false;
					break;
				}
				else {
					QString kNumber = QString::number(TextTools::singleCharToUnicode(arg));
					if (!hasComponentSearch.contains(kNumber)) hasComponentSearch << kNumber;
				}
			}
			// Break command if one of the arguments were invalid
			if (!valid) continue;
			statement.addJoin(QueryBuilder::Column("jmdict.senses", "id"));
			commands.removeOne(command);
		}
		else if (commandLabel == "jlpt") {
			statement.addJoin(QueryBuilder::Column("jmdict.jlpt", "id"));
			if (command.args().size() > 0) {
				QStringList levelsList;
				foreach (const QString &arg, command.args()) {
					bool isInt;
					int level = arg.toInt(&isInt);
					if (!isInt) continue;
					if (level < 1 || level > 5) continue;
					levelsList << QString::number(level);
				}
				statement.addWhere(QString("jmdict.jlpt.level in (%1)").arg(levelsList.join(", ")));
			}
			commands.removeOne(command);
		}
		else if (commandLabel == "withstudiedkanjis") {
			int minScore = -1, maxScore = -1;
			QString scoreCondition;
			if (command.args().size() == 2) {
				bool ok;
				minScore = command.args()[0].toInt(&ok);
				if (!ok) continue;
				maxScore = command.args()[1].toInt(&ok);
				if (!ok) continue;
				scoreCondition = QString("and training.score between %1 and %2").arg(minScore).arg(maxScore);
			}
			else if (command.args().size() != 0) continue;
			// Performance is very bad when using :train with this command. changing the first jmdict.entries.id to training.id in that case fixes the issue. We must
			// keep track of the "id column" and use it for this kind of reference
			QString condition(QString("{{leftcolumn}} in "
				"(select jmdict.entries.id from jmdict.entries "
				"join jmdict.kanjiChar on jmdict.entries.id = jmdict.kanjiChar.id and jmdict.kanjiChar.priority = 0 "
				"join training on training.id = jmdict.kanjiChar.kanji and training.type = 2 %1 "
				"group by jmdict.entries.id "
				"having count(jmdict.kanjiChar.kanji) = jmdict.entries.kanjiCount)").arg(scoreCondition));
			// Check whether we should include kana only words as well
			if (allCommands.contains("withkanaonly")) {
				condition = "(" + condition + QString(" or {{leftcolumn}} in "
					"(select id from jmdict.entries where kanjiCount == 0))");
			}
			statement.addWhere(condition);
			commands.removeOne(command);
		}
		else if (commandLabel == "withkanaonly") {
			if (command.args().size() != 0) continue;

			QSet<QString> otherCommands;
			otherCommands << "withstudiedkanjis" << "haskanji" << "hascomponent";
			if (otherCommands.intersect(allCommands).size() == 0) {
				statement.addJoin(QueryBuilder::Column("jmdict.entries", "id"));
				statement.addWhere(QString("jmdict.entries.kanjiCount == 0"));
			}
			commands.removeOne(command);
		}
		// Check for sense property commands
		else if (commandLabel == "pos") {
			bool allArgsProcessed = true;
			foreach (const QString &arg, command.args()) {
				// Check if the argument is defined
				auto it = JMdictPlugin::posMap().find(arg);
				if (it != JMdictPlugin::posMap().end())
					posFilter << it->second;
				else
					allArgsProcessed = false;
			}
			if (!allArgsProcessed) continue;
			commands.removeOne(command);
		}
		else if (commandLabel == "misc") {
			bool allArgsProcessed = true;
			foreach (const QString &arg, command.args()) {
				// Check if the argument is defined
				auto it = JMdictPlugin::miscMap().find(arg);
				if (it != JMdictPlugin::miscMap().end()) {
					miscFilter |= 1ULL << it->second;
					_explicitlyRequestedMiscs << arg;
				} else {
					allArgsProcessed = false;
				}
			}
			if (!allArgsProcessed) continue;
			commands.removeOne(command);
		}
		else if (commandLabel == "dial") {
			bool allArgsProcessed = true;
			foreach (const QString &arg, command.args()) {
				// Check if the argument is defined
				auto it = JMdictPlugin::dialMap().find(arg);
				if (it != JMdictPlugin::dialMap().end())
					dialectFilter |= 1ULL << it->second;
				else
					allArgsProcessed = false;
			}
			if (!allArgsProcessed) continue;
			commands.removeOne(command);
		}
		else if (commandLabel == "field") {
			bool allArgsProcessed = true;
			foreach (const QString &arg, command.args()) {
				// Check if the argument is defined
				auto it = JMdictPlugin::fieldMap().find(arg);
				if (it != JMdictPlugin::fieldMap().end())
					fieldFilter |= 1ULL << it->second;
				else
					allArgsProcessed = false;
			}
			if (!allArgsProcessed) continue;
			commands.removeOne(command);
		}
	}

	// Add where statements for text search
	foreach (const QString &rword, romajiSearch) {
		QueryBuilder::Where where("OR");
		QString kword(TextTools::romajiToKana(rword));
		if (kword.isEmpty()) transReadingsMatch << rword;
		else {
			where.addWhere(buildTextSearchCondition(QStringList() << kword, "kana"));
			where.addWhere(buildTextSearchCondition(QStringList() << rword, "gloss"));
			statement.addWhere(where);
		}
	}
	if (!kanjiReadingsMatch.isEmpty()) statement.addWhere(buildTextSearchCondition(kanjiReadingsMatch, "kanji"));
	if (!kanaReadingsMatch.isEmpty()) statement.addWhere(buildTextSearchCondition(kanaReadingsMatch, "kana"));
	if (!transReadingsMatch.isEmpty()) statement.addWhere(buildTextSearchCondition(transReadingsMatch, "gloss"));

	// Add where statements for sense filters
	// Cancel misc filters that have explicitly been required
	quint64 filteredMisc = 0;
	for (const auto &misc : miscFilterEntities() - _explicitlyRequestedMiscs) {
		auto it = JMdictPlugin::miscMap().find(misc);
		if (it != JMdictPlugin::miscMap().end())
			filteredMisc |= 1ULL << it->second;
	}

	bool mustJoinSenses = filteredMisc | !posFilter.isEmpty() | miscFilter | dialectFilter | fieldFilter;
	if (mustJoinSenses) {
		statement.addJoin(QueryBuilder::Join(QueryBuilder::Column("jmdict.senses", "id")));
		if (!posFilter.isEmpty()) {
			std::vector<qint64> masks;
			masks.resize((JMdictPlugin::posMap().size() + 63) / 64);
			for (auto shift : posFilter)
				masks[shift / 64] |= 1ULL << (shift % 64);
			for (unsigned long i = 0; i < masks.size(); i++)
				statement.addWhere(QString("jmdict.senses.pos%1 & %2 == %2").arg(i).arg(masks[i]));
		}

		if (miscFilter) statement.addWhere(QString("jmdict.senses.misc0 & %2 == %2").arg(miscFilter));
		if (dialectFilter) statement.addWhere(QString("jmdict.senses.dial0 & %2 == %2").arg(dialectFilter));
		if (fieldFilter) statement.addWhere(QString("jmdict.senses.field0 & %2 == %2").arg(fieldFilter));
		// Implicitely masked misc properties
		if (filteredMisc) statement.addWhere(QString("jmdict.senses.misc0 & %1 == 0").arg(filteredMisc));
	}

	// TODO it should be ensure that kanjisearch and componentsearch are applied on the same writing
	if (!hasKanjiSearch.isEmpty()) {
		QString condition(QString("{{leftcolumn}} in "
			"(select distinct jmdict.kanjiChar.id from jmdict.kanjiChar "
			"where jmdict.kanjiChar.kanji in (%1) "
			"group by jmdict.kanjiChar.id, jmdict.kanjiChar.priority "
			"having count(jmdict.kanjiChar.kanji) = %2)").arg(hasKanjiSearch.join(", ")).arg(hasKanjiSearch.size()));
		// Check whether we should include kana only words as well
		if (allCommands.contains("withkanaonly")) {
			condition = "(" + condition + QString(" or {{leftcolumn}} in "
				"(select id from jmdict.entries where kanjiCount == 0))");
		}
		statement.addWhere(condition);
	}

	if (!hasComponentSearch.isEmpty()) {
		QString condition(QString("{{leftcolumn}} in "
			"(select distinct jmdict.kanjiChar.id from jmdict.kanjiChar "
			"join kanjidic2.strokeGroups on jmdict.kanjiChar.kanji = kanjidic2.strokeGroups.kanji "
			"where kanjidic2.strokeGroups.element in (%1) "
			"or kanjidic2.strokeGroups.original in (%1) "
			"group by jmdict.kanjiChar.id, jmdict.kanjiChar.priority "
			"HAVING UNIQUECOUNT("
				"CASE WHEN kanjidic2.strokeGroups.element IN (%1) "
				"THEN kanjidic2.strokeGroups.element "
				"ELSE NULL END, "
				"CASE WHEN kanjidic2.strokeGroups.original IN (%1) "
				"THEN kanjidic2.strokeGroups.original "
				"ELSE NULL END) >= %2"
			")").arg(hasComponentSearch.join(", ")).arg(hasComponentSearch.size()));
		// Check whether we should include kana only words as well
		if (allCommands.contains("withkanaonly")) {
			condition = "(" + condition + QString(" or {{leftcolumn}} in "
				"(select id from jmdict.entries where kanjiCount == 0))");
		}
		statement.addWhere(condition);
	}
}

void JMdictEntrySearcher::updateMiscFilterMask()
{
	_miscFilterMask.clear();
	foreach (const QString &str, miscPropertiesFilter.value().split(',')) if (JMdictPlugin::miscMap().contains(str)) _miscFilterMask << str;
}

QueryBuilder::Column JMdictEntrySearcher::canSort(const QString &sort, const QueryBuilder::Statement &statement)
{
	QueryBuilder::Column res(EntrySearcher::canSort(sort, statement));
	if (res.column() != "0") return res;

	if (sort == "matchPos") {
		foreach(const QueryBuilder::Join &join, statement.joins()) {
			const QString &t1 = join.column1().table();
			const QString &t2 = join.column2().table();
			// TODO Update matching pattern, replace by sense.priority?
			//if (t1 == "jmdict.gloss" || t2 == "jmdict.gloss") return QueryBuilder::Column("jmdict.gloss", "sensePriority", "min");
			if (t1 == "jmdict.kana" || t2 == "jmdict.kana") return QueryBuilder::Column("jmdict.kana", "priority", "min");
			if (t1 == "jmdict.kanji" || t2 == "jmdict.kanji") return QueryBuilder::Column("jmdict.kanji", "priority", "min");
		}
	}
	else if (sort == "freq") return QueryBuilder::Column("jmdict.entries", "frequency");
	else if (sort == "jlpt") return QueryBuilder::Column("jmdict.jlpt", "level");
	return res;
}

QVector<quint64> JMdictEntrySearcher::miscFilterMask() {
	QVector<quint64> ret;

	quint64 miscMask = 0;
	for (const QString &misc : JMdictEntrySearcher::miscFilterEntities()) {
		miscMask |= (1 << JMdictPlugin::miscMap()[misc].second);
	}

	ret.append(miscMask);
	return ret;
}