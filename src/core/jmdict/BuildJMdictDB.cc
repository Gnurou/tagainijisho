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
#include "core/jmdict/JMdictParser.h"
#include "core/jmdict/JMdictEntry.h"

#include <QStringList>
#include <QByteArray>

#include <QtDebug>

// All the SQL queries used to build the database
// Having them here will allow us to prepare them once and for all
// instead of doing it for every entry.
static QSqlQuery insertEntryQuery;
static QSqlQuery insertKanjiTextQuery;
static QSqlQuery insertKanjiQuery;
static QSqlQuery insertKanjiCharQuery;
static QSqlQuery insertKanaTextQuery;
static QSqlQuery insertKanaQuery;
static QSqlQuery insertSenseQuery;
static QSqlQuery insertGlossTextQuery;
static QSqlQuery insertGlossQuery;
static QSqlQuery insertJLPTQuery;
static QSqlQuery insertDeletedEntryQuery;

#define BIND(query, val) query.addBindValue(val)
#define AUTO_BIND(query, val, nval) if (val == nval) BIND(query, QVariant::Int); else BIND(query, val)
#define EXEC(query) if (!query.exec()) { qDebug() << query.lastError().text(); return false; }

class JMdictDBParser : public JMdictParser
{
public:
	JMdictDBParser(const QStringList &languages) : JMdictParser(languages) {}
	virtual bool onItemParsed(const JMdictItem &entry);
	virtual bool onDeletedItemParsed(const JMdictDeletedItem &entry);
};

bool JMdictDBParser::onItemParsed(const JMdictItem &entry)
{
	// Insert writings
	int kanjiCount = 0;
	quint8 idx = 0;
	foreach (const JMdictKanjiWritingItem &kWriting, entry.kanji) {
		BIND(insertKanjiTextQuery, kWriting.writing);
		EXEC(insertKanjiTextQuery);
		int rowId = insertKanjiTextQuery.lastInsertId().toInt();
		BIND(insertKanjiQuery, entry.id);
		BIND(insertKanjiQuery, idx);
		BIND(insertKanjiQuery, rowId);
		BIND(insertKanjiQuery, kWriting.frequency);
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
		int rowId = insertKanaTextQuery.lastInsertId().toInt();
		BIND(insertKanaQuery, entry.id);
		BIND(insertKanaQuery, idx);
		BIND(insertKanaQuery, rowId);
		BIND(insertKanaQuery, (quint8) kReading.noKanji);
		BIND(insertKanaQuery, kReading.frequency);
		QStringList restrictedToList;
		foreach (quint8 res, kReading.restrictedTo) restrictedToList << QString::number(res);
		AUTO_BIND(insertKanaQuery, restrictedToList.join(","), "");
		EXEC(insertKanaQuery);
		++idx;
	}
	
	// Insert senses
	// Holds the senses mapped to the language in which they will be displayed
	QMap<QString, QList<const JMdictSenseItem *> > senseByLang;
	foreach (const JMdictSenseItem &sense, entry.senses) {
		foreach (const QString &lang, languages) if (sense.gloss.contains(lang)) {
			senseByLang[lang] << &sense;
			break;
		}
	}
	// Now map all the senses ordered by language
	idx = 0;
	foreach (const QString &lang, languages) if (senseByLang.contains(lang)) {
		const QList<const JMdictSenseItem *> senses(senseByLang[lang]);
		foreach (const JMdictSenseItem *sense, senses) {
			BIND(insertSenseQuery, entry.id);
			BIND(insertSenseQuery, idx);
			BIND(insertSenseQuery, sense->posBitField(*this));
			BIND(insertSenseQuery, sense->miscBitField(*this));
			BIND(insertSenseQuery, sense->dialectBitField(*this));
			BIND(insertSenseQuery, sense->fieldBitField(*this));
			QStringList restrictedToList;
			foreach (quint8 res, sense->restrictedToKanji) restrictedToList << QString::number(res);
			AUTO_BIND(insertSenseQuery, restrictedToList.join(","), "");
			restrictedToList.clear();
			foreach (quint8 res, sense->restrictedToKana) restrictedToList << QString::number(res);
			AUTO_BIND(insertSenseQuery, restrictedToList.join(","), "");
			EXEC(insertSenseQuery);
			
			// Insert the gloss of the selected language
			BIND(insertGlossTextQuery, sense->gloss[lang].join(", "));
			EXEC(insertGlossTextQuery);
			int rowId = insertGlossTextQuery.lastInsertId().toInt();
			BIND(insertGlossQuery, entry.id);
			BIND(insertGlossQuery, idx);
			BIND(insertGlossQuery, lang);
			BIND(insertGlossQuery, rowId);
			EXEC(insertGlossQuery);
			++idx;
		}
	}
	
	// Insert entry
	BIND(insertEntryQuery, entry.id);
	BIND(insertEntryQuery, entry.frequency);
	BIND(insertEntryQuery, kanjiCount);
	EXEC(insertEntryQuery);
	return true;
}

