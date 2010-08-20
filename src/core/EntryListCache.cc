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
	_rowId = 0;
	_parent = 0;
	_position = -1;
	_type = -1;
	_id = -1;
	QSqlQuery query;
	query.exec("select count(*) from lists where parent is null");
	if (query.next()) _count = query.value(0).toInt();
	else _count = 0;
}

EntryListCachedEntry::EntryListCachedEntry(QSqlQuery &query) : _next(0), _prev(0)
{
	_rowId = query.value(0).toULongLong();
	_parent = query.value(1).toULongLong();
	_nextId = query.value(2).toULongLong();
	_type = query.value(3).isNull() ? -1 : query.value(3).toInt();
	_id = query.value(4).toInt();
	_label = query.value(5).toString();

	// If the type is a list, get its number of childs
	if (_type == -1) {
		QSqlQuery query2;
		// TODO cache this!
		query2.prepare("select count(*) from lists where parent = ?");
		query2.addBindValue(rowId());
		query2.exec();
		if (query2.next()) _count = query2.value(0).toInt();
	}
	else _count = 0;
}

EntryListCachedList::EntryListCachedList(quint64 id) : _dirty(true), _tail(0)
{
	QSqlQuery query;
	// TODO cache this
	query.prepare("select lists.rowid, parent, next, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where parent = ?");
	query.addBindValue(id);
	query.exec();

	while (query.next()) {
		EntryListCachedEntry cachedEntry(query);
		_entries[cachedEntry.rowId()] = cachedEntry;
	}

	// Now link the entries together
	foreach(const EntryListCachedEntry &entry, _entries.values()) {
		EntryListCachedEntry *curEntry = &_entries[entry.rowId()];
		if (entry._nextId != 0) {
			curEntry->_next = &_entries[entry._nextId];
			_entries[entry._nextId]._prev = curEntry;
		}
		else _tail = curEntry;
	}
	// Necessary to give a position to all the items
	rebuildEntriesArray();
}

const QVector<const EntryListCachedEntry *> &EntryListCachedList::entriesArray() const
{
	if (_dirty) rebuildEntriesArray();
	return _entriesArray;
}

void EntryListCachedList::rebuildEntriesArray() const
{
	int entryCpt = _entries.size();
	EntryListCachedEntry *tail(_tail);
	_entriesArray.resize(entryCpt);
	while (tail != 0) {
		tail->_position = --entryCpt;
		_entriesArray[entryCpt] = tail;
		tail = tail->_prev;
	}
	_dirty = false;
}

EntryListCache::EntryListCache()
{
//	getByIdQuery.prepare("select lists.rowid, parent, position, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.rowid = ?");
//	getByParentPosQuery.prepare("select lists.rowid, parent, position, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.parent = ? and position = ?");
//	getByParentPosRootQuery.prepare("select lists.rowid, parent, position, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.parent is null and position = ?");
}

EntryListCache &EntryListCache::instance()
{
	static EntryListCache _instance;
	return _instance;
}

const EntryListCachedEntry &EntryListCache::rootEntry()
{
	// Represents root list
	static EntryListCachedEntry invalidEntry;
	static const EntryListCachedList &rootList = getList(0);
	invalidEntry._count = rootList.entriesArray().size();
	return invalidEntry;
}

const EntryListCachedEntry &EntryListCache::get(int rowId)
{
	if (rowId == 0) return rootEntry();
	// No entry for this item in the id cache, we must therefore load the list it belongs to
	if (!rowIdCache.contains(rowId)) {
		QSqlQuery query;
		// TODO cache this
		query.prepare("select parent from lists where rowid = ?");
		query.addBindValue(rowId);
		query.exec();
		if (!query.next()) return rootEntry();
		getList(query.value(0).toULongLong());
	}
	// Still no hit, we have a problem then
	if (!rowIdCache.contains(rowId)) return rootEntry();
	return *rowIdCache[rowId];
}

const EntryListCachedList &EntryListCache::getList(quint64 id)
{
	QMutexLocker ml(&_cacheLock);
	if (!_cachedLists.contains(id))  {
		_cachedLists[id] = EntryListCachedList(id);
		// Add the entries to the global id cache
		foreach (const EntryListCachedEntry *entry, _cachedLists[id].entriesArray()) {
			rowIdCache[entry->rowId()] = entry;
		}
	}
	return _cachedLists[id];
}

const EntryListCachedEntry &EntryListCache::get(int parent, int pos)
{
	const EntryListCachedList &parentList = getList(parent);
	if (pos < 0 || pos >= parentList.entriesArray().size()) return rootEntry();
	else return *(parentList.entriesArray()[pos]);
}
/*
void EntryListCache::invalidate(uint rowId)
{
	QMutexLocker ml(&_cacheLock);
	if (!rowIdCache.contains(rowId)) return;
//	const EntryListCachedEntry &cEntry = *rowIdCache[rowId];
//	rowParentCache.remove(QPair<int, int>(cEntry.parent(), cEntry.position()));
	rowIdCache.remove(rowId);
}

void EntryListCache::invalidateAll()
{
	QMutexLocker ml(&_cacheLock);
//	rowParentCache.clear();
	rowIdCache.clear();
}
*/
