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

#include "sqlite3.h"
#include "sqlite/SQLite.h"
#include "sqlite/Connection.h"
#include "tagaini_config.h"

#include <QtDebug>
#include <QMutexLocker>

using namespace SQLite;

Connection::Connection() : _handler(0)
{
#ifdef DEBUG_TRANSACTIONS
	_tr_count = 0;
#endif
}

Connection::~Connection()
{
	if (connected()) close();
}

const Error &Connection::updateError() const
{
	_lastError = Error(*this);
#ifdef DEBUG_QUERIES
	if (_lastError.isError()) {
		qDebug("Query error: %s", _lastError.message().toUtf8().constData());
	}
#endif
	return _lastError;
}

bool Connection::connect(const QString &dbFile, OpenFlags flags)
{
	if (connected()) {
		_lastError = Error(-1, "already connected to a database");
		return false;
	}

	int res = sqlite3_open_v2(dbFile.toUtf8().data(), &_handler, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
	updateError();
	if (res != SQLITE_OK) goto err;
	// Enable extended error codes
	sqlite3_extended_result_codes(_handler, 1);
	// Try to enable shared cache
	sqlite3_enable_shared_cache(1);
	// Set busy timeout to 20 seconds - this should be more than enough
	sqlite3_busy_timeout(_handler, 20000);
	// Register our tokenizers (extensions can be handled by SQLite's auto mechanism, not tokenizers
	// as they need the connection to be opened
	   sqlite3ext_register_tokenizers(_handler);
	// Configure the connection
	exec("pragma encoding=\"UTF-16le\"");
	if (!flags & JournalInFile) exec("pragma journal_mode=MEMORY");

	_dbFile = dbFile;
	return true;

err:
	if (_handler) {
		sqlite3_free(_handler);
		_handler = 0;
	}
	return false;
}

bool Connection::close()
{
#ifdef DEBUG_TRANSACTIONS
	qDebug("Closing connection %p with #%d remaining transactions", this, _tr_count);
#endif
	if (!connected()) {
		// Will put the appropriate error (library routine called out of sequence)
		// because the handler is null
		updateError();
		return false;
	}

	int res = sqlite3_close(_handler);
	if (res != SQLITE_OK) {
		updateError();
		return false;
	}
	// Handle is invalid, so we cannot use updateError to get the error
	_lastError = Error();
	_handler = 0;
	_dbFile.clear();
	return true;
}

bool Connection::attach(const QString &dbFile, const QString &alias)
{
	return exec(QString("attach database '%1' as %2").arg(dbFile).arg(alias));
}

bool Connection::detach(const QString &alias)
{
	return exec(QString("detach database %1").arg(alias));
}

const Error &Connection::lastError() const
{
	return _lastError;
}

bool Connection::exec(const QString &statement)
{
	sqlite3_stmt *stmt;
	int res = sqlite3_prepare_v2(_handler, statement.toUtf8().data(), -1, &stmt, 0);
	if (res != SQLITE_OK) {
		updateError();
#ifdef DEBUG_QUERIES
	       if (_lastError.isError())
			qDebug("On query: %s", statement.toUtf8().constData());
#endif
		return false;
	}
	res = sqlite3_step(stmt);
	updateError();
	sqlite3_finalize(stmt);
	return !_lastError.isError();
}

bool Connection::transaction()
{
	bool res = exec("savepoint spoint");
#ifdef DEBUG_TRANSACTIONS
	if (res) qDebug("Transaction #%d started on connection %p", _tr_count++, this);
	else qDebug("Failed to start transaction #%d on connection %p", _tr_count, this);
#endif
	return res;
}

bool Connection::commit()
{
	bool res = exec("release spoint");
#ifdef DEBUG_TRANSACTIONS
	if (res) qDebug("Transaction #%d commited on connection %p", --_tr_count, this);
	else qDebug("Failed to commit transaction #%d on connection %p", _tr_count - 1, this);
#endif
	return res;
}

bool Connection::rollback()
{
	bool res = exec("rollback to spoint;");
#ifdef DEBUG_TRANSACTIONS
	if (res) qDebug("Rollbacking transaction #%d on connection %p (commit will follow)", _tr_count - 1, this);
	else qDebug("Failed to rollback transaction #%d on connection %p", _tr_count - 1, this);
#endif
	if (!res) return false;
	res = commit();
	return res;
}

void Connection::interrupt()
{
	sqlite3_interrupt(_handler);
}
