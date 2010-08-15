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

#include "EntryListCache.h"

EntryListCachedEntry::EntryListCachedEntry()
{
	_rowId = -1;
	_parent = -1;
	_position = -1;
	_type = -1;
	_id = -1;
	QSqlQuery query;
	query.exec("select count(*) from lists where parent is null");
	if (query.next()) _count = query.value(0).toInt();
	else _count = 0;
}

EntryListCachedEntry::EntryListCachedEntry(QSqlQuery &query)
{
	// Invalid model, return root list
	if (!query.next()) {
		*this = EntryListCachedEntry();
	} else {
		_rowId = query.value(0).toInt();
		_parent = query.value(1).isNull() ? -1 : query.value(1).toInt();
		_position = query.value(2).toInt();
		_type = query.value(3).isNull() ? -1 : query.value(3).toInt();
		_id = query.value(4).toInt();
		_label = query.value(5).toString();

		// If the type is a list, get its number of childs
		if (_type == -1) {
			QSqlQuery query2;
			query2.prepare("select count(*) from lists where parent = ?");
			query2.addBindValue(rowId());
			query2.exec();
			if (query2.next()) _count = query2.value(0).toInt();
		}
		else _count = 0;
	}
}

EntryListCache::EntryListCache()
{
	getByIdQuery.prepare("select lists.rowid, parent, position, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.rowid = ?");
//	getByParentPosQuery.prepare("select lists.rowid, parent, position, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.parent = ? order by position limit 1 offset ?");
	getByParentPosQuery.prepare("select lists.rowid, parent, position, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.parent = ? and position = ?");
//	getByParentPosRootQuery.prepare("select lists.rowid, parent, position, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.parent is null order by position limit 1 offset ?");
	getByParentPosRootQuery.prepare("select lists.rowid, parent, position, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.parent is null and position = ?");
//	fixListPositionQuery.prepare("update lists set position = ? where rowid = ?");
}

EntryListCache &EntryListCache::instance()
{
	static EntryListCache _instance;
	return _instance;
}

const EntryListCachedEntry &EntryListCache::get(int rowId)
{
	QMutexLocker ml(&_cacheLock);
	if (!rowIdCache.contains(rowId)) {
		getByIdQuery.addBindValue(rowId);
		getByIdQuery.exec();
		EntryListCachedEntry nCache(getByIdQuery);
		rowIdCache[nCache.rowId()] = nCache;
		rowParentCache[QPair<int, int>(nCache.parent(), nCache.position())] = nCache;
	}
	return rowIdCache[rowId];
}

const EntryListCachedEntry &EntryListCache::get(int parent, int pos)
{
	QMutexLocker ml(&_cacheLock);
	QPair<int, int> key(parent, pos);
	if (!rowParentCache.contains(key)) {
		QSqlQuery &query = parent == -1 ? getByParentPosRootQuery : getByParentPosQuery;
		if (parent != -1) query.addBindValue(parent);
		query.addBindValue(pos);
		query.exec();

		EntryListCachedEntry nCache(query);
/*		// Row inconsistency, try to fix!
		if (pos != nCache.position()) {
			fixListPositionQuery.addBindValue(pos);
			fixListPositionQuery.addBindValue(nCache.rowId());
			fixListPositionQuery.exec();
			nCache._position = pos;
		}*/
		rowIdCache[nCache.rowId()] = nCache;
		rowParentCache[key] = nCache;
	}
	return rowParentCache[key];
}

void EntryListCache::invalidate(uint rowId)
{
	QMutexLocker ml(&_cacheLock);
	if (!rowIdCache.contains(rowId)) return;
	const EntryListCachedEntry &cEntry = rowIdCache[rowId];
	rowParentCache.remove(QPair<int, int>(cEntry.parent(), cEntry.position()));
	rowIdCache.remove(rowId);
}

void EntryListCache::invalidateAll()
{
	QMutexLocker ml(&_cacheLock);
	rowParentCache.clear();
	rowIdCache.clear();
}
