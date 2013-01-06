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
#include "sqlite/Query.h"
#include "sqlite/Connection.h"
#include "core/Compress.h"
#include "tagaini_config.h"

#include <QtDebug>

#include <iostream>

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
	clear();
	_connection = connection;
	if (connection) _state = BLANK;
}

#ifdef DEBUG_QUERIES
static void checkQueryError(const Query &query, const TString &statement)
{
	if (query.lastError().isError())
		std::cerr << "On query: " << statement << std::endl;
}
#else
static void checkQueryError(const Query &query, const TString &statement)
{
}
#endif

bool Query::prepare(const TString &query)
{
	if (!_connection) return false;
	clear();

	int res;
	// Busy loop while the shared cache is locked. This is ugly.
	while ((res = sqlite3_prepare_v2(_connection->_handler, query.c_str(), -1, &_stmt, 0)) == SQLITE_LOCKED_SHAREDCACHE){};
	_lastError = _connection->updateError();
	checkQueryError(*this, query);
	if (res != SQLITE_OK) {
		_state = ERROR;
		return false;
	}
	_state = PREPARED;
	return true;
}

bool Query::checkBind(int &col)
{
	if (!_stmt) return false;

	if (col == 0) col = ++_bindIndex;
	else _bindIndex = col;

	return true;
}

bool Query::checkBindRes()
{
	_lastError = _connection->updateError();
	checkQueryError(*this, queryText());
	if (_lastError.code() != SQLITE_OK) {
		_state = ERROR;
		return false;
	}
	return true;
}

bool Query::bindValue(const int32_t val, int col)
{
	if (!checkBind(col)) return false;
	sqlite3_bind_int(_stmt, col, val);
	return checkBindRes();
}

bool Query::bindValue(const uint32_t val, int col)
{
	return bindValue((qint32)val, col);
}

bool Query::bindValue(const bool val, int col)
{
	return bindValue((qint32)val, col);
}

bool Query::bindValue(const int64_t val, int col)
{
	if (!checkBind(col)) return false;
	sqlite3_bind_int64(_stmt, col, val);
	return checkBindRes();
}

bool Query::bindValue(const uint64_t val, int col)
{
	return bindValue((int64_t)val, col);
}

bool Query::bindValue(const double val, int col)
{
	if (!checkBind(col)) return false;
	sqlite3_bind_double(_stmt, col, val);
	return checkBindRes();
}

bool Query::bindValue(const TString &val, int col)
{
	if (!checkBind(col)) return false;
	sqlite3_bind_text(_stmt, col,
		val.c_str(), -1, SQLITE_TRANSIENT);
	return checkBindRes();
}

bool Query::bindValue(const QByteArray &val, int col)
{
	if (!checkBind(col)) return false;
	sqlite3_bind_blob(_stmt, col,
		val.data(), val.length(), SQLITE_TRANSIENT);
	return checkBindRes();
}

bool Query::bindCompressedValue(const std::vector<uint8_t> &val, int col)
{
	if (!checkBind(col)) return false;
	// Try to compress and see if it is worth storing a compressed version
	std::vector<uint8_t> src(val), comp;
	Tagaini::Compress(src, comp);
	if (comp.size() < src.size() + 8) {
		comp.insert(comp.begin(), 1);
		sqlite3_bind_blob(_stmt, col,
			comp.data(), comp.size(), SQLITE_TRANSIENT);
	} else {
		src.insert(src.begin(), 0);
		sqlite3_bind_blob(_stmt, col,
			src.data(), src.size(), SQLITE_TRANSIENT);
	}

	return checkBindRes();
}

bool Query::bindNullValue(int col)
{
	if (!checkBind(col)) return false;
	sqlite3_bind_null(_stmt, col);
	return checkBindRes();
}

void Query::reset()
{
	_bindIndex = 0;
	if (!_stmt) return;
	sqlite3_reset(_stmt);
	_lastError = _connection->updateError();
	checkQueryError(*this, queryText());
	_state = PREPARED;
}

