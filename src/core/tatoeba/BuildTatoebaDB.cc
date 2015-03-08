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

#include "sqlite/Connection.h"
#include "sqlite/Query.h"
#include "core/Database.h"
#include "core/TextTools.h"
#include "core/tatoeba/TatoebaEntry.h"

#include <QString>
#include <QRegExp>
#include <QFile>
#include <QHash>

#include <QtDebug>

static QMap<QString, SQLite::Connection> connection;
static SQLite::Connection jmdictConnection;
// Queries that insert a sentence into the right database
static QMap<QString, SQLite::Query> insertSentenceQuery;
static SQLite::Query insertWordToSentenceQuery;

static SQLite::Query jmdictLookupWRQuery;
static SQLite::Query jmdictLookupWQuery;
static SQLite::Query jmdictLookupRQuery;

#define BIND(query, val) { if (!query.bindValue(val)) { qFatal(query.lastError().message().toUtf8().data()); return false; } }
#define BINDNULL(query) { if (!query.bindNullValue()) { qFatal(query.lastError().message().toUtf8().data()); return false; } }
#define AUTO_BIND(query, val, nval) if (val == nval) BINDNULL(query) else BIND(query, val)
#define EXEC(query) if (!query.exec()) { qFatal(query.lastError().message().toUtf8().data()); return false; }
#define EXEC_STMT(query, stmt) if (!query.exec(stmt)) { qFatal(query.lastError().message().toUtf8().data()); return false; }
#define ASSERT(cond) if (!(cond)) return 1;

// Languages to parse
static QSet<QString> languages;
static QMap<QString, QString> languagesCodes;

// Sentence id
typedef unsigned int sid;

// A sentence and the language it is written in
typedef QMap<QString, QString> LangSentence;
// All the sentences we need to record
// The Japanese sentence id is used as a reference here.
static QHash<sid, LangSentence > sentencesToRecord;

// Links foreign sentences to their japanese translation id.
static QHash<sid, sid> links;

#define TATOEBA_DB_DEBUG

static bool createTables(SQLite::Connection &connection, const QString &lang)
{
	SQLite::Query query(&connection);
	EXEC_STMT(query, "create table info(version INT)");
	EXEC_STMT(query, QString("insert into info values(%1)").arg(TATOEBADB_REVISION));
	EXEC_STMT(query, "create table entries(id INTEGER PRIMARY KEY, sentence BLOB)");
	if (lang == "jpn") EXEC_STMT(query, "create table words(jmdictId INTEGER, sentenceId INTEGER SECONDARY KEY REFERENCES sentences, position tinyInt)");
	return true;
}

