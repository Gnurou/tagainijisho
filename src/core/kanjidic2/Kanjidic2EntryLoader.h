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

#ifndef __CORE_KANJIDIC2_ENTRY_LOADER
#define __CORE_KANJIDIC2_ENTRY_LOADER

#include "core/EntryLoader.h"
#include "core/kanjidic2/Kanjidic2Entry.h"

class Kanjidic2EntryLoader : public EntryLoader {
  private:
    SQLite::Query kanjiQuery, variationsQuery, readingsQuery, nanoriQuery, componentsQuery,
        radicalsQuery, skipQuery, fourCornerQuery;
    QMap<QString, SQLite::Query> meaningsQueries;

  protected:
    QList<Kanjidic2Entry::KanjiMeaning> getMeanings(int id);

  public:
    Kanjidic2EntryLoader();
    virtual ~Kanjidic2EntryLoader() {}

    virtual Entry *loadEntry(EntryId id);
};

#endif
