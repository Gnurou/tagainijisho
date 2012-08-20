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

#include "core/EntryListDB.h"
#include "core/EntryListCache.h"

template <> QString DBListEntry<EntryListData>::tableDataMembers()
{
	//return "listId INTEGER, type TINYINT, id INTEGER";
	return "type TINYINT, id INTEGER";
}

template <> void DBListEntry<EntryListData>::bindDataValues(SQLite::Query &query) const
{
	//query.bindValue(data.listId);
	query.bindValue(data.type);
	//if (data.type == 0) query.bindValue(data.name);
	query.bindValue(data.id);
}

template <> void DBListEntry<EntryListData>::readDataValues(SQLite::Query &query, int start)
{
	data.type = query.valueUInt(start++);
	data.id = query.valueUInt(start++);
}

EntryListDBAccess::EntryListDBAccess(const QString &tableName, SQLite::Connection *connection) : DBList<EntryListData>(tableName, connection)
{
}

bool EntryListDBAccess::createDataIndexes(SQLite::Connection *connection)
{
	if (!connection->exec(QString("CREATE INDEX idx_%1_type_id ON %1(type,id)").arg(tableName().asQString()))) return false;
	if (!connection->exec(QString("CREATE INDEX idx_%1Roots_rootId ON %1Roots(rootId)").arg(tableName().asQString()))) return false;
	return true;
}

bool EntryList::remove(int index)
{
	TreeType::Node *node = getNode(index);
	if (!node) return false;
	const EntryListData &data = node->value();
	// We have to recursively erase the child list
	if (data.isList()) {
		EntryList *list = EntryListCache::get(data.id);
		while(1) {
			int ls = list->size();
			if (ls == 0) break;
			while (ls--) if (!list->remove(0)) return false;
		}
		// Invalidate the cache for the removed list
		EntryListCache::clearListCache(data.id);
		// Finally, delete the list
		list->tree()->removeList();
	}
	return OrderedRBTree<OrderedRBDBTree<EntryListData> >::remove(index);
}
