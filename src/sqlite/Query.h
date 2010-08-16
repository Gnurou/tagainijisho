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

#include <QObject>
#include <QVariant>

struct sqlite3_stmt;

namespace SQLite {

class Connection;

/**
 * A lightweight Qt/SQLite query wrapper class that provides an interface
 * similar to that of QSqlQuery but does not require QtSql.
 */
class Query
{
friend class Connection;
private:
	sqlite3_stmt *_stmt;
	Connection *_connection;
	enum { INVALID, BLANK, PREPARED, RUN, FIRSTRES, DONE, ERROR } _state;

	/// Copy is forbidden
	Query &operator =(const Query &query);

public:
	/**
	 * Makes an invalid query. Useful only as a placeholder.
	 */
	Query();
	Query(Connection *connection);
	~Query();

	void useWith(Connection *connection);

	bool isValid() const { return _connection != 0; }
	Connection *connection() { return _connection; }

	void reset();
	bool prepare(const QString &query);
	bool exec();
	bool bindValue(const QVariant &val, int col);
	
	bool next();
	bool seek(int index, bool relative = false);
	qint64 lastInsertedRowId() const;
	QVariant value(int column) const;
	
	void clear();
};
	
}

#endif
