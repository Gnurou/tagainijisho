/*
 *  Copyright (C) 2008  Alexandre Courbot
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
#include "core/jmdict/JMdictEntry.h"
#include "core/jmdict/JMdictEntrySearcher.h"
#include "core/jmdict/JMdictPlugin.h"

PreferenceItem<QString> JMdictEntrySearcher::miscPropertiesFilter("jmdict", "miscPropertiesFilter", "arch,obs");
quint64 JMdictEntrySearcher::_miscFilterMask = 0;
quint64 JMdictEntrySearcher::_explicitlyRequestedMiscs = 0;

JMdictEntrySearcher::JMdictEntrySearcher(QObject *parent) : EntrySearcher(parent), kanjiQuery(Database::connection()), kanaQuery(Database::connection()), sensesQuery(Database::connection()), glossQuery(Database::connection()), jlptQuery(Database::connection())
{
	connect(&JMdictEntrySearcher::miscPropertiesFilter, SIGNAL(valueChanged(QVariant)), this, SLOT(updateMiscFilterMask()));

	QueryBuilder::Join::addTablePriority("jmdict.entries", 50);
	QueryBuilder::Join::addTablePriority("jmdict.kanjiChar", 45);
	QueryBuilder::Join::addTablePriority("jmdict.kanji", 40);
	QueryBuilder::Join::addTablePriority("jmdict.kanjiText", 35);
	QueryBuilder::Join::addTablePriority("jmdict.kana", 30);
	QueryBuilder::Join::addTablePriority("jmdict.kanaText", 25);
	QueryBuilder::Join::addTablePriority("jmdict.gloss", 20);
	QueryBuilder::Join::addTablePriority("jmdict.glossText", 15);
	QueryBuilder::Join::addTablePriority("jmdict.senses", 10);
	QueryBuilder::Join::addTablePriority("jmdict.jlpt", 8);

	QueryBuilder::Order::orderingWay["freq"] = QueryBuilder::Order::DESC;

	// Register text search commands
	validCommands << "mean" << "kana" << "kanji" << "jmdict" << "haskanji" << "jlpt" << "withstudiedkanjis" << "hascomponent" << "withkanaonly";
	// Also register commands that are sense properties
	validCommands << "pos" << "misc" << "dial" << "field";

	// Prepare queries so that we just have to bind and execute them
	kanjiQuery.prepare("select reading, frequency from jmdict.kanji join jmdict.kanjiText on kanji.docid == kanjiText.docid where id=? order by priority");
	kanaQuery.prepare("select reading, nokanji, frequency, restrictedTo from jmdict.kana join jmdict.kanaText on kana.docid == kanaText.docid where id=? order by priority");
	sensesQuery.prepare("select priority, pos, misc, dial, field, restrictedToKanji, restrictedToKana from jmdict.senses where id=? order by priority asc");
	glossQuery.prepare("select gloss.lang, glossText.reading from jmdict.gloss join jmdict.glossText on gloss.docid == glossText.docid where gloss.id=? and gloss.sensePriority=?");
	jlptQuery.prepare("select jlpt.level from jmdict.jlpt where jlpt.id=?");
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
	// Then we are probably looking for a translation
	else return SearchCommand::fromString(QString(":mean=\"%1\"").arg(word));
	// Nothing? Return an invalid command
	//return SearchCommand::invalid();
}

/**
 * Turns the string given as parameter into the equivalent regexp string.
 */
static QString escapeForRegexp(const QString &string)
{
	QString ret = string;

	ret.replace('?', "[\\w]");
	ret.replace('*', "[\\w]*");
	return "\\b" + ret + "\\b";
}

static QString buildTextSearchCondition(const QStringList &words, const QString &table)
{
	static QRegExp regExpChars = QRegExp("[\\?\\*]");
	static QString ftsMatch("%2Text.reading match '%1'");
	static QString regexpMatch("%2Text.reading regexp %1");
	static QString globalMatch("%2.docid in (select docid from %2Text where %1)");

	QStringList fts;
	QStringList conds;
	foreach (const QString &w, words) {
		if (w.contains(regExpChars)) {
			// First check if we can optimize by using the FTS index (i.e. the first character is not a wildcard)
			int wildcardIdx = 0;
			while (!regExpChars.exactMatch(w[wildcardIdx])) wildcardIdx++;
			if (wildcardIdx != 0) fts << "\"" + w.mid(0, wildcardIdx) + "*\"";
			// If the wildcard we found is the last character and a star, there is no need for a regexp search
			if (wildcardIdx == w.size() - 1 && w.size() > 1 && w[wildcardIdx] == '*') continue;
			// Otherwise insert the regular expression search
			int idx = Database::staticRegExps.size();
			QRegExp regExp(escapeForRegexp(TextTools::hiragana2Katakana(w)));
			regExp.setCaseSensitivity(Qt::CaseInsensitive);
			Database::staticRegExps.append(regExp);
			conds << regexpMatch.arg(idx);
		} else fts << "\"" + w + "\"";
	}
	if (!fts.isEmpty()) conds.insert(0, ftsMatch.arg(fts.join(" ")));
	return globalMatch.arg(conds.join(" AND ")).arg(table);
}

