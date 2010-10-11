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

EntryListDB::EntryListDB(const QString &tableName, SQLite::Connection *connection) : _tableName(tableName){
	prepareForConnection(connection);
}

void EntryListDB::prepareForConnection(SQLite::Connection *connection)
{
	_connection = connection;
	getEntryQuery.useWith(_connection);
	insertEntryQuery.useWith(_connection);
	removeEntryQuery.useWith(_connection);
	updateLeftQuery.useWith(_connection);
	updateRightQuery.useWith(_connection);
	updateParentQuery.useWith(_connection);

	if (!connection) return;
	getEntryQuery.prepare(QString("select type, id, parent, left, right from %1 where rowid = ?").arg(_tableName));
	insertEntryQuery.prepare(QString("insert into %1 values(NULL, ?, ?, ?, ?, ?, ?)").arg(_tableName));
	removeEntryQuery.prepare(QString("delete from %1 where rowid == ?").arg(_tableName));
	updateLeftQuery.prepare(QString("update %1 set left = ? where rowid == ?").arg(_tableName));
	updateRightQuery.prepare(QString("update %1 set right = ? where rowid == ?").arg(_tableName));
	updateParentQuery.prepare(QString("update %1 set parent = ? where rowid == ?").arg(_tableName));
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
	ret.rowId = rowid;
	ret.type = getEntryQuery.valueUInt(0);
	if (ret.type != 0) ret.id = getEntryQuery.valueUInt(1);
	else { ret.id = 0; ret.name = getEntryQuery.valueString(1); }
	ret.parent = getEntryQuery.valueUInt(2);
	ret.left = getEntryQuery.valueUInt(3);
	ret.right = getEntryQuery.valueUInt(4);
	return ret;
}

quint32 EntryListDB::insertEntry(quint32 listId, quint8 type, quint32 id, quint32 parent, quint32 left, quint32 right)
{
	insertEntryQuery.bindValue(listId);
	insertEntryQuery.bindValue(type);
	insertEntryQuery.bindValue(id);
	insertEntryQuery.bindValue(parent);
	insertEntryQuery.bindValue(left);
	insertEntryQuery.bindValue(right);

	if (!insertEntryQuery.exec()) {
		insertEntryQuery.reset();
		return 0;
	} else {
		return insertEntryQuery.lastInsertId();
	}
}

quint32 EntryListDB::insertList(quint32 listId, const QString &name, quint32 parent, quint32 left, quint32 right)
{
	insertEntryQuery.bindValue(listId);
	insertEntryQuery.bindValue(0);
	insertEntryQuery.bindValue(name);
	insertEntryQuery.bindValue(parent);
	insertEntryQuery.bindValue(left);
	insertEntryQuery.bindValue(right);

	if (!insertEntryQuery.exec()) {
		insertEntryQuery.reset();
		return 0;
	} else {
		return insertEntryQuery.lastInsertId();
	}
}
