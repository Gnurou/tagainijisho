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

#include "core/Paths.h"
#include "core/ASyncQuery.h"
#include "core/Database.h"

#include <QtDebug>

#include <QMutex>
#include <QMutexLocker>

int ThreadedDatabaseConnection::_conCpt = 0;

static QMutex _connCptMutex;

ASyncQuery::ASyncQuery(DatabaseThread *dbThread) : _dbConn(dbThread->connection()), _query(_dbConn->_connection), _active(false)
{
	// Move to database thread
	moveToThread(dbThread);

	// Set up connection for executing queries in the started thread
	QObject::connect(this, SIGNAL(queryReady()), _dbConn, SLOT(processQueries()));
}

ASyncQuery::~ASyncQuery()
{
	// Make sure the query is stopped when we die
	abort();
}

bool ASyncQuery::exec(const QString &qString)
{
	// Add us to the connection waiting queue, unless we
	// are already active
	if (!_active) {
		_currentQuery = qString;
		_active = true;
		_dbConn->_waitingQueueMutex.lock();
		_dbConn->_waitingQueue << this;
		_dbConn->_waitingQueueMutex.unlock();
		// Inform the thread that it has stuff to process!
		emit queryReady();
		return true;
	}
	return false;
//	emit execQuery(qString);
}

void ASyncQuery::process()
{
	// This method is run by the database connection thread.
	// Therefore we are sure that there is no running query when
	// we enter it.

	Q_ASSERT(_active == true);
	Q_ASSERT(_dbConn->_activeQuery == this);
	// Aborted already??
	if (_dbConn->_abortCurrentQuery) goto process_abort;

	// Run the query
	if (!_query.exec(_currentQuery)) {
		// Got error code - check if it was a real error or if we were just interrupted
		if (_dbConn->_abortCurrentQuery || _query.lastError().number() == SQLITE_INTERRUPT) {
/*			if (!_dbConn->_abortCurrentQuery && _query.lastError().number() == SQLITE_INTERRUPT)
				qDebug() << "WARNING: query interrupted but _abortCurrentQuery not set!";*/
			goto process_abort;
		}
		else {
			_active = false;
			emit error(_query.lastError().text());
			goto process_end;
		}
	}
	if (_query.next()) {
		if (_dbConn->_abortCurrentQuery) goto process_abort;
		emit firstResult();
		do {
			emit result(_query.record());
			// Have we been interrupted while emiting of
			// results?
			if (_dbConn->_abortCurrentQuery) {
				goto process_abort;
			}
		} while(_query.next());
	}
	_active = false;
	emit completed();
	goto process_end;

process_abort:
	_active = false;
	emit aborted();

process_end:
	_query.finish();
}

bool ASyncQuery::abort()
{
	if (!active()) return false;
	// Remove our instance from the waiting queue, if any
	_dbConn->_waitingQueueMutex.lock();
	_dbConn->_waitingQueue.removeAll(this);
	_dbConn->_waitingQueueMutex.unlock();
	// Stop our execution if we are running;
	_dbConn->_abortRunningQuery(this);
	// Here we are sure we are not running
	Q_ASSERT(_dbConn->_activeQuery != this);
	_query.finish();
	_active = false;
	return true;
}

bool ASyncQuery::active()
{
	return _active;
}

ThreadedDatabaseConnection::ThreadedDatabaseConnection() : _waitingQueue(), _waitingQueueMutex(), _activeQuery(0), _queryInProgressMutex(), _abortCurrentQuery(false)
{
	// Create a new database connection and add it
	_driver = new QSQLiteDriver();
	_connCptMutex.lock();
	_connectionName = "sqlite" + _conCpt++;
	_connCptMutex.unlock();
	_connection = QSqlDatabase::addDatabase(_driver, _connectionName);
}

ThreadedDatabaseConnection::~ThreadedDatabaseConnection()
{
	// Clear queries queue. As we keep this mutex, no
	// other query will ever be executed.
	QMutexLocker queueLocker(&_waitingQueueMutex);
	_waitingQueue.clear();
	// Ensure the running query is aborted
	while (_activeQuery) _abortRunningQuery(_activeQuery);
	// And make really sure by locking _queryInProgress
	QMutexLocker queryInProgressLock(&_queryInProgressMutex);

	// Close the database and remove the connection
	_connection = QSqlDatabase();
	QSqlDatabase::removeDatabase(_connectionName);
}

