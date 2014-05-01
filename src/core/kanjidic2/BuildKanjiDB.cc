/*
 * Copyright (C) 2010 Alexandre Courbot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sqlite/Connection.h"
#include "sqlite/Query.h"
#include "core/Database.h"
#include "core/TextTools.h"
#include "core/kanjidic2/Kanjidic2Parser.h"
#include "core/kanjidic2/KanjiVGParser.h"
#include "core/kanjidic2/Kanjidic2Entry.h"

#include <QStringList>
#include <QByteArray>

#include <QtDebug>

#define BIND(query, val) { if (!query.bindValue(val)) { qCritical("%s", query.lastError().message().toUtf8().data()); return false; } }
#define BINDNULL(query) { if (!query.bindNullValue()) { qCritical("%s", query.lastError().message().toUtf8().data()); return false; } }
#define AUTO_BIND(query, val, nval) { if (val == nval) BINDNULL(query) else BIND(query, val) }
#define EXEC(query) { if (!query.exec()) { qCritical("%s", query.lastError().message().toUtf8().data()); return false; } }
#define EXEC_STMT(query, stmt)  { if (!query.exec(stmt)) { qCritical("%s", query.lastError().message().toUtf8().data()); return false; } }
#define ASSERT(cond) { if (!cond) { qCritical("%s: assert condition failed, line %d", __FILE__, __LINE__); return false; } }

QMap<quint32, quint8> knownRadicals;
QSet<QPair<uint, quint8> > insertedRadicals;
QString srcDir, dstDir;

SQLite::Query insertOrIgnoreEntryQuery;
SQLite::Query insertRadicalQuery;

SQLite::Query addRadicalQuery;
SQLite::Query insertRootComponentQuery;

SQLite::Query insertEntryQuery;
SQLite::Query insertReadingQuery;
SQLite::Query insertReadingTextQuery;
SQLite::Query insertNanoriQuery;
SQLite::Query insertNanoriTextQuery;
SQLite::Query insertSkipCodeQuery;
SQLite::Query insertFourCornerQuery;
SQLite::Query updateJLPTLevelsQuery;
SQLite::Query updateStrokeCountQuery;

SQLite::Query insertStrokeGroupQuery;
SQLite::Query updatePathsString;

QMap<QString, SQLite::Query> insertMeaningQueries;
QMap<QString, SQLite::Query> insertMeaningTextQueries;

class Kanjidic2DBParser : public Kanjidic2Parser
{
public:
	Kanjidic2DBParser(const QStringList &languages) : Kanjidic2Parser(languages) {}
	virtual bool onItemParsed(Kanjidic2Item &kanji);
	bool updateJLPTLevel(const QString &fName, int level);
	bool updateJLPTLevels();

private:
};

bool Kanjidic2DBParser::onItemParsed(Kanjidic2Item &kanji)
{
	// Entries table
	AUTO_BIND(insertEntryQuery, kanji.id, 0);
	AUTO_BIND(insertEntryQuery, kanji.grade, 0);
	AUTO_BIND(insertEntryQuery, kanji.stroke_count, 0);
	AUTO_BIND(insertEntryQuery, (kanji.freq == 0 ? 0 : 2502 - kanji.freq), 0);
	AUTO_BIND(insertEntryQuery, (kanji.jlpt >= 3 ? kanji.jlpt + 1 : kanji.jlpt), 0);
	AUTO_BIND(insertEntryQuery, kanji.heisig, 0);
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
	foreach (const QString &lang, kanji.meanings.keys()) {
		if (languages.contains(lang)) {
			foreach (const QString &meaning, kanji.meanings[lang]) {
				SQLite::Query &mQuery = insertMeaningQueries[lang];
				SQLite::Query &mtQuery = insertMeaningTextQueries[lang];
				// TODO factorize identical meanings! Record the row id into a hash table
				BIND(mtQuery, meaning);
				EXEC(mtQuery);
				BIND(mQuery, mtQuery.lastInsertId());
				BIND(mQuery, kanji.id);
				BIND(mQuery, qCompress(meaning.toUtf8(), 9));
				EXEC(mQuery);
			}
		}
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
		quint8 topLeft(kanji.fourCorner[0].toLatin1() - '0');
		quint8 topRight(kanji.fourCorner[1].toLatin1() - '0');
		quint8 botLeft(kanji.fourCorner[2].toLatin1() - '0');
		quint8 botRight(kanji.fourCorner[3].toLatin1() - '0');
		quint8 extra(kanji.fourCorner[5].toLatin1() - '0');
		
		BIND(insertFourCornerQuery, kanji.id);
		BIND(insertFourCornerQuery, topLeft);
		BIND(insertFourCornerQuery, topRight);
		BIND(insertFourCornerQuery, botLeft);
		BIND(insertFourCornerQuery, botRight);
		BIND(insertFourCornerQuery, extra);
		EXEC(insertFourCornerQuery);
	}
	
	// Radicals
	typedef QPair<quint8, Kanjidic2Item::RadicalType> KRadType;
	foreach (const KRadType &rad, kanji.radicals) {
		BIND(insertRadicalQuery, rad.first);
		BIND(insertRadicalQuery, kanji.id);
		BIND(insertRadicalQuery, rad.second);
		EXEC(insertRadicalQuery);
		insertedRadicals << QPair<uint, quint8>(kanji.id, rad.first);
	}
	return true;
}

bool Kanjidic2DBParser::updateJLPTLevel(const QString &fName, int level)
{
	QFile file(fName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		return false;
	};
	QTextStream in(&file);
	QString line(in.readLine());
	while (!line.isNull()) {
		BIND(updateJLPTLevelsQuery, level);
		BIND(updateJLPTLevelsQuery, TextTools::singleCharToUnicode(line));
		EXEC(updateJLPTLevelsQuery);
		line = in.readLine();
	}
	return true;
}

bool Kanjidic2DBParser::updateJLPTLevels()
{
	ASSERT(updateJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/kanjidic2/jlpt-n1.csv"), 1));
	ASSERT(updateJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/kanjidic2/jlpt-n2.csv"), 2));
	ASSERT(updateJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/kanjidic2/jlpt-n3.csv"), 3));
	ASSERT(updateJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/kanjidic2/jlpt-n4.csv"), 4));
	ASSERT(updateJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/kanjidic2/jlpt-n5.csv"), 5));
	return true;
}

class KanjiVGDBParser : public KanjiVGParser
{
public:
	virtual bool onItemParsed(KanjiVGItem &kanji);
private:
};

bool KanjiVGDBParser::onItemParsed(KanjiVGItem &kanji)
{
	// First ensure the kanji is into the DB by attempting to
	// insert a dummy entry
	AUTO_BIND(insertOrIgnoreEntryQuery, kanji.id, 0);
	BINDNULL(insertOrIgnoreEntryQuery);
	BINDNULL(insertOrIgnoreEntryQuery);
	BINDNULL(insertOrIgnoreEntryQuery);
	BINDNULL(insertOrIgnoreEntryQuery);
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
	QByteArray compressedPaths(paths.join("|").toLatin1());
	if (!paths.isEmpty()) {
		BIND(updatePathsString, paths.size());
		BIND(updatePathsString, qCompress(compressedPaths, 9));
		BIND(updatePathsString, kanji.id);
		EXEC(updatePathsString);
	}
	// Insert radicals
	//foreach (const KanjiVGGroupItem &group, kanji.groups) if (group.radicalType != KanjiVGGroupItem::NONE) {
	foreach (const KanjiVGGroupItem &group, kanji.groups)
	{
		if (knownRadicals.contains(group.element) || knownRadicals.contains(group.original)) {
			quint8 radCode;
			if (knownRadicals.contains(group.element)) radCode = knownRadicals[group.element];
			else if (group.original && knownRadicals.contains(group.original)) {
				radCode = knownRadicals[group.original];
			} else {
				qDebug("Radical (%s,%s) for kanji %s not in the radicals list", 
					   TextTools::unicodeToSingleChar(group.element).toUtf8().constData(),
					   TextTools::unicodeToSingleChar(group.original).toUtf8().constData(),
					   TextTools::unicodeToSingleChar(kanji.id).toUtf8().constData());
				continue;
			}
			// Do not insert radicals already inserted from kanjidic2
			QPair<uint, quint8> rad(kanji.id, radCode);
			if (insertedRadicals.contains(rad)) continue;
			BIND(insertRadicalQuery, radCode);
			BIND(insertRadicalQuery, kanji.id);
			if (group.radicalType != 0) {
				BIND(insertRadicalQuery, group.radicalType);
			} else {
				BINDNULL(insertRadicalQuery);
			}
			EXEC(insertRadicalQuery);
			insertedRadicals << rad;
		}
	}
	return true;
}

class KanjiDB {
public:
	KanjiDB(const QStringList &lngs, QString sourceDirectory, QString destinationDirectory) {
		languages = lngs;
		srcDir = sourceDirectory;
		dstDir = destinationDirectory;
		kdicParser = new Kanjidic2DBParser(languages);
	}
	bool prepareQueries();
	bool clearQueries();
	bool createRadicalsTable(const QString &fName);
	bool createRootComponentsTable();
	bool createTables();
	bool createIndexes();
	bool finalize();
	bool openDatabase(QString databaseName, QString handle);
	bool closeDatabase(QString handle);
	bool parse();
	bool fillMainInfoTable();
	bool fillLanguagesInfoTable();

	bool updateTranslations(const QStringList &supportedLanguages);
	bool updateTranslation(const QString &fName, const QString &lang);

private:
	QStringList languages;
	KanjiVGDBParser kvgParser;
	Kanjidic2DBParser* kdicParser;
	QMap<QString, SQLite::Connection> connections;
};

bool KanjiDB::prepareQueries()
{
#define PREPQUERY(query, text) query.useWith(&connections["main"]); query.prepare(text)
	PREPQUERY(insertRadicalQuery, "insert into radicals values(?, ?, ?)");
	PREPQUERY(insertOrIgnoreEntryQuery, "insert or ignore into entries values(?, ?, ?, ?, ?, ?, null)");
	PREPQUERY(addRadicalQuery, "insert into radicalsList values(?, ?)");
	PREPQUERY(insertRootComponentQuery, "insert into rootComponents values(?)");
	
	PREPQUERY(insertEntryQuery, "insert into entries values(?, ?, ?, ?, ?, ?, null)");
	PREPQUERY(insertReadingQuery, "insert into reading values(?, ?, ?)");
	PREPQUERY(insertReadingTextQuery, "insert into readingText values(?)");
	PREPQUERY(insertNanoriQuery, "insert into nanori values(?, ?)");
	PREPQUERY(insertNanoriTextQuery, "insert into nanoriText values(?)");
	PREPQUERY(insertSkipCodeQuery, "insert into skip values(?, ?, ?, ?)");
	PREPQUERY(insertFourCornerQuery, "insert into fourCorner values(?, ?, ?, ?, ?, ?)");
	PREPQUERY(updateJLPTLevelsQuery, "update entries set jlpt = ? where id = ?");
	PREPQUERY(updateStrokeCountQuery, "update entries set strokeCount = ? where id = ?");
	
	PREPQUERY(insertStrokeGroupQuery, "insert into strokeGroups values(?, ?, ?, ?, ?)");
	PREPQUERY(updatePathsString, "update entries set strokeCount = ?, paths = ? where id = ?");
	
#undef PREPQUERY

	foreach (const QString &lang, languages) {
#define PREPQUERY(query, text) query.useWith(&connections[lang]); query.prepare(text)
		PREPQUERY(insertMeaningQueries[lang], "insert into meaning values(?, ?, ?)");
		PREPQUERY(insertMeaningTextQueries[lang], "insert into meaningText values(?)");
#undef PREPQUERY
	}
	return true;
}

bool KanjiDB::clearQueries()
{
	insertRadicalQuery.clear();
	insertOrIgnoreEntryQuery.clear();
	
	addRadicalQuery.clear();
	insertRootComponentQuery.clear();
	
	insertEntryQuery.clear();
	insertReadingQuery.clear();
	insertReadingTextQuery.clear();
	insertNanoriQuery.clear();
	insertNanoriTextQuery.clear();
	insertSkipCodeQuery.clear();
	insertFourCornerQuery.clear();
	updateJLPTLevelsQuery.clear();
	updateStrokeCountQuery.clear();
	
	insertStrokeGroupQuery.clear();
	updatePathsString.clear();
	
	foreach (const QString &lang, languages) {
		insertMeaningQueries[lang].clear();
		insertMeaningTextQueries[lang].clear();
	}

	return true;
}

bool KanjiDB::openDatabase(QString databaseName, QString handle)
{	
	QString dbFile = QDir(dstDir).absoluteFilePath(databaseName);
	QFile dst(dbFile);
	SQLite::Connection &connection = connections[handle];
	if (dst.exists() && !dst.remove()) {
		qCritical("Error - cannot remove existing destination file!");
		return false;
	}
	if (!connection.connect(dbFile, SQLite::Connection::JournalInFile)) {
		qCritical("Cannot open database: %s", connection.lastError().message().toLatin1().data());
		return false;
	}
	connection.transaction();
	return true;	
}

bool KanjiDB::closeDatabase(QString handle)
{	
	SQLite::Connection &connection = connections[handle];
	connection.exec("analyze");
	ASSERT(connection.commit());
	ASSERT(connection.exec("VACUUM"));
	QFile(connection.dbFileName()).setPermissions(QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther);
	ASSERT(connection.close());
	return true;
}

bool KanjiDB::createRootComponentsTable()
{
	SQLite::Query query(&connections["main"]);
	if (!query.exec("select distinct ks.element from strokeGroups as ks join entries as e on ks.element = e.id where ks.element not in (select distinct kanji from strokeGroups where element != kanji) "
	        // We are not counting components that are only components of themselves (whatever that means)
	        "and ks.kanji != ks.element "
	        "order by strokeCount")) {
		return false;
	}
	while (query.next()) {
		BIND(insertRootComponentQuery, query.valueUInt(0));
		EXEC(insertRootComponentQuery);
	}
	return true;
}

bool KanjiDB::createRadicalsTable(const QString &fName)
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

bool KanjiDB::createTables()
{
	SQLite::Query query(&connections["main"]);
	EXEC_STMT(query, "create table info(version INT, kanjidic2Version TEXT, kanjiVGVersion TEXT)");
	EXEC_STMT(query, "create table entries(id INTEGER PRIMARY KEY, grade TINYINT, strokeCount TINYINT, frequency SMALLINT, jlpt TINYINT, heisig SMALLINT, paths BLOB)");
	EXEC_STMT(query, "create table reading(docid INTEGER PRIMARY KEY, entry INTEGER SECONDARY KEY REFERENCES entries, type TEXT)");
	EXEC_STMT(query, "create virtual table readingText using fts4(reading, TOKENIZE katakana)");
	EXEC_STMT(query, "create table nanori(docid INTEGER PRIMARY KEY, entry INTEGER SECONDARY KEY REFERENCES entries)");
	EXEC_STMT(query, "create virtual table nanoriText using fts4(reading, TOKENIZE katakana)");
	EXEC_STMT(query, "create table strokeGroups(kanji INTEGER, element INTEGER, original INTEGER, isRoot BOOLEAN, pathsRefs BLOB)");
	EXEC_STMT(query, "create table rootComponents(kanji INTEGER PRIMARY KEY)");
	EXEC_STMT(query, "create table skip(entry INTEGER, type TINYINT, c1 TINYINT, c2 TINYINT)");
	EXEC_STMT(query, "create table fourCorner(entry INTEGER, topLeft TINYINT, topRight TINYINT, botLeft TINYINT, botRight TINYINT, extra TINYINT)");
	EXEC_STMT(query, "create table radicalsList(kanji INTEGER REFERENCES entries, number SHORTINT)");
	EXEC_STMT(query, "create table radicals(number INTEGER REFERENCES radicalsList, kanji INTEGER REFERENCES entries, type TINYINT)");

	foreach (const QString &lang, languages) {
		query.useWith(&connections[lang]);
		EXEC_STMT(query, "create table info(version INT, kanjidic2Version TEXT, kanjiVGVersion TEXT)");
		EXEC_STMT(query, "create table meaning(docid INTEGER PRIMARY KEY, entry INTEGER SECONDARY KEY REFERENCES entries, meanings BLOB)");
		EXEC_STMT(query, "create virtual table meaningText using fts4(reading)");
	}

	return true;
}

bool KanjiDB::createIndexes()
{
	SQLite::Query query(&connections["main"]);
	EXEC_STMT(query, "create index idx_entries_frequency on entries(frequency)");
	EXEC_STMT(query, "create index idx_jlpt on entries(jlpt)");
	EXEC_STMT(query, "create index idx_reading_entry on reading(entry)");
	EXEC_STMT(query, "create index idx_nanori_entry on nanori(entry)");
	EXEC_STMT(query, "create index idx_strokeGroups_kanji on strokeGroups(kanji)");
	EXEC_STMT(query, "create index idx_strokeGroups_element on strokeGroups(element)");
	EXEC_STMT(query, "create index idx_strokeGroups_original on strokeGroups(original)");
	EXEC_STMT(query, "create index idx_skip on skip(entry)");
	EXEC_STMT(query, "create index idx_skip_type on skip(type, c1, c2)");
	EXEC_STMT(query, "create index idx_fourCorner on fourCorner(entry)");
	EXEC_STMT(query, "create index idx_radicalsList_number on radicalsList(number)");
	EXEC_STMT(query, "create index idx_radicals on radicals(kanji)");

	foreach (const QString &lang, languages) {
		query.useWith(&connections[lang]);
		EXEC_STMT(query, "create index idx_meaning_entry on meaning(entry)");
	}

	return true;
}

bool KanjiDB::finalize()
{
	SQLite::Query query;
	foreach (const QString &lang, languages) {
		query.useWith(&connections[lang]);
		EXEC_STMT(query, "DELETE FROM meaningText_content");
	}

	return true;
}

bool KanjiDB::fillMainInfoTable()
{
	SQLite::Query query(&connections["main"]);
	query.prepare("insert into info values(?, ?, ?)");
	query.bindValue(KANJIDIC2DB_REVISION);
	query.bindValue(kdicParser->dateOfCreation());
	query.bindValue(kvgParser.version());
	ASSERT(query.exec());
	return true;
}

bool KanjiDB::fillLanguagesInfoTable()
{
	SQLite::Query query;
	foreach (const QString &lang, languages) {
		query.useWith(&connections[lang]);
		query.prepare("insert into info values(?, ?, ?)");
		query.bindValue(KANJIDIC2DB_REVISION);
		query.bindValue(kdicParser->dateOfCreation());
		query.bindValue(kvgParser.version());
		ASSERT(query.exec());
	}
	return true;
}

bool KanjiDB::updateTranslations(const QStringList &supportedLanguages)
{
	QDir dir(QDir(srcDir).absoluteFilePath("src/core/kanjidic2"));

	foreach (QString fName, dir.entryList(QStringList() << "*.jmf")) {
		QString lang(fName.split('.')[0]);
		if (supportedLanguages.contains(lang))
			updateTranslation(dir.absoluteFilePath(fName), lang);
	}

	return true;
}

bool KanjiDB::updateTranslation(const QString &fName, const QString &lang)
{
	QFile file(fName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		return false;
	}

	SQLite::Query deleteMeaningQuery(&connections[lang]);
	SQLite::Query deleteMeaningTextQuery(&connections[lang]);
	deleteMeaningTextQuery.prepare("delete from meaningText where docid in (select docid from meaning where entry = ?)");
	deleteMeaningQuery.prepare("delete from meaning where entry = ?");
	SQLite::Query &mQuery = insertMeaningQueries[lang];
	SQLite::Query &mtQuery = insertMeaningTextQueries[lang];
	QTextStream in(&file);

	int lastKanji = 0;
	QString line(in.readLine());
	while (!line.isNull()) {
		int pos = line.indexOf(' ');
		int kanji(TextTools::singleCharToUnicode(line.left(pos)));
		QString meaning(line.mid(pos + 1));

		if (lastKanji != kanji) {
			BIND(deleteMeaningTextQuery, kanji);
			EXEC(deleteMeaningTextQuery);
			BIND(deleteMeaningQuery, kanji);
			EXEC(deleteMeaningQuery);
			lastKanji = kanji;
		}

		// TODO factorize identical meanings! Record the row id into a hash table
		BIND(mtQuery, meaning);
		EXEC(mtQuery);
		BIND(mQuery, mtQuery.lastInsertId());
		BIND(mQuery, kanji);
		BIND(mQuery, qCompress(meaning.toUtf8(), 9));
		EXEC(mQuery);

		line = in.readLine();
	}
	return true;
}

bool KanjiDB::parse()
{
	// Parse and insert kanjidic2
	QFile file(QDir(srcDir).absoluteFilePath("3rdparty/kanjidic2.xml"));
	ASSERT(file.open(QFile::ReadOnly | QFile::Text));
	QXmlStreamReader reader(&file);
	if (!kdicParser->parse(reader)) {
		qDebug() << "Error during kanjidic2 parsing:" << connections["main"].lastError().message();
		return 1;
	}
	file.close();
	
	ASSERT(createRadicalsTable(QDir(srcDir).absoluteFilePath("src/core/kanjidic2/radicals.txt")));
	
	// Parse and insert KanjiVG data
	file.setFileName(QDir(srcDir).absoluteFilePath("3rdparty/kanjivg.xml"));
	ASSERT(file.open(QFile::ReadOnly | QFile::Text));
	reader.setDevice(&file);
	if (!kvgParser.parse(reader)) {
		qDebug() << "Error during KanjiVG parsing" << connections["main"].lastError().message();
		return 1;
	}
	file.close();
	
	ASSERT(createRootComponentsTable());
	ASSERT(fillMainInfoTable());	
	ASSERT(fillLanguagesInfoTable());	
	ASSERT(kdicParser->updateJLPTLevels());
	return true;
}

void printUsage(char *argv[])
{
	qCritical("Usage: %s [-l<lang>] source_dir dest_file\nWhere <lang> is a two-letters language code (en, fr, de, es or ru)", argv[0]);
}

bool buildDB(const QStringList &languages, const QString &srcDir, const QString &dstDir)
{
	KanjiDB kanjiDB(languages, srcDir, dstDir);

	ASSERT(kanjiDB.openDatabase("kanjidic2.db", "main"));
	foreach (const QString &lang, languages) {
		ASSERT(kanjiDB.openDatabase(QString("kanjidic2-%1.db").arg(lang), lang));
	}
	ASSERT(kanjiDB.createTables());
	ASSERT(kanjiDB.prepareQueries());
	ASSERT(kanjiDB.parse());
	ASSERT(kanjiDB.updateTranslations(languages));
	ASSERT(kanjiDB.createIndexes());
	ASSERT(kanjiDB.clearQueries());
	ASSERT(kanjiDB.finalize());
	ASSERT(kanjiDB.closeDatabase("main"));
	foreach (const QString &lang, languages) {
		ASSERT(kanjiDB.closeDatabase(lang));
	}
	return true;
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	
	if (argc < 3) { 
		printUsage(argv);
		return 1;
	}
	QStringList languages;
	int argCpt = 1;
	while (argCpt < argc && argv[argCpt][0] == '-') {
		QString param(argv[argCpt]);
		if (!param.startsWith("-l")) {
			printUsage(argv);
			return 1;
		}
		QStringList langs(param.mid(2).split(',', QString::SkipEmptyParts));
		foreach (const QString &lang, langs) {
			languages << lang;
		};
		++argCpt;
	}
	if (argCpt > argc - 2) {
		printUsage(argv);
		return -1;
	}
	
	QString srcDir(argv[argCpt]);
	QString dstDir(argv[argCpt + 1]);
	
	// English is used as a backup if nothing else is available
	languages << "en";
	languages.removeDuplicates();
	
	return !buildDB(languages, srcDir, dstDir);
}
