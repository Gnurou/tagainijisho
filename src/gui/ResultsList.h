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

#ifndef __GUI_RESULTS_LIST_H__
#define __GUI_RESULTS_LIST_H__

#include "core/Entry.h"
#include "core/EntriesCache.h"

#include <QAbstractListModel>
#include <QList>
#include <QTimer>

class ResultsList : public QAbstractListModel
{
	Q_OBJECT
private:
	QList<EntryPointer<Entry> > entries;
	QTimer timer;
	int displayedUntil;

protected slots:
	void updateViews();
	void onEntryChanged(Entry *entry);

public:
	enum { EntryRole = Qt::UserRole };
	ResultsList(QObject *parent = 0);
	~ResultsList();

	int rowCount(const QModelIndex &parent = QModelIndex()) const { return nbResults(); }
	int nbResults() const { return entries.size(); }
	const EntryPointer<Entry> getEntry(int pos) const { return entries[pos]; }
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

public slots:
	void startReceive();
	void endReceive();
	void addResult(EntryPointer<Entry> entry);
	void clear();
};

#endif
