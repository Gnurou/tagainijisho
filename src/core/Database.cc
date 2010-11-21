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

#include "sqlite3.h"

#include "core/Paths.h"
#include "core/TextTools.h"
#include "core/Database.h"
#include "core/ASyncQuery.h"
#include "core/EntryListDB.h"

#include <QtDebug>
#include <QSemaphore>
#include <QMessageBox>
#include <QQueue>

#define USERDB_REVISION 8

#define ASSERT(Q) if (!(Q)) return false
#define QUERY(Q) if (!query.exec(Q)) return false

QString Database::_userDBFile;
Database *Database::_instance = 0;
QMap<QString, QString> Database::_attachedDBs;

/**
 * Creates the user database. The database file on which
 * this takes place *must* be cleared.
 */

bool Database::createUserDB()
{
	if (!_connection.transaction()) return false;
	SQLite::Query query(&_connection);
	// Versions table
	QUERY("CREATE TABLE versions(id TEXT PRIMARY KEY, version INTEGER)");
	QUERY(QString("INSERT INTO versions VALUES(\"userDB\", %1)").arg(USERDB_REVISION));

	// Study table
	QUERY("CREATE TABLE training(type INT NOT NULL, id INTEGER SECONDARY KEY NOT NULL, score INT NOT NULL, dateAdded UNSIGNED INT NOT NULL, dateLastTrain UNSIGNED INT, nbTrained UNSIGNED INT NOT NULL, nbSuccess UNSIGNED INT NOT NULL, dateLastMistake UNSIGNED INT, CONSTRAINT training_unique_ids UNIQUE(type, id))");
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
	
	// Lists tables
	QUERY("CREATE TABLE lists(parent INTEGER REFERENCES lists, position INTEGER NOT NULL, type INTEGER, id INTEGER)");
	QUERY("CREATE INDEX idx_lists_ref ON lists(parent, position)");
	QUERY("CREATE INDEX idx_lists_entry ON lists(type, id)");
	QUERY("CREATE VIRTUAL TABLE listsLabels using fts3(label)");
	if (!_connection.commit()) return false;
	return true;
}

