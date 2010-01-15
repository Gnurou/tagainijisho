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
#include "core/kanjidic2/Kanjidic2Entry.h"

#include <QXmlStreamReader>
#include <QLocale>

#include <QtDebug>

typedef struct {
private:
	static QSqlQuery insertEntryQuery;
	static QSqlQuery insertReadingQuery;
	
public:
	int id;
	int grade;
	int strCount;
	int freq;
	int jlpt;
	QList<QString> skip;
	QList<QString> readings;
	QList<QString> meanings;
	QList<QString> nanori;
	
	static void initializeQueries(QSqlDatabase &database);
	
	bool insertIntoDatabase();
} Kanji;
QSqlQuery Kanji::insertEntryQuery;
QSqlQuery Kanji::insertReadingQuery;

void Kanji::initializeQueries(QSqlDatabase& database)
{
	insertEntryQuery = QSqlQuery(database);
	insertEntryQuery.prepare("insert into entries values(?, ?, ?, ?, ?)");
	insertReadingQuery = QSqlQuery(database);
	insertReadingQuery.prepare("insert into reading values(?, ?, ?)");
}

/**
 * Returns true if the entry has successfully been inserted, false otherwise.
 */
bool Kanji::insertIntoDatabase()
{
	insertEntryQuery.addBindValue(id);
	insertEntryQuery.addBindValue(grade);
	insertEntryQuery.addBindValue(strCount);
	insertEntryQuery.addBindValue(freq);
	insertEntryQuery.addBindValue(jlpt);
	bool res = insertEntryQuery.exec();
	if (!res) qDebug() << insertEntryQuery.lastError();
	return res;
}

#define __IGNORE if (ttype == QXmlStreamReader::Comment || ttype == QXmlStreamReader::DTD) continue;

#define __PROCESS_BEGIN(tag) \
	while (!reader.atEnd()) {     \
		QXmlStreamReader::TokenType ttype = reader.readNext(); \
		__IGNORE \
		if (ttype == QXmlStreamReader::EndElement && reader.name() == #tag) return false;

#define PROCESS_BEGIN(tag, ...) \
	static bool process##_##tag(QXmlStreamReader &reader, ## __VA_ARGS__) { \
	__PROCESS_BEGIN(tag)
	
#define PROCESS_END return true; \
	} \
	}

#define DOCUMENT_BEGIN(reader) \
	if (reader.readNext() != QXmlStreamReader::StartDocument) return true; \
	while (!reader.atEnd()) {     \
		QXmlStreamReader::TokenType ttype = reader.readNext(); \
		__IGNORE
		
#define DOCUMENT_END \
		} \
		return reader.tokenType() != QXmlStreamReader::EndDocument;

#define CHARACTERS if (ttype == QXmlStreamReader::Characters) {
#define TEXT reader.text()
#define IGNORE_CHARACTERS CHARACTERS DONE

#define TAG(n) if (ttype == QXmlStreamReader::StartElement && reader.name() == #n) {
#define DONE continue; }
#define PASS(n) do reader.readNext(); while (!reader.atEnd() && !(reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == #n));
#define IGNORE_OTHER_TAGS if (ttype == QXmlStreamReader::StartElement) { \
	QStringRef tName(reader.name()); \
	do reader.readNext(); while (!reader.atEnd() && !(reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == tName)); \
	continue; \
	}

#define PROCESS(part, ...) { if (process##_##part(__VA_ARGS__)) return true; }

PROCESS_BEGIN(literal, Kanji &kanji)
	CHARACTERS
		int kanjiCode = TextTools::singleCharToUnicode(TEXT.toString());
		kanji.id = kanjiCode;
	DONE
PROCESS_END

PROCESS_BEGIN(character, Kanji &kanji)
	TAG(literal)
	// TODO Allow to embed CHARACTERS in order to avoid defining a new function - use cases instead of new functions
		PROCESS(literal, reader, kanji)
	DONE
	IGNORE_OTHER_TAGS
	IGNORE_CHARACTERS
PROCESS_END

