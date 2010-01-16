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
#include "core/XmlParserHelper.h"
#include "sqlite/qsql_sqlite.h"
#include "core/kanjidic2/Kanjidic2Entry.h"

#include <QLocale>

#include <QtDebug>

class Kanji {
private:
	static QSqlQuery insertEntryQuery;
	static QSqlQuery insertReadingQuery;
	static QSqlQuery insertReadingTextQuery;
	static QSqlQuery insertMeaningQuery;
	static QSqlQuery insertMeaningTextQuery;
	static QSqlQuery insertNanoriQuery;
	static QSqlQuery insertNanoriTextQuery;
	
public:
	int id;
	int grade;
	int stroke_count;
	int freq;
	int jlpt;
	QList<QString> skip;
	QList<QString> readings;
	QList<QString> meanings;
	QList<QString> nanori;
	
	static void initializeQueries(QSqlDatabase &database);

	Kanji() : id(0), grade(0), stroke_count(0), freq(0), jlpt(0) {}
	bool insertIntoDatabase();
};
QSqlQuery Kanji::insertEntryQuery;
QSqlQuery Kanji::insertReadingQuery;
QSqlQuery Kanji::insertReadingTextQuery;
QSqlQuery Kanji::insertMeaningQuery;
QSqlQuery Kanji::insertMeaningTextQuery;
QSqlQuery Kanji::insertNanoriQuery;
QSqlQuery Kanji::insertNanoriTextQuery;

void Kanji::initializeQueries(QSqlDatabase& database)
{
#define PREPQUERY(query, text) query = QSqlQuery(database); query.prepare(text)
	PREPQUERY(insertEntryQuery, "insert into entries values(?, ?, ?, ?, ?)");
	PREPQUERY(insertReadingQuery, "insert into reading values(?, ?, ?)");
	PREPQUERY(insertReadingTextQuery, "insert into readingText values(?)");
	PREPQUERY(insertMeaningQuery, "insert into meaning values(?, ?, ?)");
	PREPQUERY(insertMeaningTextQuery, "insert into meaningText values(?)");
	PREPQUERY(insertNanoriQuery, "insert into nanori values(?, ?)");
	PREPQUERY(insertNanoriTextQuery, "insert into nanoriText values(?)");
#undef PREPQUERY
}

#define AUTO_BIND(query, nval, val) if (val == nval) insertEntryQuery.addBindValue(QVariant::Int); \
	else insertEntryQuery.addBindValue(val)

/**
 * Returns true if the entry has successfully been inserted, false otherwise.
 */
bool Kanji::insertIntoDatabase()
{
	insertEntryQuery.addBindValue(id);
	AUTO_BIND(insertEntryQuery, grade, 0);
	AUTO_BIND(insertEntryQuery, stroke_count, 0);
	AUTO_BIND(insertEntryQuery, freq, 0);
	AUTO_BIND(insertEntryQuery, jlpt, 0);
	bool res = insertEntryQuery.exec();
	if (!res) qDebug() << insertEntryQuery.lastError();
	return res;
}

static bool process_main(QXmlStreamReader &reader)
{
	DOCUMENT_BEGIN(reader)
		TAG(kanjidic2)
			TAG_PRE(character)
			Kanji kanji;
			TAG_BEGIN(character)
				TAG(literal)
				CHARACTERS
					int kanjiCode = TextTools::singleCharToUnicode(TEXT.toString());
					kanji.id = kanjiCode;
				DONE
				ENDTAG
				TAG(misc)
					TAG(grade)
					CHARACTERS
						kanji.grade = TEXT.toString().toInt();
					DONE
					ENDTAG
					TAG(stroke_count)
					CHARACTERS
						kanji.stroke_count = TEXT.toString().toInt();
					DONE
					ENDTAG
					TAG(freq)
					CHARACTERS
						kanji.freq = TEXT.toString().toInt();
					DONE
					ENDTAG
					TAG(jlpt)
					CHARACTERS
						kanji.jlpt = TEXT.toString().toInt();
					DONE
					ENDTAG
				ENDTAG
			TAG_POST
			kanji.insertIntoDatabase();
			DONE
			TAG(header)
			ENDTAG
		ENDTAG
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
