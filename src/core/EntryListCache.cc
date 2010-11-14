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

EntryListCache *EntryListCache::_instance = 0;

EntryListCache::EntryListCache() : _dbAccess(LISTS_DB_TABLES_PREFIX)
{
	if (!_connection.connect(Database::userDBFile())) {
		qFatal("EntryListCache cannot connect to user database!");
	}
	_dbAccess.prepareForConnection(&_connection);
}

EntryListCache::~EntryListCache()
{
	foreach (EntryList *list, _cachedLists) {
		delete list;
	}
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

const EntryList *EntryListCache::_get(quint64 id)
{
	QMutexLocker ml(&_cacheLock);
	if (!_cachedLists.contains(id))  {
		// We know we can only be called with list Ids that actually exist in the DB,
		// so no need to bother about non-existing entries
		_cachedLists.insert(id, new EntryList(&_dbAccess, id));
	}
	// Will always git (see above)
	return _cachedLists[id];
}

QPair<const EntryList *, quint32> EntryListCache::_getOwner(quint64 id)
{
	SQLite::Query ownerQuery(_dbAccess.connection());
	ownerQuery.prepare(QString("select rowid, leftSize from %1 where type = 0 and id = ?").arg(_dbAccess.tableName()));
	ownerQuery.bindValue(id);
	ownerQuery.exec();
	if (!ownerQuery.next()) QPair<const EntryList *, quint32>();
	quint64 rowid = ownerQuery.valueUInt64(0);
	quint32 pos = ownerQuery.valueUInt(1);
	ownerQuery.reset();

	// Now go back to the root of the list, and calculate the position of the item
	SQLite::Query goUpQuery(_dbAccess.connection());
	goUpQuery.prepare(QString("select parent, leftSize from %1 where rowid = ?").arg(_dbAccess.tableName()));
	do {
		goUpQuery.bindValue(rowid);
		goUpQuery.exec();
		quint64 parent = goUpQuery.valueUInt64(0);
		goUpQuery.reset();
		if (parent == 0) break;
		else {
			pos += goUpQuery.valueUInt(1);
			rowid = parent;
		}
	} while (1);

	// rowid now contains the root of the containing list, we can look its id up
	SQLite::Query listFromRootQuery(_dbAccess.connection());
	listFromRootQuery.prepare(QString("select listId from %1Roots where rootId = ?").arg(_dbAccess.tableName()));
	listFromRootQuery.bindValue(rowid);
	listFromRootQuery.exec();
	if (!listFromRootQuery.next()) return QPair<const EntryList *, quint32>();
	else {
		const EntryList *ret = get(listFromRootQuery.valueUInt64(0));
		listFromRootQuery.reset();
		return QPair<const EntryList *, quint32>(ret, pos);
	}
}

