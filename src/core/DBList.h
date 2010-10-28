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
 * Low-level database manipulation for RB trees. Avoid direct use!
 */
template <class T> class DBList
{
private:
	const QString _tableName;
	SQLite::Connection *_connection;

	SQLite::Query getEntryQuery;
	SQLite::Query insertEntryQuery;
	SQLite::Query removeEntryQuery;


public:
	DBList(const QString &tableName, SQLite::Connection *connection = 0);

	/**
	 * Prepare all the cached statements to be used with the given connection.
	 * Must be called before any use of the functions below is done.
	 */
	bool prepareForConnection(SQLite::Connection *connection);


	bool createTables(SQLite::Connection *connection);
	/// Returns the entry list corresponding to the given row id
	DBListEntry<T> getEntry(quint32 rowid);
	/// Inserts the given entry into a list, returns the rowid
	quint32 insertEntry(const DBListEntry<T> &entry);
	/// Removes the given entry from a list
	bool removeEntry(quint32 rowid);
};

template <class T> DBList<T>::DBList(const QString &tableName, SQLite::Connection *connection) : _tableName(tableName)
{
	prepareForConnection(connection);
}

template <class T> bool DBList<T>::createTables(SQLite::Connection *connection)
{
	if (!connection->exec(QString("create table %1(rowid INTEGER PRIMARY KEY, leftSize INTEGER, red TINYINT, parent INTEGER, left INTEGER, right INTEGER, %2)").arg(_tableName).arg(DBListEntry<T>::tableDataMembers()))) return false;
	return true;
}

template <class T> bool DBList<T>::prepareForConnection(SQLite::Connection *connection)
{
	_connection = connection;
	getEntryQuery.useWith(_connection);
	insertEntryQuery.useWith(_connection);
	removeEntryQuery.useWith(_connection);

	int nbDataMembers = DBListEntry<T>::tableDataMembers().count(',') + 1;
	QStringList dataHoldersList;
	while (nbDataMembers-- > 0) dataHoldersList << "?";
	QString dataHolders(dataHoldersList.join(", "));

	if (connection) {
		if (!getEntryQuery.prepare(QString("select leftSize, red, parent, left, right, listId, type, id from %1 where rowid = ?").arg(_tableName))) return false;
		if (!insertEntryQuery.prepare(QString("insert or replace into %1 values(NULL, ?, ?, ?, ?, ?, %2)").arg(_tableName).arg(dataHolders))) return false;
		if (!removeEntryQuery.prepare(QString("delete from %1 where rowid == ?").arg(_tableName))) return false;
	}
	return true;
}

template <class T> DBListEntry<T> DBList<T>::getEntry(quint32 rowid)
{
	DBListEntry<T> ret;
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

	ret.readDataValues(getEntryQuery, 5);
	getEntryQuery.reset();
	return ret;
}

template <class T> quint32 DBList<T>::insertEntry(const DBListEntry<T> &entry)
{
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

#endif

