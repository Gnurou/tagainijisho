/*
 *  Copyright (C) 2008  Alexandre Courbot
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

#include "sqlite/qsql_sqlite.h"
#include "sqlite3.h"

#include "core/Paths.h"
#include "core/TextTools.h"
#include "core/Database.h"
#include "core/ASyncQuery.h"

#include <QtDebug>
#include <QSemaphore>

#define USERDB_REVISION 4

#define QUERY(Q) if (!query.exec(Q)) return false

QMap<QString, QString> Database::_attachedDBs;

bool Database::createUserDB()
{
	if (!database.transaction()) return false;
	QSqlQuery query;
	// Info table
	QUERY("CREATE TABLE info(version INT)");
	QUERY(QString("INSERT INTO info VALUES(") + QString::number(USERDB_REVISION) + ")");

	// Training table
	QUERY("CREATE TABLE training(type INT NOT NULL, id INTEGER SECONDARY_KEY NOT NULL, score INT NOT NULL, dateAdded UNSIGNED INT NOT NULL, dateLastTrain UNSIGNED INT, nbTrained UNSIGNED INT NOT NULL, nbSuccess UNSIGNED INT NOT NULL, dateLastMistake UNSIGNED INT, CONSTRAINT training_unique_ids UNIQUE(type, id))");
	QUERY("CREATE INDEX idx_training_type_id ON training(type, id)");
	QUERY("CREATE INDEX idx_training_score ON training(score)");

	// Tags tables
	QUERY("CREATE VIRTUAL TABLE tags USING fts3(tag)");
	QUERY("CREATE TABLE taggedEntries(type INT, id INTEGER SECONDARY KEY, tagId INTEGER SECONDARY KEY REFERENCES tags, date UNSIGNED INT)");

	// Notes tables
	QUERY("CREATE TABLE notes(noteId INTEGER PRIMARY KEY, type INT, id INTEGER SECONDARY KEY, dateAdded UNSIGNED INT, dateLastChange UNSIGNED INT)");
	QUERY("CREATE VIRTUAL TABLE notesText using fts3(note)");

	// Sets table
	QUERY("CREATE TABLE sets(parent INT, position INT NOT NULL, label TEXT, state BLOB)");
	QUERY("CREATE INDEX idx_sets_id ON sets(parent, position)");
	if (!database.commit()) return false;
	return true;
}

/// Changes the training table structure to store the date of last training
/// and removes trainingLog
bool update1to2(QSqlQuery &query) {
	// Drop training indexes
	QUERY("DROP TRIGGER update_score");
	QUERY("DROP INDEX idx_training_type_id");
	QUERY("DROP INDEX idx_training_score");
	QUERY("ALTER TABLE training RENAME TO oldtraining");
	// Fix invalid dates
	QUERY("UPDATE oldtraining set dateLastTrain = null where dateLastTrain = 0");
	// Fix invalid dates...
	QUERY("UPDATE oldtraining set dateLastTrain = null where dateLastTrain = 4294967295");
	// Create the new training table and populate it
	QUERY("CREATE TABLE training(type INT NOT NULL, id INTEGER SECONDARY_KEY NOT NULL, score INT NOT NULL, dateAdded UNSIGNED INT NOT NULL, dateLastTrain UNSIGNED INT, nbTrained UNSIGNED INT NOT NULL, nbSuccess UNSIGNED INT NOT NULL, dateLastMistake UNSIGNED INT, CONSTRAINT training_unique_ids UNIQUE(type, id))");
	QUERY("INSERT INTO training(type, id, score, dateAdded, dateLastTrain, nbTrained, nbSuccess, dateLastMistake) SELECT *, null from oldtraining");
	QUERY("UPDATE training SET dateLastMistake = (SELECT MAX(date) FROM trainingLog WHERE result = 0 and trainingLog.id = training.id and trainingLog.type = training.type)");
	// Delete the old training tables
	QUERY("DROP TABLE oldtraining");
	QUERY("DROP TABLE trainingLog");
	// Restore training indexes
	QUERY("CREATE INDEX idx_training_score on training(score)");
	QUERY("CREATE INDEX idx_training_type_id on training(type, id)");
	QUERY("CREATE TRIGGER update_score after update of nbTrained, nbSuccess on training begin update training set score = (nbSuccess * 100) / (nbTrained + 1) where type = old.type and id = old.id; end;");
	// Don't know if this is even useful, but it doesn't hurt
	QUERY("DROP TABLE IF EXISTS temp");
	return true;
}

/// Add the sets table
bool update2to3(QSqlQuery &query) {
	// Create the sets table
	QUERY("CREATE TABLE sets(parent INT, position INT NOT NULL, label TEXT, state BLOB)");
	QUERY("CREATE INDEX idx_sets_id ON sets(parent, position)");

	return true;
}

/// Remove the score trigger
bool update3to4(QSqlQuery &query) {
	QUERY("DROP TRIGGER update_score");

	return true;
}

#undef QUERY

bool (*dbUpdateFuncs[USERDB_REVISION - 1])(QSqlQuery &) = {
	&update1to2,
	&update2to3,
	&update3to4
};

static Qt::ConnectionType alwaysSync = Qt::BlockingQueuedConnection;

Database *Database::instance = NULL;

Qt::ConnectionType Database::aSyncConnection() { return alwaysSync; }

// This semaphore is used to block the startThreaded() function until the database
// thread is ready to work (i.e. slots are correctly connected).
QSemaphore startSem(0);

void Database::startThreaded()
{
	instance = new Database();
	instance->start();
	// Block until the database thread is ready
	startSem.acquire();
}

void Database::startUnthreaded()
{
	alwaysSync = Qt::AutoConnection;
	instance = new Database();
	instance->run();
}

void Database::stop()
{
	instance->quit();
	delete instance;
}

Database::Database(QObject *parent) : QThread(parent), sqliteHandler(0)
{
	connectUserDB();
}

QVector<QRegExp> Database::staticRegExps;


static void regexpFunc(sqlite3_context *context, int argc, sqlite3_value **argv)
{
	QString text(QString::fromUtf8((const char *)sqlite3_value_text(argv[1])));
	// Get the regexp referenced by the request
	QRegExp &regexp = Database::staticRegExps[sqlite3_value_int(argv[0])];
/*
	const char * text = (const char *)sqlite3_value_text(argv[1]);
	const char * text2 = "test";

	int textLength = qstrlen(text);
	int text2Length = qstrlen(text2);
	int text2Pos = 0;
	bool res = false;
	for (int i = 0; i < textLength; i++) {
		if (text[i] == text2[text2Pos]) {
				// Matched first character, must check whether
				// we are at the boundary of a word
				if (text2Pos == 0) {
					// Not the beginning of a word!
					if (i != 0 && QChar(text[i - 1]).isLetterOrNumber()) continue;
				}
				if (++text2Pos == text2Length) {
					// Not the end of a word!
					if (i != textLength - 1 && QChar(text[i + 1]).isLetterOrNumber()) continue;
					res = true;
					break;
				}
		} else {
				// Did not match - reset text2Pos
				text2Pos = 0;
		}

	}
*/
	bool res = regexp.exactMatch(text);
	sqlite3_result_int(context, res);
}

