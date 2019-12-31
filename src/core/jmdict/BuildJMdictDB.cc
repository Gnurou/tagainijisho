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
#include "sqlite/SQLite.h"
#include "core/TextTools.h"
#include "core/jmdict/JMdictParser.h"
#include "core/jmdict/JMdictEntry.h"

#include <QStringList>
#include <QByteArray>
#include <QFile>
#include <QDir>

#include <QtDebug>

#define BIND(query, val) { if (!query.bindValue(val)) { qCritical("%s", query.lastError().message().toUtf8().data()); return false; } }
#define BINDNULL(query) { if (!query.bindNullValue()) { qCritical("%s", query.lastError().message().toUtf8().data()); return false; } }
#define AUTO_BIND(query, val, nval) if (val == nval) BINDNULL(query) else BIND(query, val)
#define EXEC(query) if (!query.exec()) { qCritical("%s", query.lastError().message().toUtf8().data()); return false; }
#define EXEC_STMT(query, stmt) if (!query.exec(stmt)) { qCritical("%s", query.lastError().message().toUtf8().data()); return false; }
#define ASSERT(cond) { if (!cond) { qCritical("%s: assert condition failed, line %d", __FILE__, __LINE__); return false; } }

class JMdictDBParser : public JMdictParser
{
public:
	JMdictDBParser(const QStringList &languages, QString sourceDirectory, QString destinationDirectory) : JMdictParser(languages) {
		srcDir = sourceDirectory;
		dstDir = destinationDirectory;
	}
	virtual bool onItemParsed(const JMdictItem &entry);
	virtual bool onDeletedItemParsed(const JMdictDeletedItem &entry);
	bool createMainDatabase();
	bool createMainTables();
	bool createMainIndexes();
	bool finalizeSensesTable();
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
	bool parseJMFs(const QStringList &supportedLanguages);
	bool parseJMF(const QString &fname, const QString &lang);
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
	QMap<QString, SQLite::Query> insertGlossesQueries;
	// lang ; id ; pri ; str
	QMap<QString, QMap<int, QMap<int, QStringList> > > jmf;

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
	QMap<QString, QStringList> allGlosses;
	foreach (const QString &lang, languages)
		allGlosses[lang] = QStringList();
	foreach (const JMdictSenseItem &sense, entry.senses) {
		BIND(insertSenseQuery, entry.id);
		BIND(insertSenseQuery, idx);
		BIND(insertSenseQuery, sense.pos.join(','));
		BIND(insertSenseQuery, sense.misc.join(','));
		BIND(insertSenseQuery, sense.dialect.join(','));
		BIND(insertSenseQuery, sense.field.join(','));
		QStringList restrictedToList;
		foreach (quint8 res, sense.restrictedToKanji) restrictedToList << QString::number(res);
		AUTO_BIND(insertSenseQuery, restrictedToList.join(","), "");
		restrictedToList.clear();
		foreach (quint8 res, sense.restrictedToKana) restrictedToList << QString::number(res);
		AUTO_BIND(insertSenseQuery, restrictedToList.join(","), "");
		EXEC(insertSenseQuery);


		// Insert the gloss for all languages (search table)
		foreach (const QString &lang, languages) {
			// Do we have a replacement for the glosses?
			const QStringList &glosses = jmf[lang][entry.id][idx].isEmpty() ?
				sense.gloss[lang] : jmf[lang][entry.id][idx];
			if (glosses.isEmpty()) {
				allGlosses[lang] << QString();
				continue;
			}
			allGlosses[lang] << glosses.join("\n");
			BIND(insertGlossTextQueries[lang], glosses.join(", "));
			EXEC(insertGlossTextQueries[lang]);
			BIND(insertGlossQueries[lang], entry.id);
			BIND(insertGlossQueries[lang], insertGlossTextQueries[lang].lastInsertId());
			EXEC(insertGlossQueries[lang]);
		}
		++idx;
	}
	// For every language, insert all glosses of an entry (load table)
	foreach (const QString &lang, languages) {
		QString all(allGlosses[lang].join("\n\n"));
		if (all.split("\n", QString::SkipEmptyParts).empty()) continue;
		BIND(insertGlossesQueries[lang], entry.id);
		BIND(insertGlossesQueries[lang], qCompress(all.toUtf8(), 9));
		EXEC(insertGlossesQueries[lang])
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
	ASSERT(insertJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/jmdict/jlpt-n1.csv"), 1));
	ASSERT(insertJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/jmdict/jlpt-n2.csv"), 2));
	ASSERT(insertJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/jmdict/jlpt-n3.csv"), 3));
	ASSERT(insertJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/jmdict/jlpt-n4.csv"), 4));
	ASSERT(insertJLPTLevel(QDir(srcDir).absoluteFilePath("src/core/jmdict/jlpt-n5.csv"), 5));
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
		qCritical("Cannot open database: %s", connection.lastError().message().toLatin1().data());
		return false;
	}
	connection.transaction();
	return true;
}

bool JMdictDBParser::closeDatabase(QString handle)
{
	SQLite::Connection &connection = connections[handle];
	ASSERT(connection.exec("ANALYZE"));
	ASSERT(connection.commit());
	ASSERT(connection.exec("VACUUM"));
	QFile(connection.dbFileName()).setPermissions(QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther);
	ASSERT(connection.close());
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
	PREPQUERY(insertSenseQuery, "insert into sensesTMP values(?, ?, ?, ?, ?, ?, ?, ?)");
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
	// Temporary table until we figure out how many pos, misc,... columns we need
	EXEC_STMT(query, "create table sensesTMP(id INTEGER SECONDARY KEY REFERENCES entries, priority TINYINT, pos TEXT, misc TEXT, dial TEXT, field TEXT, restrictedToKanji TEXT, restrictedToKana TEXT)");
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

static QString entityString(const int count, const QString &entity, const QString &suffix = "")
{
	QStringList l;

	for (int i = 0; i <= count / 64; i++)
		l.append(QString("%1%2%3").arg(entity).arg(i).arg(suffix));

	return l.join(", ");
}

static QVector<quint64> entityInsert(const QString &entities, const QHash<QString, quint16> &entityBitFields)
{
	QVector<quint64> columns((entityBitFields.size() / 64) + 1, 0);

	foreach (const QString &entity, entities.split(',', QString::SkipEmptyParts)) {
		const int idx = entityBitFields[entity] / 64;
		const quint16 bit = entityBitFields[entity] % 64;
		const quint64 mask = ((quint64)1) << bit;

		columns[idx] |= mask;
	}

	return columns;
}

bool JMdictDBParser::finalizeSensesTable()
{
	SQLite::Query query(&connections["main"]);
	SQLite::Query query2(&connections["main"]);
	int posCount, miscCount, dialCount, fieldCount;
	QString posStr, miscStr, dialStr, fieldStr;

	// First figure out how many 64-bit integer columns we need to encode the entities
	posCount = posBitFields.size();
	miscCount = miscBitFields.size();
	dialCount = dialBitFields.size();
	fieldCount = fieldBitFields.size();

	posStr = entityString(posCount, "pos", " INT");
	miscStr = entityString(miscCount, "misc", " INT");
	dialStr = entityString(dialCount, "dial", " INT");
	fieldStr = entityString(fieldCount, "field", " INT");

	// Create final senses table
	EXEC_STMT(query, QString("create table senses(id INTEGER SECONDARY KEY REFERENCES entries, priority TINYINT, %1, %2, %3, %4, restrictedToKanji TEXT, restrictedToKana TEXT)").arg(posStr).arg(miscStr).arg(dialStr).arg(fieldStr));

	// Copy data from temporary table into final one
	query2.prepare(QString("insert into senses(rowid, id, priority, %1, %2, %3, %4, restrictedToKanji, restrictedToKana) values(?, ?, ?, %5%6%7%8?, ?)")
		.arg(entityString(posCount, "pos"))
		.arg(entityString(miscCount, "misc"))
		.arg(entityString(dialCount, "dial"))
		.arg(entityString(fieldCount, "field"))
		.arg(QString("?, ").repeated((posCount / 64) + 1))
		.arg(QString("?, ").repeated((miscCount / 64) + 1))
		.arg(QString("?, ").repeated((dialCount / 64) + 1))
		.arg(QString("?, ").repeated((fieldCount / 64) + 1)));
	EXEC_STMT(query, "select rowid, * from sensesTMP");
	while (query.next()) {
		BIND(query2, query.valueInt64(0));
		BIND(query2, query.valueInt64(1));
		BIND(query2, query.valueInt(2));
		foreach (quint64 mask, entityInsert(query.valueString(3), posBitFields))
			BIND(query2, mask);
		foreach (quint64 mask, entityInsert(query.valueString(4), miscBitFields))
			BIND(query2, mask);
		foreach (quint64 mask, entityInsert(query.valueString(5), dialBitFields))
			BIND(query2, mask);
		foreach (quint64 mask, entityInsert(query.valueString(6), fieldBitFields))
			BIND(query2, mask);
		BIND(query2, query.valueString(7));
		BIND(query2, query.valueString(8));
		EXEC(query2)
	}

	// Delete temporary senses table
	EXEC_STMT(query, "drop table sensesTMP");

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
		PREPQUERY(insertGlossQueries[lang], "insert into gloss values(?, ?)");
		PREPQUERY(insertGlossesQueries[lang], "insert into glosses values(?, ?)");
#undef PREPQUERY
	}
	return true;
}

bool JMdictDBParser::clearLanguagesQueries()
{
	foreach (const QString &lang, languages) {
		insertGlossTextQueries[lang].clear();
		insertGlossQueries[lang].clear();
		insertGlossesQueries[lang].clear();
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
		EXEC_STMT(query, "create table gloss(id INTEGER SECONDARY KEY, docid INTEGER PRIMARY KEY)");
		EXEC_STMT(query, "create virtual table glossText using fts4(reading)");
		EXEC_STMT(query, "create table glosses(id INTEGER PRIMARY KEY, glosses BLOB)");
	}
	return true;
}

bool JMdictDBParser::createLanguagesIndexes()
{
	foreach (const QString &lang, languages) {
		SQLite::Query query(&connections[lang]);
		EXEC_STMT(query, "DELETE FROM glossText_content");
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
	foreach (const QString &name, dialBitFields.keys()) {
		entitiesQuery.bindValue(dialBitFields[name]);
		entitiesQuery.bindValue(name);
		entitiesQuery.bindValue(entities[name]);
		ASSERT(entitiesQuery.exec());
	}
	return true;
}

bool JMdictDBParser::parseJMF(const QString &fName, const QString &lang)
{
	QFile file(fName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		return false;
	}

	QTextStream in(&file);
	QString line(in.readLine());
	while (!line.isNull()) {
		int pos = line.indexOf(' ');
		int pos2 = line.indexOf(' ', pos + 1);
		int eid(line.left(pos).toInt());
		int pri(line.mid(pos + 1, pos2 - (pos + 1)).toInt());

		// Flush previous
		QString gloss(line.mid(pos2 + 1));
		jmf[lang][eid][pri] << gloss;
		line = in.readLine();
	}
	return true;
}

bool JMdictDBParser::parseJMFs(const QStringList &supportedLanguages)
{
	QDir dir(QDir(srcDir).absoluteFilePath("src/core/jmdict"));

	foreach (QString fName, dir.entryList(QStringList() << "*.jmf")) {
		QString lang(fName.split('.')[0]);
		if (supportedLanguages.contains(lang))
			parseJMF(dir.absoluteFilePath(fName), lang);
	}

	return true;
}

void printUsage(char *argv[])
{
	qCritical("Usage: %s [-l<lang>] source_dir dest_dir\nWhere <lang> is a two-letters language code (en, fr, de, es or ru)", argv[0]);
}

bool buildDB(const QStringList &languages, const QString &srcDir, const QString &dstDir)
{
	JMdictDBParser parser(languages, srcDir, dstDir);

	parser.parseJMFs(languages);
	parser.createMainDatabase();
	parser.createMainTables();
	parser.prepareMainQueries();

	parser.createLanguagesDatabases();
	parser.createLanguagesTables();
	parser.prepareLanguagesQueries();

	QFile file(QDir(srcDir).absoluteFilePath("3rdparty/JMdict"));
	ASSERT(file.open(QFile::ReadOnly | QFile::Text));
	QXmlStreamReader reader(&file);
	if (!parser.parse(reader)) {
		return 1;
	}
	file.close();

	parser.fillMainInfoTable();
	parser.fillLanguagesInfoTable();
	parser.insertJLPTLevels();
	parser.populateEntitiesTable();
	parser.finalizeSensesTable();
	parser.createMainIndexes();
	parser.createLanguagesIndexes();

	parser.clearMainQueries();
	parser.finalizeMainDatabase();

	parser.clearLanguagesQueries();
	parser.finalizeLanguagesDatabases();

	return true;
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	sqlite3ext_init();

	if (argc < 3) {
		printUsage(argv); return 1;
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

	return (!buildDB(languages, srcDir, dstDir));
}
