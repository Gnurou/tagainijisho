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

#include "core/EntryLoader.h"
#include "core/Database.h"

EntryLoader::EntryLoader() {
    if (!connection.connect(Database::userDBFile())) {
        qFatal("EntrySearcher cannot connect to user database!");
    }
    trainQuery.useWith(&connection);
    tagsQuery.useWith(&connection);
    notesQuery.useWith(&connection);
    listsQuery.useWith(&connection);

    // Cache queries
    trainQuery.prepare("select dateAdded, dateLastTrain, nbTrained, nbSuccess, "
                       "dateLastMistake, "
                       "score from training where type = ? and id = ?");
    tagsQuery.prepare("select tagId from taggedEntries where type = ? and id = ? "
                      "order by date");
    notesQuery.prepare("select noteId, dateAdded, dateLastChange, note from notes join "
                       "notesText on notes.noteId == notesText.docid where type = ? and id = "
                       "? "
                       "order by dateAdded ASC, noteId ASC");
    listsQuery.prepare("select rowid from lists where type = ? and id = ?");
}

EntryLoader::~EntryLoader() {
    // Clear the cached queries to ensure they do not survive longer than their
    // connection
    trainQuery.clear();
    tagsQuery.clear();
    notesQuery.clear();
    listsQuery.clear();
}

static QDateTime variantToDate(const SQLite::Query &query, int col) {
    if (query.valueType(col) == SQLite::Integer)
        return QDateTime::fromSecsSinceEpoch(query.valueUInt64(col));
    else
        return QDateTime();
}

void EntryLoader::loadMiscData(Entry *entry) {
    // Load training data
    trainQuery.bindValue(entry->type());
    trainQuery.bindValue(entry->id());
    trainQuery.exec();
    // The entry is registered, so lets load its data
    if (trainQuery.next()) {
        entry->setDateAdded(variantToDate(trainQuery, 0));
        entry->setDateLastTrained(variantToDate(trainQuery, 1));
        entry->setNbTrained(trainQuery.valueInt(2));
        entry->setNbSuccess(trainQuery.valueInt(3));
        entry->setDateLastMistake(variantToDate(trainQuery, 4));
        entry->_score = trainQuery.valueInt(5);
    }
    trainQuery.reset();

    // Tags data
    tagsQuery.bindValue(entry->type());
    tagsQuery.bindValue(entry->id());
    tagsQuery.exec();
    while (tagsQuery.next())
        entry->_tags << Tag::getTag(tagsQuery.valueUInt(0));
    tagsQuery.reset();

    // Notes data
    notesQuery.bindValue(entry->type());
    notesQuery.bindValue(entry->id());
    notesQuery.exec();
    while (notesQuery.next()) {
        entry->_notes << Entry::Note(
            notesQuery.valueInt(0), QDateTime::fromSecsSinceEpoch(notesQuery.valueInt(1)),
            QDateTime::fromSecsSinceEpoch(notesQuery.valueInt(2)), notesQuery.valueString(3));
    }
    notesQuery.reset();

    // Lists data
    listsQuery.bindValue(entry->type());
    listsQuery.bindValue(entry->id());
    listsQuery.exec();
    while (listsQuery.next()) {
        entry->_lists << listsQuery.valueUInt64(0);
    }
    listsQuery.reset();
}