/**
 * Returns a pseudo-random value which is biaised by the parameter given (which must
 * be between 0 and 100). The bigger the parameter, the biggest chances the generated
 * has to be low.
 */
static void biaised_random(sqlite3_context *context, int nParams, sqlite3_value **values)
{
	if (nParams != 1) {
		sqlite3_result_error(context, "Invalid number of arguments!", -1);
		return;
	}
	int score = sqlite3_value_int(values[0]);
//	int minVal = score == 0 ? 0 : qrand() % score;
	int res = qrand() % (101 - score);
	sqlite3_result_int(context, res);
}

typedef struct {
	QSet<int>* _set;
} uniquecount_aggr;

void uniquecount_aggr_step(sqlite3_context *context, int nParams, sqlite3_value **values)
{
	uniquecount_aggr *aggr_struct = static_cast<uniquecount_aggr *>(sqlite3_aggregate_context(context, sizeof(uniquecount_aggr)));
	if (!aggr_struct->_set) aggr_struct->_set = new QSet<int>();
	for (int i = 0; i < nParams; i++) {
		if (sqlite3_value_type(values[i]) == SQLITE_NULL) continue;
		aggr_struct->_set->insert(sqlite3_value_int(values[i]));
	}
}

void uniquecount_aggr_finalize(sqlite3_context *context)
{
	uniquecount_aggr *aggr_struct = static_cast<uniquecount_aggr *>(sqlite3_aggregate_context(context, sizeof(uniquecount_aggr)));
	int res = aggr_struct->_set->size();
	delete aggr_struct->_set;
	sqlite3_result_int(context, res);
}


void Database::connectUserDB()
{
	// Connect to the user DB
	QString filename(QDir(userProfile()).absoluteFilePath("user.db"));

	// Instanciate our custom driver
	QSQLiteDriver *driver = new QSQLiteDriver();

	database = QSqlDatabase::addDatabase(driver);
	database.setDatabaseName(filename);
	if (!database.open()) {
		qFatal("Cannot open database: %s", database.lastError().text().toLatin1().data());
		return;
	}

	// Attach custom functions
	QVariant handler = database.driver()->handle();
	if (handler.isValid() && !qstrcmp(handler.typeName(), "sqlite3*")) {
		sqliteHandler = *static_cast<sqlite3 **>(handler.data());
		sqlite3_create_function(sqliteHandler, "regexp", 2, SQLITE_UTF8, 0, regexpFunc, 0, 0);
		sqlite3_create_function(sqliteHandler, "biaised_random", 1, SQLITE_UTF8, 0, biaised_random, 0, 0);
		sqlite3_create_function(sqliteHandler, "uniquecount", -1, SQLITE_UTF8, 0, 0, uniquecount_aggr_step, uniquecount_aggr_finalize);
	}

	checkUserDB();
}

