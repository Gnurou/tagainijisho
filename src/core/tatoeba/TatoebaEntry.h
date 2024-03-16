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

#ifndef __CORE_TATOEBA_ENTRY_H_
#define __CORE_TATOEBA_ENTRY_H_

#include "core/EntriesCache.h"

#define TATOEBAENTRY_GLOBALID 3
#define TATOEBADB_REVISION 1

class TatoebaEntry : public Entry {
    Q_OBJECT

  public:
    TatoebaEntry(int id);

    friend class TatoebaEntrySearcher;
};

typedef QSharedPointer<TatoebaEntry> TatoebaEntryPointer;
Q_DECLARE_METATYPE(TatoebaEntryPointer)
typedef QSharedPointer<const TatoebaEntry> ConstTatoebaEntryPointer;
Q_DECLARE_METATYPE(ConstTatoebaEntryPointer)

class TatoebaEntryRef : public EntryRef {
  public:
    TatoebaEntryRef(quint32 id) : EntryRef(TATOEBAENTRY_GLOBALID, id) {}
    TatoebaEntryPointer get() const { return EntryRef::get().staticCast<TatoebaEntry>(); }
};

#endif
