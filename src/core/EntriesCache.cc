/*
 *  Copyright (C) 2008  Alexandre Courbot
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

#include "core/EntriesCache.h"
#include "core/EntrySearcherManager.h"

#include <QtDebug>
#include <QCoreApplication>
#include <QMutexLocker>

EntriesCache *EntriesCache::_instance = 0;
PreferenceItem<int> EntriesCache::cacheSize("", "entriesCacheSize", 1000);

EntriesCache::EntriesCache(QObject *parent) : QObject(parent), _loadedEntriesMutex(QMutex::Recursive), _cacheMutex(QMutex::Recursive)
{
}

EntriesCache::~EntriesCache()
{
	// Clear the cache to (hopefully) remove all loaded entries
	_cache.clear();
}

void EntriesCache::init()
{
	if (!_instance) _instance = new EntriesCache();
}

void EntriesCache::cleanup()
{
	delete _instance;
}

EntryPointer<Entry> EntriesCache::_get(int type, int id)
{
	QPair<int, int> key(type, id);
	// First look if the entry is already loaded
	// Keep that lock until the Entry is loaded, as the entry searchers
	// are not thread-safe yet. :(
	// TODO Make the entry searchers thread-safe!
	QMutexLocker loadedLocker(&_loadedEntriesMutex);
	if (_loadedEntries.contains(key)) {
		return EntryPointer<Entry>(_loadedEntries[key]);
	}

	// Nope, we must load it from the database
	Entry *entry = EntrySearcherManager::instance().loadEntry(type, id);
	// If the entry is not found, do not add anything to the cache and return
	// immediatly
	if (!entry) return EntryPointer<Entry>(entry);
	// All the signal processing of the entry must take place in the main thread
	entry->moveToThread(QCoreApplication::instance()->thread());
	// Used to remove entries from the list of loaded entries
	//connect(entry, SIGNAL(entryDestroyed(Entry *)), this, SLOT(onDestroyed(Entry *)));

	_loadedEntries[key] = entry;
	// Keep a reference to the entry now - this is needed
	// for the case the cache size is zero
	EntryPointer<Entry> ret(entry);
	_cacheMutex.lock();
	_cache << ret;
	while (_cache.size() > cacheSize.value()) _cache.removeFirst();
	_cacheMutex.unlock();
	loadedLocker.unlock();

	return ret;
}

void EntriesCache::_removeAndDelete(const Entry *entry)
{
	QMutexLocker loadedEntriesLock(&_loadedEntriesMutex);
	// From here we know that the reference counter of our entry will not be changed
	// Have we created a new reference to this entry by the meantime?
	if (entry->ref > 0) return;
	// No, we can safely remove and delete it then!
	_loadedEntries.remove(QPair<int, int>(entry->type(), entry->id()));
	delete entry;
}
