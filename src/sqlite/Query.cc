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

#include "sqlite/Query.h"
#include "sqlite/Connection.h"
#include "sqlite3.h"
#include "tagaini_config.h"

#include <QtDebug>

using namespace SQLite;

Query::Query() : _stmt(0), _connection(0), _state(INVALID), _bindIndex(0) {}

Query::Query(Connection *connection) : _stmt(0) { useWith(connection); }

Query::~Query() { useWith(0); }

void Query::useWith(Connection *connection) {
    clear();
    _connection = connection;
    if (connection)
        _state = BLANK;
}

#ifdef DEBUG_QUERIES
void checkQueryError(const Query &query, const QString &statement) {
    if (query.lastError().isError())
        qDebug("On query: %s", statement.toUtf8().constData());
}
#else
static void checkQueryError(const Query &query, const QString &statement) {}
#endif

bool Query::prepare(const QString &statement) {
    if (!_connection)
        return false;
    clear();

    int res;
    // Busy loop while the shared cache is locked. This is ugly.
    while ((res = sqlite3_prepare_v2(_connection->_handler, statement.toUtf8().data(), -1, &_stmt,
                                     0)) == SQLITE_LOCKED_SHAREDCACHE) {
    };
    _lastError = _connection->updateError();
    checkQueryError(*this, statement);
    if (res != SQLITE_OK) {
        _state = ERROR;
        return false;
    }
    _state = PREPARED;
    return true;
}

bool Query::checkBind(int &col) {
    if (!_stmt)
        return false;

    if (col == 0)
        col = ++_bindIndex;
    else
        _bindIndex = col;

    return true;
}

bool Query::checkBindRes() {
    _lastError = _connection->updateError();
    checkQueryError(*this, queryText());
    if (_lastError.code() != SQLITE_OK) {
        _state = ERROR;
        return false;
    }
    return true;
}

bool Query::bindValue(const qint32 val, int col) {
    if (!checkBind(col))
        return false;
    sqlite3_bind_int(_stmt, col, val);
    return checkBindRes();
}

bool Query::bindValue(const quint32 val, int col) { return bindValue((qint32)val, col); }

bool Query::bindValue(const bool val, int col) { return bindValue((qint32)val, col); }

bool Query::bindValue(const qint64 val, int col) {
    if (!checkBind(col))
        return false;
    sqlite3_bind_int64(_stmt, col, val);
    return checkBindRes();
}

bool Query::bindValue(const quint64 val, int col) { return bindValue((qint64)val, col); }

bool Query::bindValue(const double val, int col) {
    if (!checkBind(col))
        return false;
    sqlite3_bind_double(_stmt, col, val);
    return checkBindRes();
}

bool Query::bindValue(const QString &val, int col) {
    if (!checkBind(col))
        return false;
    sqlite3_bind_text(_stmt, col, val.toUtf8().data(), -1, SQLITE_TRANSIENT);
    return checkBindRes();
}

bool Query::bindValue(const QByteArray &val, int col) {
    if (!checkBind(col))
        return false;
    sqlite3_bind_blob(_stmt, col, val.data(), val.length(), SQLITE_TRANSIENT);
    return checkBindRes();
}

bool Query::bindNullValue(int col) {
    if (!checkBind(col))
        return false;
    sqlite3_bind_null(_stmt, col);
    return checkBindRes();
}

void Query::reset() {
    _bindIndex = 0;
    if (!_stmt)
        return;
    sqlite3_reset(_stmt);
    _lastError = _connection->updateError();
    checkQueryError(*this, queryText());
    _state = PREPARED;
}

bool Query::exec() {
    if (_state != PREPARED)
        return false;
    // Busy-loop while the shared cache is locked. This is ugly.
    while (sqlite3_step(_stmt) == SQLITE_LOCKED_SHAREDCACHE) {
    };
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

bool Query::next() {
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

bool Query::exec(const QString &query) {
    if (!prepare(query))
        return false;
    return exec();
}

qint64 Query::lastInsertId() const {
    if (!_stmt)
        return 0;

    return sqlite3_last_insert_rowid(_connection->_handler);
}

bool Query::valueAvailable(int column) const { return (_state == RUN && column < columnsCount()); }

int Query::columnsCount() const { return sqlite3_column_count(_stmt); }

Type Query::valueType(int column) const {
    if (!valueAvailable(column))
        return None;

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

bool Query::valueBool(int column) const { return (bool)valueInt(column); }

qint32 Query::valueInt(int column) const { return sqlite3_column_int(_stmt, column); }

quint32 Query::valueUInt(int column) const { return (quint32)sqlite3_column_int(_stmt, column); }

qint64 Query::valueInt64(int column) const { return sqlite3_column_int64(_stmt, column); }

quint64 Query::valueUInt64(int column) const {
    return (quint64)sqlite3_column_int64(_stmt, column);
}

double Query::valueDouble(int column) const { return sqlite3_column_double(_stmt, column); }

QString Query::valueString(int column) const {
    return QString::fromUtf16((const ushort *)sqlite3_column_text16(_stmt, column));
}

QByteArray Query::valueBlob(int column) const {
    return QByteArray((const char *)sqlite3_column_blob(_stmt, column),
                      sqlite3_column_bytes(_stmt, column));
}

bool Query::valueIsNull(int column) const { return valueType(column) == Null; }

void Query::clear() {
    if (_stmt) {
        QString qtext = queryText();
        sqlite3_finalize(_stmt);
        _lastError = _connection->updateError();
        checkQueryError(*this, qtext);
        _stmt = 0;
    }
    _state = INVALID;
    _bindIndex = 0;
}

QString Query::queryText() const {
    if (_state >= PREPARED)
        return sqlite3_sql(_stmt);
    else
        return QString();
}
