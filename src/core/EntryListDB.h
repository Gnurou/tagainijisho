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

/**
 * @file src/core/EntryListDB.h
 *
 * Specialization of DB lists classes to support nested entries lists through
 * an additional root table.
 *
 */

#include "core/DBList.h"
#include "core/OrderedRBNode.h"
#include "core/OrderedRBDBNode.h"


struct EntryListData {
	//quint32 listId;
	quint8 type;
	quint32 id;
	QString name;
};

// Specializations for EntryListData
template <> void DBListEntry<EntryListData>::bindDataValues(SQLite::Query &query) const;
template <> void DBListEntry<EntryListData>::readDataValues(SQLite::Query &query, int start);

/**
 * Behaves exactly like DBList when used with regular DB list classes, but allows nested
 * classes with OrderedRBDBNestedTree.
 */
template <class T> class DBNestedList : public DBList<T>
{
private:
	SQLite::Query insertRootQuery;
	SQLite::Query removeRootQuery;

public:
	DBNestedList(const QString &tableName, SQLite::Connection *connection = 0) : DBList<T>(tableName, connection)
	{
	}

	bool createTables(SQLite::Connection *connection);
	bool prepareForConnection(SQLite::Connection *connection);
	bool updateList(quint32 listId, quint32 rootId, const QString &str);
	bool removeList(quint32 listId);
};

template <class T> bool DBNestedList<T>::createTables(SQLite::Connection *connection)
{
	if (!DBList<T>::createTables(connection)) return false;

	if (!connection->exec(QString("create table %1Roots(listId INTEGER PRIMARY KEY, rootId INTEGER, label TEXT)").arg(DBList<T>::tableName()))) return false;
	// Entry for root list (rowid = 0)
	if (!connection->exec(QString("insert into %1Roots values(0, 0, \"\")").arg(DBList<T>::tableName()))) return false;
	return true;
}

template <class T> bool DBNestedList<T>::prepareForConnection(SQLite::Connection *connection)
{
	if (!DBList<T>::prepareForConnection(connection)) return false;

	insertRootQuery.useWith(connection);
	removeRootQuery.useWith(connection);

	if (connection) {
		if (!insertRootQuery.prepare(QString("insert or replace into %1Roots values(?, ?, ?)").arg(DBList<T>::tableName()))) return false;
		if (!removeRootQuery.prepare(QString("delete from %1Roots where listId = ?").arg(DBList<T>::tableName()))) return false;
	}
	return true;
}

template <class T> bool DBNestedList<T>::updateList(quint32 listId, quint32 rootId, const QString &str)
{
	insertRootQuery.bindValue(listId);
	insertRootQuery.bindValue(rootId);
	insertRootQuery.bindValue(str);
	if (!insertRootQuery.exec()) {
		insertRootQuery.reset();
		return false;
	} else return true;
}

template <class T> bool DBNestedList<T>::removeList(quint32 listId)
{
	removeRootQuery.bindValue(listId);
	if (!removeRootQuery.exec()) {
		removeRootQuery.reset();
		return false;
	} else return true;
}

/**
 * Overloads some methods from OrderedRBDBTree in order to maintain the lists root
 * table
 */
template <class T> class OrderedRBDBNestedTree : public OrderedRBDBTree<T>
{
private:
	quint32 _listId;
	bool mustUpdateRootTable;

public:
	OrderedRBDBNestedTree() : OrderedRBDBTree<T>(), _listId(0), mustUpdateRootTable(false)
	{
	}

	void setRoot(typename OrderedRBDBTree<T>::Node *node)
	{
		mustUpdateRootTable = true;
		OrderedRBDBTree<T>::setRoot(node);
	}

	bool commitChanges()
	{
		if (mustUpdateRootTable) {
			// TODO update root table if needed
			mustUpdateRootTable = false;
		}
		return OrderedRBDBTree<T>::commitChanges();
	}

	quint32 listId() const
	{
		return _listId;
	}

	void setListId(quint32 id)
	{
		_listId = id;
		OrderedRBDBTree<T>::clearMemCache();
	}

	void setDBAccess(DBNestedList<T> *ldb)
	{
		OrderedRBDBTree<T>::_ldb = ldb;
	}

	DBNestedList<T> *dbAccess() { return static_cast<DBNestedList<T> >(OrderedRBDBTree<T>::_ldb); }
};

// Hide the complexity of template classes behind simple names...
typedef DBListEntry<EntryListData> EntryListEntry;
typedef DBNestedList<EntryListData> EntryListDBAccess;
typedef OrderedRBTree<OrderedRBDBNestedTree<EntryListData> > EntryList;

#endif

