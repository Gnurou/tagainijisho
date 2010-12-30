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

#ifndef __CORE_JMDICT_ENTRY_LOADER_H_
#define __CORE_JMDICT_ENTRY_LOADER_H_

#include "core/EntryLoader.h"
#include "core/jmdict/JMdictEntry.h"

class JMdictEntryLoader : public EntryLoader
{
	Q_OBJECT
private:

protected:
	SQLite::Query kanjiQuery, kanaQuery, sensesQuery, glossQuery, jlptQuery;
public:
	JMdictEntryLoader(QObject *parent = 0);
	virtual ~JMdictEntryLoader();

	virtual Entry *loadEntry(EntryId id);
};

#endif
