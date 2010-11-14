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

#ifndef CORE_DBLIST_H
#define CORE_DBLIST_H

#include "sqlite/Query.h"
#include "sqlite/Connection.h"

#include <QStringList>

/**
 * Represents how entries lists are actually recorded inside the DB.
 */
template <class T> struct DBListEntry
{
	quint32 rowId;
	quint32 leftSize;
	bool red;
	quint32 parent;
	quint32 left;
	quint32 right;

	T data;

	// The following methods must be specialized by instances of this template.
	
	/**
	 * Returns the string that defines the columns of the data members in the database.
	 */
	static QString tableDataMembers();
	/**
	 * Bind all values of data to query.
	 */
	void bindDataValues(SQLite::Query &query) const;
	/**
	 * Read values of data from the query, starting from column start.
	 */
	void readDataValues(SQLite::Query &query, int start);
};

/**
 * Represents list roots recordings
 */
struct DBListInfo
{
	quint32 listId;
	quint32 rootId;
	QString label;
};

/**
 * Low-level database layer for persistent RB trees. Trees are stored in two tables, one that stores all
 * the nodes of the lists, another that associates the list id to the index of its root node.
 *
 * The naming of tables is configurable at construction time, so it is possible to store different kind
 * of lists within different tables.
 */
template <class T> class DBList
{
private:
	const QString _tableName;
	SQLite::Connection *_connection;

	SQLite::Query getEntryQuery;
	SQLite::Query insertEntryQuery;
	SQLite::Query removeEntryQuery;

	SQLite::Query newListQuery;
	SQLite::Query getListQuery;
	SQLite::Query insertListQuery;
	SQLite::Query removeListQuery;

public:
	DBList(const QString &tableName, SQLite::Connection *connection = 0);
	const QString &tableName() const { return _tableName; }

	/**
	 * Prepare all the cached statements to be used with the given connection.
	 * Must be called before any use of the functions below is done.
	 */
	bool prepareForConnection(SQLite::Connection *connection);

	bool createTables(SQLite::Connection *connection);
	/// Returns the entry list corresponding to the given row id
	DBListEntry<T> getEntry(quint32 rowid);
	/// Inserts the given entry into a list, returns the rowid
	/// If the entry already exists, replaces it.
	quint32 insertEntry(const DBListEntry<T> &entry);
	/// Removes the given entry from a list
	bool removeEntry(quint32 rowid);

	/// Insert a new list and return it
	DBListInfo newList();
	/// Get the root node id for a given list
	DBListInfo getList(quint32 listId);
	/// Inserts or updates the root id and label for a given list
	bool insertList(const DBListInfo &info);
	/// Remove a given list. All nodes of the list should be deleted before calling this method!
	bool removeList(quint32 listId);

	SQLite::Connection *connection() { return _connection; }
};

template <class T> DBList<T>::DBList(const QString &tableName, SQLite::Connection *connection) : _tableName(tableName)
{
	prepareForConnection(connection);
}

template <class T> bool DBList<T>::createTables(SQLite::Connection *connection)
{
	if (!connection->exec(QString("create table %1(rowid INTEGER PRIMARY KEY, leftSize INTEGER, red TINYINT, parent INTEGER, left INTEGER, right INTEGER, %2)").arg(_tableName).arg(DBListEntry<T>::tableDataMembers()))) return false;
	if (!connection->exec(QString("create table %1Roots(listId INTEGER PRIMARY KEY, rootId INTEGER, label TEXT)").arg(DBList<T>::tableName()))) return false;
	// Entry for root list (rowid = 0)
	//if (!connection->exec(QString("insert into %1Root values(0, 0, \"\")").arg(DBList<T>::tableName()))) return false;
	return true;
}

