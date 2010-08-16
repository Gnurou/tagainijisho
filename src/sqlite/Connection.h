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

#include <QObject>

struct sqlite3;
namespace SQLite {

class Connection : public QObject
{
Q_OBJECT
friend class Error;
private:
	sqlite3 *_handler;
	QString _dbFile;
	mutable Error _lastError;

	void getError() const;
	void noError() const;

public:
	Connection(QObject *parent = 0);
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
	Error lastError();
};

}

#endif
