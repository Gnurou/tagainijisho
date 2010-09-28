#ifndef CORE_ENTRYLISTDB_H
#define CORE_ENTRYLISTDB_H

#include "sqlite/Query.h"

/**
 * Represents how entries lists are actually recorded inside the DB.
 */
struct EntryList
{
	quint32 rowId;
	quint8 type;
	quint32 id;
	quint32 left;
	quint32 right;
	quint32 parent;
};

/**
 * Low-level database manipulation for entry lists. Avoid direct use!
 */
class EntryListDB
{
private:
	SQLite::Query insertEntryQuery;
	SQLite::Query removeEntryQuery;
	SQLite::Query updateLeftQuery;
	SQLite::Query updateRightQuery;
	SQLite::Query updateParentQuery;

public:
	EntryListDB();
	/**
	 * Prepare all the cached statements to be used with the given connection.
	 * Must be called before any use of the functions below is done.
	 */
	void prepareForConnection(const SQLite::Connection &connection);

	/// Returns the entry list corresponding to the given row id
	EntryList getEntry(quint32 rowId);
	/// Inserts the given entry into a list, returns the rowid
	quint32 insertEntry(quint8 type, quint32 id, quint32 listId);
	/// Removes the given entry from a list
	bool removeEntry(quint8 type, quint32 id, quint32 listId);
	/// Update the left member of an entry
	bool updateLeft(quint32 rowId, quint32 left);
	/// Update the right member of an entry
	bool updateRight(quint32 rowId, quint32 left);
};

#endif
