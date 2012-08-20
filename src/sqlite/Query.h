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

#ifndef __SQLITE_QUERY_H
#define __SQLITE_QUERY_H

#include "sqlite/Error.h"
#include "core/TString.h"

#include <stdint.h>

#include <QByteArray>

struct sqlite3_stmt;

namespace SQLite {

class Connection;

typedef enum { None, Null, Integer, Float, String, Blob } Type;

/**
 * A lightweight Qt/SQLite query wrapper class that provides an interface
 * similar to that of SQLite::Query but does not require QtSql.
 */
class Query
{
friend class Connection;
private:
	sqlite3_stmt *_stmt;
	Connection *_connection;
	Error _lastError;
	enum { INVALID, ERROR, BLANK, PREPARED, RUN, FIRSTRES } _state;
	uint8_t _bindIndex;

	/// Copy is forbidden
	Query &operator =(const Query &query);

	bool checkBind(int &col);
	bool checkBindRes();

public:
	/**
	 * Makes an invalid query. Useful only as a placeholder.
	 */
	Query();
	Query(Connection *connection);
	~Query();

	void useWith(Connection *connection);

	bool isValid() const { return _connection != 0; }
	bool active() const { return _state == RUN || _state == FIRSTRES; }
	Connection *connection() { return _connection; }

	void reset();
	bool prepare(const TString &query);
	bool exec();
	bool exec(const TString &query);

	bool bindValue(const bool val, int col = 0);
	bool bindValue(const int32_t val, int col = 0);
	bool bindValue(const uint32_t val, int col = 0);
	bool bindValue(const int64_t val, int col = 0);
	bool bindValue(const uint64_t val, int col = 0);
	bool bindValue(const double val, int col = 0);
	bool bindValue(const TString &val, int col = 0);
	bool bindValue(const char *val, int col = 0) { return bindValue(TString(val), col); }
	bool bindValue(const QByteArray &val, int col = 0);
	bool bindValue(const size_t val, int col = 0);
	bool bindCompressedValue(const std::vector<uint8_t> &val, int col = 0);
	bool bindNullValue(int col = 0);
	
	bool next();
	int64_t lastInsertId() const;

	/// Returns the number of columns in a results row, or
	/// 0 if the statement does not yield any result.
	int columnsCount() const;
	bool valueAvailable(int column) const;
	Type valueType(int column) const;
	bool valueBool(int column) const;
	int32_t valueInt(int column) const;
	uint32_t valueUInt(int column) const;
	int64_t valueInt64(int column) const;
	uint64_t valueUInt64(int column) const;
	double valueDouble(int column) const;
	TString valueString(int column) const;
	QByteArray valueBlob(int column) const;
	std::vector<uint8_t> valueCompressedBlob(int column) const;
	bool valueIsNull(int column) const;

	void clear();

	const Error &lastError() const { return _lastError; }
	TString queryText() const;
};

}

#endif