bool Query::exec()
{
	if (_state != PREPARED) return false;
	// Busy-loop while the shared cache is locked. This is ugly.
	while (sqlite3_step(_stmt) == SQLITE_LOCKED_SHAREDCACHE){};
	_lastError = _connection->updateError();
	checkQueryError(*this, queryText());
	switch (_lastError.code()) {
	case SQLITE_ROW:
		_state = FIRSTRES;
		return true;
	case SQLITE_DONE:
		reset();
		return true;
	default:
		_state = ERROR;
		return false;
	}
}

bool Query::next()
{
	switch (_state) {
	case FIRSTRES:
		_state = RUN;
		return true;
	case RUN:
		sqlite3_step(_stmt);
		_lastError = _connection->updateError();
		checkQueryError(*this, queryText());
		switch (_lastError.code()) {
		case SQLITE_ROW:
			return true;
		case SQLITE_DONE:
			reset();
			return false;
		default:
			_state = ERROR;
			return false;
		}
	default:
		return false;
	}
}

bool Query::exec(const TString &query)
{
	if (!prepare(query)) return false;
	return exec();
}

int64_t Query::lastInsertId() const
{
	if (!_stmt) return 0;

	return sqlite3_last_insert_rowid(_connection->_handler);
}

bool Query::valueAvailable(int column) const
{
	return (_state == RUN && column < columnsCount());
}

int Query::columnsCount() const
{
	return sqlite3_column_count(_stmt);
}

Type Query::valueType(int column) const
{
	if (!valueAvailable(column)) return None;

	switch (sqlite3_column_type(_stmt, column)) {
	case SQLITE_INTEGER:
		return Integer;
	case SQLITE_FLOAT:
		return Float;
	case SQLITE_TEXT:
		return String;
	case SQLITE_BLOB:
		return Blob;
	case SQLITE_NULL:
		return Null;
	default:
		return None;
	}
}

bool Query::valueBool(int column) const
{
	return (bool)valueInt(column);
}

int32_t Query::valueInt(int column) const
{
	return sqlite3_column_int(_stmt, column);
}

uint32_t Query::valueUInt(int column) const
{
	return (quint32)sqlite3_column_int(_stmt, column);
}

int64_t Query::valueInt64(int column) const
{
	return sqlite3_column_int64(_stmt, column);
}

uint64_t Query::valueUInt64(int column) const
{
	return (quint64)sqlite3_column_int64(_stmt, column);
}

double Query::valueDouble(int column) const
{
	return sqlite3_column_double(_stmt, column);
}

TString Query::valueString(int column) const
{
	// sqlite3_column_text converts to UTF-8 if needed
	const char *s = (const char *)sqlite3_column_text(_stmt, column);
	if (!s) return TString();
	else return TString(s);
}

QByteArray Query::valueBlob(int column) const
{
	return QByteArray((const char *)sqlite3_column_blob(_stmt, column), sqlite3_column_bytes(_stmt, column));
}

std::vector<uint8_t> Query::valueCompressedBlob(int column) const
{
	const uint8_t *data = (const uint8_t *)sqlite3_column_blob(_stmt, column);
	int len = sqlite3_column_bytes(_stmt, column);

	if (len == 0)
		return std::vector<uint8_t>();

	// Uncompressed data?
	if (data[0] == 0) {
		return std::vector<uint8_t>(data + 1, data + len);
	} else if (data[0] == 1) {
		std::vector<uint8_t> comp(data + 1, data + len);
		std::vector<uint8_t> ret;
		int err = Tagaini::Decompress(comp, ret);
		if (err) {
			std::cerr << "error while uncompressing blob data: " << err << std::endl;
		}
		return ret;
	} else {
		std::cerr << "not a valid blob data!" << std::endl;
		return std::vector<uint8_t>();
	}
}

bool Query::valueIsNull(int column) const
{
	return valueType(column) == Null;
}

void Query::clear()
{
	if (_stmt) {
		sqlite3_finalize(_stmt);
		_lastError = _connection->updateError();
		checkQueryError(*this, queryText());
		_stmt = 0;
	}
	_state = INVALID;
	_bindIndex = 0;
}

TString Query::queryText() const
{
	if (_state >= PREPARED) return sqlite3_sql(_stmt);
	else return TString();
}
