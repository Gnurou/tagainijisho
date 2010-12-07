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
#include "core/EntriesCache.h"

#define LISTS_DB_TABLES_PREFIX "lists"

struct EntryListData {
	quint8 type;
	quint32 id;

	bool isList() const { return type == 0; }
	EntryRef entryRef() { return EntryRef(type, id); }
};

// Specializations for EntryListData
template <> void DBListEntry<EntryListData>::bindDataValues(SQLite::Query &query) const;
template <> void DBListEntry<EntryListData>::readDataValues(SQLite::Query &query, int start);

// Hide the complexity of template classes behind simple names...
typedef DBListEntry<EntryListData> EntryListEntry;
typedef DBList<EntryListData> EntryListDBAccess;

class EntryList : public OrderedRBTree<OrderedRBDBTree<EntryListData> >
{
private:
	EntryList() : OrderedRBTree<OrderedRBDBTree<EntryListData> >() { qCritical("Warning: initializing an EntryList from its default constructor (this should never happen)"); }

public:
	EntryList(EntryListDBAccess *dbAccess, quint64 listId) : OrderedRBTree<OrderedRBDBTree<EntryListData> >()
	{
		tree()->setDBAccess(dbAccess);
		tree()->setListId(listId);
	}

	const QString &label() const { return tree()->label(); }
	bool setLabel(const QString &label) { return tree()->setLabel(label); }
	int listId() const { return tree()->listId(); }

	void newList()
	{
		tree()->newList();
	}
// Needed because EntryListModel uses a QMap - but never called, actually.
friend class QMap<quint64, EntryList>;
};

#endif

