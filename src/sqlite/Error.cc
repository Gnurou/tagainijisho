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

#include "sqlite/Error.h"
#include "sqlite/Connection.h"
#include "sqlite3.h"

using namespace SQLite;

Error::Error(const QString &message) : _isError(true), _message(message)
{
}

Error::Error(const Connection &connection)
{
	int code = sqlite3_errcode(connection._handler);
	switch (code) {
	case SQLITE_OK:
	case SQLITE_DONE:
	case SQLITE_ROW:
		_isError = false;
	default:
		_isError = true;
	}

	_message = sqlite3_errmsg(connection._handler);
}
