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

#ifndef __QUERY_H_
#define __QUERY_H_

#include "core/EntriesCache.h"
#include "core/QueryBuilder.h"

#include <QString>
#include <QList>
#include <QHash>
#include <QMutex>

#include "sqlite/Query.h"
#include <QWaitCondition>

/**
 * Builds a database query incrementally and generates the corresponding
 * SQL statement when ready. Constraints like where selectors and table
 * joins can be added and removed easily in a structured way.
 */
class Query : public QObject
{
	Q_OBJECT

private:
	SQLite::Query query;
	/// The query currently being executed
	QString _statement;
	/// The statement to obtain the results count of the current query
	QString _countStatement;
	/// The maximum number of results to return - necessary to return the correct count.
	QueryBuilder::Limit _limit;
	int _currentPos;

	/// Current status of this query
	enum { Idle, Running, Emitting, Interrupted } _status;
	mutable QMutex _statusMutex;
	QWaitCondition _dbTaskInterrupted;

protected:
	/**
	 * Emit a signal with the number of results for the current query.
	 */
	void emitNbResults();

	/**
	 * Check whether the query has been interrupted - if it has,
	 * clear the database state, emit aborted() signal, restore
	 * status to Idle and return true. Otherwise, return false.
	 */
	bool checkAbort();

public:
	Query(QObject *parent = 0);

	int currentPos() const { return _currentPos; }
	bool isRunning() const;

public slots:
	/**
	 * Prepares the query to be executed.
	 */
	void prepare(const QueryBuilder &builder);

	/**
	 * Fetch results corresponding to the range min..min+nb.
	 * If min = -1, all the results are fetched. The foundResult
	 * signal is emitted for each result found with the
	 * corresponding row. After the last result is sent, the
	 * lastResult signal is emitted. If this is the first time
	 * this query is used, the nbResults signal is emitted to
	 * indicate the total number of results (without regard for
	 * the selected range).
	 */
	void fetch(int min = -1, int nb = -1);

	/**
	 * Finish the current query, i.e. the next call to fetch
	 * will run it again.
	 */
	void finish();

	/**
	 * Resets the whole query to its post-constructor state.
	 */
	void clear();

	void abort();

private slots:
	/**
	 * Actual implementation of the fetch method, run from the DB
	 * thread after the switch triggered by emitting the runInDBThread
	 * signal. Results are actually emitted from here.
	 */
	void __fetch(int min, int nb);

signals:
	/// Emitted when a result is found
	void foundEntry(EntryPointer entry);
	/// Automatically emitted when we are able to figure out the number of results
	void nbResults(unsigned int nbResults);
	/// Emitted before the first result is sent
	void firstResult();
	/// Emitted after the last result has been sent
	void lastResult();
	/// Emitted when the query has been aborted during execution
	void aborted();
	/// Emitted when an error has been encountered running the query
	void error();

	/// Private signal - emitted to switch to the DB thread
	/// to execute a database query.
	void runInDBThread(int min, int nb);
};

#endif