template <class T> bool DBList<T>::prepareForConnection(SQLite::Connection *connection)
{
	_connection = connection;
	getEntryQuery.useWith(_connection);
	insertEntryQuery.useWith(_connection);
	removeEntryQuery.useWith(_connection);

	newListQuery.useWith(_connection);
	getListQuery.useWith(_connection);
	insertListQuery.useWith(connection);
	removeListQuery.useWith(connection);

	int nbDataMembers = DBListEntry<T>::tableDataMembers().count(',') + 1;
	QStringList dataHoldersList;
	while (nbDataMembers-- > 0) dataHoldersList << "?";
	QString dataHolders(dataHoldersList.join(", "));

	if (connection) {
		if (!getEntryQuery.prepare(QString("select * from %1 where rowid = ?").arg(_tableName))) return false;
		if (!insertEntryQuery.prepare(QString("insert or replace into %1 values(?, ?, ?, ?, ?, ?, %2)").arg(_tableName).arg(dataHolders))) return false;
		if (!removeEntryQuery.prepare(QString("delete from %1 where rowid == ?").arg(_tableName))) return false;

		if (!newListQuery.prepare(QString("insert into %1Roots values(NULL, 0, \"\")").arg(_tableName))) return false;
		if (!getListQuery.prepare(QString("select * from %1Roots where listId = ?").arg(_tableName))) return false;
		if (!insertListQuery.prepare(QString("insert or replace into %1Roots values(?, ?, ?)").arg(DBList<T>::tableName()))) return false;
		if (!removeListQuery.prepare(QString("delete from %1Roots where listId = ?").arg(DBList<T>::tableName()))) return false;
	}
	return true;
}

template <class T> DBListEntry<T> DBList<T>::getEntry(quint32 rowid)
{
	DBListEntry<T> ret;
	getEntryQuery.bindValue(rowid);
	if (!getEntryQuery.exec() || !getEntryQuery.next()) {
		getEntryQuery.reset();
		ret.rowId = 0;
		return ret;
	}
	ret.rowId = getEntryQuery.valueUInt(0);
	ret.leftSize = getEntryQuery.valueUInt(1);
	ret.red = getEntryQuery.valueBool(2);
	ret.parent = getEntryQuery.valueUInt(3);
	ret.left = getEntryQuery.valueUInt(4);
	ret.right = getEntryQuery.valueUInt(5);

	ret.readDataValues(getEntryQuery, 6);
	getEntryQuery.reset();
	return ret;
}

template <class T> quint32 DBList<T>::insertEntry(const DBListEntry<T> &entry)
{
	if (entry.rowId == 0) insertEntryQuery.bindNullValue();
	else insertEntryQuery.bindValue(entry.rowId);
	insertEntryQuery.bindValue(entry.leftSize);
	insertEntryQuery.bindValue(entry.red);
	insertEntryQuery.bindValue(entry.parent);
	insertEntryQuery.bindValue(entry.left);
	insertEntryQuery.bindValue(entry.right);
	entry.bindDataValues(insertEntryQuery);

	if (!insertEntryQuery.exec()) {
		insertEntryQuery.reset();
		return 0;
	} else return insertEntryQuery.lastInsertId();
}

template <class T> bool DBList<T>::removeEntry(quint32 rowid)
{
	if (rowid == 0) return false;
	removeEntryQuery.bindValue(rowid);
	if (!removeEntryQuery.exec()) {
		removeEntryQuery.reset();
		return false;
	} else return true;
}

template <class T> DBListInfo DBList<T>::newList()
{
	DBListInfo ret = { 0 };

	if (!newListQuery.exec()) {
		newListQuery.reset();
		return ret;
	}

	ret.listId = newListQuery.lastInsertId();
	return ret;
}

template <class T> DBListInfo DBList<T>::getList(quint32 listId)
{
	DBListInfo ret = { 0 };
	getListQuery.bindValue(listId);

	if (!getListQuery.exec() || !getListQuery.next()) {
		getListQuery.reset();
		return ret;
	}
	ret.listId = getListQuery.valueUInt(0);
	ret.rootId = getListQuery.valueUInt(1);
	ret.label = getListQuery.valueString(2);

	getListQuery.reset();
	return ret;
}

template <class T> bool DBList<T>::insertList(const DBListInfo &info)
{
	insertListQuery.bindValue(info.listId);
	insertListQuery.bindValue(info.rootId);
	insertListQuery.bindValue(info.label);
	if (!insertListQuery.exec()) {
		insertListQuery.reset();
		return false;
	} else return true;
}

template <class T> bool DBList<T>::removeList(quint32 listId)
{
	removeListQuery.bindValue(listId);
	if (!removeListQuery.exec()) {
		removeListQuery.reset();
		return false;
	} else return true;
}

#endif

