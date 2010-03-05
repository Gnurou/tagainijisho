/*
 *  Copyright (C) 2010  Alexandre Courbot
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
#include "core/Database.h"
#include "core/TextTools.h"
#include "sqlite/qsql_sqlite.h"
#include "core/kanjidic2/Kanjidic2Parser.h"
#include "core/kanjidic2/KanjiVGParser.h"
#include "core/kanjidic2/Kanjidic2Entry.h"

#include <QStringList>
#include <QByteArray>

#include <QtDebug>

static QMap<quint32, quint8> knownRadicals;

// All the SQL queries used to build the database
// Having them here will allow us to prepare them once and for all
// instead of doing it for every entry.
static QSqlQuery insertEntryQuery;
static QSqlQuery insertOrIgnoreEntryQuery;
static QSqlQuery insertReadingQuery;
static QSqlQuery insertReadingTextQuery;
static QSqlQuery insertMeaningQuery;
static QSqlQuery insertMeaningTextQuery;
static QSqlQuery insertNanoriQuery;
static QSqlQuery insertNanoriTextQuery;
static QSqlQuery insertSkipCodeQuery;
static QSqlQuery insertFourCornerQuery;
static QSqlQuery insertStrokeGroupQuery;
static QSqlQuery updateJLPTLevelsQuery;
static QSqlQuery updateStrokeCountQuery;
static QSqlQuery updatePathsString;
static QSqlQuery addRadicalQuery;
static QSqlQuery insertRadicalQuery;

#define BIND(query, val) query.addBindValue(val)
#define AUTO_BIND(query, val, nval) if (val == nval) BIND(query, QVariant::Int); else BIND(query, val)
#define EXEC(query) if (!query.exec()) { qDebug() << query.lastError().text(); return false; }

class Kanjidic2DBParser : public Kanjidic2Parser
{
public:
	Kanjidic2DBParser(const QStringList &languages) : Kanjidic2Parser(languages) {}
	virtual bool onItemParsed(Kanjidic2Item &kanji);
};

class KanjiVGDBParser : public KanjiVGParser
{
public:
	virtual bool onItemParsed(KanjiVGItem &kanji);
};

bool Kanjidic2DBParser::onItemParsed(Kanjidic2Item &kanji)
{
	// Entries table
	AUTO_BIND(insertEntryQuery, kanji.id, 0);
	AUTO_BIND(insertEntryQuery, kanji.grade, 0);
	AUTO_BIND(insertEntryQuery, kanji.stroke_count, 0);
	AUTO_BIND(insertEntryQuery, kanji.freq, 0);
	AUTO_BIND(insertEntryQuery, kanji.jlpt, 0);
	EXEC(insertEntryQuery);

	// Readings
	foreach (const QString &readingType, kanji.readings.keys()) {
		foreach (const QString &reading, kanji.readings[readingType]) {
			// TODO factorize identical readings! Record the row id into a hash table
			BIND(insertReadingTextQuery, reading);
			EXEC(insertReadingTextQuery);
			BIND(insertReadingQuery, insertReadingTextQuery.lastInsertId());
			BIND(insertReadingQuery, kanji.id);
			// TODO reading type should be a tinyInt, not a string!
			BIND(insertReadingQuery, readingType);
			EXEC(insertReadingQuery);
		}
	}
	
	// Meanings - output the first language that we can satisfy
	foreach (const QString &lang, languages) if (kanji.meanings.contains(lang)) {
		foreach (const QString &meaning, kanji.meanings[lang]) {
			// TODO factorize identical meanings! Record the row id into a hash table
			BIND(insertMeaningTextQuery, meaning);
			EXEC(insertMeaningTextQuery);
			BIND(insertMeaningQuery, insertMeaningTextQuery.lastInsertId());
			BIND(insertMeaningQuery, kanji.id);
			BIND(insertMeaningQuery, lang);
			EXEC(insertMeaningQuery);
		}
		break;
	}
	
	// Nanori
	foreach (const QString &n, kanji.nanori) {
		// TODO factorize identical nanoris! Record the row id into a hash table
		BIND(insertNanoriTextQuery, n);
		EXEC(insertNanoriTextQuery);
		BIND(insertNanoriQuery, insertNanoriTextQuery.lastInsertId());
		BIND(insertNanoriQuery, kanji.id);
		EXEC(insertNanoriQuery);
	}
	
	// Skip code
	if (!kanji.skip.isEmpty()) {
		QStringList code(kanji.skip.split('-'));
		if (code.size() == 3) {
			BIND(insertSkipCodeQuery, kanji.id);
			BIND(insertSkipCodeQuery, code[0].toInt());
			BIND(insertSkipCodeQuery, code[1].toInt());
			BIND(insertSkipCodeQuery, code[2].toInt());
			EXEC(insertSkipCodeQuery);
		}
	}
	
	// Four corner
	if (!kanji.fourCorner.isEmpty()) {
		quint8 topLeft(kanji.fourCorner[0].toAscii() - '0');
		quint8 topRight(kanji.fourCorner[1].toAscii() - '0');
		quint8 botLeft(kanji.fourCorner[2].toAscii() - '0');
		quint8 botRight(kanji.fourCorner[3].toAscii() - '0');
		quint8 extra(kanji.fourCorner[5].toAscii() - '0');
		
		BIND(insertFourCornerQuery, kanji.id);
		BIND(insertFourCornerQuery, topLeft);
		BIND(insertFourCornerQuery, topRight);
		BIND(insertFourCornerQuery, botLeft);
		BIND(insertFourCornerQuery, botRight);
		BIND(insertFourCornerQuery, extra);
		EXEC(insertFourCornerQuery);
	}
	return true;
}

bool KanjiVGDBParser::onItemParsed(KanjiVGItem &kanji)
{
	// First ensure the kanji is into the DB by attempting to
	// insert a dummy entry
	AUTO_BIND(insertOrIgnoreEntryQuery, kanji.id, 0);
	BIND(insertOrIgnoreEntryQuery, 0);
	BIND(insertOrIgnoreEntryQuery, 0);
	BIND(insertOrIgnoreEntryQuery, 0);
	BIND(insertOrIgnoreEntryQuery, 0);
	EXEC(insertOrIgnoreEntryQuery);

	// Insert groups
	//bool skipFirst = false;
	foreach (const KanjiVGGroupItem &group, kanji.groups) {
		// The first group is only used for radical information - we don't need it here
		//if (!skipFirst) { skipFirst = true; continue; }
		BIND(insertStrokeGroupQuery, kanji.id);
		AUTO_BIND(insertStrokeGroupQuery, group.element, 0);
		AUTO_BIND(insertStrokeGroupQuery, group.original, 0);
		BIND(insertStrokeGroupQuery, (quint8)group.isRoot);
		QByteArray pathsIndexes;
		foreach (quint8 index, group.pathsIndexes) pathsIndexes.append(index);
		BIND(insertStrokeGroupQuery, pathsIndexes);
		EXEC(insertStrokeGroupQuery);
	}
	
	// Insert strokes
	QStringList paths;
	foreach (const KanjiVGStrokeItem &stroke, kanji.strokes) {
		paths << stroke.path;
	}
	QByteArray compressedPaths(paths.join("|").toAscii());
	if (!paths.isEmpty()) {
		BIND(updatePathsString, paths.size());
		BIND(updatePathsString, qCompress(compressedPaths, 9));
		BIND(updatePathsString, kanji.id);
		EXEC(updatePathsString);
	}
	// Insert radicals
	//foreach (const KanjiVGGroupItem &group, kanji.groups) if (group.radicalType != KanjiVGGroupItem::NONE) {
	foreach (const KanjiVGGroupItem &group, kanji.groups) if (knownRadicals.contains(group.element) || knownRadicals.contains(group.original)) {
		if (knownRadicals.contains(group.element)) BIND(insertRadicalQuery, knownRadicals[group.element]);
		else if (group.original && knownRadicals.contains(group.original)) BIND(insertRadicalQuery, knownRadicals[group.original]);
		else {
			qDebug("Radical (%s,%s) for kanji %s not in the radicals list", 
			       TextTools::unicodeToSingleChar(group.element).toUtf8().constData(),
			       TextTools::unicodeToSingleChar(group.original).toUtf8().constData(),
			       TextTools::unicodeToSingleChar(kanji.id).toUtf8().constData());
			continue;
		}
		BIND(insertRadicalQuery, kanji.id);
		BIND(insertRadicalQuery, group.radicalType);
		EXEC(insertRadicalQuery);
	}
	return true;
}

bool updateJLPTLevels(const QString &fName, int level)
{
	QFile file(fName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) return false;
	char line[50];
	int lineSize;
	while ((lineSize = file.readLine(line, 50)) != -1) {
		if (lineSize == 0) continue;
		if (line[lineSize - 1] == '\n') line[lineSize - 1] = 0;
		BIND(updateJLPTLevelsQuery, level);
		BIND(updateJLPTLevelsQuery, TextTools::singleCharToUnicode(QString::fromUtf8(line)));
		EXEC(updateJLPTLevelsQuery);
	}
	return true;
}

bool createRadicalsTable(const QString &fName)
{
	QFile file(fName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) return false;
	char line[50];
	int lineSize;
	int cpt = 0;
	while ((lineSize = file.readLine(line, 50)) != -1) {
		if (lineSize == 0) continue;
		++cpt;
		if (line[lineSize - 1] == '\n') line[lineSize - 1] = 0;
		QString kChr(QString::fromUtf8(line));
		for (int pos = 0; pos < kChr.size(); ) {
			int code = TextTools::singleCharToUnicode(kChr, pos);
			knownRadicals[code] = cpt;
			BIND(addRadicalQuery, code);
			BIND(addRadicalQuery, cpt);
			EXEC(addRadicalQuery);
			pos += TextTools::unicodeToSingleChar(code).size();
		}
	}
	return true;
}

static void create_tables()
{
	QSqlQuery query;
	query.exec("create table info(version INT, kanjidic2Version TEXT, kanjiVGVersion TEXT)");
	query.exec("create table entries(id INTEGER PRIMARY KEY, grade TINYINT, strokeCount TINYINT, frequency SMALLINT, jlpt TINYINT, paths BLOB)");
	query.exec("create table reading(docid INTEGER PRIMARY KEY, entry INTEGER SECONDARY KEY REFERENCES entries, type TEXT)");
	query.exec("create virtual table readingText using fts3(reading, TOKENIZE katakana)");
	query.exec("create table meaning(docid INTEGER PRIMARY KEY, entry INTEGER SECONDARY KEY REFERENCES entries, lang TEXT)");
	query.exec("create virtual table meaningText using fts3(reading)");
	query.exec("create table nanori(docid INTEGER PRIMARY KEY, entry INTEGER SECONDARY KEY REFERENCES entries)");
	query.exec("create virtual table nanoriText using fts3(reading, TOKENIZE katakana)");
	query.exec("create table strokeGroups(kanji INTEGER, element INTEGER, original INTEGER, isRoot BOOLEAN, pathsRefs BLOB)");
	query.exec("create table skip(entry INTEGER, type TINYINT, c1 TINYINT, c2 TINYINT)");
	query.exec("create table fourCorner(entry INTEGER, topLeft TINYINT, topRight TINYINT, botLeft TINYINT, botRight TINYINT, extra TINYINT)");
	query.exec("create table radicalsList(kanji INTEGER REFERENCES entries, number SHORTINT)");
	query.exec("create table radicals(number INTEGER REFERENCES radicalsList, kanji INTEGER REFERENCES entries, type TINYINT)");
}

static void create_indexes()
{
	QSqlQuery query;
	query.exec("create index idx_entries_frequency on entries(frequency)");
	query.exec("create index idx_jlpt on entries(jlpt)");
	query.exec("create index idx_reading_entry on reading(entry)");
	query.exec("create index idx_meaning_entry on meaning(entry)");
	query.exec("create index idx_nanori_entry on nanori(entry)");
	query.exec("create index idx_strokeGroups_kanji on strokeGroups(kanji)");
	query.exec("create index idx_strokeGroups_element on strokeGroups(element)");
	query.exec("create index idx_strokeGroups_original on strokeGroups(original)");
	query.exec("create index idx_skip on skip(entry)");
	query.exec("create index idx_skip_type on skip(type, c1, c2)");
	query.exec("create index idx_fourCorner on fourCorner(entry)");
	query.exec("create index idx_Radicals on radicals(kanji)");
}

void printUsage(char *argv[])
{
	qCritical("Usage: %s [-l<lang>] source_dir dest_file\nWhere <lang> is a two-letters language code (en, fr, de, es or ru)", argv[0]);
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	
	if (argc < 3) { printUsage(argv); return 1; }

	int argCpt = 1;
	QStringList languages;
	while (argCpt < argc && argv[argCpt][0] == '-') {
		QString param(argv[argCpt]);
		if (!param.startsWith("-l")) { printUsage(argv); return 1; }
		QStringList langs(param.mid(2).split(',', QString::SkipEmptyParts));
		foreach (const QString &lang, langs) if (!languages.contains(lang)) languages << lang;
		++argCpt;
	}
	if (argCpt > argc - 2) { printUsage(argv); return -1; }

	QString srcDir(argv[argCpt]);
	QString dstFile(argv[argCpt + 1]);
	
	QFile dst(dstFile);
	if (dst.exists() && !dst.remove()) {
		qCritical("Error - cannot remove existing destination file!");
		return 1;
	}
	
	// Open the database to write to
	QSQLiteDriver *driver = new QSQLiteDriver();
	QSqlDatabase database(QSqlDatabase::addDatabase(driver));
	database.setDatabaseName(dstFile);
	if (!database.open()) {
		qFatal("Cannot open database: %s", database.lastError().text().toLatin1().data());
		return 1;
	}
	// Attach custom functions and tokenizers
	QVariant handler = database.driver()->handle();
	if (handler.isValid() && !qstrcmp(handler.typeName(), "sqlite3*")) {
		sqlite3 *sqliteHandler = *static_cast<sqlite3 **>(handler.data());
		// TODO Move into dedicated open function? Since it cannot use
		// the sqlite3_auto_extension
		register_all_tokenizers(sqliteHandler);
	}
	database.exec("pragma encoding = \"UTF-16le\"");
	if (database.lastError().isValid()) {
		qDebug() << database.lastError().text();
		return 1;
	}
	database.transaction();
	create_tables();

	// Prepare the queries
	#define PREPQUERY(query, text) query = QSqlQuery(database); query.prepare(text)
	PREPQUERY(insertEntryQuery, "insert into entries values(?, ?, ?, ?, ?, null)");
	PREPQUERY(insertOrIgnoreEntryQuery, "insert or ignore into entries values(?, ?, ?, ?, ?, null)");
	PREPQUERY(insertReadingQuery, "insert into reading values(?, ?, ?)");
	PREPQUERY(insertReadingTextQuery, "insert into readingText values(?)");
	PREPQUERY(insertMeaningQuery, "insert into meaning values(?, ?, ?)");
	PREPQUERY(insertMeaningTextQuery, "insert into meaningText values(?)");
	PREPQUERY(insertNanoriQuery, "insert into nanori values(?, ?)");
	PREPQUERY(insertNanoriTextQuery, "insert into nanoriText values(?)");
	PREPQUERY(insertSkipCodeQuery, "insert into skip values(?, ?, ?, ?)");
	PREPQUERY(insertFourCornerQuery, "insert into fourCorner values(?, ?, ?, ?, ?, ?)");
	PREPQUERY(insertStrokeGroupQuery, "insert into strokeGroups values(?, ?, ?, ?, ?)");
	PREPQUERY(updateJLPTLevelsQuery, "update entries set jlpt = ? where id = ?");
	PREPQUERY(updateStrokeCountQuery, "update entries set strokeCount = ? where id = ?");
	PREPQUERY(updatePathsString, "update entries set strokeCount = ?, paths = ? where id = ?");
	PREPQUERY(addRadicalQuery, "insert into radicalsList values(?, ?)");
	PREPQUERY(insertRadicalQuery, "insert into radicals values(?, ?, ?)");
	#undef PREPQUERY

	// Parse and insert kanjidic2
	QFile file(QDir(srcDir).absoluteFilePath("3rdparty/kanjidic2.xml"));
	if (!file.open(QFile::ReadOnly | QFile::Text)) return 1;
	QXmlStreamReader reader(&file);
	// English is always used as a backup
	if (!languages.contains("en")) languages << "en";
	Kanjidic2DBParser kdicParser(languages);
	if (!kdicParser.parse(reader)) {
		qDebug() << database.lastError().text();
		return 1;
	}
	file.close();
	
	// Create radicals table
	createRadicalsTable(QDir(srcDir).absoluteFilePath("src/core/kanjidic2/radicals.txt"));

	// Parse and insert KanjiVG data
	file.setFileName(QDir(srcDir).absoluteFilePath("3rdparty/kanjivg.xml"));
	if (!file.open(QFile::ReadOnly | QFile::Text)) return 1;
	reader.setDevice(&file);
	KanjiVGDBParser kvgParser;
	if (!kvgParser.parse(reader)) {
		qDebug() << database.lastError().text();
		return 1;
	}
	file.close();

	// Fill in the info table
	{
		QSqlQuery query;
		query.prepare("insert into info values(?, ?, ?)");
		query.addBindValue(KANJIDIC2DB_REVISION);
		query.addBindValue(kdicParser.dateOfCreation());
		query.addBindValue(kvgParser.version());
		query.exec();
	}
	
	// Insert JLPT levels
	updateJLPTLevels(QDir(srcDir).absoluteFilePath("src/core/kanjidic2/jlpt-level1.txt"), 1);
	updateJLPTLevels(QDir(srcDir).absoluteFilePath("src/core/kanjidic2/jlpt-level2.txt"), 2);
	updateJLPTLevels(QDir(srcDir).absoluteFilePath("src/core/kanjidic2/jlpt-level3.txt"), 3);
	updateJLPTLevels(QDir(srcDir).absoluteFilePath("src/core/kanjidic2/jlpt-level4.txt"), 4);
	
	// Create indexes
	create_indexes();
	
	// Analyze for hopefully better performance
	database.exec("analyze");
	
	// Commit everything
	database.commit();
	
	// Close the database and set the file to read-only
	database = QSqlDatabase();
	QFile(dstFile).setPermissions(QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther);
	
	return 0;
}