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

#include "EntryListDB.h"
#include "sqlite/Connection.h"

EntryListDB::EntryListDB(const QString &tableName, SQLite::Connection *connection) : _tableName(tableName){
	prepareForConnection(connection);
}

bool EntryListDB::prepareForConnection(SQLite::Connection *connection)
{
	_connection = connection;
	getEntryQuery.useWith(_connection);
	insertEntryQuery.useWith(_connection);
	removeEntryQuery.useWith(_connection);
	updateLeftQuery.useWith(_connection);
	updateRightQuery.useWith(_connection);
	updateParentQuery.useWith(_connection);

	if (connection) {
		if (!getEntryQuery.prepare(QString("select leftSize, red, parent, left, right, listId, type, id from %1 where rowid = ?").arg(_tableName))) return false;
		if (!insertEntryQuery.prepare(QString("insert into %1 values(NULL, ?, ?, ?, ?, ?, ?, ?, ?)").arg(_tableName))) return false;
		if (!removeEntryQuery.prepare(QString("delete from %1 where rowid == ?").arg(_tableName))) return false;
		if (!updateLeftQuery.prepare(QString("update %1 set left = ? where rowid == ?").arg(_tableName))) return false;
		if (!updateRightQuery.prepare(QString("update %1 set right = ? where rowid == ?").arg(_tableName))) return false;
		if (!updateParentQuery.prepare(QString("update %1 set parent = ? where rowid == ?").arg(_tableName))) return false;
	}
	return true;
}

bool EntryListDB::createTables(SQLite::Connection *connection)
{
	if (!connection->exec(QString("create table %1(rowid INTEGER PRIMARY KEY, leftSize INTEGER, red TINYINT, parent INTEGER, left INTEGER, right INTEGER, listId INTEGER, type TINYINT, id INTEGER)").arg(_tableName))) return false;
	return true;
}

EntryList EntryListDB::getEntry(quint32 rowid)
{
	EntryList ret;
	getEntryQuery.bindValue(rowid);
	if (!getEntryQuery.exec()) {
		getEntryQuery.reset();
		ret.rowId = 0;
		return ret;
	}
	getEntryQuery.next();
	ret.rowId = rowid;
	ret.leftSize = getEntryQuery.valueUInt(0);
	ret.red = getEntryQuery.valueBool(1);
	ret.parent = getEntryQuery.valueUInt(2);
	ret.left = getEntryQuery.valueUInt(3);
	ret.right = getEntryQuery.valueUInt(4);

	ret.listId = getEntryQuery.valueUInt(5);
	ret.type = getEntryQuery.valueUInt(6);
	if (ret.type != 0) ret.id = getEntryQuery.valueUInt(7);
	else { ret.id = 0; ret.name = getEntryQuery.valueString(7); }
	getEntryQuery.reset();
	return ret;
}

quint32 EntryListDB::insertEntry(quint32 leftSize, bool red, quint32 parent, quint32 left, quint32 right, quint32 listId, quint8 type, quint32 id)
{
	insertEntryQuery.bindValue(leftSize);
	insertEntryQuery.bindValue(red);
	insertEntryQuery.bindValue(parent);
	insertEntryQuery.bindValue(left);
	insertEntryQuery.bindValue(right);
	insertEntryQuery.bindValue(listId);
	insertEntryQuery.bindValue(type);
	insertEntryQuery.bindValue(id);

	if (!insertEntryQuery.exec()) {
		insertEntryQuery.reset();
		return 0;
	} else {
		return insertEntryQuery.lastInsertId();
	}
}

quint32 EntryListDB::insertList(quint32 leftSize, bool red, quint32 parent, quint32 left, quint32 right, quint32 listId, const QString &name)
{
	insertEntryQuery.bindValue(leftSize);
	insertEntryQuery.bindValue(red);
	insertEntryQuery.bindValue(parent);
	insertEntryQuery.bindValue(left);
	insertEntryQuery.bindValue(right);
	insertEntryQuery.bindValue(listId);
	insertEntryQuery.bindValue(0);
	insertEntryQuery.bindValue(name);

	if (!insertEntryQuery.exec()) {
		insertEntryQuery.reset();
		return 0;
	} else {
		return insertEntryQuery.lastInsertId();
	}
}
