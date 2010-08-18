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

using namespace SQLite;

Connection::Connection() : _handler(0)
{
}

Connection::~Connection()
{
}

void Connection::getError() const
{
	_lastError = Error(*this);
}

void Connection::noError() const
{
	_lastError = Error();
}

bool Connection::connect(const QString &dbFile)
{
	if (connected()) {
		_lastError = Error("Already connected to a database");
		return false;
	}

	int res = sqlite3_open_v2(dbFile.toUtf8().data(), &_handler, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
	if (res != SQLITE_OK) {
		getError();
		goto err;
	}
	_dbFile = dbFile;
	noError();
	return true;

err:
	if (_handler) sqlite3_free(_handler);
	return false;
}

bool Connection::close()
{
	if (!connected()) {
		getError();
		return false;
	}


	int res = sqlite3_close(_handler);
	if (res != SQLITE_OK) {
		getError();
		return false;
	}
	_handler = 0;
	_dbFile.clear();
	noError();
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
		getError();
		return false;
	}
	res = sqlite3_step(stmt);
	if (res != SQLITE_DONE) {
		getError();
		goto cleanError;
	}
	res = sqlite3_finalize(stmt);
	if (res != SQLITE_OK) {
		getError();
		return false;
	}
	noError();
	return true;

cleanError:
	sqlite3_finalize(stmt);
	return false;
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
