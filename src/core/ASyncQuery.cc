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

#include "core/ASyncQuery.h"
#include "core/Database.h"
#include "core/Paths.h"

#include <QtDebug>

#include <QMutex>
#include <QMutexLocker>

ASyncQuery::ASyncQuery(DatabaseThread *dbThread)
    : _dbConn(dbThread->connection()), _query(&_dbConn->_connection), _active(false) {
    // Move to database thread
    moveToThread(dbThread);

    // Set up connection for executing queries in the started thread
    QObject::connect(this, SIGNAL(queryReady()), _dbConn, SLOT(processQueries()));
}

ASyncQuery::~ASyncQuery() {
    // Make sure the query is stopped when we die
    abort();
}

bool ASyncQuery::exec(const QString &qString) {
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

void ASyncQuery::process() {
    // This method is run by the database connection thread.
    // Therefore we are sure that there is no running query when
    // we enter it.

    Q_ASSERT(_active == true);
    Q_ASSERT(_dbConn->_activeQuery == this);
    // Aborted already??
    if (_dbConn->_abortCurrentQuery)
        goto process_abort;

    // Run the query
    if (!_query.exec(_currentQuery)) {
        // Got error code - check if it was a real error or if we were just
        // interrupted
        if (_dbConn->_abortCurrentQuery || _query.lastError().isInterrupted()) {
            /*			if (!_dbConn->_abortCurrentQuery &&
               _query.lastError().number() == SQLITE_INTERRUPT) qDebug() <<
               "WARNING: query interrupted but _abortCurrentQuery not set!";*/
            goto process_abort;
        } else {
            _active = false;
            emit error(_query.lastError().message());
            goto process_end;
        }
    }
    if (_query.next()) {
        if (_dbConn->_abortCurrentQuery)
            goto process_abort;
        emit firstResult();
        do {
            // Wrap the results into a list of QVariants
            QList<QVariant> record;
            int colCount = _query.columnsCount();
            for (int i = 0; i < colCount; ++i) {
                QVariant value;
                switch (_query.valueType(i)) {
                case SQLite::Integer:
                    value = _query.valueInt64(i);
                    break;
                case SQLite::Float:
                    value = _query.valueDouble(i);
                    break;
                case SQLite::String:
                    value = _query.valueString(i);
                    break;
                case SQLite::Blob:
                    value = _query.valueBlob(i);
                    break;
                default:
                    break;
                }

                record << value;
            }
            emit result(record);
            // Have we been interrupted while emiting of
            // results?
            if (_dbConn->_abortCurrentQuery) {
                goto process_abort;
            }
        } while (_query.next());
    }
    _active = false;
    _query.clear();
    emit completed();
    return;

process_abort:
    _active = false;
    _query.clear();
    emit aborted();
    return;

process_end:
    _query.clear();
}

bool ASyncQuery::abort() {
    if (!active())
        return false;
    // Remove our instance from the waiting queue, if any
    _dbConn->_waitingQueueMutex.lock();
    _dbConn->_waitingQueue.removeAll(this);
    _dbConn->_waitingQueueMutex.unlock();
    // Stop our execution if we are running;
    _dbConn->_abortRunningQuery(this);
    // Here we are sure we are not running
    Q_ASSERT(_dbConn->_activeQuery != this);
    _query.clear();
    _active = false;
    return true;
}

bool ASyncQuery::active() { return _active; }

ThreadedDatabaseConnection::ThreadedDatabaseConnection()
    : _waitingQueue(), _waitingQueueMutex(), _activeQuery(0), _queryInProgressMutex(),
      _abortCurrentQuery(false) {}

ThreadedDatabaseConnection::~ThreadedDatabaseConnection() {
    // Clear queries queue. As we keep this mutex, no
    // other query will ever be executed.
    QMutexLocker queueLocker(&_waitingQueueMutex);
    _waitingQueue.clear();
    // Ensure the running query is aborted
    while (_activeQuery)
        _abortRunningQuery(_activeQuery);
    // And make really sure by locking _queryInProgress
    QMutexLocker queryInProgressLock(&_queryInProgressMutex);

    _connection.close();
}

bool ThreadedDatabaseConnection::connect(const QString &dbFile) {
    if (!_connection.connect(dbFile)) {
        qWarning("Cannot open database: %s", _connection.lastError().message().toLatin1().data());
        return false;
    }
    return true;
}

bool ThreadedDatabaseConnection::attach(const QString &dbFile, const QString &alias) {
    if (!_connection.attach(dbFile, alias)) {
        qWarning("Failed to attach dictionary file %s: %s", dbFile.toLatin1().data(),
                 _connection.lastError().message().toLatin1().data());
        return false;
    }
    return true;
}

bool ThreadedDatabaseConnection::detach(const QString &alias) {
    if (!_connection.detach(alias)) {
        qWarning("Failed to detach database %s: %s", alias.toLatin1().data(),
                 _connection.lastError().message().toLatin1().data());
        return false;
    }
    return true;
}

void ThreadedDatabaseConnection::processQueries() {
    QMutexLocker queueLocker(&_waitingQueueMutex);
    if (_waitingQueue.isEmpty())
        return;
    QMutexLocker queryInProgressLocker(&_queryInProgressMutex);
    // Only set _activeQuery when queryInProgressMutex is acquired
    _activeQuery = _waitingQueue.dequeue();
    queueLocker.unlock();
    Q_ASSERT(_activeQuery != 0);
    Q_ASSERT(_activeQuery->active() == true);

    _activeQuery->process();
    _activeQuery = 0;
}

void ThreadedDatabaseConnection::_abortRunningQuery(ASyncQuery *query) {
    // Block any new incoming queries from being executed while we stop the
    // current one
    QMutexLocker waitingQueueLocker(&_waitingQueueMutex);
    // The running query is not the one we want to stop, never mind!
    if (query == 0 || _activeQuery != query)
        return;
    // The running query is stopped when we can lock _queryInProgress
    if (!_queryInProgressMutex.tryLock()) {
        _abortCurrentQuery = true;
        // Interrupt the running query - if running. The while is to ensure that
        // the DB process is actually within sqlite3_exec(), for the case we
        // called abort() before that happens.
        while (!_queryInProgressMutex.tryLock(2))
            _connection.interrupt();
    }
    // The current query is now stopped
    _abortCurrentQuery = false;
    // At this point the DB thread is done and we should be back to ready.
    Q_ASSERT(_activeQuery == 0);
    _queryInProgressMutex.unlock();
}

QSet<DatabaseThread *> DatabaseThread::_instances;
DatabaseThread::DatabaseThread() : _startSem(0) {
    // Start the thread
    QThread::start();
    // Wait until the thread is ready to perform
    _startSem.acquire();
}

DatabaseThread::~DatabaseThread() {
    // Wait for all ongoing operations in the child thread to be finished
    QThread::quit();
    QThread::wait();

    // Remove from instances list
    _instances.remove(this);

    delete _connection;
}

void DatabaseThread::run() {
    // Create our connection in the thread space
    _connection = new ThreadedDatabaseConnection();

    // Connect to the main database
    connection()->connect(Database::instance()->userDBFile());

    // Attach all databases
    const QMap<QString, QString> &dbsToAttach(Database::attachedDBs());
    foreach (const QString &alias, dbsToAttach.keys()) {
        connection()->attach(dbsToAttach[alias], alias);
    }

    // Add to instances list
    _instances << this;
    // Let the main thread continue
    _startSem.release();

    // And enter event loop
    QThread::exec();
}
