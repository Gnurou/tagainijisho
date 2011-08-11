/*
 *	Copyright (C) 2010	Alexandre Courbot
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sqlite/Connection.h"
#include "sqlite/Query.h"
#include "core/Database.h"
#include "core/TextTools.h"
#include "core/jmdict/JMdictParser.h"
#include "core/jmdict/JMdictEntry.h"

#include <QStringList>
#include <QByteArray>

#include <QtDebug>

#define BIND(query, val) { if (!query.bindValue(val)) { qFatal(query.lastError().message().toUtf8().data(),"%s"); return false; } }
#define BINDNULL(query) { if (!query.bindNullValue()) { qFatal(query.lastError().message().toUtf8().data(),"%s"); return false; } }
#define AUTO_BIND(query, val, nval) if (val == nval) BINDNULL(query) else BIND(query, val)
#define EXEC(query) if (!query.exec()) { qFatal(query.lastError().message().toUtf8().data(),"%s"); return false; }
#define EXEC_STMT(query, stmt) if (!query.exec(stmt)) { qFatal(query.lastError().message().toUtf8().data(),"%s"); return false; }
#define ASSERT(cond) Q_ASSERT(cond)

class JMdictDBParser : public JMdictParser
{
public:
	JMdictDBParser(const QSet<QString> &languages, QString sourceDirectory, QString destinationDirectory) : JMdictParser(languages) {
		srcDir = sourceDirectory;
		dstDir = destinationDirectory;
	}
	virtual bool onItemParsed(const JMdictItem &entry);
	virtual bool onDeletedItemParsed(const JMdictDeletedItem &entry);
	bool createMainDatabase();
	bool createMainTables();
	bool createMainIndexes();
	bool finalizeMainDatabase();
	bool prepareMainQueries();
	bool clearMainQueries();
	bool fillMainInfoTable();
	bool createLanguagesDatabases();
	bool createLanguagesTables();
	bool createLanguagesIndexes();
	bool finalizeLanguagesDatabases();
	bool prepareLanguagesQueries();
	bool clearLanguagesQueries();
	bool fillLanguagesInfoTable();	
	bool insertJLPTLevel(const QString& fName, int level);
	bool insertJLPTLevels();
	bool populateEntitiesTable();
private:
	QMap<QString, SQLite::Connection> connections;
	QString dstDir, srcDir;
	SQLite::Query insertEntryQuery;
	SQLite::Query insertKanjiTextQuery;
	SQLite::Query insertKanjiQuery;
	SQLite::Query insertKanjiCharQuery;
	SQLite::Query insertKanaTextQuery;
	SQLite::Query insertKanaQuery;
	SQLite::Query insertSenseQuery;
	SQLite::Query insertJLPTQuery;
	SQLite::Query insertDeletedEntryQuery;
	QMap<QString, SQLite::Query> insertGlossTextQueries;
	QMap<QString, SQLite::Query> insertGlossQueries;
	
	bool openDatabase(QString databaseName, QString handle);
	bool closeDatabase(QString handle);
};

bool JMdictDBParser::onItemParsed(const JMdictItem &entry)
{
	// Insert writings
	int kanjiCount = 0;
	quint8 idx = 0;
	foreach (const JMdictKanjiWritingItem &kWriting, entry.kanji) {
		BIND(insertKanjiTextQuery, kWriting.writing);
		EXEC(insertKanjiTextQuery);
		qint64 rowId = insertKanjiTextQuery.lastInsertId();
		BIND(insertKanjiQuery, entry.id);
		BIND(insertKanjiQuery, idx);
		BIND(insertKanjiQuery, rowId);
		AUTO_BIND(insertKanjiQuery, kWriting.frequency, 0);
		EXEC(insertKanjiQuery);
		
		// Insert kanji mappings
		for (int i = 0; i < kWriting.writing.size(); ) {
			int code = TextTools::singleCharToUnicode(kWriting.writing, i);
			if (code == 0) { ++i; continue; }
			QString codeS(TextTools::unicodeToSingleChar(code));
			if (TextTools::isKanjiChar(codeS)) {
				BIND(insertKanjiCharQuery, code);
				BIND(insertKanjiCharQuery, entry.id);
				BIND(insertKanjiCharQuery, idx);
				EXEC(insertKanjiCharQuery);
				// Calculate the kanji count for the first reading
				if (idx == 0) ++kanjiCount;
			}
			i += codeS.size();
		}
		++idx;
	}
	
	// Insert readings
	idx = 0;
	foreach (const JMdictKanaReadingItem &kReading, entry.kana) {
		BIND(insertKanaTextQuery, kReading.reading);
		EXEC(insertKanaTextQuery);
		qint64 rowId = insertKanaTextQuery.lastInsertId();
		BIND(insertKanaQuery, entry.id);
		BIND(insertKanaQuery, idx);
		BIND(insertKanaQuery, rowId);
		BIND(insertKanaQuery, (quint8) kReading.noKanji);
		AUTO_BIND(insertKanaQuery, kReading.frequency, 0);
		QStringList restrictedToList;
		foreach (quint8 res, kReading.restrictedTo) restrictedToList << QString::number(res);
		AUTO_BIND(insertKanaQuery, restrictedToList.join(","), "");
		EXEC(insertKanaQuery);
		++idx;
	}
	
	// Insert senses and glosses
	idx = 0;
	foreach (const JMdictSenseItem &sense, entry.senses) {
		BIND(insertSenseQuery, entry.id);
		BIND(insertSenseQuery, idx);
		BIND(insertSenseQuery, sense.posBitField(*this));
		BIND(insertSenseQuery, sense.miscBitField(*this));
		BIND(insertSenseQuery, sense.dialectBitField(*this));
		BIND(insertSenseQuery, sense.fieldBitField(*this));
		QStringList restrictedToList;
		foreach (quint8 res, sense.restrictedToKanji) restrictedToList << QString::number(res);
		AUTO_BIND(insertSenseQuery, restrictedToList.join(","), "");
		restrictedToList.clear();
		foreach (quint8 res, sense.restrictedToKana) restrictedToList << QString::number(res);
		AUTO_BIND(insertSenseQuery, restrictedToList.join(","), "");
		EXEC(insertSenseQuery);
		
		// Insert the gloss for all languages
		foreach (const QString &lang, languages) {
			if (!sense.gloss.contains(lang)) continue;
			BIND(insertGlossTextQueries[lang], sense.gloss[lang].join(", "));
			EXEC(insertGlossTextQueries[lang]);
			qint64 rowId = insertGlossTextQueries[lang].lastInsertId();
			BIND(insertGlossQueries[lang], entry.id);
			BIND(insertGlossQueries[lang], idx);
			BIND(insertGlossQueries[lang], lang);
			BIND(insertGlossQueries[lang], rowId);
			EXEC(insertGlossQueries[lang]);
		}
		++idx;
	}
	
	// Insert entry
	BIND(insertEntryQuery, entry.id);
	AUTO_BIND(insertEntryQuery, entry.frequency, 0);
	BIND(insertEntryQuery, kanjiCount);
	EXEC(insertEntryQuery);
	return true;
}

bool JMdictDBParser::onDeletedItemParsed(const JMdictDeletedItem &entry)
{
	BIND(insertDeletedEntryQuery, entry.id);
	if (entry.replacedBy) {
		BIND(insertDeletedEntryQuery, entry.replacedBy);
	} else { 
		BINDNULL(insertDeletedEntryQuery);
	}
	EXEC(insertDeletedEntryQuery);
	return true;
}

bool JMdictDBParser::insertJLPTLevel(const QString &fName, int level)
{
	QFile file(fName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		return false;
	};
	QTextStream in(&file);
	QString line = in.readLine();
	while (!line.isNull()) {
		BIND(insertJLPTQuery, line.toInt());
		BIND(insertJLPTQuery, level);
		EXEC(insertJLPTQuery);
		line = in.readLine();
	}
	return true;
}

bool JMdictDBParser::insertJLPTLevels()
{
	ASSERT(insertJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/jmdict/jlpt-level4.txt"), 4));
	ASSERT(insertJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/jmdict/jlpt-level3.txt"), 3));
	ASSERT(insertJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/jmdict/jlpt-level2.txt"), 2));
	ASSERT(insertJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/jmdict/jlpt-level1.txt"), 1));
	return true;	
}

bool JMdictDBParser::openDatabase(QString databaseName, QString handle)
{	
	QString dbFile = QDir(dstDir).absoluteFilePath(QString(databaseName));
	QFile dst(dbFile);
	SQLite::Connection &connection = connections[handle];
	if (dst.exists() && !dst.remove()) {
		qCritical("Error - cannot remove existing destination file!");
		return false;
	}
	if (!connection.connect(dbFile, SQLite::Connection::JournalInFile)) {
		qFatal("Cannot open database: %s", connection.lastError().message().toLatin1().data());
		return false;
	}
	connection.transaction();
	return true;	
}

bool JMdictDBParser::closeDatabase(QString handle)
{	
	connections[handle].exec("analyze");
	ASSERT(connections[handle].commit());
	QFile(connections[handle].dbFileName()).setPermissions(QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther);
	ASSERT(connections[handle].close());
	return true;
}

bool JMdictDBParser::prepareMainQueries()
{
#define PREPQUERY(query, text) query.useWith(&connections["main"]); ASSERT(query.prepare(text))
	PREPQUERY(insertEntryQuery, "insert into entries values(?, ?, ?)");
	PREPQUERY(insertKanjiTextQuery, "insert into kanjiText values(?)");
	PREPQUERY(insertKanjiQuery, "insert into kanji values(?, ?, ?, ?)");
	PREPQUERY(insertKanjiCharQuery, "insert into kanjiChar values(?, ?, ?)");
	PREPQUERY(insertKanaTextQuery, "insert into kanaText values(?)");
	PREPQUERY(insertKanaQuery, "insert into kana values(?, ?, ?, ?, ?, ?)");
	PREPQUERY(insertSenseQuery, "insert into senses values(?, ?, ?, ?, ?, ?, ?, ?)");
	PREPQUERY(insertJLPTQuery, "insert or ignore into jlpt values(?, ?)");
	PREPQUERY(insertDeletedEntryQuery, "insert into deletedEntries values(?, ?)");
#undef PREPQUERY
	return true;
}

bool JMdictDBParser::clearMainQueries()
{
	insertEntryQuery.clear();
	insertKanjiTextQuery.clear();
	insertKanjiQuery.clear();
	insertKanjiCharQuery.clear();
	insertKanaTextQuery.clear();
	insertKanaQuery.clear();
	insertSenseQuery.clear();
	insertJLPTQuery.clear();
	insertDeletedEntryQuery.clear();
	return true;
}

bool JMdictDBParser::createMainDatabase() 
{
	return openDatabase("jmdict.db", "main");
}

bool JMdictDBParser::createMainTables()
{
	SQLite::Query query(&connections["main"]);
	EXEC_STMT(query, "create table info(version INT, JMdictVersion TEXT)");
	EXEC_STMT(query, "create table posEntities(bitShift INTEGER PRIMARY KEY, name TEXT, description TEXT)");
	EXEC_STMT(query, "create table miscEntities(bitShift INTEGER PRIMARY KEY, name TEXT, description TEXT)");
	EXEC_STMT(query, "create table fieldEntities(bitShift INTEGER PRIMARY KEY, name TEXT, description TEXT)");
	EXEC_STMT(query, "create table dialectEntities(bitShift INTEGER PRIMARY KEY, name TEXT, description TEXT)");
	EXEC_STMT(query, "create table entries(id INTEGER PRIMARY KEY, frequency SMALLINT, kanjiCount TINYINT)");
	EXEC_STMT(query, "create table kanji(id INTEGER SECONDARY KEY REFERENCES entries, priority TINYINT, docid INTEGER PRIMARY KEY, frequency TINYINT)");
	EXEC_STMT(query, "create virtual table kanjiText using fts4(reading)");
	EXEC_STMT(query, "create table kana(id INTEGER SECONDARY KEY REFERENCES entries, priority TINYINT, docid INTEGER PRIMARY KEY, nokanji BOOLEAN, frequency TINYINT, restrictedTo TEXT)");
	EXEC_STMT(query, "create virtual table kanaText using fts4(reading, TOKENIZE katakana)");
	EXEC_STMT(query, "create table senses(id INTEGER SECONDARY KEY REFERENCES entries, priority TINYINT, pos INT, misc INT, dial INT, field INT, restrictedToKanji TEXT, restrictedToKana TEXT)");
	EXEC_STMT(query, "create table kanjiChar(kanji INTEGER, id INTEGER SECONDARY KEY REFERENCES entries, priority INT)");
	EXEC_STMT(query, "create table jlpt(id INTEGER PRIMARY KEY, level TINYINT)");
	EXEC_STMT(query, "create table deletedEntries(id INTEGER PRIMARY KEY, movedTo INTEGER REFERENCES entries)");
	return true;
}

bool JMdictDBParser::createMainIndexes() 
{
	SQLite::Query query(&connections["main"]);
	EXEC_STMT(query, "create index idx_entries_frequency on entries(frequency)");
	EXEC_STMT(query, "create index idx_kanji on kanji(id)");
	EXEC_STMT(query, "create index idx_kana on kana(id)");
	EXEC_STMT(query, "create index idx_senses on senses(id)");
	EXEC_STMT(query, "create index idx_kanjichar on kanjiChar(kanji)");
	EXEC_STMT(query, "create index idx_kanjichar_id on kanjiChar(id)");
	EXEC_STMT(query, "create index idx_jlpt on jlpt(level)");
	return true;
}

bool JMdictDBParser::finalizeMainDatabase() 
{
	return closeDatabase("main");
}

bool JMdictDBParser::prepareLanguagesQueries()
{
	foreach (const QString &lang, languages) {
#define PREPQUERY(query, text) query.useWith(&connections[lang]); ASSERT(query.prepare(text))
		PREPQUERY(insertGlossTextQueries[lang], "insert into glossText values(?)");
		PREPQUERY(insertGlossQueries[lang], "insert into gloss values(?, ?, ?, ?)");
#undef PREPQUERY
	}
	return true;
}

bool JMdictDBParser::clearLanguagesQueries() 
{
	foreach (const QString &lang, languages) {
		insertGlossTextQueries[lang].clear();
		insertGlossQueries[lang].clear();
	}
	return true;
}

bool JMdictDBParser::createLanguagesDatabases()
{
	foreach (const QString &lang, languages) {
		bool created = openDatabase(QString("jmdict-%1.db").arg(lang),lang);
		if (!created) {
			return false;
		}
	}	
	return true;
}

bool JMdictDBParser::createLanguagesTables()
{
	foreach (const QString &lang, languages) {
		SQLite::Query query(&connections[lang]);
		EXEC_STMT(query, "create table info(version INT, JMdictVersion TEXT)");
		EXEC_STMT(query, "create table gloss(id INTEGER SECONDARY KEY REFERENCES entries, sensePriority INT, lang CHAR(3), docid INTEGER PRIMARY KEY)");
		EXEC_STMT(query, "create virtual table glossText using fts4(reading)");
	}	
	return true;
}

bool JMdictDBParser::createLanguagesIndexes()
{
	foreach (const QString &lang, languages) {
		SQLite::Query query(&connections[lang]);
		EXEC_STMT(query, "create index idx_gloss on gloss(id)");
	}
	return true;
}

bool JMdictDBParser::finalizeLanguagesDatabases()
{
	foreach (const QString &lang, languages) {
		bool closed = closeDatabase(lang);
		if(!closed) {
			return false;
		}
	}
	return true;
}

bool JMdictDBParser::fillMainInfoTable()
{
	SQLite::Query query(&connections["main"]);
	query.prepare("insert into info values(?, ?)");
	query.bindValue(JMDICTDB_REVISION);
	query.bindValue(dictVersion());
	ASSERT(query.exec());
	return true;
}

bool JMdictDBParser::fillLanguagesInfoTable()
{
	SQLite::Query query;
	foreach (const QString &lang, languages) {
		query.useWith(&connections[lang]);
		query.prepare("insert into info values(?, ?)");
		query.bindValue(JMDICTDB_REVISION);
		query.bindValue(dictVersion());
		ASSERT(query.exec());
	}
	return true;
}

bool JMdictDBParser::populateEntitiesTable()
{	
	SQLite::Query entitiesQuery(&connections["main"]);
	entitiesQuery.prepare("insert into posEntities values(?, ?, ?)");
	foreach (const QString &name, posBitFields.keys()) {
		entitiesQuery.bindValue(posBitFields[name]);
		entitiesQuery.bindValue(name);
		entitiesQuery.bindValue(entities[name]);
		ASSERT(entitiesQuery.exec());
	}
	entitiesQuery.prepare("insert into miscEntities values(?, ?, ?)");
	foreach (const QString &name, miscBitFields.keys()) {
		entitiesQuery.bindValue(miscBitFields[name]);
		entitiesQuery.bindValue(name);
		entitiesQuery.bindValue(entities[name]);
		ASSERT(entitiesQuery.exec());
	}
	entitiesQuery.prepare("insert into fieldEntities values(?, ?, ?)");
	foreach (const QString &name, fieldBitFields.keys()) {
		entitiesQuery.bindValue(fieldBitFields[name]);
		entitiesQuery.bindValue(name);
		entitiesQuery.bindValue(entities[name]);
		ASSERT(entitiesQuery.exec());
	}
	entitiesQuery.prepare("insert into dialectEntities values(?, ?, ?)");
	foreach (const QString &name, dialectBitFields.keys()) {
		entitiesQuery.bindValue(dialectBitFields[name]);
		entitiesQuery.bindValue(name);
		entitiesQuery.bindValue(entities[name]);
		ASSERT(entitiesQuery.exec());
	}
	return true;
}

void printUsage(char *argv[])
{
	qCritical("Usage: %s [-l<lang>] source_dir dest_dir\nWhere <lang> is a two-letters language code (en, fr, de, es or ru)", argv[0]);
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	
	if (argc < 3) { 
		printUsage(argv); return 1;
	}
	QSet<QString> languages;
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
	
	// Parse and insert JMdict
	JMdictDBParser parser(languages, srcDir, dstDir);

	parser.createMainDatabase();
	parser.createMainTables();
	parser.createMainIndexes(); 
	parser.prepareMainQueries();
	parser.populateEntitiesTable();
	
	parser.createLanguagesDatabases();
	parser.createLanguagesTables();
	parser.createLanguagesIndexes();	
	parser.prepareLanguagesQueries();
	
	QFile file(QDir(srcDir).absoluteFilePath("3rdparty/JMdict"));
	ASSERT(file.open(QFile::ReadOnly | QFile::Text));
	QXmlStreamReader reader(&file);
	// English is used as a backup if nothing else is available
	if (!languages.contains("en")) {
		languages << "en";
	};
	if (!parser.parse(reader)) {
		return 1;
	}
	file.close();	
	
	parser.fillMainInfoTable();
	parser.fillLanguagesInfoTable();
	parser.insertJLPTLevels();

	parser.clearMainQueries();
	parser.finalizeMainDatabase();

	parser.clearLanguagesQueries();
	parser.finalizeLanguagesDatabases();	
		
	return 0;
}
