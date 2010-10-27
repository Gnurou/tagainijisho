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

#ifndef CORE_ENTRYLISTDB_H
#define CORE_ENTRYLISTDB_H

#include "sqlite/Query.h"
#include "sqlite/Connection.h"

/**
 * Represents how entries lists are actually recorded inside the DB.
 */
template <class T> struct EntryList
{
	quint32 rowId;
	quint32 leftSize;
	bool red;
	quint32 parent;
	quint32 left;
	quint32 right;

	T data;
};

/**
 * Low-level database manipulation for RB trees. Avoid direct use!
 */
template <class T> class EntryListDB
{
private:
	const QString _tableName;
	SQLite::Connection *_connection;

	SQLite::Query getEntryQuery;
	SQLite::Query insertEntryQuery;
	SQLite::Query removeEntryQuery;
	SQLite::Query updateEntryQuery;


public:
	EntryListDB(const QString &tableName, SQLite::Connection *connection = 0);

	/**
	 * Prepare all the cached statements to be used with the given connection.
	 * Must be called before any use of the functions below is done.
	 */
	bool prepareForConnection(SQLite::Connection *connection);


	bool createTables(SQLite::Connection *connection);
	/// Returns the entry list corresponding to the given row id
	EntryList<T> getEntry(quint32 rowid);
	/// Inserts the given entry into a list, returns the rowid
	quint32 insertEntry(const EntryList<T> &entry);
	/// Removes the given entry from a list
	bool removeEntry(quint32 rowid);
	/// Update the parent member of an entry
	bool updateEntry(const EntryList<T> &entry);
};

template <class T> EntryListDB<T>::EntryListDB(const QString &tableName, SQLite::Connection *connection) : _tableName(tableName)
{
	prepareForConnection(connection);
}

template <class T> bool EntryListDB<T>::prepareForConnection(SQLite::Connection *connection)
{
	_connection = connection;
	getEntryQuery.useWith(_connection);
	insertEntryQuery.useWith(_connection);
	removeEntryQuery.useWith(_connection);
	updateEntryQuery.useWith(_connection);

	if (connection) {
		if (!getEntryQuery.prepare(QString("select leftSize, red, parent, left, right, listId, type, id from %1 where rowid = ?").arg(_tableName))) return false;
		if (!insertEntryQuery.prepare(QString("insert into %1 values(NULL, ?, ?, ?, ?, ?, ?, ?, ?)").arg(_tableName))) return false;
		if (!removeEntryQuery.prepare(QString("delete from %1 where rowid == ?").arg(_tableName))) return false;
		if (!updateEntryQuery.prepare(QString("update %1 set leftSize = ?, red = ?, parent = ?, left = ?, right = ?, listId = ?, type = ?, id = ? where rowid == ?").arg(_tableName))) return false;
	}
	return true;
}

template <class T> bool EntryListDB<T>::createTables(SQLite::Connection *connection)
{
	if (!connection->exec(QString("create table %1(rowid INTEGER PRIMARY KEY, leftSize INTEGER, red TINYINT, parent INTEGER, left INTEGER, right INTEGER, listId INTEGER, type TINYINT, id INTEGER)").arg(_tableName))) return false;
	return true;
}

template <class T> EntryList<T> EntryListDB<T>::getEntry(quint32 rowid)
{
	EntryList<T> ret;
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

template <class T> quint32 EntryListDB<T>::insertEntry(const EntryList<T> &entry)
{
	insertEntryQuery.bindValue(entry.leftSize);
	insertEntryQuery.bindValue(entry.red);
	insertEntryQuery.bindValue(entry.parent);
	insertEntryQuery.bindValue(entry.left);
	insertEntryQuery.bindValue(entry.right);
	insertEntryQuery.bindValue(entry.listId);
	insertEntryQuery.bindValue(entry.type);
	insertEntryQuery.bindValue(entry.id);

	if (!insertEntryQuery.exec()) {
		insertEntryQuery.reset();
		return 0;
	} else return insertEntryQuery.lastInsertId();
}

template <class T> bool EntryListDB<T>::updateEntry(const EntryList<T> &entry)
{
	updateEntryQuery.bindValue(entry.leftSize);
	updateEntryQuery.bindValue(entry.red);
	updateEntryQuery.bindValue(entry.parent);
	updateEntryQuery.bindValue(entry.left);
	updateEntryQuery.bindValue(entry.right);
	updateEntryQuery.bindValue(entry.listId);
	updateEntryQuery.bindValue(entry.type);
	updateEntryQuery.bindValue(entry.id);

	if (!updateEntryQuery.exec()) {
		updateEntryQuery.reset();
		return false;
	} else return true;
}

#endif

