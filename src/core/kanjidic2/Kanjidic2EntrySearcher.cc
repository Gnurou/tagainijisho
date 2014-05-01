/*
 *  Copyright (C) 2008, 2009, 2010  Alexandre Courbot
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
#include "core/kanjidic2/Kanjidic2Plugin.h"
#include "core/kanjidic2/Kanjidic2EntrySearcher.h"
#include "core/kanjidic2/Kanjidic2Entry.h"
#include "sqlite/SQLite.h"

#include <QtDebug>

#include <QByteArray>

Kanjidic2EntrySearcher::Kanjidic2EntrySearcher() : EntrySearcher(KANJIDIC2ENTRY_GLOBALID)
{
	QueryBuilder::Join::addTablePriority("kanjidic2.entries", 10);
	QueryBuilder::Join::addTablePriority("kanjidic2.strokeGroups", 20);
	QueryBuilder::Join::addTablePriority("kanjidic2.strokes", 20);

	QueryBuilder::Order::orderingWay["freq"] = QueryBuilder::Order::DESC;

	validCommands << "kanji" << "romaji" << "kana" << "mean" << "jlpt" << "grade" << "stroke" << "radical" << "component" << "unicode" << "skip" << "fourcorner" << "kanjidic";
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
	else if (TextTools::isRomaji(checkString)) {
		if (allowRomajiSearch()) {
			QString kn(TextTools::romajiToKana(word));
			if (!kn.isEmpty()) return SearchCommand::fromString(QString(":romaji=\"%1\"").arg(word));
		}
		return SearchCommand::fromString(QString(":mean=\"%1\"").arg(word));
	}
	return SearchCommand::invalid();
}

static QString buildTextSearchCondition(const QStringList &words, const QString &table)
{
	static QRegExp regExpChars = QRegExp("[\\?\\*]");
	static QString ftsMatch("kanjidic2%3.%2Text.reading MATCH '%1'");
	static QString regexpMatch("kanjidic2%3.%2Text.reading REGEXP '%1'");
	static QString glossRegexpMatch("{{leftcolumn}} in (select entry from kanjidic2_%2.meaning where FTSUNCOMPRESS(meanings) REGEXP '%1')");
	static QString globalMatch("{{leftcolumn}} IN (SELECT entry FROM kanjidic2%3.%2 JOIN kanjidic2%3.%2Text ON kanjidic2%3.%2.docid = kanjidic2%3.%2Text.docid WHERE %1)");

	QStringList globalMatches;
	QStringList langs(Kanjidic2Plugin::instance()->attachedDBs().keys());
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
				if (table != "meaning")
					conds << regexpMatch.arg(regExp);
				else
					condsGloss << glossRegexpMatch.arg(regExp).arg(lang);
			} else fts << "\"" + w + "\"";
		}
		if (!fts.isEmpty()) conds.insert(0, ftsMatch.arg(fts.join(" ")));
		if (!conds.isEmpty()) globalMatches << globalMatch.arg(conds.join(" AND ")).arg(table).arg(table == "meaning" ? "_" + lang : "");
		globalMatches += condsGloss;
	}
	return globalMatches.join(" OR ");
}

void Kanjidic2EntrySearcher::buildStatement(QList<SearchCommand> &commands, QueryBuilder::Statement &statement)
{
	// Delegate to the parent first
	EntrySearcher::buildStatement(commands, statement);

	// And do the rest
	QStringList kanaSearch;
	QStringList transSearch;
	QStringList romajiSearch;
	QStringList radicalSearch;
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
			foreach(const QString &arg, command.args()) kanaSearch << arg;
		}
		else if (command.command() == "romaji") {
			foreach(const QString &arg, command.args()) romajiSearch << arg;
		}
		else if (command.command() == "mean") {
			foreach(const QString &arg, command.args())
				transSearch << arg;
		}
		else if (command.command() == "jlpt") {
			if (command.args().isEmpty()) statement.addWhere(QString("kanjidic2.entries.jlpt not null"));
			else {
				QStringList levelsList;
				foreach (const QString &arg, command.args()) {
					bool isInt;
					int level = arg.toInt(&isInt);
					if (!isInt) continue;
					if (level < 1 || level > 5) continue;
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
			if (command.args().size() == 1) {
				statement.addWhere(QString("kanjidic2.entries.strokeCount = %1").arg(command.args()[0].toInt()));
			}
			else if (command.args().size() == 2) {
				statement.addWhere(QString("kanjidic2.entries.strokeCount between %1 and %2").arg(command.args()[0].toInt()).arg(command.args()[1].toInt()));
			}
			else continue;
			// Also ensure we do not return kana or roman characters
			statement.addWhere(QString("kanjidic2.entries.id > %1").arg(0x31ff));
		}
		else if (command.command() == "radical") {
			if (command.args().size() > 0) {
				bool valid = true;
				foreach (const QString &arg, command.args()) {
					if (arg.size() != 1 || !TextTools::isKanjiChar(arg)) {
						valid = false;
						break;
					}
					else radicalSearch << QString::number(TextTools::singleCharToUnicode(arg));
				}
				// Break command if one of the arguments were invalid
				if (!valid) continue;
			}
		}
		else if (command.command() == "component") {
			if (command.args().size() > 0) {
				bool valid = true;
				foreach (const QString &arg, command.args()) {
					if (arg.size() != 1 || !TextTools::isKanjiChar(arg)) {
						valid = false;
						break;
					}
					else componentSearch << QString::number(TextTools::singleCharToUnicode(arg));
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
		else if (command.command() == "skip") {
			if (command.args().size() != 1) continue;
			QStringList codeParts = command.args()[0].split('-');
			if (codeParts.size() != 3) continue;
			bool ok;
			int t = codeParts[0].toInt(&ok); if (!ok && codeParts[0] != "?") continue;
			int c1 = codeParts[1].toInt(&ok); if (!ok && codeParts[1] != "?") continue;
			int c2 = codeParts[2].toInt(&ok); if (!ok && codeParts[2] != "?") continue;
			if (t || c1 || c2) {
				statement.addJoin(QueryBuilder::Join(QueryBuilder::Column("kanjidic2.skip", "entry")));
				if (t) statement.addWhere(QString("kanjidic2.skip.type = %1").arg(t));
				if (c1) statement.addWhere(QString("kanjidic2.skip.c1 = %1").arg(c1));
				if (c2) statement.addWhere(QString("kanjidic2.skip.c2 = %1").arg(c2));
			}
		}
		else if (command.command() == "fourcorner") {
			if (command.args().size() != 1) continue;
			const QString &value = command.args()[0];
			// Sanity check
			if (value.size() != 6 || value[4] != '.') continue;
			
			int topLeft, topRight, botLeft, botRight, extra;
			topLeft = value[0].isDigit() ? value[0].toLatin1() - '0' : -1;
			topRight = value[1].isDigit() ? value[1].toLatin1() - '0' : -1;
			botLeft = value[2].isDigit() ? value[2].toLatin1() - '0' : -1;
			botRight = value[3].isDigit() ? value[3].toLatin1() - '0' : -1;
			extra = value[5].isDigit() ? value[5].toLatin1() - '0' : -1;
			if (topLeft != -1 || topRight != -1 || botLeft != -1 || botRight != -1 || extra != -1) {
				statement.addJoin(QueryBuilder::Join(QueryBuilder::Column("kanjidic2.fourCorner", "entry")));
				if (topLeft != -1) statement.addWhere(QString("kanjidic2.fourCorner.topLeft = %1").arg(topLeft));
				if (topRight != -1) statement.addWhere(QString("kanjidic2.fourCorner.topRight = %1").arg(topRight));
				if (botLeft != -1) statement.addWhere(QString("kanjidic2.fourCorner.botLeft = %1").arg(botLeft));
				if (botRight != -1) statement.addWhere(QString("kanjidic2.fourCorner.botRight = %1").arg(botRight));
				if (extra != -1) statement.addWhere(QString("kanjidic2.fourCorner.extra = %1").arg(extra));
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
	foreach (const QString &rword, romajiSearch) {
		QueryBuilder::Where where("OR");
		QString kword(TextTools::romajiToKana(rword));
		if (kword.isEmpty()) transSearch << rword;
		else {
			where.addWhere(buildTextSearchCondition(QStringList() << kword, "reading"));
			where.addWhere(buildTextSearchCondition(QStringList() << rword, "meaning"));
			statement.addWhere(where);
		}
	}
	if (!kanaSearch.isEmpty()) statement.addWhere(buildTextSearchCondition(kanaSearch, "reading"));
	if (!transSearch.isEmpty()) statement.addWhere(buildTextSearchCondition(transSearch, "meaning"));

	if (!radicalSearch.isEmpty()) {
		statement.addWhere(QString("kanjidic2.entries.id IN("
			"select r1.kanji from kanjidic2.radicals as r1 "
			"join kanjidic2.entries as e on r1.kanji = e.id "
			"where r1.number in ("
				"select distinct number from radicalsList "
				"where kanji in (%1)) "
			"and r1.type is not null "
			"group by r1.kanji "
			"having uniquecount(r1.number) >= %2)").arg(radicalSearch.join(", ")).arg(radicalSearch.size()));
	}
	
	if (!componentSearch.isEmpty()) {
		QString onlyDirectComponentsString("and ks1.isRoot = 1");
		QString qString;
		// TODO We should be able to control this - probably when we have a more powerful command system based on subclasses
		if (0) qString = onlyDirectComponentsString;
		statement.addWhere(QString("kanjidic2.entries.id IN ("
			"SELECT DISTINCT ks1.kanji FROM kanjidic2.strokeGroups AS ks1 "
			"WHERE (ks1.element IN (%1) OR ks1.original IN (%1)) %3 "
			"GROUP BY ks1.kanji "
			"HAVING UNIQUECOUNT("
				"CASE WHEN ks1.element IN (%1) "
				"THEN ks1.element "
				"ELSE NULL END, "
				"CASE WHEN ks1.original IN (%1) "
				"THEN ks1.original "
				"ELSE NULL END) >= %2"
			")").arg(componentSearch.join(", ")).arg(componentSearch.size()).arg(qString));
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
	else if (sort == "jlpt") return QueryBuilder::Column("kanjidic2.entries", "frequency");
	return res;
}
