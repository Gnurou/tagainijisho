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

#ifndef __CORE_KANJIDIC2_ENTRYSEARCHER
#define __CORE_KANJIDIC2_ENTRYSEARCHER

#include "core/EntrySearcher.h"
#include "core/kanjidic2/Kanjidic2Entry.h"

class Kanjidic2EntrySearcher : public EntrySearcher
{
	Q_OBJECT
private:
	SQLite::Query kanjiQuery, variationsQuery, readingsQuery, nanoriQuery,
		componentsQuery, radicalsQuery, skipQuery, fourCornerQuery, meaningsQuery;

protected:
	QList<Kanjidic2Entry::KanjiMeaning> getMeanings(int id);

public:
	Kanjidic2EntrySearcher(QObject *parent = 0);
	virtual ~Kanjidic2EntrySearcher() {}

	virtual int entryType() const { return KANJIDIC2ENTRY_GLOBALID; }
	virtual QueryBuilder::Column entryId() const { return QueryBuilder::Column("kanjidic2.entries", "id"); }

	virtual SearchCommand commandFromWord(const QString &word) const;

	virtual void buildStatement(QList<SearchCommand> &commands, QueryBuilder::Statement &statement);
	virtual QueryBuilder::Column canSort(const QString &sort, const QueryBuilder::Statement &statement);

	virtual Entry *loadEntry(int id);
};

#endif
