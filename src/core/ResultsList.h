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
#include "core/QueryBuilder.h"
#include "core/Query.h"

#include <QAbstractListModel>
#include <QList>
#include <QTimer>
#include <QMimeData>

/**
 * An entity that fetches and store results emitted by a query in pages of
 * given size. It can also be used as a list model to display the results.
 */
class ResultsList : public QAbstractListModel
{
	Q_OBJECT
private:
	QList<EntryRef> entries;
	QTimer timer;
	
	Query query;
	bool _active;

	void startPreparedQuery();
	int displayedUntil;
	
protected slots:
	void updateViews();
	void onEntryChanged(const EntryPointer &entry);

public:
	ResultsList(QObject *parent = 0);
	~ResultsList();

	int rowCount(const QModelIndex &parent = QModelIndex()) const { return nbResults(); }
	int nbResults() const { return entries.size(); }
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QMimeData *mimeData(const QModelIndexList & indexes) const;

public slots:
	void search(const QueryBuilder &qBuilder);
	void abortSearch();

	void startReceive();
	void endReceive();
	void addResult(EntryRef entry);
	void clear();

signals:
	void queryStarted();
	void nbResults(unsigned int);
	void queryEnded();
};

#endif