// Parse the indices file and extract japanese sentences for which we have a matching word.
// The sentence id is recorded into sentencesToRecord, with an empty list of sentences to be
// filled by parse_links and parse_sentences
static bool parseIndices(const QString &sfile)
{
	QRegExp lineRegExp("(\\d+)\t(-?\\d+)\t(.*)\n");
	QRegExp wordRegExp("(\\w+)(?:\\|\\d*)?(?:\\(([^\\)]+)\\))?(?:\\[([^\\]]+)\\])?(?:\\{([^\\}]+)\\})?~?");
	//wordRegExp.setMinimal(true);
	QFile f(sfile);
	if (!f.open(QIODevice::ReadOnly)) return false;

	// Read the file line by line and extract the sentences
	unsigned int lineCpt = 0;
	QString line;
	while (1) {
		++lineCpt;
		line = QString::fromUtf8(f.readLine());
		if (line.isEmpty()) break;
		if (!lineRegExp.exactMatch(line)) {
#ifdef TATOEBA_DB_DEBUG
			qDebug("Cannot match line %d", lineCpt);
#endif
			continue;
		}
		sid jid = lineRegExp.cap(1).toInt();
		//sid eNbr = lineRegExp.cap(2).toInt();
		QString sentenceBits(lineRegExp.cap(3));

		//if (eNbr == -1) continue;

		// Break the sentence
		QStringList words(sentenceBits.split(' ', QString::SkipEmptyParts));
		for (int wordPos = 0; wordPos < words.size(); wordPos++) {
			const QString &word = words[wordPos];

			if (!wordRegExp.exactMatch(word)) {
#ifdef TATOEBA_DB_DEBUG
				qDebug("Cannot match word %s at line %d", word.toUtf8().data(), lineCpt);
#endif
				continue;
			}
			QString writing(wordRegExp.cap(1));
			QString reading(wordRegExp.cap(2));
			//unsigned int meaning(wordRegExp.cap(3).toUInt());
			//QString original(wordRegExp.cap(4));
			bool checked = word.contains('~');
			if (writing.isEmpty() && reading.isEmpty()) continue;
			if (!checked) continue;
			if (reading.isEmpty() && TextTools::isKana(writing)) {
				reading = writing;
				writing.clear();
			}

			SQLite::Query &jmdictLookupQuery = reading.isEmpty() ? jmdictLookupWQuery : writing.isEmpty() ? jmdictLookupRQuery : jmdictLookupWRQuery;
			if (!writing.isEmpty()) jmdictLookupQuery.bindValue(writing);
			if (!reading.isEmpty()) jmdictLookupQuery.bindValue(reading);

			jmdictLookupQuery.exec();
			// Word not found in DB, never mind...
			if (!jmdictLookupQuery.next()) {
				//qDebug("Cannot find word %s in database", writing.toUtf8().data());
				continue;
			}

			// We must record that sentence in the DB, just store an empty index for now
			sentencesToRecord.insert(jid, LangSentence());
			// Link the sentence to itself
			links[jid] = jid;

			// Insert the association between word and sentence
			insertWordToSentenceQuery.bindValue(jmdictLookupQuery.valueUInt(0));
			insertWordToSentenceQuery.bindValue(jid);
			insertWordToSentenceQuery.bindValue(wordPos);
			insertWordToSentenceQuery.exec();

			jmdictLookupQuery.reset();
		}
	}

	return true;
}

static bool parseLinks(const QString &sfile)
{
	QRegExp lineRegExp("(\\d+)\t(\\d+)\n");
	QFile f(sfile);
	if (!f.open(QIODevice::ReadOnly)) return false;
	while (1) {
		QString line = QString::fromUtf8(f.readLine());
		if (line.isEmpty()) break;
		if (!lineRegExp.exactMatch(line)) continue;
		sid jid = lineRegExp.cap(2).toInt();
		if (!sentencesToRecord.contains(jid)) continue;
		sid fid = lineRegExp.cap(1).toInt();
		links[fid] = jid;
	}
	return true;
}

static bool parseSentences(const QString &sfile)
{
	QRegExp lineRegExp("(\\d+)\t(...)\t(.*)\n");
	QFile f(sfile);
	if (!f.open(QIODevice::ReadOnly)) return false;
	while (1) {
		QString line = QString::fromUtf8(f.readLine());
		if (line.isEmpty()) break;
		if (!lineRegExp.exactMatch(line)) continue;
		sid fid = lineRegExp.cap(1).toInt();
		if (!links.contains(fid)) continue;
		QString lang(lineRegExp.cap(2));
		if (!languages.contains(lang)) continue;
		sid jid = links[fid];
		QString sentence(lineRegExp.cap(3));
		if (!sentencesToRecord[jid].contains(lang))
			sentencesToRecord[jid].insert(lang, sentence);
	}
	return true;
}

static bool recordSentences()
{
	foreach (sid id, sentencesToRecord.keys()) {
		const LangSentence &sentences = sentencesToRecord[id];
		foreach (const QString &lang, sentences.keys()) {
			SQLite::Query &q = insertSentenceQuery[lang];
			ASSERT(q.bindValue(id));
			ASSERT(q.bindValue(sentences[lang]));
			ASSERT(q.exec());
		}
	}

	return true;
}

static void printUsage(char *argv[])
{
	qCritical("Usage: %s [-l<lang>] source_dir dest_file\nWhere <lang> is a two-letters language code (en, fr, de, es or ru)", argv[0]);
}