/// Changes the training table structure to store the date of last training
/// and removes trainingLog
static bool update1to2(SQLite::Query &query) {
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
static bool update2to3(SQLite::Query &query) {
	// Create the sets table
	QUERY("CREATE TABLE sets(parent INT, position INT NOT NULL, label TEXT, state BLOB)");
	QUERY("CREATE INDEX idx_sets_id ON sets(parent, position)");

	return true;
}

/// Remove the score trigger
static bool update3to4(SQLite::Query &query) {
	QUERY("DROP TRIGGER update_score");

	return true;
}

/// Add the lists tables
static bool update4to5(SQLite::Query &query) {
	QUERY("CREATE TABLE lists(parent INTEGER REFERENCES lists, position INTEGER NOT NULL, type INTEGER, id INTEGER)");
	QUERY("CREATE INDEX idx_lists_ref ON lists(parent, position)");
	QUERY("CREATE INDEX idx_lists_entry ON lists(type, id)");
	QUERY("CREATE VIRTUAL TABLE listsLabels using fts3(label)");

	return true;
}

/// Add the versions table, drop info
static bool update5to6(SQLite::Query &query) {
	QUERY("CREATE TABLE versions(id TEXT PRIMARY KEY, version INTEGER)");
	QUERY("INSERT INTO versions VALUES(\"userDB\", 6)");
	QUERY("DROP TABLE info");

	return true;
}

/// Do nothing - this is because some users database version got inadvertedly
/// upgraded one step too much during 0.2.5rc1
static bool update6to7(SQLite::Query &query) {
	return true;
}

/// Reorganize the lists into the new structure
static bool update7to8(SQLite::Query &query) {

	QUERY("ALTER TABLE lists RENAME TO oldLists");

	EntryListDBAccess dbAccess(LISTS_DB_TABLES_PREFIX);
	ASSERT(dbAccess.createTables(query.connection()));
	ASSERT(dbAccess.prepareForConnection(query.connection()));

	// Prepare for the root list
	QQueue<QPair<quint64, quint64> > nextLists;
	QUERY("SELECT oldLists.rowid, type, id, label FROM oldLists JOIN listsLabels ON oldLists.rowid = listsLabels.rowid WHERE parent is null ORDER BY position ASC");
	int listId = 0;
	do {
		EntryList list(&dbAccess, listId);
		while (query.next()) {
			quint64 rowid = query.valueUInt64(0);
	
			EntryListData entryData;
			entryData.type = query.valueUInt(1);
			if (entryData.type != 0) entryData.id = query.valueUInt64(2);
			// If type is zero, this means we have a sub-list - create it and schedule it for migration
			else {
				EntryList subList(&dbAccess, 0);
				subList.newList();
				subList.setLabel(query.valueString(3));
				entryData.id = subList.listId();
				// Associate the old parent id to the new list id
				nextLists << QPair<quint64, quint64>(rowid, subList.listId());
			}
			ASSERT(list.insert(entryData, list.size()));
		}
		// Check that the list is valid
		list.checkValid();
		// Go on to the next list, if any
		if (nextLists.isEmpty()) break;
		QPair<quint64, quint64> next(nextLists.dequeue());
		QUERY(QString("SELECT oldLists.rowid, type, id, label FROM oldLists LEFT JOIN listsLabels ON oldLists.rowid = listsLabels.rowid WHERE parent = %1 ORDER BY position ASC").arg(next.first));
		listId = next.second;
	} while (true);

	//QUERY("DROP TABLE oldLists");
	// Also drop listsLabels!
	//QUERY("CREATE INDEX idx_lists_parent ON lists(parent)");
	//QUERY("CREATE INDEX idx_lists_entry ON lists(type, id)");
	return true;
}

#undef QUERY

bool (*dbUpdateFuncs[USERDB_REVISION - 1])(SQLite::Query &) = {
	&update1to2,
	&update2to3,
	&update3to4,
	&update4to5,
	&update5to6,
	&update6to7,
	&update7to8,
};

void Database::dbWarning(const QString &message)
{
	QMessageBox::warning(0, tr("Tagaini Jisho warning"), message);
}

/**
 * Upgrade the database from the version number given in parameter to
 * the current version.
 */
bool Database::updateUserDB(int currentVersion)
{
	// The database is older than our version of Tagaini - we have to update the database
	if (!_connection.transaction()) return false;
	SQLite::Query query2(&_connection);
	for (; currentVersion < USERDB_REVISION; ++currentVersion) {
		if (!dbUpdateFuncs[currentVersion - 1](query2)) goto failed;
		query2.clear();
	}
	// Update version number
	if (!query2.exec(QString("UPDATE versions SET version=%1 where id=\"userDB\"").arg(USERDB_REVISION))) goto failed;
	if (!_connection.commit()) goto failed;
	return true;
failed:
	_connection.rollback();
	return false;
}

/**
 * Checks if the user DB exists and create it if necessary. In case of failure,
 * backs on a temporary file for the database.
 * @return true if the database exists or have been successfully created; false
 *         if an error occured while building the database.
 */
bool Database::checkUserDB()
{
	int currentVersion;
	SQLite::Query query(&_connection);
	// Try to get the version from the versions table
	query.exec("SELECT version FROM versions where id=\"userDB\"");
	if (query.next()) currentVersion = query.valueInt(0);
	else {
		// No versions table, we have an older version!
		query.exec("SELECT version FROM info");
		if (query.next()) currentVersion = query.valueInt(0);
		else currentVersion = -1;
	}
	query.clear();

	if (currentVersion != -1) {
		if (currentVersion < USERDB_REVISION) {
			if (!updateUserDB(currentVersion)) {
				// Big issue here - start with a temporary database
				dbWarning(tr("Error while upgrading user database: %1").arg(_connection.lastError().message().toLatin1().constData()));
				return false;
			}
		}
		// The database is more recent than our version of Tagaini - there is nothing we can do!
		else if (currentVersion > USERDB_REVISION) {
			dbWarning(tr("Wrong user database version: expected %1, got %2.").arg(USERDB_REVISION).arg(currentVersion));
			return false;
		}
	}
	else {
		if (!createUserDB()) {
			_connection.rollback();
			// Big issue here - start with a temporary database
			dbWarning(tr("Cannot create user database: %1").arg(_connection.lastError().message().toLatin1().constData()));
			return false;
		}
	}
	return true;
}

bool Database::connectUserDB(QString filename)
{
	// Connect to the user DB
	if (filename.isEmpty()) filename = defaultDBFile(); 

	if (!_connection.connect(filename)) {
		dbWarning(tr("Cannot open database: %1").arg(_connection.lastError().message().toLatin1().data()));
		return false;
	}
	if (!checkUserDB()) return false;
	_userDBFile = _connection.dbFileName();
	return true;
}

bool Database::connectToTemporaryDatabase()
{
	_tFile = new QTemporaryFile();
	_tFile->open();
	_tFile->close();
	
	// Now reopen the DB using the temporary file and create a clear database
	_connection.close();
	return connectUserDB(_tFile->fileName());
}

void Database::init(const QString &userDBFile, bool temporary)
{
	_instance = new Database(userDBFile, temporary);
}

void Database::stop()
{
	if (!_instance) return;

	// The query must not live until the end of the method, as the connection is uses
	// will be deleted before.
	{
		SQLite::Query query(&_instance->_connection);

		// Remove unreferenced tags
		if (!query.exec("delete from tags where docid not in (select tagId from taggedEntries)")) qWarning("Could not cleanup unused tags!");

		// VACUUM the database
		if (!query.exec("vacuum")) qWarning("Final VACUUM failed %s", query.lastError().message().toLatin1().data());
	}
	// Close the database
	_instance->_connection.close();
	delete _instance;
	_instance = 0;
}

static void regexpFunc(sqlite3_context *context, int argc, sqlite3_value **argv)
{
	QString text(TextTools::hiragana2Katakana(QString::fromUtf8((const char *)sqlite3_value_text(argv[1]))));
	// Get the regexp referenced by the request
	QRegExp &regexp = Database::staticRegExps[sqlite3_value_int(argv[0])];

	bool res = text.contains(regexp);
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

static void load_extensions(sqlite3 *handler)
{
	// Attach custom functions
	sqlite3_create_function(handler, "regexp", 2, SQLITE_UTF8, 0, regexpFunc, 0, 0);
	sqlite3_create_function(handler, "biaised_random", 1, SQLITE_UTF8, 0, biaised_random, 0, 0);
	sqlite3_create_function(handler, "uniquecount", -1, SQLITE_UTF8, 0, 0, uniquecount_aggr_step, uniquecount_aggr_finalize);
	// Must be done later
	//register_all_tokenizers(handler);
}

Database::Database(const QString &userDBFile, bool temporary) : _tFile(0)
{
	sqlite3_auto_extension((void (*)())load_extensions);
	
	// Temporary database explicitly required or cannot connect to user DB:
	// Switch to the temporary database
	if (temporary || !connectUserDB(userDBFile)) {
		if (!connectToTemporaryDatabase()) {
			dbWarning(tr("Temporary database fallback failed. The program will now exit."));
			qFatal("All database fallbacks failed, exiting...");
		} else if (!temporary) {
			dbWarning(tr("Tagaini is working on a temporary database. This allows the program to work, but user data is unavailable and any change will be lost upon program exit. If you corrupted your database file, please recreate it from the preferences."));
		}
	}
}

Database::~Database()
{
	if (_tFile) delete _tFile;
}

QVector<QRegExp> Database::staticRegExps;

/**
 * Attach the dictionary DB to the opened user database.
 * @return true if the dictionary DB has successfully been attached; false
 *         in an error occured.
 */
bool Database::attachDictionaryDB(const QString &file, const QString &alias, int expectedVersion)
{
#define QUERY(Q) if (!query.exec(Q)) goto error
	SQLite::Query query(&instance()->_connection);
	// Try to attach the dictionary DB
	QUERY("attach database '" + file + "' as " + alias);

	// Check the version is compatible
	QUERY("select version from " + alias + ".info");
	// No result, not good
	if (!query.next()) goto error;
	if (query.valueInt(0) != expectedVersion) goto errorDetach;
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
	qCritical() << QString("Failed to attach database: %1").arg(query.lastError().message());
	qCritical() << QString("Attached dictionary file was %1").arg(file);
	return false;
}

bool Database::detachDictionaryDB(const QString &alias)
{
	SQLite::Query query(&instance()->_connection);
	if (!query.exec("detach database " + alias)) {
		qCritical() << QString("Failed to attach database: %2").arg(query.lastError().message());
		return false;
	}
	_attachedDBs.remove(alias);
	return true;
}