bool ThreadedDatabaseConnection::connect(const QString &dbFile)
{
	_connection.setDatabaseName(dbFile);
	if (!_connection.open()) {
		qWarning("Cannot open database: %s", _connection.lastError().text().toLatin1().data());
		return false;
	}
	QVariant handler = _driver->handle();
	if (handler.isValid() && !qstrcmp(handler.typeName(), "sqlite3*")) {
		_handler = *static_cast<sqlite3 **>(handler.data());
		register_all_tokenizers(_handler);
	}
	else {
		qWarning("Cannot fetch sqlite3 handler - will most probably crash when a query is interrupted...");
		_handler = 0;
	}
	_connection.exec("pragma journal_mode=MEMORY");
	_connection.exec("pragma encoding=\"UTF-16le\"");
	return true;
}

bool ThreadedDatabaseConnection::attach(const QString &dbFile, const QString &alias)
{
	QSqlQuery query(_connection);
	if (!query.exec(QString("attach database '%1' as %2").arg(dbFile).arg(alias))) {
		qWarning("Failed to attach dictionary file %s: %s", dbFile.toLatin1().data(), query.lastError().text().toLatin1().data());
		return false;
	}
	return true;
}

bool ThreadedDatabaseConnection::detach(const QString &alias)
{
	QSqlQuery query(_connection);
	if (!query.exec(QString("detach database %1").arg(alias))) {
		qWarning("Failed to detach database %s: %s", alias.toLatin1().data(), query.lastError().text().toLatin1().data());
		return false;
	}
	return true;
}

void ThreadedDatabaseConnection::processQueries()
{
	QMutexLocker queueLocker(&_waitingQueueMutex);
	if (_waitingQueue.isEmpty()) return;
	QMutexLocker queryInProgressLocker(&_queryInProgressMutex);
	// Only set _activeQuery when queryInProgressMutex is acquired
	_activeQuery = _waitingQueue.dequeue();
	queueLocker.unlock();
	Q_ASSERT(_activeQuery != 0);
	Q_ASSERT(_activeQuery->active() == true);

	_activeQuery->process();
	_activeQuery = 0;
}

/*
 * The code for fixing the SQLITE_INTERRUPT bug is here. The commented function should be
 * put at the end of sqlite3.c.
 */
extern "C" {
void tagaini_sqlite3_fix_activevdbecnt(sqlite3 *db);
}
/*
SQLITE_API void tagaini_sqlite3_fix_activevdbecnt(sqlite3 *db)
{
    db->activeVdbeCnt = 0;
}
*/

void ThreadedDatabaseConnection::_abortRunningQuery(ASyncQuery *query)
{
	// Block any new incoming queries from being executed while we stop the current one
	QMutexLocker waitingQueueLocker(&_waitingQueueMutex);
	// The running query is not the one we want to stop, never mind!
	if (query == 0 || _activeQuery != query) return;
	// The running query is stopped when we can lock _queryInProgress
	if (!_queryInProgressMutex.tryLock()) {
		_abortCurrentQuery = true;
		// Interrupt the running query - if running. The while is to ensure that the
		// DB process is actually within sqlite3_exec(), for the case we called abort()
		// before that happens.
		while (!_queryInProgressMutex.tryLock(2))
			sqlite3_interrupt(_handler);
	}
	// The current query is now stopped
	_abortCurrentQuery = false;
	// Try to workaround what appears to be a sqlite bug... :(
	tagaini_sqlite3_fix_activevdbecnt(_handler);
	// At this point the DB thread is done and we should be back to ready.
	Q_ASSERT(_activeQuery == 0);
	_queryInProgressMutex.unlock();
}

QSet<DatabaseThread *> DatabaseThread::_instances;
DatabaseThread::DatabaseThread() : _startSem(0)
{
	// Start the thread
	QThread::start();
	// Wait until the thread is ready to perform
	_startSem.acquire();

	// Connect to the main database
	connection()->connect(Database::instance()->userDBFile());

	// Attach all databases
	const QMap<QString, QString> &dbsToAttach(Database::attachedDBs());
	foreach (const QString &alias, dbsToAttach.keys()) {
		connection()->attach(dbsToAttach[alias], alias);
	}

	// Add to instances list
	_instances << this;
}

DatabaseThread::~DatabaseThread()
{
	// Wait for all ongoing operations in the child thread to be finished
	QThread::quit();
	QThread::wait();

	// Remove from instances list
	_instances.remove(this);

	delete _connection;
}

void DatabaseThread::run()
{
	// Create our connection in the thread space
	_connection = new ThreadedDatabaseConnection();

	// Let the main thread continue
	_startSem.release();

	// And enter event loop
	QThread::exec();
}