void JMdictEntrySearcher::buildStatement(QList<SearchCommand> &commands, QueryBuilder::Statement &statement)
{
	// First delegate to the parent
	EntrySearcher::buildStatement(commands, statement);

	// Then process remaining commands
	QString table;
	QStringList kanjiReadingsMatch;
	QStringList kanaReadingsMatch;
	QStringList transReadingsMatch;
	QStringList hasKanjiSearch;
	QStringList hasComponentSearch;
	quint64 posFilter(0), miscFilter(0), dialectFilter(0), fieldFilter(0);

	QSet<QString> allCommands;
	// First build the global list of all commands
	foreach(const SearchCommand &command, commands) allCommands << command.command();

	foreach(const SearchCommand &command, commands) {
		const QString &commandLabel = command.command();

		// Search for text search commands
		if (commandLabel == "mean" || commandLabel == "kana" || commandLabel == "kanji") {
			if (command.command() == "mean") table = "jmdict.gloss";
			else if (command.command() == "kana") table = "jmdict.kana";
			else if (command.command() == "kanji") table = "jmdict.kanji";

			statement.addJoin(QueryBuilder::Join(QueryBuilder::Column(table, "id")));

			foreach(const QString &arg, command.args()) {
				if (command.command() == "mean" ||command.command() == "kanji" || command.command() == "kana") {
					// Protect multi-words arguments within quotes
					if (command.command() == "mean")
						transReadingsMatch << arg;
					if (command.command() == "kanji")
						kanjiReadingsMatch << arg;
					if (command.command() == "kana")
						kanaReadingsMatch << arg;
				}
			}
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
					if (level < 1 || level > 4) continue;
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
				if (JMdictPlugin::posBitShifts().contains(arg)) {
					quint8 bitShift(JMdictPlugin::posBitShifts()[arg]);
					posFilter |= 1L << bitShift;
				} else allArgsProcessed = false;
			}
			if (!allArgsProcessed) continue;
			commands.removeOne(command);
		}
		else if (commandLabel == "misc") {
			bool allArgsProcessed = true;
			foreach (const QString &arg, command.args()) {
				// Check if the argument is defined
				if (JMdictPlugin::miscBitShifts().contains(arg)) {
					quint8 bitShift(JMdictPlugin::miscBitShifts()[arg]);
					miscFilter |= 1L << bitShift;
				} else allArgsProcessed = false;
			}
			if (!allArgsProcessed) continue;
			commands.removeOne(command);
		}
		else if (commandLabel == "dial") {
			bool allArgsProcessed = true;
			foreach (const QString &arg, command.args()) {
				// Check if the argument is defined
				if (JMdictPlugin::dialectBitShifts().contains(arg)) {
					quint8 bitShift(JMdictPlugin::dialectBitShifts()[arg]);
					dialectFilter |= 1L << bitShift;
				} else allArgsProcessed = false;
			}
			if (!allArgsProcessed) continue;
			commands.removeOne(command);
		}
		else if (commandLabel == "field") {
			bool allArgsProcessed = true;
			foreach (const QString &arg, command.args()) {
				// Check if the argument is defined
				if (JMdictPlugin::fieldBitShifts().contains(arg)) {
					quint8 bitShift(JMdictPlugin::fieldBitShifts()[arg]);
					fieldFilter |= 1L << bitShift;
				} else allArgsProcessed = false;
			}
			if (!allArgsProcessed) continue;
			commands.removeOne(command);
		}
	}

	// Add where statements for text search
	if (!kanjiReadingsMatch.isEmpty()) statement.addWhere(buildTextSearchCondition(kanjiReadingsMatch, "jmdict.kanji"));
	if (!kanaReadingsMatch.isEmpty()) statement.addWhere(buildTextSearchCondition(kanaReadingsMatch, "jmdict.kana"));
	if (!transReadingsMatch.isEmpty()) statement.addWhere(buildTextSearchCondition(transReadingsMatch, "jmdict.gloss"));

	// Add where statements for sense filters
	// Cancel misc filters that have explicitly been required
	_explicitlyRequestedMiscs = miscFilter;
	quint64 _miscFilterMask = miscFilterMask() & ~_explicitlyRequestedMiscs;

	bool mustJoinSenses = _miscFilterMask | posFilter | miscFilter | dialectFilter | fieldFilter;
	if (mustJoinSenses) {
		statement.addJoin(QueryBuilder::Join(QueryBuilder::Column("jmdict.senses", "id")));
		if (posFilter) statement.addWhere(QString("jmdict.senses.pos & %2 == %2").arg(posFilter));
		if (miscFilter) statement.addWhere(QString("jmdict.senses.misc & %2 == %2").arg(miscFilter));
		if (dialectFilter) statement.addWhere(QString("jmdict.senses.dial & %2 == %2").arg(dialectFilter));
		if (fieldFilter) statement.addWhere(QString("jmdict.senses.field & %2 == %2").arg(fieldFilter));
		// Implicitely masked misc properties
		if (_miscFilterMask) statement.addWhere(QString("jmdict.senses.misc & %1 == 0").arg(_miscFilterMask));
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
	_miscFilterMask = 0;
	foreach (const QString &str, miscPropertiesFilter.value().split(',')) if (JMdictPlugin::miscBitShifts().contains(str)) _miscFilterMask |= 1 << JMdictPlugin::miscBitShifts()[str];
}