bool JMdictDBParser::onDeletedItemParsed(const JMdictDeletedItem &entry)
{
	BIND(insertDeletedEntryQuery, entry.id);
	BIND(insertDeletedEntryQuery, entry.replacedBy ? entry.replacedBy : QVariant(QVariant::Int));
	EXEC(insertDeletedEntryQuery);
	return true;
}

bool insertJLPTLevels(const QString &fName, int level)
{
	QFile file(fName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) return false;
	char line[50];
	int lineSize;
	while ((lineSize = file.readLine(line, 50)) != -1) {
		if (lineSize == 0) continue;
		if (line[lineSize - 1] == '\n') line[lineSize - 1] = 0;
		BIND(insertJLPTQuery, QString(line).toInt());
		BIND(insertJLPTQuery, level);
		EXEC(insertJLPTQuery);
	}
	return true;
}

static void create_tables()
{
	QSqlQuery query;
	query.exec("create table info(version INT, JMdictVersion TEXT)");
	query.exec("create table posEntities(bitShift INTEGER PRIMARY KEY, name TEXT, description TEXT)");
	query.exec("create table miscEntities(bitShift INTEGER PRIMARY KEY, name TEXT, description TEXT)");
	query.exec("create table fieldEntities(bitShift INTEGER PRIMARY KEY, name TEXT, description TEXT)");
	query.exec("create table dialectEntities(bitShift INTEGER PRIMARY KEY, name TEXT, description TEXT)");
	query.exec("create table entries(id INTEGER PRIMARY KEY, frequency TINYINT, kanjiCount TINYINT)");
	query.exec("create table kanji(id INTEGER SECONDARY KEY REFERENCES entries, priority TINYINT, docid INT, frequency TINYINT)");
	query.exec("create virtual table kanjiText using fts3(reading)");
	query.exec("create table kana(id INTEGER SECONDARY KEY REFERENCES entries, priority TINYINT, docid INT, nokanji BOOLEAN, frequency TINYINT, restrictedTo TEXT)");
	query.exec("create virtual table kanaText using fts3(reading, TOKENIZE katakana)");
	query.exec("create table senses(id INTEGER SECONDARY KEY REFERENCES entries, priority TINYINT, pos INT, misc INT, dial INT, field INT, restrictedToKanji TEXT, restrictedToKana TEXT)");
	query.exec("create table gloss(id INTEGER SECONDARY KEY REFERENCES entries, sensePriority INT, lang CHAR(3), docid INTEGER SECONDARY KEY NOT NULL)");
	query.exec("create virtual table glossText using fts3(reading)");
	query.exec("create table kanjiChar(kanji INTEGER, id INTEGER SECONDARY KEY REFERENCES entries, priority INT)");
	query.exec("create table jlpt(id INTEGER PRIMARY KEY, level TINYINT)");
	query.exec("create table deletedEntries(id INTEGER PRIMARY KEY, movedTo INTEGER REFERENCES entries)");
}

