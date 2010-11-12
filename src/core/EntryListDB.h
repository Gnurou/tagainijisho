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
};

// Specializations for EntryListData
template <> void DBListEntry<EntryListData>::bindDataValues(SQLite::Query &query) const;
template <> void DBListEntry<EntryListData>::readDataValues(SQLite::Query &query, int start);

// Hide the complexity of template classes behind simple names...
typedef DBListEntry<EntryListData> EntryListEntry;
typedef DBList<EntryListData> EntryListDBAccess;
typedef OrderedRBTree<OrderedRBDBTree<EntryListData> > EntryList;

#endif