QueryBuilder::Column JMdictEntrySearcher::canSort(const QString &sort, const QueryBuilder::Statement &statement)
{
	QueryBuilder::Column res(EntrySearcher::canSort(sort, statement));
	if (res.column() != "0") return res;

	if (sort == "matchPos") {
		foreach(const QueryBuilder::Join &join, statement.joins()) {
			const QString &t1 = join.column1().table();
			const QString &t2 = join.column2().table();
			if (t1 == "jmdict.gloss" || t2 == "jmdict.gloss") return QueryBuilder::Column("jmdict.gloss", "sensePriority", "min");
			if (t1 == "jmdict.kana" || t2 == "jmdict.kana") return QueryBuilder::Column("jmdict.kana", "priority", "min");
			if (t1 == "jmdict.kanji" || t2 == "jmdict.kanji") return QueryBuilder::Column("jmdict.kanji", "priority", "min");
		}
	}
	else if (sort == "freq") return QueryBuilder::Column("jmdict.entries", "frequency");
	else if (sort == "jlpt") return QueryBuilder::Column("jmdict.jlpt", "level");
	return res;
}

Entry *JMdictEntrySearcher::loadEntry(int id)
{
	JMdictEntry *entry = new JMdictEntry(id);

	EntrySearcher::loadMiscData(entry);

	// Now load readings
	// Kanji readings
	kanjiQuery.bindValue(entry->id());
	kanjiQuery.exec();
	while(kanjiQuery.next()) {
		entry->kanjis << KanjiReading(kanjiQuery.valueString(0), 0, kanjiQuery.valueUInt(1));
	}
	kanjiQuery.reset();

	// Kana readings
	kanaQuery.bindValue(entry->id());
	kanaQuery.exec();
	while(kanaQuery.next())
	{
		KanaReading kana(kanaQuery.valueString(0), 0, kanaQuery.valueUInt(2));
		// Get kana readings
		if (kanaQuery.valueBool(1) == false) {
			QStringList restrictedTo(kanaQuery.valueString(3).split(',', QString::SkipEmptyParts));
			if (restrictedTo.isEmpty()) for (int i = 0; i < entry->getKanjiReadings().size(); i++) {
				kana.addKanjiReading(i);
			}
			else for (int i = 0; i < restrictedTo.size(); i++) {
				kana.addKanjiReading(restrictedTo[i].toInt());
			}
		}
		entry->addKanaReading(kana);
	}
	kanaQuery.reset();

	// Senses
	sensesQuery.bindValue(entry->id());
	sensesQuery.exec();
	while(sensesQuery.next()) {
		Sense sense(sensesQuery.valueUInt64(1), sensesQuery.valueUInt64(2), sensesQuery.valueUInt64(3), sensesQuery.valueUInt64(4));
		// Get restricted readings/writing
		QStringList restrictedTo(sensesQuery.valueString(5).split(',', QString::SkipEmptyParts));
		foreach (const QString &idx, restrictedTo) sense.addStagK(idx.toInt());
		restrictedTo = sensesQuery.valueString(6).split(',', QString::SkipEmptyParts);
		foreach (const QString &idx, restrictedTo) sense.addStagR(idx.toInt());

		glossQuery.bindValue(entry->id());
		glossQuery.bindValue(sensesQuery.valueInt(0));
		glossQuery.exec();
		while(glossQuery.next())
			sense.addGloss(Gloss(glossQuery.valueString(0), glossQuery.valueString(1)));
		entry->senses << sense;
	}
	glossQuery.reset();
	sensesQuery.reset();

	// JLPT level
	jlptQuery.bindValue(entry->id());
	jlptQuery.exec();
	if (jlptQuery.next()) {
		entry->_jlpt = jlptQuery.valueInt(0);
	}
	jlptQuery.reset();
	return entry;
}
