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

#include "sqlite/Query.h"

/**
 * Represents an item within the entries list. A list item
 * is uniquely identified by its rowid ; it has a parent which rowid
 * can be retrieved through the parent() method (root elements will
 * return -1 here), and a position relative to its parent. List
 * items can either be lists or references to entries. Lists have a label
 * and childs, whereas entries have no child, but can provide an entry
 * reference.
 *
 * Instances of this class are never created manually - instead, constant
 * references are obtained through the EntryListCache singleton.
 */
class EntryListCachedEntry {
private:
	int _rowId;
	int _parent;
	int _position;
	int _count;
	int _type;
	int _id;
	QString _label;
	
	/// Prepare the cached entry from the result row of the query
	/// The query is advanced to the next row
	EntryListCachedEntry(SQLite::Query &query);
	
public:
	/// Root entry
	EntryListCachedEntry();
	
	bool isRoot() const { return _rowId == -1; }
	bool isList() const { return type() == -1; }
	bool isEntry() const { return type() >= 0; }
	int rowId() const { return _rowId; }
	int parent() const { return _parent; }
	int position() const { return _position; }
	
	/// For lists

	/// Returns the label of the list
	const QString &label() const { return _label; }
	/// Returns the number of childs within the list
	int count() const { return _count; }
	
	/// For entries
	
	/// Type of the entry
	int type() const { return _type; }
	/// Id of the entry
	int id() const { return _id; }
	/// Reference to the entry
	EntryRef entryRef() const { return EntryRef(type(), id()); }

friend class EntryListCache;
};

/**
 * A cache class that is responsible for providing information about the
 * lists. Whenever an entry is changed in the DB, it has to be invalidated
 * here.
 *
 * Methods of this class are thread-safe.
 */
class EntryListCache {
private:
	EntryListCache();

	mutable QHash<int, EntryListCachedEntry> rowIdCache;
	mutable QHash<QPair<int, int>, EntryListCachedEntry> rowParentCache;
	QMutex _cacheLock;

	SQLite::Query getByIdQuery;
	SQLite::Query getByParentPosQuery;
	SQLite::Query getByParentPosRootQuery;
//	SQLite::Query fixListPositionQuery;

public:
	/// Returns a reference to the unique instance of this class.
	static EntryListCache &instance();
	
	/// Returns the entry with the corresponding rowid, or the root
	/// entry if it does not exist.
	const EntryListCachedEntry& get(int rowId);
	/// Returns the entry at the given position, or the root
	/// if it does not exist.
	const EntryListCachedEntry& get(int parent, int pos);
	
	/// Invalidate the entry which id is given as parameter
	void invalidate(uint rowId);
	/// Invalidate all entries
	void invalidateAll();
};

#endif
