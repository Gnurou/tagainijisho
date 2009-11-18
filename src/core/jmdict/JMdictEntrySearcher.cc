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
#include "core/jmdict/JMdictDefs.h"
#include "core/jmdict/JMdictEntry.h"
#include "core/jmdict/JMdictEntrySearcher.h"

QVector<QMap<QString, quint64> > JMdictEntrySearcher::JMdictReversedSenseTags;

PreferenceItem<QStringList> JMdictEntrySearcher::miscPropertiesFilter("jmdict", "miscPropertiesFilter", QStringList());
JMdictMiscTagType JMdictEntrySearcher::_miscFilterMask = 0;
JMdictMiscTagType JMdictEntrySearcher::_explicitlyRequestedMiscs = 0;

void JMdictEntrySearcher::buildJMdictReversedSenseTags()
{
	JMdictReversedSenseTags.resize(JMdictNbSenseTags);
	for (int i = 0; i < JMdictReversedSenseTags.size(); i++) {
		for (int j = 0; JMdictSenseTags[i][j] != ""; j++) {
			JMdictReversedSenseTags[i][JMdictSenseTags[i][j]] = JMdictSenseTagsValues[i][j];
		}
	}
}

JMdictEntrySearcher::JMdictEntrySearcher(QObject *parent) : EntrySearcher(parent)
{
	// First check if the entities lookup table should be built
	if (JMdictReversedSenseTags.isEmpty()) {
		buildJMdictReversedSenseTags();
		updateMiscFilterMask();
		connect(&JMdictEntrySearcher::miscPropertiesFilter, SIGNAL(valueChanged(QVariant)), this, SLOT(updateMiscFilterMask()));
	}

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
	validCommands << "mean" << "kana" << "kanji" << "jmdict" << "haskanji" << "jlpt" << "withstudiedkanjis" << "hascomponent";
	// Also register commands that are sense properties
	for (int i = 0; i < JMdictNbSenseTags; i++) validCommands << JMdictSenseTagsList[i];

	// Prepare queries so that we just have to bind and execute them
	kanjiQuery.prepare("select reading, frequency from jmdict.kanji join jmdict.kanjiText on kanji.docid == kanjiText.docid where id=? order by priority");
	kanaQuery.prepare("select reading, nokanji, frequency, restrictedTo from jmdict.kana join jmdict.kanaText on kana.docid == kanaText.docid where id=? order by priority");
	sensesQuery.prepare("select priority, pos, misc, dial, field from jmdict.senses where id=? order by priority asc");
	stagKQuery.prepare("select kanjiPriority from stagk where id=? and sensePriority=?");
	stagRQuery.prepare("select kanaPriority from stagr where id=? and sensePriority=?");
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
QString escapeForRegexp(const QString &string)
{
	QString ret = string;

	ret.replace('?', '.');
	ret.replace('*', ".*");
	return ret;
}

void JMdictEntrySearcher::buildStatement(QList<SearchCommand> &commands, QueryBuilder::Statement &statement)
{
	// First delegate to the parent
	EntrySearcher::buildStatement(commands, statement);

	QueryBuilder::Column frequencyColumn("jmdict.entries", "frequency");

	// Then process remaining commands
	static QRegExp regExpChars = QRegExp("[\\?\\*]");
	// Default frequency table is the global entry one
	QString table, freqTable = "jmdict.entries";
	QStringList kanjiReadingsMatch;
	QStringList kanaReadingsMatch;
	QStringList transReadingsMatch;
	QStringList hasKanjiSearch;
	QStringList hasComponentSearch;
	QVector<quint64> senseFilters(JMdictNbSenseTags);
	foreach(const SearchCommand &command, commands) {
		const QString &commandLabel = command.command();

		// Search for text search commands
		if (commandLabel == "mean" || commandLabel == "kana" || commandLabel == "kanji") {
			if (command.command() == "mean") table = "jmdict.gloss";
			else if (command.command() == "kana") table = "jmdict.kana";
			else if (command.command() == "kanji") table = "jmdict.kanji";

			if (command.command() != "mean") freqTable = table;

			statement.addJoin(QueryBuilder::Join(QueryBuilder::Column(table, "id")));

			foreach(const QString &arg, command.args()) {
				if (command.command() == "mean" ||command.command() == "kanji" || command.command() == "kana") {
					// Protect multi-words arguments within quotes
					if (command.command() == "mean")
						transReadingsMatch << "\"" + arg + "\"";
					if (command.command() == "kanji")
						kanjiReadingsMatch << "\"" + arg + "\"";
					if (command.command() == "kana")
						kanaReadingsMatch << "\"" + arg + "\"";
				} /* else {
					// For non-japanese words we want to match entries that contain the expression
					if (arg.contains(regExpChars)) {
						QString s;
						s = escapeForRegexp(arg);
						int idx = Database::staticRegExps.size();
						QRegExp regExp = QRegExp(s);
						regExp.setCaseSensitivity(Qt::CaseInsensitive);
						Database::staticRegExps.append(regExp);
						query.addWhere(Query::Where(table + ".reading regexp " + QString::number(idx)));
					}
					else query.addWhere(Query::Where(table + ".reading like \"" + arg + "\""));
				}*/
			}
			commands.removeOne(command);
		}
		else if (commandLabel == "jmdict") {
			if (command.args().size() > 1) continue;
			statement.addJoin(QueryBuilder::Column("jmdict.entries", "id"));
			if (command.args().size() == 1) {
				int entryId;
				bool ok;
				entryId = command.args()[0].toInt(&ok);
				if (!ok) continue;
				statement.addWhere(QString("jmdict.entries.id = %1").arg(entryId));
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
			statement.addJoin(QueryBuilder::Column("jmdict.entries", "id"));
			// Performance is very bad when using :train with this command. changing the first jmdict.entries.id to training.id in that case fixes the issue. We must
			// keep track of the "id column" and use it for this kind of reference
			statement.addWhere(QString("{{leftcolumn}} in (select jmdict.entries.id from jmdict.entries join jmdict.kanjiChar on jmdict.entries.id = jmdict.kanjiChar.id and jmdict.kanjiChar.priority = 0 join training on training.id = jmdict.kanjiChar.kanji and training.type = 2 %1 group by jmdict.entries.id having count(jmdict.kanjiChar.kanji) = jmdict.entries.kanjiCount)").arg(scoreCondition));
			commands.removeOne(command);
		}
		// Check for sense property commands
		else for (int i = 0; i < JMdictNbSenseTags; i++) {
			if (commandLabel == JMdictSenseTagsList[i]) {
				bool allArgsProcessed = true;
				foreach(const QString &arg, command.args()) {
					// Check if the argument is defined
					if (JMdictReversedSenseTags[i].contains(arg)) {
						senseFilters[i] |= JMdictReversedSenseTags[i][arg];
					} else allArgsProcessed = false;
				}
				// The command is considered as recognized only if all its arguments
				// were successfully recognized.
				if (allArgsProcessed && !command.args().isEmpty()) commands.removeOne(command);
				break;
			}
		}
	}

	// Add where statements for text search
	static QString ftsString = "%1.docid in (select docid from %1Text where %1Text.reading match '%2')";
	if (!kanjiReadingsMatch.isEmpty()) statement.addWhere(ftsString.arg("jmdict.kanji").arg(kanjiReadingsMatch.join(" ")));
	if (!kanaReadingsMatch.isEmpty()) statement.addWhere(ftsString.arg("jmdict.kana").arg(kanaReadingsMatch.join(" ")));
	if (!transReadingsMatch.isEmpty()) statement.addWhere(ftsString.arg("jmdict.gloss").arg(transReadingsMatch.join(" ")));

	// Add where statements for sense filters
	// Cancel misc filters that have explicitly been required
	_explicitlyRequestedMiscs = senseFilters[JMdictMiscIndex];
	JMdictMiscTagType _miscFilterMask = miscFilterMask();
	_miscFilterMask &= ~_explicitlyRequestedMiscs;

	bool mustJoinSenses = false;
	if (_miscFilterMask != 0) mustJoinSenses = true;
	else for (int i = 0; i < JMdictNbSenseTags; i++) if (senseFilters[i] != 0) mustJoinSenses = true;
	if (mustJoinSenses) {
		statement.addJoin(QueryBuilder::Join(QueryBuilder::Column("jmdict.senses", "id")));
		for (int i = 0; i < JMdictNbSenseTags; i++) if (senseFilters[i] != 0) {
			// If we remove the right part of the == in that condition, it becomes an "OR"
			// instead of an "AND"
			statement.addWhere(QString("jmdict.senses.%1 & %2 == %2").arg(JMdictSenseTagsList[i]).arg(senseFilters[i]));
		}
		// Implicitely masked misc properties
		if (_miscFilterMask) statement.addWhere(QString("jmdict.senses.misc & %1 == 0").arg(_miscFilterMask));
	}

	// TODO it should be ensure that kanjisearch and componentsearch are applied on the same writing
	if (!hasKanjiSearch.isEmpty()) {
		statement.addJoin(QueryBuilder::Column("jmdict.entries", "id"));
		statement.addWhere(QString("{{leftcolumn}} in (select distinct jmdict.kanjiChar.id from jmdict.kanjiChar where jmdict.kanjiChar.kanji in (%1) group by jmdict.kanjiChar.id, jmdict.kanjiChar.priority having count(jmdict.kanjiChar.kanji) = %2)").arg(hasKanjiSearch.join(", ")).arg(hasKanjiSearch.size()));
	}

	if (!hasComponentSearch.isEmpty()) {
		statement.addJoin(QueryBuilder::Column("jmdict.entries", "id"));
		statement.addWhere(QString("{{leftcolumn}} in (select distinct jmdict.kanjiChar.id from jmdict.kanjiChar join kanjidic2.strokeGroups on jmdict.kanjiChar.kanji = kanjidic2.strokeGroups.kanji where kanjidic2.strokeGroups.element in (%1) or kanjidic2.strokeGroups.original in (%1) group by jmdict.kanjiChar.id, jmdict.kanjiChar.priority HAVING UNIQUECOUNT(CASE WHEN kanjidic2.strokeGroups.element IN (%1) THEN kanjidic2.strokeGroups.element ELSE NULL END, CASE WHEN kanjidic2.strokeGroups.original IN (%1) THEN kanjidic2.strokeGroups.original ELSE NULL END) >= %2)").arg(hasComponentSearch.join(", ")).arg(hasComponentSearch.size()));
	}
}

void JMdictEntrySearcher::updateMiscFilterMask()
{
	_miscFilterMask = 0;
	foreach (const QString &str, miscPropertiesFilter.value()) _miscFilterMask |= JMdictReversedSenseTags[JMdictMiscIndex][str];
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
	return res;
}

Entry *JMdictEntrySearcher::loadEntry(int id)
{
	JMdictEntry *entry = new JMdictEntry(id);

	EntrySearcher::loadMiscData(entry);

	// Now load readings
	// Kanji readings
	kanjiQuery.addBindValue(entry->id());
	kanjiQuery.exec();
	while(kanjiQuery.next()) {
		entry->kanjis << KanjiReading(kanjiQuery.value(0).toString(), 0, kanjiQuery.value(1).toUInt());
	}
	kanjiQuery.finish();

	// Kana readings
	kanaQuery.addBindValue(entry->id());
	kanaQuery.exec();
	while(kanaQuery.next())
	{
		KanaReading kana(kanaQuery.value(0).toString(), 0, kanaQuery.value(2).toUInt());
		// Get kana readings
		if (kanaQuery.value(1).toBool() == false) {
			QStringList restrictedTo = kanaQuery.value(3).toString().split(',', QString::SkipEmptyParts);
			if (restrictedTo.isEmpty()) for (int i = 0; i < entry->getKanjiReadings().size(); i++) {
				kana.addKanjiReading(i);
			}
			else for (int i = 0; i < restrictedTo.size(); i++) {
				kana.addKanjiReading(restrictedTo[i].toInt());
			}
		}
		entry->addKanaReading(kana);
	}
	kanaQuery.finish();

	// Senses
	sensesQuery.addBindValue(entry->id());
	sensesQuery.exec();
	while(sensesQuery.next()) {
		Sense sense(sensesQuery.value(1).toULongLong(), sensesQuery.value(2).toULongLong(), sensesQuery.value(3).toULongLong(), sensesQuery.value(4).toULongLong());
		// Get restricted readings/writing
		stagKQuery.addBindValue(entry->id());
		stagKQuery.addBindValue(sensesQuery.value(0).toInt());
		stagKQuery.exec();
		while(stagKQuery.next()) sense.addStagK(stagKQuery.value(0).toInt());
		stagRQuery.addBindValue(entry->id());
		stagRQuery.addBindValue(sensesQuery.value(0).toInt());
		stagRQuery.exec();
		while(stagRQuery.next()) sense.addStagR(stagRQuery.value(0).toInt());

		glossQuery.addBindValue(entry->id());
		glossQuery.addBindValue(sensesQuery.value(0).toInt());
		glossQuery.exec();
		while(glossQuery.next())
			sense.addGloss(Gloss(glossQuery.value(0).toString(), glossQuery.value(1).toString()));
		entry->senses << sense;
	}
	glossQuery.finish();
	sensesQuery.finish();

	// JLPT level
	jlptQuery.addBindValue(entry->id());
	jlptQuery.exec();
	if (jlptQuery.next()) {
		entry->_jlpt = jlptQuery.value(0).toInt();
	}
	jlptQuery.finish();
	return entry;
}
