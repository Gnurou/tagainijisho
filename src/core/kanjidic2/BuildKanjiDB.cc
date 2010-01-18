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
#include <QStringList>
#include <QMap>

#include <QtDebug>

class Kanji {
private:
	static const QStringList _validReadings;
	static QSqlQuery _insertEntryQuery;
	static QSqlQuery _insertOrIgnoreEntryQuery;
	static QSqlQuery _insertReadingQuery;
	static QSqlQuery _insertReadingTextQuery;
	static QSqlQuery _insertMeaningQuery;
	static QSqlQuery _insertMeaningTextQuery;
	static QSqlQuery _insertNanoriQuery;
	static QSqlQuery _insertNanoriTextQuery;
	static QSqlQuery _insertSkipCode;
	static QSqlQuery _insertStrokeGroup;
	static QSqlQuery _insertStroke;
	static QSqlQuery _updateJLPTLevels;
	static QSqlQuery _updateStrokeCount;
	
public:
	static QStringList languages;
	int id;
	int grade;
	int stroke_count;
	int freq;
	int jlpt;
	QString skip;
	// TODO Add a rmgroup parameter to allow grouping of readings and meanings
	// Associate the reading types to the readings
	QMap<QString, QStringList> readings;
	// Associate the language to the meaning
	QMap<QString, QStringList> meanings;
	QStringList nanori;
	
	static void initializeQueries(QSqlDatabase &database);

	Kanji() : id(0), grade(0), stroke_count(0), freq(0), jlpt(0) {}
	bool insertIntoDatabase(bool orIgnore = false);
	int insertStrokeGroup(int parentGroup, u_int8_t number, int element, int original);
	bool insertStroke(int parentGroup, int strokeType, const QString &path);
	
	static bool updateJLPTLevels(const QString &fName, int level);
};

const QStringList Kanji::_validReadings(QString("ja_on,ja_kun").split(','));
QStringList Kanji::languages(QString("en").split(','));
QSqlQuery Kanji::_insertEntryQuery;
QSqlQuery Kanji::_insertOrIgnoreEntryQuery;
QSqlQuery Kanji::_insertReadingQuery;
QSqlQuery Kanji::_insertReadingTextQuery;
QSqlQuery Kanji::_insertMeaningQuery;
QSqlQuery Kanji::_insertMeaningTextQuery;
QSqlQuery Kanji::_insertNanoriQuery;
QSqlQuery Kanji::_insertNanoriTextQuery;
QSqlQuery Kanji::_insertSkipCode;
QSqlQuery Kanji::_insertStrokeGroup;
QSqlQuery Kanji::_insertStroke;
QSqlQuery Kanji::_updateJLPTLevels;
QSqlQuery Kanji::_updateStrokeCount;

void Kanji::initializeQueries(QSqlDatabase& database)
{
#define PREPQUERY(query, text) query = QSqlQuery(database); query.prepare(text)
	PREPQUERY(_insertEntryQuery, "insert into entries values(?, ?, ?, ?, ?)");
	PREPQUERY(_insertOrIgnoreEntryQuery, "insert or ignore into entries values(?, ?, ?, ?, ?)");
	PREPQUERY(_insertReadingQuery, "insert into reading values(?, ?, ?)");
	PREPQUERY(_insertReadingTextQuery, "insert into readingText values(?)");
	PREPQUERY(_insertMeaningQuery, "insert into meaning values(?, ?, ?)");
	PREPQUERY(_insertMeaningTextQuery, "insert into meaningText values(?)");
	PREPQUERY(_insertNanoriQuery, "insert into nanori values(?, ?)");
	PREPQUERY(_insertNanoriTextQuery, "insert into nanoriText values(?)");
	PREPQUERY(_insertSkipCode, "insert into skip values(?, ?, ?, ?)");
	PREPQUERY(_insertStrokeGroup, "insert into strokeGroups values(?, ?, ?, ?, ?)");
	PREPQUERY(_insertStroke, "insert into strokes values(?, ?, ?)");
	PREPQUERY(_updateJLPTLevels, "update entries set jlpt = ? where id = ?");
	PREPQUERY(_updateStrokeCount, "update entries set strokeCount = ? where id = ?");
#undef PREPQUERY
}

#define BIND(query, val) query.addBindValue(val)
#define AUTO_BIND(query, val, nval) if (val == nval) BIND(query, QVariant::Int); else BIND(query, val)
#define EXEC(query) if (!query.exec()) { qDebug() << query.lastError().text(); return false; }

/**
 * Returns true if the entry has successfully been inserted, false otherwise.
 */
