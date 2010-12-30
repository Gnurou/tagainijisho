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

#ifndef __CORE_JMDICT_ENTRY_SEARCHER_H_
#define __CORE_JMDICT_ENTRY_SEARCHER_H_

#include <QVector>
#include <QMap>

#include "core/EntrySearcher.h"
#include "core/Preferences.h"

class JMdictEntrySearcher : public EntrySearcher
{
	Q_OBJECT
private:
	static quint64 _miscFilterMask;
	static quint64 _explicitlyRequestedMiscs;

protected slots:
	void updateMiscFilterMask();

public:
	static quint64 miscFilterMask() { return _miscFilterMask; }
	static quint64 explicitlyRequestedMiscs() { return _explicitlyRequestedMiscs; }

	JMdictEntrySearcher(QObject *parent = 0);
	virtual ~JMdictEntrySearcher() {}

	virtual QueryBuilder::Column entryId() const { return QueryBuilder::Column("jmdict.entries", "id"); }

	virtual SearchCommand commandFromWord(const QString &word) const;

	virtual void buildStatement(QList<SearchCommand> &commands, QueryBuilder::Statement &statement);
	virtual QueryBuilder::Column canSort(const QString &sort, const QueryBuilder::Statement &statement);

	static PreferenceItem<QString> miscPropertiesFilter;
};

#endif
