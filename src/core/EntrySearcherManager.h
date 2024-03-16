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

#ifndef __CORE_ENTRY_SEARCHER_MANAGER_H_
#define __CORE_ENTRY_SEARCHER_MANAGER_H_

#include "core/EntrySearcher.h"
#include "core/Preferences.h"
#include "core/QueryBuilder.h"

#include <QRegularExpression>

class EntrySearcherManager {
  private:
    static EntrySearcherManager *_instance;
    QList<EntrySearcher *> _instances;

    QRegularExpression quotedWordsMatch;
    QRegularExpression validSearchCompoundMatch;
    QRegularExpression validSearchMatch;

  public:
    EntrySearcherManager();

    void addInstance(EntrySearcher *searcher);
    const QList<EntrySearcher *> &instances() { return _instances; }
    bool removeInstance(EntrySearcher *searcher);

    static EntrySearcherManager &instance();

    QStringList splitSearchString(const QString &searchString);

    /**
     * Attempts to build the query corresponding to the search string
     * using the attached entry searchers. Returns true if a query
     * has been build, false otherwise - in which case the content
     * of the query is unspecified.
     */
    bool buildQuery(const QString &search, QueryBuilder &query);

    /**
     * Returns a pointer to the entry searcher capable of handling
     * entries of type entryType. Returns null if no such entry
     * searcher has been registered.
     */
    EntrySearcher *getEntrySearcher(EntryType entryType);

    static PreferenceItem<bool> studiedEntriesFirst;
};

#endif
