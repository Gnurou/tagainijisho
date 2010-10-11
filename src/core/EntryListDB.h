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

/**
 * Represents how entries lists are actually recorded inside the DB.
 */
struct EntryList
{
	quint32 rowId;
	// 0: list, > 0: entry type
	quint8 type;
	quint32 id;
	QString name;
	quint32 parent;
	quint32 left;
	quint32 right;
};

/**
 * Low-level database manipulation for RB trees. Avoid direct use!
 */
class EntryListDB
{
private:
	const QString _tableName;
	SQLite::Connection *_connection;

	SQLite::Query getEntryQuery;
	SQLite::Query insertEntryQuery;
	SQLite::Query removeEntryQuery;
	SQLite::Query updateLeftQuery;
	SQLite::Query updateRightQuery;
	SQLite::Query updateParentQuery;


public:
	EntryListDB(const QString &tableName, SQLite::Connection *connection = 0);

	/**
	 * Prepare all the cached statements to be used with the given connection.
	 * Must be called before any use of the functions below is done.
	 */
	void prepareForConnection(SQLite::Connection *connection);


	QString createTableStatement() {
		return QString("create table %1(rowid INTEGER PRIMARY KEY, listId INTEGER, type TINYINT, id INTEGER, parent INTEGER, left INTEGER, right INTEGER)").arg(_tableName);

	}
	/// Returns the entry list corresponding to the given row id
	EntryList getEntry(quint32 rowid);
	/// Inserts the given entry into a list, returns the rowid
	quint32 insertEntry(quint32 listId, quint8 type, quint32 id, quint32 parent, quint32 left, quint32 right);
	/// Inserts the given list, returns the rowid
	quint32 insertList(quint32 listId, const QString &name, quint32 parent, quint32 left, quint32 right);
	/// Removes the given entry from a list
	bool removeEntry(quint32 rowid);
	/// Update the parent member of an entry
	bool updateParent(quint32 rowid, quint32 parent);
	/// Update the left member of an entry
	bool updateLeft(quint32 rowid, quint32 left);
	/// Update the right member of an entry
	bool updateRight(quint32 rowid, quint32 right);
};

#endif