bool Kanji::insertIntoDatabase(bool orIgnore)
{
	// Entries table
	QSqlQuery &insertQuery = orIgnore ? _insertOrIgnoreEntryQuery : _insertEntryQuery;
	AUTO_BIND(insertQuery, id, 0);
	AUTO_BIND(insertQuery, grade, 0);
	AUTO_BIND(insertQuery, stroke_count, 0);
	AUTO_BIND(insertQuery, freq, 0);
	AUTO_BIND(insertQuery, jlpt, 0);
	EXEC(insertQuery);

	// Readings
	foreach (const QString &readingType, _validReadings) {
		if (readings.contains(readingType)) {
			foreach (const QString &reading, readings[readingType]) {
				// TODO factorize identical readings! Record the row id into a hash table
				BIND(_insertReadingTextQuery, reading);
				EXEC(_insertReadingTextQuery);
				BIND(_insertReadingQuery, _insertReadingTextQuery.lastInsertId());
				BIND(_insertReadingQuery, id);
				// TODO reading type should be a tinyInt, not a string!
				BIND(_insertReadingQuery, readingType);
				EXEC(_insertReadingQuery);
			}
		}
	}
	
	// Meanings
	foreach (const QString &lang, languages) {
		if (meanings.contains(lang)) {
			foreach (const QString &meaning, meanings[lang]) {
				// TODO factorize identical meanings! Record the row id into a hash table
				BIND(_insertMeaningTextQuery, meaning);
				EXEC(_insertMeaningTextQuery);
				BIND(_insertMeaningQuery, _insertMeaningTextQuery.lastInsertId());
				BIND(_insertMeaningQuery, id);
				BIND(_insertMeaningQuery, lang);
				EXEC(_insertMeaningQuery);
			}
		}
	}
	
	// Nanori
	foreach (const QString &n, nanori) {
		// TODO factorize identical nanoris! Record the row id into a hash table
		BIND(_insertNanoriTextQuery, n);
		EXEC(_insertNanoriTextQuery);
		BIND(_insertNanoriQuery, _insertNanoriTextQuery.lastInsertId());
		BIND(_insertNanoriQuery, id);
		EXEC(_insertNanoriQuery);
	}
	
	// Skip code
	if (!skip.isEmpty()) {
		QStringList code(skip.split('-'));
		if (code.size() == 3) {
			BIND(_insertSkipCode, id);
			BIND(_insertSkipCode, code[0].toInt());
			BIND(_insertSkipCode, code[1].toInt());
			BIND(_insertSkipCode, code[2].toInt());
			EXEC(_insertSkipCode);
		}
	}
	return true;
}

bool Kanji::updateJLPTLevels(const QString &fName, int level)
{
	QFile file(fName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) return false;
	char line[50];
	int lineSize;
	while ((lineSize = file.readLine(line, 50)) != -1) {
		if (lineSize == 0) continue;
		if (line[lineSize - 1] == '\n') line[lineSize - 1] = 0;
		BIND(_updateJLPTLevels, level);
		BIND(_updateJLPTLevels, TextTools::singleCharToUnicode(QString::fromUtf8(line)));
		EXEC(_updateJLPTLevels);
	}
}

int Kanji::insertStrokeGroup(int parentGroup, u_int8_t number, int element, int original)
{
	BIND(_insertStrokeGroup, id);
	AUTO_BIND(_insertStrokeGroup, parentGroup, 0);
	BIND(_insertStrokeGroup, number);
	AUTO_BIND(_insertStrokeGroup, element, 0);
	AUTO_BIND(_insertStrokeGroup, original, 0);
	if (!_insertStrokeGroup.exec()) return -1;
	else return _insertStrokeGroup.lastInsertId().toInt();
}

bool Kanji::insertStroke(int parentGroup, int strokeType, const QString &path)
{
	BIND(_insertStroke, parentGroup);
	AUTO_BIND(_insertStroke, strokeType, 0);
	AUTO_BIND(_insertStroke, path, "");
	EXEC(_insertStroke);
	return true;
}

