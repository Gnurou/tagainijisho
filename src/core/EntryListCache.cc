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
#include "Database.h"

EntryListCachedEntry::EntryListCachedEntry()
{
	_rowId = 0;
	_parent = 0;
	_position = -1;
	_type = -1;
	_id = -1;
	SQLite::Query query(Database::connection());
	query.exec("select count(*) from lists where parent is null");
	if (query.next()) _count = query.valueInt(0);
	else _count = 0;
}

EntryListCachedEntry::EntryListCachedEntry(SQLite::Query &query) : _next(0), _prev(0)
{
	_rowId = query.valueUInt64(0);
	_parent = query.valueUInt64(1);
	_nextId = query.valueUInt64(2);
	_type = query.valueIsNull(3) ? -1 : query.valueInt(3);
	_id = query.valueInt(4);
	_label = query.valueString(5);

	// If the type is a list, get its number of childs
	if (_type == -1) {
		SQLite::Query query2(Database::connection());
		// TODO cache this!
		query2.prepare("select count(*) from lists where parent = ?");
		query2.bindValue(rowId());
		query2.exec();
		if (query2.next()) _count = query2.valueInt(0);
	}
	else _count = 0;
}

EntryListCachedList::EntryListCachedList(quint64 id) : _dirty(true), _tail(0)
{
	SQLite::Query query(Database::connection());
	// TODO cache this
	query.prepare("select lists.rowid, parent, next, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where parent = ?");
	query.bindValue(id);
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

EntryListCache *EntryListCache::_instance = 0;

EntryListCache::EntryListCache()
{
	//getByIdQuery.useWith(Database::connection());
	//getByIdQuery.prepare("select lists.rowid, parent, position, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.rowid = ?");
	//getByParentPosQuery.useWith(Database::connection());
	//getByParentPosQuery.prepare("select lists.rowid, parent, position, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.parent = ? and position = ?");
	//getByParentPosRootQuery.useWith(Database::connection());
	//getByParentPosRootQuery.prepare("select lists.rowid, parent, position, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.parent is null and position = ?");
}

EntryListCache &EntryListCache::instance()
{
	if (!_instance) _instance = new EntryListCache();
	return *_instance;
}

void EntryListCache::cleanup()
{
	delete _instance;
	_instance = 0;
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
		SQLite::Query query(Database::connection());
		// TODO cache this
		query.prepare("select parent from lists where rowid = ?");
		query.bindValue(rowId);
		query.exec();
		if (!query.next()) return rootEntry();
		getList(query.valueUInt64(0));
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
