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

#include <QString>
#include <QRegExp>
#include <QFile>
#include <QHash>

#include <QtDebug>

static SQLite::Connection connection;
static SQLite::Connection jmdictConnection;
// All the SQL queries used to build the database
// Having them here will allow us to prepare them once and for all
// instead of doing it for every entry.
static SQLite::Query insertSentenceQuery;
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

// Where all sentences that should be recorded into the database are stored
static QHash<int, QString> sentencesToRecord;

#define TATOEBA_DB_DEBUG

static bool create_tables()
{
	SQLite::Query query(&connection);
	EXEC_STMT(query, "create table entries(id INTEGER PRIMARY KEY, sentence BLOB)");
	EXEC_STMT(query, "create table words(jmdictId INTEGER, sentenceId INTEGER SECONDARY KEY REFERENCES sentences, position tinyInt)");
	return true;
}

static bool parse_sentences(const QString &sfile)
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
		int jNbr = lineRegExp.cap(1).toInt();
		int eNbr = lineRegExp.cap(2).toInt();
		QString sentenceBits(lineRegExp.cap(3));

		if (eNbr == -1) continue;

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
			if (!jmdictLookupQuery.next()) {
				qDebug("Cannot find word %s in database", writing.toUtf8().data());
				continue;
			}

			// We must record that sentence in the DB
			sentencesToRecord[jNbr] = sentenceBits;

			// Insert the association between word and sentence
			insertWordToSentenceQuery.bindValue(jmdictLookupQuery.valueUInt(0));
			insertWordToSentenceQuery.bindValue(jNbr);
			insertWordToSentenceQuery.bindValue(wordPos);
			insertWordToSentenceQuery.exec();

			jmdictLookupQuery.reset();
		}
	}
	foreach (quint32 id, sentencesToRecord.keys()) {
		insertSentenceQuery.bindValue(id);
		insertSentenceQuery.bindValue(sentencesToRecord[id]);
		insertSentenceQuery.exec();
	}

	return true;
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
	if (!connection.connect(dstFile, SQLite::Connection::JournalInFile)) {
		qFatal("Cannot open database: %s", connection.lastError().message().toLatin1().data());
		return 1;
	}
	if (!jmdictConnection.connect("jmdict-en.db")) {
		qFatal("Cannot connect to JMdict database: %s", jmdictConnection.lastError().message().toLatin1().data());
		return 1;
	}
	ASSERT(connection.transaction());
	ASSERT(create_tables());
	
	// Prepare the queries
	#define PREPQUERY(query, text) query.useWith(&connection); query.prepare(text)
	PREPQUERY(insertSentenceQuery, "insert into entries values(?, ?)");
	PREPQUERY(insertWordToSentenceQuery, "insert into words values(?, ?, ?)");
	#undef PREPQUERY

	#define PREPQUERY(query, text) query.useWith(&jmdictConnection); query.prepare(text)
	PREPQUERY(jmdictLookupWRQuery, "select entries.id from entries join kanji on kanji.id = entries.id join kanjiText on kanjiText.docid == kanji.docid join kana on kana.id == entries.id join kanaText on kanaText.docid == kana.docid where kanjiText.reading match ? and kanaText.reading match ?");
	PREPQUERY(jmdictLookupWQuery, "select entries.id from entries join kanji on kanji.id = entries.id join kanjiText on kanjiText.docid == kanji.docid where kanjiText.reading match ?");
	PREPQUERY(jmdictLookupRQuery, "select entries.id from entries join kana on kana.id == entries.id join kanaText on kanaText.docid == kana.docid where kanaText.reading match ?");
	#undef PREPQUERY

	// Parse the files
	ASSERT(parse_sentences(QDir(srcDir).absoluteFilePath("3rdparty/tatoeba/jpn_indices.csv")));

	// Analyze for hopefully better performance
	connection.exec("analyze");
	
	// Commit everything
	ASSERT(connection.commit());
	
	// Clear queries, close the database and set the file to read-only
	insertSentenceQuery.clear();
	insertWordToSentenceQuery.clear();
	ASSERT(connection.close());
	QFile(dstFile).setPermissions(QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther);

	return 0;
}

