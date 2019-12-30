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

#include "tagaini_config.h"
#include "core/EntriesCache.h"

#include <QtDebug>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QWeakPointer>
#include <QDataStream>

EntriesCache *EntriesCache::_instance = 0;
PreferenceItem<int> EntriesCache::cacheSize("", "entriesCacheSize", 1000);

QDataStream &operator<<(QDataStream &out, const EntryRef &ref)
{
	out << ref.first << ref.second;
	return out;
}

QDataStream &operator>>(QDataStream &in, EntryRef &ref)
{
	in >> ref.first >> ref.second;
	return in;
}

EntriesCache::EntriesCache() : _loadedEntriesMutex(QMutex::Recursive), _cacheMutex(QMutex::Recursive)
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
	_instance = 0;
}

bool EntriesCache::addLoader(EntryType type, EntryLoader *loader)
{
	if (_loaders.contains(type)) return false;
	_loaders.insert(type, loader);
	return true;
}

bool EntriesCache::removeLoader(EntryType type)
{
	if (!_loaders.contains(type)) return false;
	_loaders.remove(type);
	return true;
}

EntryLoader *EntriesCache::loaderFor(EntryType type)
{
	if (!_loaders.contains(type)) return 0;
	return _loaders[type];
}

EntryPointer EntriesCache::_get(EntryType type, EntryId id)
{
	EntryRef key(type, id);
	// First look if the entry is already loaded
	// Keep that lock until the Entry is loaded, as the entry searchers
	// are not thread-safe yet. :(
	// TODO Make the entry searchers thread-safe!
	QMutexLocker loadedLocker(&_loadedEntriesMutex);
	if (_loadedEntries.contains(key)) {
		return _loadedEntries[key].toStrongRef();
	}

	// Nope, we must load it from the database
	EntryLoader *loader = loaderFor(type);
	Entry *entry = loader ? loader->loadEntry(id) : 0;
	// If the entry is not found, do not add anything to the cache and return
	// a null pointer
	if (!entry) { return EntryPointer(); }
	// All the signal processing of the entry must take place in the main thread
	entry->moveToThread(QCoreApplication::instance()->thread());

	// Keep a reference to the entry now - this is needed
	// for the case the cache size is zero
	EntryPointer ret(entry, &_removeAndDelete);
	_cacheMutex.lock();
	_cache << ret;
	while (_cache.size() > cacheSize.value()) _cache.removeFirst();
	// Keep a weak pointer in the list of loaded entries - it is convertible to a
	// QSharedPointer but will not influence the reference count.
	_loadedEntries[key] = ret.toWeakRef();
#ifdef DEBUG_ENTRIES_CACHE
	qDebug("Entry <%d,%d> loaded, %d in cache", entry->type(), entry->id(), _loadedEntries.size());
#endif
	_cacheMutex.unlock();
	loadedLocker.unlock();

	return ret;
}

void EntriesCache::_removeAndDelete(const Entry *entry)
{
	QMutexLocker loadedEntriesLock(&_instance->_loadedEntriesMutex);
	// From here we know that the reference counter of our entry will not be changed
	// Have we created a new reference to this entry by the meantime?
	if (entry->ref.loadRelaxed() > 0) return;
	// No, we can safely remove and delete it then!
	_instance->_loadedEntries.remove(EntryRef(entry->type(), entry->id()));
#ifdef DEBUG_ENTRIES_CACHE
	qDebug("Entry <%d,%d> deleted, %d in cache", entry->type(), entry->id(), _instance->_loadedEntries.size());
#endif
	delete entry;
}
