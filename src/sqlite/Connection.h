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

#ifndef __SQLITE_CONNECTION_H
#define __SQLITE_CONNECTION_H

#include "sqlite/Error.h"
#include "sqlite/Query.h"

#include <QList>

struct sqlite3;
namespace SQLite {

/**
 * THIS CLASS IS NOT THREAD-SAFE!
 * It is perfectly fine to use many queries on the same connection, however it is
 * the responsability of the user to make sure all operations involving them happen
 * in the same thread. Only interrupt() may be called from another thread to termine
 * a running query.
 */
class Connection
{
friend class Error;
friend class Query;
private:
	sqlite3 *_handler;
	QString _dbFile;
	mutable Error _lastError;

	QList<Query> _queries;

	const Error &updateError() const;

public:
	Connection();
	~Connection();

	/**
	 * Connect to the database file given as parameter. Returns true in case
	 * of success, false otherwise.
	 */
	bool connect(const QString &dbFile);

	bool connected() const { return _handler != 0; }

	const QString &dbFileName() const { return _dbFile; }

	bool close();

	/**
	 * Attach the database file given as parameter to alias. Returns true
	 * in case of success, false otherwise.
	 */
	bool attach(const QString &dbFile, const QString &alias);

	/**
	 * Detach the previously attached database alias.
	 */
	bool detach(const QString &alias);

	/// Returns the last error that happened on this connection
	const Error &lastError() const;

	sqlite3 *sqlite3Handler() { return _handler; }

	/**
	 * Execute a single statement directly. Should only be used
	 * to execute non-queries like table creation or pragmas.
	 */
	bool exec(const QString &statement);

	bool transaction();
	bool commit();
	bool rollback();

	/**
	 * Interrupts all queries being executed on this connection.
	 * Interrupted queries will return SQLITE_INTERRUPT.
	 */
	void interrupt();
};

}

#endif
