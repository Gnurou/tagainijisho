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
#include "core/kanjidic2/Kanjidic2EntrySearcher.h"

#include <QtDebug>

#include <QSqlQuery>
#include <QSqlRecord>

Kanjidic2EntrySearcher::Kanjidic2EntrySearcher(QObject *parent) : EntrySearcher(parent)
{
	QueryBuilder::Join::addTablePriority("kanjidic2.entries", 10);
	QueryBuilder::Join::addTablePriority("kanjidic2.strokeGroups", 20);
	QueryBuilder::Join::addTablePriority("kanjidic2.strokes", 20);

	QueryBuilder::Order::orderingWay["freq"] = QueryBuilder::Order::DESC;

	validCommands << "kanji" << "kana" << "mean" << "jlpt" << "grade" << "stroke" << "component" << "unicode" << "kanjidic";
}

SearchCommand Kanjidic2EntrySearcher::commandFromWord(const QString &word) const
{
	// First see what the parent has to propose
	SearchCommand fromParent = EntrySearcher::commandFromWord(word);
	if (fromParent.isValid()) return fromParent;

	QString checkString(word);
	// Remove all wild cards
	checkString.remove(QRegExp("[\\?\\*]"));

	// We have a kanji command if the string is of size 1 and the character is a kanji.
	if (word.size() == 1 && TextTools::isKanji(word[0])) return SearchCommand::fromString(QString(":kanji=\"%1\"").arg(word[0]));
	else if (TextTools::isKana(checkString)) return SearchCommand::fromString(QString(":kana=\"%1\"").arg(word));
	else if (TextTools::isRomaji(checkString)) return SearchCommand::fromString(QString(":mean=\"%1\"").arg(word));
	return SearchCommand::invalid();
}

void Kanjidic2EntrySearcher::buildStatement(QList<SearchCommand> &commands, QueryBuilder::Statement &statement)
{
	// Delegate to the parent first
	EntrySearcher::buildStatement(commands, statement);

	// And do the rest
	QStringList kanaSearch;
	QStringList transSearch;
	QStringList componentSearch;
	foreach (const SearchCommand &command, commands) {
		bool processed = true;
		if (command.command() == "kanji") {
			if (command.args().size() > 1) continue;
			if (command.args().size() == 1) {
				if (command.args()[0].size() != 1) continue;
				statement.addWhere(QString("kanjidic2.entries.id = %1").arg(command.args()[0][0].unicode()));
			}
		}
		else if (command.command() == "kana") {
			statement.addJoin(QueryBuilder::Column("kanjidic2.reading", "entry"));
			foreach(const QString &arg, command.args()) kanaSearch << "\"" + arg + "\"";
		}
		else if (command.command() == "mean") {
 			statement.addJoin(QueryBuilder::Column("kanjidic2.meaning", "entry"));
			foreach(const QString &arg, command.args()) transSearch << "\"" + arg + "\"";
		}
		else if (command.command() == "jlpt") {
			if (command.args().isEmpty()) statement.addWhere(QString("kanjidic2.entries.jlpt not null"));
			else {
				QStringList levelsList;
				foreach (const QString &arg, command.args()) {
					bool isInt;
					int level = arg.toInt(&isInt);
					if (!isInt) continue;
					if (level < 1 || level > 4) continue;
					levelsList << QString::number(level);
				}
				statement.addWhere(QString("kanjidic2.entries.jlpt in (%1)").arg(levelsList.join(", ")));
			}
		}
		else if (command.command() == "grade") {
			if (command.args().isEmpty()) statement.addWhere(QString("kanjidic2.entries.grade not null"));
			else {
				QStringList levelsList;
				foreach (const QString &arg, command.args()) {
					bool isInt;
					int level = arg.toInt(&isInt);
					if (!isInt) continue;
					if (level < 1 || level > 10) continue;
					levelsList << QString::number(level);
				}
				statement.addWhere(QString("kanjidic2.entries.grade in (%1)").arg(levelsList.join(", ")));
			}
		}
		else if (command.command() == "stroke") {
			if (command.args().size() != 1) continue;
			statement.addWhere(QString("kanjidic2.entries.strokeCount = %1").arg(command.args()[0].toInt()));
		}
		else if (command.command() == "component") {
			if (command.args().size() > 0) {
				bool valid = true;
				foreach (const QString &arg, command.args()) {
					if (arg.size() != 1 || !TextTools::isKanjiChar(arg[0])) {
						valid = false;
						break;
					}
					else componentSearch << QString::number(arg[0].unicode());
				}
				// Break command if one of the arguments were invalid
				if (!valid) continue;
			}
		}
		else if (command.command() == "unicode") {
			if (command.args().size() > 1) continue;
			if (command.args().size() == 1) {
				bool ok;
				int code = command.args()[0].toInt(&ok, 16);
				if (!ok) continue;
				statement.addWhere(QString("kanjidic2.entries.id = %1").arg(code));
			}
		}
		// Filter command
		else if (command.command() == "kanjidic") ;
		else processed = false;
		if (processed) {
			commands.removeOne(command);
			statement.addJoin(QueryBuilder::Column("kanjidic2.entries", "id"));
		}
	}
	if (!kanaSearch.isEmpty())
		statement.addWhere(QString("kanjidic2.reading.docid in (select docid from kanjidic2.readingText where reading match '%1')").arg(kanaSearch.join(" ")));
	if (!transSearch.isEmpty())
		statement.addWhere(QString("kanjidic2.meaning.docid in (select docid from kanjidic2.meaningText where meaning match '%1')").arg(transSearch.join(" ")));
	if (!componentSearch.isEmpty()) {
		QString onlyDirectComponentsString("join kanjidic2.strokeGroups as ks2 on ks1.parentGroup = ks2.rowid and ks2.parentGroup is null ");
		QString qString;
		// TODO We should be able to control this - probably when we have a more powerful command system based on subclasses
		if (0) qString = onlyDirectComponentsString;
		statement.addWhere(QString("kanjidic2.entries.id IN (SELECT DISTINCT ks1.kanji FROM kanjidic2.strokeGroups AS ks1 %3WHERE (ks1.element IN (%1) OR ks1.original IN (%1)) AND ks1.parentGroup NOT NULL GROUP BY ks1.kanji HAVING UNIQUECOUNT(CASE WHEN ks1.element IN (%1) THEN ks1.element ELSE NULL END, CASE WHEN ks1.original IN (%1) THEN ks1.original ELSE NULL END) >= %2)").arg(componentSearch.join(", ")).arg(componentSearch.size()).arg(qString));
	}
}

