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
	qDebug() << "list cache initialized";
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

const EntryList &EntryListCache::get(quint64 id)
{
	QMutexLocker ml(&_cacheLock);
	if (!_cachedLists.contains(id))  {
		// We know we can only be called with list Ids that actually exist in the DB,
		// so no need to bother about non-existing entries
		_cachedLists.insert(id, EntryList(&_dbAccess, id));
	}
	// Will always git (see above)
	return _cachedLists[id];
}