void Database::run()
{
	// We can let the GUI thread go!
	startSem.release();

	// Are we running in threaded mode?
	if (isRunning()) exec();
}

bool Database::isThreaded()
{
	return (alwaysSync == Qt::BlockingQueuedConnection);
}

void Database::quit()
{
	// Stop the on ongoing query, if any
	abortQuery();
	closeDB();
	// The database being closed, we can exit the thread
	QThread::quit();
	QThread::wait();
}

bool Database::updateUserDB(int currentVersion)
{
	// Check user DB version number, upgrade if necessary
	// The database is older than our version of Tagaini - we have to update the database
	for (; currentVersion < USERDB_REVISION; ++currentVersion) {
		if (!database.transaction()) return false;
		QSqlQuery query2;
		if (!dbUpdateFuncs[currentVersion - 1](query2)) return false;
		// Update version number
		if (!query2.exec(QString("UPDATE info SET version=%1").arg(currentVersion + 1))) return false;
		query2.clear();
		if (!database.commit()) return false;
	}
	return true;
}

/**
 * Checks if the user DB exists and create it if necessary.
 * @return true if the database exists or have been successfully created; false
 *         if an error occured while building the database.
 */
void Database::checkUserDB()
{
	int currentVersion;
	QSqlQuery query;
	query.exec("pragma encoding=\"UTF-16le\"");
	query.exec("SELECT version FROM info");
	if (query.next()) currentVersion = query.value(0).toInt();
	else currentVersion = -1;
	query.clear();

	if (currentVersion != -1) {
		if (currentVersion < USERDB_REVISION) {
			if (!updateUserDB(currentVersion)) {
				qFatal("Error while upgrading user database: %s", database.lastError().text().toLatin1().constData());
				return;
			}
		}
		// The database is more recent than our version of Tagaini - there is nothing we can do!
		else if (currentVersion > USERDB_REVISION) {
			qFatal("Bad user database version: expected %d, got %d.", USERDB_REVISION, currentVersion);
			return;
		}
	}
	else {
		if (!createUserDB()) {
			qFatal("Cannot create user database: %s", database.lastError().text().toLatin1().constData());
			return;
		}
	}
}

/**
 * Attach the dictionary DB to the opened user database.
 * @return true if the dictionary DB has successfully been attached; false
 *         in an error occured.
 */
bool Database::attachDictionaryDB(const QString &file, const QString &alias, int expectedVersion)
{
#define QUERY(Q) if (!query.exec(Q)) goto error
	QSqlQuery query;
	// Try to attach the dictionary DB
	QUERY("attach database '" + file + "' as " + alias);

	// Check the version is compatible
	QUERY("select version from " + alias + ".info");
	// No result, not good
	if (!query.next()) goto error;
	if (query.value(0).toInt() != expectedVersion) goto errorDetach;
	// More than one result, not good
	if (query.next()) goto errorDetach;
	_attachedDBs[alias] = file;

	// Now attach the database on all other threaded connections
	foreach(DatabaseThread *dbThread, DatabaseThread::instances()) {
		if (!dbThread->connection()->attach(file, alias)) goto errorDetachAll;
	}
	return true;
errorDetachAll:
	foreach(DatabaseThread *dbThread, DatabaseThread::instances())
		dbThread->connection()->detach(alias);
errorDetach:
	QUERY("detach database " + alias);
#undef QUERY
error:
	qCritical() << QString("Failed query: %1: %2").arg(query.lastQuery()).arg(query.lastError().text());
	qCritical() << QString("Attached dictionary file was %1").arg(file);
	return false;
}

bool Database::detachDictionaryDB(const QString &alias)
{
	QSqlQuery query;
	if (!query.exec("detach database " + alias)) {
		qCritical() << QString("Failed query: %1: %2").arg(query.lastQuery()).arg(query.lastError().text());
		return false;
	}
	_attachedDBs.remove(alias);
	return true;
}

void Database::abortQuery()
{
	sqlite3_interrupt(instance->sqliteHandler);
}

void Database::closeDB()
{
	QSqlQuery query;

	// Remove unreferenced tags
	if (!query.exec("delete from tags where docid not in (select tagId from taggedEntries)")) qWarning("Could not cleanup unused tags!");

	// VACUUM the database
	if (!query.exec("vacuum")) qWarning() << "Final VACUUM failed" << query.lastError().text();
	// Call destructor for the database object
	database = QSqlDatabase();
//	qDebug("Database closed");
}