static bool process_kanjidic2(QXmlStreamReader &reader)
{
	DOCUMENT_BEGIN(reader)
		TAG(kanjidic2)
			TAG_PRE(character)
			Kanji kanji;
			TAG_BEGIN(character)
				TAG(literal)
				CHARACTERS
					int kanjiCode = TextTools::singleCharToUnicode(TEXT);
					kanji.id = kanjiCode;
				DONE
				ENDTAG
				TAG(misc)
					TAG(grade)
					CHARACTERS
						kanji.grade = TEXT.toInt();
					DONE
					ENDTAG
					TAG(stroke_count)
					CHARACTERS
						kanji.stroke_count = TEXT.toInt();
					DONE
					ENDTAG
					TAG(freq)
					CHARACTERS
						kanji.freq = TEXT.toInt();
					DONE
					ENDTAG
					TAG(jlpt)
					CHARACTERS
						kanji.jlpt = TEXT.toInt();
					DONE
					ENDTAG
				ENDTAG
				TAG(reading_meaning)
					TAG(rmgroup)
						TAG_PRE(reading)
							QString r_type(ATTR("r_type"));
						TAG_BEGIN(reading)
						CHARACTERS
							kanji.readings[r_type] << TEXT;
						DONE
						ENDTAG
						TAG_PRE(meaning)
							QString lang;
							if (HAS_ATTR("m_lang")) lang = ATTR("m_lang");
							else lang = "en";
						TAG_BEGIN(meaning)
							kanji.meanings[lang] << TEXT;
						CHARACTERS
						DONE
						ENDTAG
					ENDTAG
					TAG(nanori)
					CHARACTERS
						kanji.nanori << TEXT;
					DONE
					ENDTAG
				ENDTAG
				TAG(query_code)
					TAG_PRE(q_code)
						QString qc_type(ATTR("qc_type"));
						QString skip_misclass(ATTR("skip_misclass"));
					TAG_BEGIN(q_code)
					CHARACTERS
						if (qc_type == "skip" && skip_misclass.isEmpty()) kanji.skip = TEXT;
					DONE
					ENDTAG
				ENDTAG
			TAG_POST
			if (!kanji.insertIntoDatabase()) return false;
			DONE
			TAG(header)
			ENDTAG
		ENDTAG
	DOCUMENT_END
}

static bool process_kanjivg_strokegr(QXmlStreamReader &reader, Kanji &kanji, int parentGroup)
{
	TAG_BEGIN(strokegr)
		TAG_PRE(strokegr)
			if (!process_kanjivg_strokegr(reader, kanji, parentGroup)) return false;
		DONE
		TAG_PRE(stroke)
			QString path(ATTR("path"));
			if (!kanji.insertStroke(parentGroup, 0, path)) return false;
		TAG_BEGIN(stroke)
		ENDTAG
	TAG_POST
	return true;
}

static bool process_kanjivg(QXmlStreamReader &reader)
{
	DOCUMENT_BEGIN(reader)
		TAG(kanjis)
			TAG_PRE(kanji)
				int midashi(TextTools::singleCharToUnicode(ATTR("midashi")));
				bool shallInsert(TextTools::isJapaneseChar(TextTools::unicodeToSingleChar(midashi)));
				Kanji kanji;
				kanji.id = midashi;
				if (shallInsert) {
					// Insert dummy entry for kanji that are not defined by kanjidic2
					kanji.insertIntoDatabase(true);
				}
			TAG_BEGIN(kanji)
				if (shallInsert) {
					TAG_PRE(strokegr)
						// Add dummy group
						int groupId = kanji.insertStrokeGroup(0, 0, 0, 0);
						if (groupId == -1) return false;
						process_kanjivg_strokegr(reader, kanji, groupId);
					DONE
				}
			TAG_POST
			DONE
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
	
	// Open the database to write to
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

	database.transaction();
	create_tables();

	// Prepare the queries
	Kanji::initializeQueries(database);

	// Parse kanjidic2
	QFile file("3rdparty/kanjidic2.xml");
	if (!file.open(QFile::ReadOnly | QFile::Text)) return 1;
	QXmlStreamReader reader(&file);
	if (!process_kanjidic2(reader)) {
		qDebug() << database.lastError().text();
		return 1;
	}
	file.close();
	
	// Parse and insert KanjiVG data
	// TODO do not forget to add kanjis not referenced in kanjidic2!
	file.setFileName("3rdparty/kanjivg.xml");
	if (!file.open(QFile::ReadOnly | QFile::Text)) return 1;
	reader.setDevice(&file);
	if (!process_kanjivg(reader)) {
		qDebug() << database.lastError().text();
		return 1;
	}
	file.close();

	// Insert JLPT levels
	Kanji::updateJLPTLevels("src/core/kanjidic2/jlpt-level1.txt", 1);
	Kanji::updateJLPTLevels("src/core/kanjidic2/jlpt-level2.txt", 2);
	Kanji::updateJLPTLevels("src/core/kanjidic2/jlpt-level3.txt", 3);
	Kanji::updateJLPTLevels("src/core/kanjidic2/jlpt-level4.txt", 4);
		
	// Create indexes
	create_indexes();
	
	// Commit everything
	database.commit();
	qDebug() << "Commited\n";
	return 0;
}
