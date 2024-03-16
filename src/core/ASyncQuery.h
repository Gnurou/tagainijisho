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

#ifndef __CORE_ASYNCQUERY_H_
#define __CORE_ASYNCQUERY_H_

#include "sqlite/Connection.h"
#include "sqlite/Error.h"
#include "sqlite/Query.h"

#include <QMutex>
#include <QQueue>
#include <QSemaphore>
#include <QSet>
#include <QThread>

class DatabaseThread;
class ThreadedDatabaseConnection;
struct sqlite3;

/**
 * Asynchronous Query - provides SQL connections that execute queries in a
 * separate thread and notify of results through signals.
 */
class ASyncQuery : public QObject {
    Q_OBJECT
  private:
    ThreadedDatabaseConnection *_dbConn;
    SQLite::Query _query;
    /// Whether the query is executing or has a pending execution
    bool _active;
    QString _currentQuery;

  public:
    ASyncQuery(DatabaseThread *dbConn);
    virtual ~ASyncQuery();
    /// Returns the last error raised by this query
    const SQLite::Error &lastError() { return _query.lastError(); }

    /**
     * Starts running the query given as argument. The ASyncQuery will emit
     * the following signals to notify of results, in that order:
     * - firstResult() to notify that the first result will arrive
     * - result(const QSqlResult &) to send all results one by one
     * One of the following termination signals:
     * - completed() to notify all the results were sent without any problem.
     * - aborted() to notify the query has been manually aborted.
     * - error() to notify an error occured.
     */
    bool exec(const QString &qString);

    /**
     * Abort the query, provided it is being executed.
     * When this method returns, it is guaranteed that the query
     * is not running, no matter what its state was when it was
     * called. This method return true if the query was running
     * and had been interrupted, and false if the query was not
     * running when being called.
     *
     * Warning: even though it is guaranteed the query is not
     * running after this method exists, it is still possible
     * that results it has emitted has not yet been processed
     * by the receiving thread, and therefore will raise the
     * connected slots when the receiving thread enters its
     * event loop again (i.e. after this method call). The
     * user should pay particular attention to this.
     */
    bool abort();

    bool active();

  public slots:
    /**
     * Actual query process function that is called from within
     * the database thread. Can be called manually for synchronous
     * execution.
     */
    void process();

  signals:
    /// Used internally to transfer from user to database thread
    void queryReady();
    /// Emitted right before the first result of the query
    void firstResult();
    /// Emits one of the results of the query
    void result(const QList<QVariant> &result);
    /// Emitted after the last result of the query
    void completed();
    /// Emitted if the query has been aborted
    void aborted();
    /// Emitted if an error occured
    void error(const QString &error);
};

/**
 * Provides a database connection that runs in a separate thread,
 * in order to run ASyncQuery instances.
 *
 * Upon creation, instances of this class starts a new thread dedicated
 * to executing database queries that lives until the instance is destroyed.
 * Instances of ASyncQuery that are given an instance of
 * ThreadedDatabaseConnection at construction time will be dependent on that
 * ThreadedDatabaseConnection in the following respect:
 * - The ASyncQuery object will be moved to the ThreadedDatabaseConnection's
 * thread
 * - The ASyncQuery queries will be performed within the
 * ThreadedDatabaseConnection's thread
 * - The ASyncQuery will be interrupted if the ThreadedDatabaseConnection's
 * abort() function is called.
 */
class ThreadedDatabaseConnection : public QObject {
    Q_OBJECT
    // Needed as ASyncQuery tightly interacts with us
    // (migrates to our thread, uses _database, ...)
    friend class ASyncQuery;
    friend class DatabaseThread;

  private:
    SQLite::Connection _connection;

    /// Queue of queries waiting to be executed
    QQueue<ASyncQuery *> _waitingQueue;
    QMutex _waitingQueueMutex;

    /// Pointer to the currently executing query
    ASyncQuery *_activeQuery;
    /// Hold by the DB thread whenever a query is active.
    /// Used to ensure an interrupted query has actually
    /// been completely interrupted.
    QMutex _queryInProgressMutex;
    /// Whenever the user requests the current query to be
    /// aborted, this boolean is set to true.
    bool _abortCurrentQuery;

    /**
     * Abort the currently running query, if any. When this function returns,
     * the query is already terminated. This function only terminates the
     * current query - if there are other queries waiting in the message queue,
     * they start being processed immediatly.
     * @arg query The query we want to stop. If the currently running query is
     * not the one given as parameter, nothing is done.
     */
    void _abortRunningQuery(ASyncQuery *query);

    /**
     * Only DatabaseThread can create instances of us.
     */
    ThreadedDatabaseConnection();

  public:
    virtual ~ThreadedDatabaseConnection();

    /**
     * Connect to the database file given as parameter. Returns true in case
     * of success, false otherwise.
     */
    bool connect(const QString &dbFile);
    /**
     * Connect the database file given as parameter to alias. Returns true
     * in case of success, false otherwise.
     */
    bool attach(const QString &dbFile, const QString &alias);
    bool detach(const QString &alias);
    /// Returns the last error that happened on this connection
    const SQLite::Error &lastError() { return _connection.lastError(); }

  public slots:
    /**
     * Process all the queries that are in the waiting queue.
     */
    virtual void processQueries();
};

class DatabaseThread : public QThread {
    Q_OBJECT
  private:
    /// List of instances, to attach databases dynamically
    static QSet<DatabaseThread *> _instances;
    /// Used to synchronize threads during construction
    QSemaphore _startSem;
    /// The object that will live in our thread space and
    /// receive messages
    ThreadedDatabaseConnection *_connection;

  protected:
    virtual void run();

  public:
    DatabaseThread();
    virtual ~DatabaseThread();
    ThreadedDatabaseConnection *connection() { return _connection; }

    static const QSet<DatabaseThread *> &instances() { return _instances; }
};

#endif /* ASYNCQUERY_H_ */