QueryBuilder::Column Kanjidic2EntrySearcher::canSort(const QString &sort, const QueryBuilder::Statement &statement)
{
	QueryBuilder::Column res(EntrySearcher::canSort(sort, statement));
	if (res.column() != "0") return res;

	// Would be nice to have, but all meanings are grouped into the same string. Maybe in a future database revision?
	/*
	if (sort == "matchPos") {
		foreach(const QueryBuilder::Join &join, statement.joins()) {
			const QString &t1 = join.column1().table();
			const QString &t2 = join.column2().table();
			if (t1 == "kanjidic2.meaning" || t2 == "kanjidic2.meaning") return QueryBuilder::Column("kanjidic2.meaning", "priority", "min");
		}
	}*/
	// TODO replace with frequency, once the sort order is changed
	else if (sort == "freq") return QueryBuilder::Column("kanjidic2.entries", "jlpt");
	return res;
}

QList<Kanjidic2Entry::KanjiMeaning> Kanjidic2EntrySearcher::getMeanings(int id)
{
	QList<Kanjidic2Entry::KanjiMeaning> ret;
	QSqlQuery query;
	query.prepare("select lang, meaning from kanjidic2.meaning join kanjidic2.meaningText on kanjidic2.meaning.docid = kanjidic2.meaningText.docid where entry = ?");
	query.addBindValue(id);
	query.exec();
	while(query.next()) {
		ret << Kanjidic2Entry::KanjiMeaning(query.value(0).toString(), query.value(1).toString());
	}
	return ret;
}

