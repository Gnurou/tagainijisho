/*
 *  Copyright (C) 2008/2009/2010  Alexandre Courbot
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

#ifndef __CORE__ENTRY_LOADER_H_
#define __CORE__ENTRY_LOADER_H_

#include "sqlite/Connection.h"
#include "sqlite/Query.h"
#include "core/Entry.h"

#include <QObject>

/**
 * Base class for loading entries of a given type.
 */
class EntryLoader : public QObject
{
	Q_OBJECT
private:
	SQLite::Query trainQuery, tagsQuery, notesQuery, listsQuery;

protected:
	/**
	 * Connection to the user db file (and possibly other dbs)
	 * that is used to load the entries.
	 */
	SQLite::Connection connection;

	/**
	 * Loads misc data about this entry. This includes user tags,
	 * training information, and notes. This function should always
	 * be called as soon as the loadEntry() method of an entry searcher
	 * has created the right instance for its entry.
	 */
	void loadMiscData(Entry *entry);

public:
	EntryLoader(QObject *parent = 0);
	virtual ~EntryLoader();

	/**
	 * Being given a reference to a result, load it
	 * from the database and return it. Returns null in
	 * case of problem, for instance if there is no other result.
	 */
	virtual Entry *loadEntry(EntryId id) = 0;
};

#endif
