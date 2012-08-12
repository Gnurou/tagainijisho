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

#include "tagaini_config.h"

#include "core/TString.h"

struct sqlite3;
namespace SQLite {

class Connection
{
friend class Error;
friend class Query;
private:
	sqlite3 *_handler;
	TString _dbFile;
	mutable Error _lastError;

	const Error &updateError() const;

#ifdef DEBUG_TRANSACTIONS
	int _tr_count;
#endif
	
public:
	Connection();
	~Connection();

	typedef enum { None = 0, JournalInFile = (1 << 0), ReadOnly = (1 << 1) } OpenFlags;
	/**
	 * Connect to the database file given as parameter. Returns true in case
	 * of success, false otherwise.
	 */
	bool connect(const TString &dbFile, OpenFlags flags = None);

	bool connected() const { return _handler != 0; }

	const char *dbFileName() const { return _dbFile.c_str(); }

	bool close();

	/**
	 * Attach the database file given as parameter to alias. Returns true
	 * in case of success, false otherwise.
	 */
	bool attach(const TString &dbFile, const TString &alias);

	/**
	 * Detach the previously attached database alias.
	 */
	bool detach(const TString &alias);

	/// Returns the last error that happened on this connection
	const Error &lastError() const;

	sqlite3 *sqlite3Handler() { return _handler; }

	/**
	 * Execute a single statement directly. Should only be used
	 * to execute non-queries like table creation or pragmas.
	 */
	bool exec(const TString &statement);

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
