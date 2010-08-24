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

#ifndef __CORE__ENTRY_SEARCHER_H_
#define __CORE__ENTRY_SEARCHER_H_

#include "sqlite/Connection.h"
#include "core/Entry.h"
#include "core/Query.h"
#include "core/SearchCommand.h"

#include <QObject>
#include <QList>
#include <QStringList>

/**
 * A class that is able to search for and load entries from an entry source
 * (typically dictionary)
 */
class EntrySearcher : public QObject
{
	Q_OBJECT
private:
	QRegExp commandMatch;

protected:
	/**
	 * Connection to the user db file (and possibly other dbs)
	 * that is used to load the entries.
	 */
	SQLite::Connection connection;

	/**
	 * List of all valid commands for this searcher. Should
	 * be completed at construction time.
	 */
	QStringList validCommands;

	/**
	 * Loads misc data about this entry. This includes user tags,
	 * training information, and notes. This function should always
	 * be called as soon as the loadEntry() method of an entry searcher
	 * has created the right instance for its entry.
	 */
	void loadMiscData(Entry *entry);

public:
	EntrySearcher(QObject *parent = 0);
	virtual ~EntrySearcher() {}

	/**
	 * Returns the entry type that this searcher can load. It is
	 * the number that is returned by Entry::type() on entries loaded
	 * by this searcher.
	 */
	virtual int entryType() const = 0;

	/**
	 * Returns the column that is used to get entries id for this
	 * searcher.
	 */
	virtual QueryBuilder::Column entryId() const = 0;

	/**
	 * Turns single words into the corresponding command for this
	 * EntrySearcher. If the word is not recognized, an invalid
	 * SearchCommand is returned.
	 *
	 * Classes overloading this method should always delegate the
	 * word to the parent class, and only attempt to process it if
	 * the returned command is invalid.
	 */
	virtual SearchCommand commandFromWord(const QString &word) const;

	/**
	 * Builds the query corresponding to the given commands.
	 * The EntrySearcher is expected to remove all the commands
	 * it has processed, only leaving those it cannot handle.
	 *
	 * Overloading classes can reimplement this method to handle
	 * their own commands.
	 */
	virtual void buildStatement(QList<SearchCommand> &commands, QueryBuilder::Statement &statement);

	/**
	 * Returns the column that corresponds to the sorting keyword
	 * given in parameter. If the keyword is not recognized, an
	 * empty column is returned.
	 */
	virtual QueryBuilder::Column canSort(const QString &sort, const QueryBuilder::Statement &statement);

	/**
	 * Being given a reference to a result, load it
	 * from the database and return it. Returns null in
	 * case of problem, for instance if there is no other result.
	 */
	virtual Entry *loadEntry(int id) = 0;

	/**
	 * Converts a list of string commands and words into a list of commands,
	 * provided all the elements are understood by this searcher.
	 * Returns true if all the string elements were understood - in this case,
	 * the corresponding commands are all inserted into commands. If the
	 * string cannot be entirely processed by this searcher, this method
	 * returns false and the state of commands is undetermined.
	 *
	 */
	bool searchToCommands(const QStringList &searches, QList<SearchCommand> &commands) const;

	/**
	 * Prepares the query for being used to fetch an entry of
	 * this searcher, by adding the minimal necessary columns.
	 */
	void setColumns(QueryBuilder::Statement &statement) const;
};

#endif
