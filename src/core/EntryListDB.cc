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

template <> QString DBListEntry<EntryListData>::tableDataMembers()
{
	//return "listId INTEGER, type TINYINT, id INTEGER";
	return "listId INTEGER, type TINYINT, id INTEGER";
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
	//data.listId = query.valueUInt(start++);
	data.type = query.valueUInt(start++);
	data.id = query.valueUInt(start++);
	//else { data.id = 0; data.name = query.valueString(start++); }
}