Entry *Kanjidic2EntrySearcher::loadEntry(int id)
{
	QString character = TextTools::unicodeToSingleChar(id);

	QSqlQuery query;
	query.prepare("select grade, strokeCount, frequency, jlpt from kanjidic2.entries where id = ?");
	query.addBindValue(id);
	query.exec();
	Kanjidic2Entry *entry;
	// We have no information about this kanji! This is probably an unknown radical
	if (!query.next())
		entry = new Kanjidic2Entry(character, false, -1, -1, -1, -1);
	// Else load the kanji
	else {
		int grade = query.value(0).isNull() ? -1 : query.value(0).toInt();
		int strokeCount = query.value(1).isNull() ? -1 : query.value(1).toInt();
		int frequency = query.value(2).isNull() ? -1 : query.value(2).toInt();
		int jlpt = query.value(3).isNull() ? -1 : query.value(3).toInt();

		entry = new Kanjidic2Entry(character, true, grade, strokeCount, frequency, jlpt);
	}

	loadMiscData(entry);

	// Find the kanjis this one is a variation of
	query.prepare("select distinct original from strokeGroups where element = ?");
	query.addBindValue(id);
	query.exec();
	while (query.next()) {
		entry->_variationOf << query.value(0).toInt();
	}

	// Now load readings, meanings and nanoris
	// Readings
	query.prepare("select type, reading from kanjidic2.reading join kanjidic2.readingText on kanjidic2.reading.docid = kanjidic2.readingText.docid where entry = ? order by type");
	query.addBindValue(id);
	query.exec();
	while (query.next()) {
		entry->_readings << Kanjidic2Entry::KanjiReading(query.value(0).toString(), query.value(1).toString());
	}

	// Meanings
	entry->_meanings = getMeanings(id);
	// If this kanji has no meaning, but is derived from a kanji that does, then the meanings are inherited
	if (entry->_meanings.isEmpty()) {
		// Look for every variation until we find one that has meanings
		foreach (int vid, entry->_variationOf) {
			QList<Kanjidic2Entry::KanjiMeaning> meanings(getMeanings(vid));
			if (!meanings.isEmpty()) {
				entry->_meanings = meanings;
			}
		}
	}

	// Nanori
	query.prepare("select reading from kanjidic2.nanori join kanjidic2.nanoriText on kanjidic2.nanori.docid = kanjidic2.nanoriText.docid where entry = ?");
	query.addBindValue(id);
	query.exec();
	while(query.next()) {
		entry->_nanoris << query.value(0).toString();
	}

	// Load components
	query.prepare("select rowid, parentGroup, number, element, original from strokeGroups where kanji = ? order by rowid");
	query.addBindValue(id);
	query.exec();
	QMap<int, KanjiComponent *> componentsMap;
	while(query.next()) {
		int componentId(query.value(1).toInt());
		KanjiComponent *parentGroup(0);
		// Should never happen
		if (componentId && !componentsMap.contains(componentId)) {
			qWarning() << "While loading kanjidic2 entry: unable to find parent group!";
		}
		else if (componentId) parentGroup = componentsMap[componentId];
		QString element(TextTools::unicodeToSingleChar(query.value(3).toUInt()));
		QString original(TextTools::unicodeToSingleChar(query.value(4).toUInt()));;
		KanjiComponent *newComp(entry->addComponent(parentGroup, element, original, query.value(2).toInt()));
		if (parentGroup) parentGroup->addChild(newComp);
		componentsMap[query.value(0).toInt()] = newComp;
	}

	// Load strokes
	QStringList keys;
	foreach (int id, componentsMap.keys()) keys << QString::number(id);
	query.prepare("select parentGroup, strokeType, path from strokes where parentGroup in (" + keys.join(",") + ") order by rowid");
	query.exec();
	while(query.next()) {
		int componentId(query.value(0).toInt());
		KanjiComponent *parentGroup(0);
		// Should never happen
		if (!componentsMap.contains(componentId)) {
			qWarning() << "While loading kanjidic2 entry: unable to find parent group!";
		}
		else parentGroup = componentsMap[componentId];
		KanjiStroke *newStroke(entry->addStroke(parentGroup, QChar(query.value(1).toInt()), query.value(2).toString()));
		KanjiComponent *it(parentGroup);
		while (it) {
			it->addStroke(newStroke);
			// const_casting is bad - but as we are building the kanji structure,
			// let's forgive it.
			it = const_cast<KanjiComponent *>(it->parent());
		}
	}
	// For kanjis that have no component at all, add a dummy root component.
	if (entry->components().isEmpty()) entry->addComponent(0, TextTools::unicodeToSingleChar(id), 0, 0);

	return entry;
}

