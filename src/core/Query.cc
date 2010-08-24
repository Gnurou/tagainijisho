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

#include "tagaini_config.h"
#include "core/Database.h"
#include "core/Query.h"
#include "core/EntriesCache.h"

#include <QtDebug>
#include <QtAlgorithms>
#include <QMutex>
#include <QMutexLocker>

Query::Query(QObject *parent) : QObject(parent), query(Database::connection()), _status(Idle), _statusMutex()
{
	// Events for this object should be handled
	// by the database event handler
	if (Database::isThreaded()) {
		moveToThread(Database::instance());
	}

	// This signal makes the query switch into the DB
	// thread to be executed
	connect(this, SIGNAL(runInDBThread(int, int)),
		this, SLOT(__fetch(int, int)));
}

bool Query::isRunning() const
{
	_statusMutex.lock();
	bool res = _status != Idle;
	_statusMutex.unlock();
	return res;
}

void Query::fetch(int min, int nb)
{
	emit runInDBThread(min, nb);
}

void Query::prepare(const QueryBuilder &builder)
{
	abort();
	_statement = builder.buildSqlStatement();
	_countStatement = builder.buildCountSqlStatement();
	_limit = builder.limit();
}


void Query::__fetch(int min, int nb)
{
	_statusMutex.lock();
	Q_ASSERT(_status == Idle);
	_statusMutex.unlock();

	bool firstRun = false;

	if (min == -1) nb = -1;
	// Query not active? Let's run it then.
	if (!query.active()) {
		firstRun = true;
		_statusMutex.lock();
		_status = Running;
		_statusMutex.unlock();
#ifdef DEBUG_QUERIES
		qDebug() << _statement;
#endif
		bool success = query.exec(_statement);

		_statusMutex.lock();
		// Check if we were aborted when in Running status
		bool shallAbort = checkAbort();

		// We have not been aborted, so if the query did not return
		// successfully this means we had an error.
		if (!shallAbort && !success) {
			//QSqlError err = query.lastError();
			//qDebug() << "Error executing query:" << err << __FILE__ << __LINE__;
			finish();
			_status = Idle;
			qDebug("Error executing query %s: %s", _statement.toLatin1().data(), query.lastError().message().toLatin1().data());
			emit error();
		}

		_statusMutex.unlock();
		// Wake up threads that asked to interrupt the query
		// while it was in Running status, if any.
		_dbTaskInterrupted.wakeAll();

		// If we were interrupted or we had an error, no need to continue
		if (shallAbort || !success)  {
			query.clear();
			Database::sqliteFix();
			return;
		}
	}
	_statusMutex.lock();
	_status = Emitting;
	_statusMutex.unlock();

	// Go to the first result
	if (firstRun) {
		// TODO BUG in QT? query.seek(min) returns false if there is only one result
		// and and min == 0 unless query.next() is called first. This looks
		// like a bug in Qt since it works if there is more than one result.
		query.next();
	}
	if (min == -1) min = 0;
	int resCpt = 0;
	_currentPos = min;
//	bool valid = query.seek(min);
	bool valid = true;

	emit firstResult();
	while (valid && resCpt++ != nb) {
		// Stop emitting results if we are interrupted
		QMutexLocker statusLocker(&_statusMutex);
		bool shallAbort = checkAbort();
		statusLocker.unlock();
		if (shallAbort) {
			query.clear();
			Database::sqliteFix();
			_dbTaskInterrupted.wakeAll();
			return;
		}
		// Load and emit found entries
		emit foundEntry(EntryRef(query.valueUInt(0), query.valueUInt(1)).get());
		valid = query.next();
		_currentPos++;
	}

	// Emit the number of results in this query
	if (firstRun) {
		// If we browsed everything already, things are simple
		if (!valid) emit nbResults(resCpt);
		else emitNbResults();
	}
	emit lastResult();
	_statusMutex.lock();
	_status = Idle;
	_statusMutex.unlock();
	_dbTaskInterrupted.wakeAll();
}

void Query::finish()
{
	query.clear();
}

void Query::emitNbResults()
{
	SQLite::Query countQuery(Database::connection());

	if (!countQuery.exec(_countStatement)) qCritical() << "Error executing query: " << _countStatement << countQuery.lastError().message();
	unsigned int cpt = 0;
	while (countQuery.next()) cpt += countQuery.valueInt(0);
	if (_limit.active()) {
		cpt = qMin(cpt, _limit.nbResults());
	}
	emit nbResults(cpt);
}

bool Query::checkAbort()
{
	if (_status == Interrupted) {
		finish();
		emit aborted();
		_status = Idle;
		return true;
	}
	return false;
}

void Query::abort()
{
	_statusMutex.lock();
	// Database query running, must abort query
	if (_status == Running) {
		_status = Interrupted;
		query.connection()->interrupt();
		// Wait for the database thread to REALLY have interrupted the query
		_dbTaskInterrupted.wait(&_statusMutex);
	}
	// If emitting results, just changing the status will be enough to stop things
	else if (_status == Emitting) {
		_status = Interrupted;
		_dbTaskInterrupted.wait(&_statusMutex);
	}
	Database::sqliteFix();
	_statusMutex.unlock();
}

void Query::clear()
{
	_statement.clear();
	_countStatement.clear();
	_limit = QueryBuilder::Limit();
	query.clear();
	_status = Idle;
}

