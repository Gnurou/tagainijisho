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

class Error {
    friend class Connection;
    friend class Query;

  private:
    int _code;

    QString _message;

    Error() : _code(0) {}
    Error(int code, const QString &message);
    Error(const Connection &connection);

  public:
    int code() const { return _code; }
    const QString &message() const { return _message; }
    bool isError() const;
    bool isInterrupted() const;
};

} // namespace SQLite

#endif
