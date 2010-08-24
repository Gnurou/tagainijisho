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
#include "sqlite3.h"

#include <QtDebug>
#include <QMutexLocker>

using namespace SQLite;

Connection::Connection() : _handler(0)
{
}

Connection::~Connection()
{
	if (connected()) close();
}

const Error &Connection::updateError() const
{
	_lastError = Error(*this);
	return _lastError;
}

bool Connection::connect(const QString &dbFile)
{
	if (connected()) {
		_lastError = Error(-1, "Already connected to a database");
		return false;
	}

	int res = sqlite3_open_v2(dbFile.toUtf8().data(), &_handler, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
	updateError();
	if (res != SQLITE_OK) goto err;
	_dbFile = dbFile;
	return true;

err:
	if (_handler) sqlite3_free(_handler);
	return false;
}

bool Connection::close()
{
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
		return false;
	}
	res = sqlite3_step(stmt);
	updateError();
	sqlite3_finalize(stmt);
	return !_lastError.isError();
}

bool Connection::transaction()
{
	return exec("begin");
}

bool Connection::commit()
{
	return exec("commit");
}

bool Connection::rollback()
{
	return exec("rollback");
}

void Connection::interrupt()
{
	sqlite3_interrupt(_handler);
}
