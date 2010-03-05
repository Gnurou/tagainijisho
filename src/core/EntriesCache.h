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

#ifndef __CORE_ENTRIES_CACHE_H
#define __CORE_ENTRIES_CACHE_H

#include "core/Preferences.h"
#include "core/Entry.h"

#include <QHash>
#include <QPair>
#include <QObject>
#include <QQueue>
#include <QMutex>

class EntryRef;

/**
 * The EntryCache plays a double role:
 *
 * # It ensures that there is no more than a unique instance of every Entry
 *   loaded at the same time, for consistency and memory saving reasons.
 * # It keeps frequently used entries in memory to avoid reloading them
 *   from the database every time they are necessary and increase
 *   performances.
 *
 * ALL Entry loading operations MUST be performed through this class, which
 * is the only one allowed to access the Entry loaders. Respecting this rule
 * ensures data consistency and safety, and greatly simplify the UI design.
 */
class EntriesCache : public QObject
{
    Q_OBJECT
private:
	static EntriesCache * _instance;

	QHash<EntryRef, QWeakPointer<Entry> > _loadedEntries;
	QMutex _loadedEntriesMutex;

	QQueue<EntryPointer> _cache;
	QMutex _cacheMutex;

	/**
	 * This method is automatically called when the reference count of
	 * an entry pointer reaches 0. It removes the entry from the list
	 * of loaded entries list and takes care to delete the entry.
	 *
	 * This operation is atomic, i.e. it ensures no reference to the
	 * deleted entry is created by the meantime.
	 */
	static void _removeAndDelete(const Entry *entry);

	friend class Entry;

	EntryPointer _get(int type, int id);
	EntriesCache(QObject *parent = 0);
	~EntriesCache();

	/**
	 * Returns the unique instance of the entry given as argument,
	 * loading it from the database if necessary. Returns null
	 * if the entry could not be loaded.
	 */
	static EntryPointer get(int type, int id) {
		return _instance->_get(type, id);
	}

public:
	static void init();
	static void cleanup();

	/**
	 * The size of the cache can be modified in real-time through this value.
	 */
	static PreferenceItem<int> cacheSize;
friend class EntryRef;
};

/**
 * A very lightweight reference to an entry.
 *
 * It can be read/written using QDataStreams and can also be encapsulated as
 * a QVariant, making it the preferred solution to keep track of an entry
 * we do not need to be loaded now but might need later.
 */
class EntryRef : protected QPair<quint8, quint32>
{
public:
	/// Constructs an invalid reference
	EntryRef() : QPair<quint8, quint32>(0, 0) {}
	EntryRef(const EntryPointer &entry) : QPair<quint8, quint32>(entry->type(), entry->id()) {}
	EntryRef(quint8 type, quint32 id) : QPair<quint8, quint32>(type, id) {}
	bool isValid() const { return first != 0; }
	quint8 type() const { return first; }
	quint32 id() const { return second; }

	/**
	 * Returns a pointer to the entry corresponding to this reference. If needed, the entry will
	 * be loaded from the database.
	 *
	 * Note that the referenced entry may not exist - it is a good idea to check whether the 
	 * returned pointer actually points to something before using it.
	 */
	EntryPointer get() const { return EntriesCache::get(type(), id()); }
	bool operator==(const EntryRef &other) const { return static_cast<const QPair<quint8, quint32> >(*this) == other; }
	bool operator!=(const EntryRef &other) const { return !(*this == other); }
	friend QDataStream &operator<<(QDataStream &out, const EntryRef &ref);
	friend QDataStream &operator>>(QDataStream &in, EntryRef &ref);
};

inline uint qHash(const EntryRef &key)
{
	uint h1 = qHash(key.type());
	uint h2 = qHash(key.id());
	return ((h1 << 16) | (h1 >> 16)) ^ h2;
}

#endif
