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

#include "sqlite/Query.h"
#include "sqlite/Connection.h"
#include "sqlite3.h"

#include <QtDebug>

using namespace SQLite;

Query::Query() : _stmt(0), _connection(0), _state(INVALID), _bindIndex(0)
{
}

Query::Query(Connection *connection) : _stmt(0)
{
	useWith(connection);
}

Query::~Query()
{
	useWith(0);
}

void Query::useWith(Connection *connection)
{
	if (_stmt) {
		sqlite3_finalize(_stmt);
		_stmt = 0;
	}
	_connection = connection;
	_bindIndex = 0;
	if (connection) _state = BLANK;
	else _state = INVALID;
}

bool Query::prepare(const QString &statement)
{
	if (!_connection) return false;
	if (_stmt) {
		sqlite3_finalize(_stmt);
		_stmt = 0;
	}

	int res = sqlite3_prepare_v2(_connection->_handler, statement.toUtf8().data(), -1, &_stmt, 0);
	if (res != SQLITE_OK) {
		_connection->getError();
		_state = ERROR;
		return false;
	}
	_state = PREPARED;
	_bindIndex = 0;
	return true;
}

bool Query::bindValue(const QVariant &val, int col)
{
	if (!_stmt) return false;
	int ret;

	if (col == 0) col = ++_bindIndex;
	else _bindIndex = col;

	if (val.isNull()) ret = sqlite3_bind_null(_stmt, col);
	else switch(val.type()) {
	case QVariant::Int:
	case QVariant::UInt:
	case QVariant::LongLong:
	case QVariant::ULongLong:
		ret = sqlite3_bind_int64(_stmt, col, val.toLongLong());
		break;
	case QVariant::Double:
		ret = sqlite3_bind_double(_stmt, col, val.toDouble());
		break;
	case QVariant::String:
		ret = sqlite3_bind_text(_stmt, col, val.toString().toUtf8().data(), -1, SQLITE_TRANSIENT);
		break;
	case QVariant::ByteArray:
		ret = sqlite3_bind_blob(_stmt, col, val.toByteArray().data(), val.toByteArray().length(), SQLITE_TRANSIENT);
		break;
	case QVariant::Invalid:
		ret = sqlite3_bind_null(_stmt, col);
		break;
	default:
		return false;
	}
	if (ret != SQLITE_OK) {
		_connection->getError();
		_state = ERROR;
		return false;
	}
	else _connection->noError();
	return true;
}

void Query::reset()
{
	if (!_stmt) return;
	sqlite3_reset(_stmt);
	_state = PREPARED;
	_bindIndex = 0;
}

bool Query::exec()
{
	if (_state != PREPARED) return false;
	int res = sqlite3_step(_stmt);
	if (res != SQLITE_ROW && res != SQLITE_DONE) {
		_connection->getError();
		_state = ERROR;
		return false;
	}
	_state = res == SQLITE_ROW ? FIRSTRES : DONE;
	return true;
}

bool Query::next()
{
	int res;
	switch (_state) {
	case FIRSTRES:
		_state = RUN;
		return true;
	case RUN:
		res = sqlite3_step(_stmt);
		if (res != SQLITE_ROW && res != SQLITE_DONE) {
			_connection->getError();
			_state = ERROR;
			return false;
		}
		_state = res == SQLITE_ROW ? RUN : DONE;
		return true;
	case DONE:
		return false;
	default:
		return false;
	}
}

bool Query::exec(const QString &query)
{
	if (!prepare(query)) return false;
	return exec();
}

bool Query::seek(int index, bool relative)
{
	return false;
}

qint64 Query::lastInsertId() const
{
	if (!_stmt) return 0;

	return sqlite3_last_insert_rowid(_connection->_handler);
}

QVariant Query::value(int column) const
{
	if (_state != RUN || column >= sqlite3_column_count(_stmt)) return QVariant();

	switch (sqlite3_column_type(_stmt, column)) {
	case SQLITE_INTEGER:
		return QVariant((qint64)sqlite3_column_int64(_stmt, column));
	case SQLITE_FLOAT:
		return QVariant(sqlite3_column_double(_stmt, column));
	case SQLITE_TEXT:
		return QVariant(QString::fromUtf16((const ushort *)sqlite3_column_text16(_stmt, column)));
	case SQLITE_BLOB:
		return QVariant(QByteArray((const char *)sqlite3_column_blob(_stmt, column), sqlite3_column_bytes(_stmt, column)));
	case SQLITE_NULL:
		return QVariant(QVariant::Int);
	default:
		return QVariant();
	}
}

const Error &Query::lastError() const
{
	return _connection->lastError();
}