int main(int argc, char *argv[])
{
	languagesCodes["jpn"] = "jp";
	languagesCodes["eng"] = "en";
	languagesCodes["fra"] = "fr";
	languagesCodes["deu"] = "de";
	languagesCodes["spa"] = "es";
	languagesCodes["rus"] = "ru";

	QCoreApplication app(argc, argv);
	
	if (argc < 3) { printUsage(argv); return 1; }

	int argCpt = 1;
	while (argCpt < argc && argv[argCpt][0] == '-') {
		QString param(argv[argCpt]);
		if (!param.startsWith("-l")) { printUsage(argv); return 1; }
		QStringList langs(param.mid(2).split(',', QString::SkipEmptyParts));
		QStringList allowedLangs(languagesCodes.values());
		foreach (const QString &lang, langs) {
			if (!allowedLangs.contains(lang)) {
				qWarning("Language %s not supported!", lang.toLatin1().constData());
				continue;
			}
			languages << languagesCodes.key(lang);
		}
		++argCpt;
	}
	if (argCpt > argc - 2) { printUsage(argv); return -1; }

	QString srcDir(argv[argCpt]);
	QString dstDir(argv[argCpt + 1]);
	
	languages << "jpn";
	// Create databases and prepare queries for every language
	foreach (const QString &lang, languages) {
		QString dbFile = QDir(dstDir).absoluteFilePath(lang == "jpn" ? QString("tatoeba.db") : QString("tatoeba-%1.db").arg(languagesCodes[lang]));
		QFile dst(dbFile);
		if (dst.exists() && !dst.remove()) {
			qCritical("Error - cannot remove existing destination file!");
			return 1;
		}
		SQLite::Connection &curConnection = connection[lang];
		if (!curConnection.connect(dbFile, SQLite::Connection::JournalInFile)) {
			qFatal("Cannot open database: %s", curConnection.lastError().message().toLatin1().data());
			return 1;
		}
		ASSERT(curConnection.transaction());
		ASSERT(createTables(curConnection, lang));
		// Prepare the queries
		#define PREPQUERY(query, text) query.useWith(&curConnection); query.prepare(text)
		PREPQUERY(insertSentenceQuery[lang], "insert into entries values(?, ?)");
		if (lang == "jpn") PREPQUERY(insertWordToSentenceQuery, "insert into words values(?, ?, ?)");
		#undef PREPQUERY
	}
			
	// Connection to the JMdict database
	if (!jmdictConnection.connect(QDir(srcDir).absoluteFilePath("jmdict.db"))) {
		qFatal("Cannot connect to JMdict database: %s", jmdictConnection.lastError().message().toLatin1().data());
		return 1;
	}
	

	#define PREPQUERY(query, text) query.useWith(&jmdictConnection); query.prepare(text)
	PREPQUERY(jmdictLookupWRQuery, "select entries.id from entries join kanji on kanji.id = entries.id join kanjiText on kanjiText.docid == kanji.docid join kana on kana.id == entries.id join kanaText on kanaText.docid == kana.docid where kanjiText.reading match ? and kanaText.reading match ?");
	PREPQUERY(jmdictLookupWQuery, "select entries.id from entries join kanji on kanji.id = entries.id join kanjiText on kanjiText.docid == kanji.docid where kanjiText.reading match ?");
	PREPQUERY(jmdictLookupRQuery, "select entries.id from entries join kana on kana.id == entries.id join kanaText on kanaText.docid == kana.docid where kanaText.reading match ?");
	#undef PREPQUERY

	// Parse the files
	ASSERT(parseIndices(QDir(srcDir).absoluteFilePath("3rdparty/tatoeba/jpn_indices.csv")));
	ASSERT(parseLinks(QDir(srcDir).absoluteFilePath("3rdparty/tatoeba/links.csv")));
	ASSERT(parseSentences(QDir(srcDir).absoluteFilePath("3rdparty/tatoeba/sentences.csv")));
	ASSERT(recordSentences());
	
	foreach (const QString &lang, languages) {
		SQLite::Connection &curConnection = connection[lang];
		// Analyze for hopefully better performance
		curConnection.exec("analyze");
		// Commit everything
		ASSERT(curConnection.commit());
		// Clear queries, close the database and set the file to read-only
		insertSentenceQuery[lang].clear();
		if (lang == "jpn") insertWordToSentenceQuery.clear();
		QFile(curConnection.dbFileName()).setPermissions(QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther);
		ASSERT(curConnection.close());
	}

	return 0;
}

