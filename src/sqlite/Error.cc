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

Error::Error(int code, const QString &message) : _code(code), _message(message)
{
}

Error::Error(const Connection &connection)
{
	_code = sqlite3_errcode(connection._handler);
	_message = sqlite3_errmsg(connection._handler);
}

bool Error::isError() const
{
	switch (_code) {
	case SQLITE_OK:
	case SQLITE_DONE:
	case SQLITE_ROW:
	case SQLITE_INTERRUPT:
	case SQLITE_ABORT:
		return false;
	default:
		return true;
	}
}

bool Error::isInterrupted() const
{
	switch (_code) {
	case SQLITE_INTERRUPT:
	case SQLITE_ABORT:
		return true;
	default:
		return false;
	}
}
