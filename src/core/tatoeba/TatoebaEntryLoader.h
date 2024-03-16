/*
 *  Copyright (C) 2011  Alexandre Courbot
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

#ifndef __CORE_TATOEBA_ENTRY_LOADER_H_
#define __CORE_TATOEBA_ENTRY_LOADER_H_

#include "core/EntryLoader.h"
#include "core/tatoeba/TatoebaEntry.h"

class TatoebaEntryLoader : public EntryLoader {
  private:
  protected:
    SQLite::Query sentencesQuery;

  public:
    TatoebaEntryLoader();
    virtual ~TatoebaEntryLoader();

    virtual Entry *loadEntry(EntryId id);
};

#endif
