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

#ifndef __CORE_ENTRYLISTCACHE_H
#define __CORE_ENTRYLISTCACHE_H

#include "core/EntriesCache.h"
#include "core/EntryListDB.h"

/**
 * A cache class that is responsible for providing information about the
 * lists. Whenever an entry is changed in the DB, it has to be invalidated
 * here.
 *
 * Methods of this class are thread-safe.
 */
class EntryListCache {
private:
	static EntryListCache *_instance;
	SQLite::Connection _connection;
	EntryListDBAccess _dbAccess;
	QMap<quint64, EntryList> _cachedLists;
	QMutex _cacheLock;

	EntryListCache();

public:
	/// Returns a reference to the unique instance of this class.
	static EntryListCache &instance();
	/// Clears all resources used by the cache. instance() can be called
	/// again in order to allocate a new one.
	static void cleanup();

	const EntryList &get(quint64 id);
};

#endif
