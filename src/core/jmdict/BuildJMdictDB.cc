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
static QSqlQuery insertStagkQuery;
static QSqlQuery insertStagrQuery;
static QSqlQuery insertGlossTextQuery;
static QSqlQuery insertGlossQuery;
static QSqlQuery insertJLPTQuery;

#define BIND(query, val) query.addBindValue(val)
#define AUTO_BIND(query, val, nval) if (val == nval) BIND(query, QVariant::Int); else BIND(query, val)
#define EXEC(query) if (!query.exec()) { qDebug() << query.lastError().text(); return false; }

class JMdictDBParser : public JMdictParser
{
public:
	virtual bool onItemParsed(JMdictItem &entry);
};

bool JMdictDBParser::onItemParsed(JMdictItem &entry)
{
	// Insert entry
	BIND(insertEntryQuery, entry.id);
	BIND(insertEntryQuery, entry.frequency);
	// TODO fix
	BIND(insertEntryQuery, 0);
	EXEC(insertEntryQuery);
	
	// Insert writings
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
		BIND(insertKanaQuery, restrictedToList.join(","));
		EXEC(insertKanaQuery);
		++idx;
	}
	
	// Insert senses
	// TODO reorder senses so that those with english meanings only are last? Is that wise with
	// respect to senses priority?
	// TODO remove English glosses from senses for which another language is available
	idx = 0;
	foreach (const JMdictSenseItem &sense, entry.senses) {
		BIND(insertSenseQuery, entry.id);
		BIND(insertSenseQuery, idx);
		BIND(insertSenseQuery, sense.pos);
		BIND(insertSenseQuery, sense.misc);
		BIND(insertSenseQuery, sense.dialect);
		BIND(insertSenseQuery, sense.field);
		EXEC(insertSenseQuery);
		// TODO handle stagk and stagr the wise way - i.e. as in kana readings restricted to
		
		foreach (const QString &lang, sense.gloss.keys()) {
			BIND(insertGlossTextQuery, sense.gloss[lang].join(", "));
			EXEC(insertGlossTextQuery);
			int rowId = insertGlossTextQuery.lastInsertId().toInt();
			BIND(insertGlossQuery, entry.id);
			BIND(insertGlossQuery, idx);
			BIND(insertGlossQuery, lang);
			BIND(insertGlossQuery, rowId);
			EXEC(insertGlossQuery);
		}
		++idx;
	}
}

static void create_tables()
{
	QSqlQuery query;
	query.exec("create table info(version INT)");
	query.prepare("insert into info values(?)");
	query.addBindValue(JMDICTDB_REVISION);
	query.exec();
	query.exec("create table entries(id INTEGER PRIMARY KEY, frequency TINYINT, kanjiCount TINYINT)");
	query.exec("create table kanji(id INTEGER SECONDARY KEY REFERENCES entries, priority INT, docid INT, frequency TINYINT)");
	query.exec("create virtual table kanjiText using fts3(reading)");
	query.exec("create table kana(id INTEGER SECONDARY KEY REFERENCES entries, priority INT, docid INT, nokanji BOOLEAN, frequency TINYINT, restrictedTo TEXT)");
	query.exec("create virtual table kanaText using fts3(reading, TOKENIZE katakana)");
	// TODO optimize the sizes of fields
	query.exec("create table senses(id INTEGER SECONDARY KEY REFERENCES entries, priority INT, pos INT, misc INT, dial INT, field INT)");
	query.exec("create table stagk(id INTEGER SECONDARY KEY REFERENCES entries, sensePriority INT, kanjiPriority INT)");
	query.exec("create table stagr(id INTEGER SECONDARY KEY REFERENCES entries, sensePriority INT, kanaPriority INT)");
	query.exec("create table gloss(id INTEGER SECONDARY KEY REFERENCES entries, sensePriority INT, lang CHAR(3), docid INTEGER SECONDARY KEY NOT NULL)");
	query.exec("create virtual table glossText using fts3(reading)");
	query.exec("create table kanjiChar(kanji INTEGER, id INTEGER SECONDARY KEY REFERENCES entries, priority INT)");
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
	query.exec("create index idx_stagk on stagk(id, sensePriority)");
	query.exec("create index idx_stagr on stagr(id, sensePriority)");
	query.exec("create index idx_gloss on gloss(id)");
	query.exec("create index idx_gloss_docid on gloss(docid)");
	query.exec("create index idx_kanjichar on kanjiChar(kanji)");
	query.exec("create index idx_kanjichar_id on kanjiChar(id)");
	query.exec("create index idx_jlpt on jlpt(level)");
}

int main(int argc, char *argv[])
{
		QCoreApplication app(argc, argv);
	
	if (argc != 3) {
		qCritical("Usage: %s source_dir dest_file", argv[0]);
		return 1;
	}
	QString srcDir(argv[1]);
	QString dstFile(argv[2]);
	
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
	PREPQUERY(insertSenseQuery, "insert into senses values(?, ?, ?, ?, ?, ?)");
	PREPQUERY(insertStagkQuery, "insert into stagk values(?, ?, ?)");
	PREPQUERY(insertStagrQuery, "insert into stagr values(?, ?, ?)");
	PREPQUERY(insertGlossTextQuery, "insert into glossText values(?)");
	PREPQUERY(insertGlossQuery, "insert into gloss values(?, ?, ?, ?)");
	PREPQUERY(insertJLPTQuery, "insert or ignore into jlpt values(?, ?)");
	#undef PREPQUERY

	// Parse and insert JMdict
	QFile file(QDir(srcDir).absoluteFilePath("3rdparty/JMdict"));
	if (!file.open(QFile::ReadOnly | QFile::Text)) return 1;
	QXmlStreamReader reader(&file);
	JMdictDBParser JMParser;
	if (!JMParser.parse(reader)) {
		qDebug() << database.lastError().text();
		return 1;
	}
	file.close();

	// Create indexes
	create_indexes();
	
	// Analyze for hopefully better performance
	database.exec("analyze");
	
	// Commit everything
	database.commit();
	return 0;
}
