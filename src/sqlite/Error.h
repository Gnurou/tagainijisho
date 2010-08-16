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

#ifndef __SQLITE_ERROR_H
#define __SQLITE_ERROR_H

#include <QString>

namespace SQLite {

class Connection;

class Error
{
friend class Connection;
private:
	bool _isError;
	QString _message;

	Error() : _isError(false) {}
	Error(const QString &message);
	Error(const Connection &connection);
public:
	const QString &message() const { return _message; }
	bool isError() const { return _isError; }
};

}

#endif