static void create_indexes()
{
	QSqlQuery query;
	query.exec("create index idx_entries_frequency on entries(frequency)");
	query.exec("create index idx_kanji on kanji(id)");
	query.exec("create index idx_kanji_docid on kanji(docid)");
	query.exec("create index idx_kana on kana(id)");
	query.exec("create index idx_kana_docid on kana(docid)");
	query.exec("create index idx_senses on senses(id)");
	query.exec("create index idx_gloss on gloss(id)");
	query.exec("create index idx_gloss_docid on gloss(docid)");
	query.exec("create index idx_kanjichar on kanjiChar(kanji)");
	query.exec("create index idx_kanjichar_id on kanjiChar(id)");
	query.exec("create index idx_jlpt on jlpt(level)");
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
	PREPQUERY(insertEntryQuery, "insert into entries values(?, ?, ?)");
	PREPQUERY(insertKanjiTextQuery, "insert into kanjiText values(?)");
	PREPQUERY(insertKanjiQuery, "insert into kanji values(?, ?, ?, ?)");
	PREPQUERY(insertKanjiCharQuery, "insert into kanjiChar values(?, ?, ?)");
	PREPQUERY(insertKanaTextQuery, "insert into kanaText values(?)");
	PREPQUERY(insertKanaQuery, "insert into kana values(?, ?, ?, ?, ?, ?)");
	PREPQUERY(insertSenseQuery, "insert into senses values(?, ?, ?, ?, ?, ?, ?, ?)");
	PREPQUERY(insertGlossTextQuery, "insert into glossText values(?)");
	PREPQUERY(insertGlossQuery, "insert into gloss values(?, ?, ?, ?)");
	PREPQUERY(insertJLPTQuery, "insert or ignore into jlpt values(?, ?)");
	PREPQUERY(insertDeletedEntryQuery, "insert into deletedEntries values(?, ?)");
	#undef PREPQUERY

	// Parse and insert JMdict
	QFile file(QDir(srcDir).absoluteFilePath("3rdparty/JMdict"));
	if (!file.open(QFile::ReadOnly | QFile::Text)) return 1;
	QXmlStreamReader reader(&file);
	// English is used as a backup if nothing else is available
	if (!languages.contains("en")) languages << "en";
	JMdictDBParser JMParser(languages);
	if (!JMParser.parse(reader)) {
		qDebug() << database.lastError().text();
		return 1;
	}
	file.close();

	// Fill in the info table
	{
		QSqlQuery query;
		query.prepare("insert into info values(?, ?)");
		query.addBindValue(JMDICTDB_REVISION);
		query.addBindValue(JMParser.dictVersion());
		if (!query.exec()) return 2;
	}
	
	// Create indexes
	create_indexes();
	
	// Insert JLPT levels
	insertJLPTLevels(QDir(srcDir).absoluteFilePath("src/core/jmdict/jlpt-level4.txt"), 4);
	insertJLPTLevels(QDir(srcDir).absoluteFilePath("src/core/jmdict/jlpt-level3.txt"), 3);
	insertJLPTLevels(QDir(srcDir).absoluteFilePath("src/core/jmdict/jlpt-level2.txt"), 2);
	insertJLPTLevels(QDir(srcDir).absoluteFilePath("src/core/jmdict/jlpt-level1.txt"), 1);
	
	// Populate the entities tables
	{
		QSqlQuery entitiesQuery(database);
		entitiesQuery.prepare("insert into posEntities values(?, ?, ?)");
		foreach (const QString &name, JMParser.posBitFields.keys()) {
			entitiesQuery.addBindValue(JMParser.posBitFields[name]);
			entitiesQuery.addBindValue(name);
			entitiesQuery.addBindValue(JMParser.entities[name]);
			if (!entitiesQuery.exec()) return 2;
		}
		entitiesQuery.prepare("insert into miscEntities values(?, ?, ?)");
		foreach (const QString &name, JMParser.miscBitFields.keys()) {
			entitiesQuery.addBindValue(JMParser.miscBitFields[name]);
			entitiesQuery.addBindValue(name);
			entitiesQuery.addBindValue(JMParser.entities[name]);
			if (!entitiesQuery.exec()) return 2;
		}
		entitiesQuery.prepare("insert into fieldEntities values(?, ?, ?)");
		foreach (const QString &name, JMParser.fieldBitFields.keys()) {
			entitiesQuery.addBindValue(JMParser.fieldBitFields[name]);
			entitiesQuery.addBindValue(name);
			entitiesQuery.addBindValue(JMParser.entities[name]);
			if (!entitiesQuery.exec()) return 2;
		}
		entitiesQuery.prepare("insert into dialectEntities values(?, ?, ?)");
		foreach (const QString &name, JMParser.dialectBitFields.keys()) {
			entitiesQuery.addBindValue(JMParser.dialectBitFields[name]);
			entitiesQuery.addBindValue(name);
			entitiesQuery.addBindValue(JMParser.entities[name]);
			if (!entitiesQuery.exec()) return 2;
		}
	}
	
	// Analyze for hopefully better performance
	database.exec("analyze");
	
	// Commit everything
	database.commit();
	
	// Close the database and set the file to read-only
	database = QSqlDatabase();
	QFile(dstFile).setPermissions(QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther);
	
	return 0;
}