PROCESS_BEGIN(header)
	TAG(file_version)
		PASS(file_version)
	DONE
	TAG(database_version)
		PASS(database_version)
	DONE
	TAG(date_of_creation)
		PASS(date_of_creation)
	DONE
	IGNORE_CHARACTERS
PROCESS_END

PROCESS_BEGIN(kanjidic2)
	TAG(character)
		Kanji kanji;
		PROCESS(character, reader, kanji)
		if (!kanji.insertIntoDatabase()) return true;
	DONE
	TAG(header)
		PROCESS(header, reader);
	DONE
	IGNORE_CHARACTERS
PROCESS_END

static bool process_main(QXmlStreamReader &reader)
{
	DOCUMENT_BEGIN(reader)
		TAG(kanjidic2)
			PROCESS(kanjidic2, reader)
		DONE
		IGNORE_CHARACTERS
	DOCUMENT_END
}

void create_tables()
{
	QSqlQuery query;
	query.exec("create table info(version INT)");
	query.prepare("insert into info values(?)");
	query.addBindValue(KANJIDIC2DB_REVISION);
	query.exec();
	query.exec("create table entries(id INTEGER PRIMARY KEY, grade TINYINT, strokeCount TINYINT, frequency SMALLINT, jlpt TINYINT)");
	query.exec("create table reading(docid INTEGER PRIMARY KEY, entry INTEGER SECONDARY KEY REFERENCES entries, type TEXT)");
	query.exec("create virtual table readingText using fts3(reading, TOKENIZE katakana)");
	query.exec("create table meaning(docid INTEGER PRIMARY KEY, entry INTEGER SECONDARY KEY REFERENCES entries, lang TEXT)");
	query.exec("create virtual table meaningText using fts3(reading)");
	query.exec("create table nanori(docid INTEGER PRIMARY KEY, entry INTEGER SECONDARY KEY REFERENCES entries)");
	query.exec("create virtual table nanoriText using fts3(reading, TOKENIZE katakana)");

	query.exec("create table strokeGroups(kanji INTEGER, parentGroup INTEGER SECONDARY KEY REFERENCES strokeGroups, number TINYINT, element INTEGER, original INTEGER)");
	query.exec("create table strokes(parentGroup INTEGER SECONDARY KEY REFERENCES strokeGroups, strokeType INTEGER, path TEXT)");
	query.exec("create table skip(entry INTEGER, type TINYINT, c1 TINYINT, c2 TINYINT)");
}

void create_indexes()
{
	QSqlQuery query;
	query.exec("create index idx_entries_frequency on entries(frequency)");
	query.exec("create index idx_jlpt on entries(jlpt)");
	query.exec("create index idx_reading_entry on reading(entry)");
	query.exec("create index idx_meaning_entry on meaning(entry)");
	query.exec("create index idx_nanori_entry on nanori(entry)");
	query.exec("create index idx_strokeGroups_kanji on strokeGroups(kanji)");
	query.exec("create index idx_strokeGroups_parentGroup on strokeGroups(parentGroup)");
	query.exec("create index idx_strokeGroups_element on strokeGroups(element)");
	query.exec("create index idx_strokeGroups_original on strokeGroups(original)");
	query.exec("create index idx_strokes_parentGroup on strokes(parentGroup)");
	query.exec("create index idx_strokes_strokeType on strokes(strokeType)");
	query.exec("create index idx_skip on skip(entry)");
	query.exec("create index idx_skip_type on skip(type, c1, c2)");
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	
	QSQLiteDriver *driver = new QSQLiteDriver();

	QSqlDatabase database(QSqlDatabase::addDatabase(driver));
	database.setDatabaseName("kanjidic2test.db");
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
	
	// Now open the file to parse
	QFile file("3rdparty/kanjidic2.xml");
	QXmlStreamReader reader(&file);
	file.open(QFile::ReadOnly | QFile::Text);
	
	// Let's go
	database.transaction();
	create_tables();

	// Prepare the queries
	Kanji::initializeQueries(database);
	if (process_main(reader)) {
		return 1;
	}
	
	create_indexes();
	
	// Commit everything
	database.commit();
	qDebug() << "Commited\n";
	return 0;
}
