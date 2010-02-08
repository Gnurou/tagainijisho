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

#include "core/Preferences.h"
#include "core/Entry.h"
#include "core/EntriesCache.h"
#include "core/QueryBuilder.h"
#include "core/Query.h"

#include <QAbstractListModel>
#include <QList>
#include <QTimer>
#include <QMimeData>

class ResultsList : public QAbstractListModel
{
	Q_OBJECT
private:
	QList<EntryPointer<Entry> > entries;
	QTimer timer;
	int displayedUntil;
	
	int _resultsPerPage;
	Query query;
	int _pageNbr;
	int totalResults;
	bool showAllResultsRequested;
	bool _active;

	void startPreparedQuery();
	
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

	Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QMimeData *mimeData(const QModelIndexList & indexes) const;

	int pageNbr() const { return _pageNbr; }
	int resultsPerPage() const { return _resultsPerPage; }
	void setResultsPerPage(int nbr) { _resultsPerPage = nbr; }
	bool queryActive() const { return _active; }

	static PreferenceItem<int> resultsPerPagePref;
	
private slots:
	void onPrefValueChanged(QVariant newValue);
	void setNbResults(unsigned int nbRes);
	void queryError();

public slots:
	void search(const QueryBuilder &qBuilder);
	void abortSearch();

	void startReceive();
	void endReceive();
	void addResult(EntryPointer<Entry> entry);
	void clear();
	
	/// Display the next results page
	void nextPage();
	/// Display the previous results page
	void previousPage();
	void scheduleShowAllResults();

signals:
	void newSearch();
	void queryStarted();
	void nbResults(unsigned int);
	void queryEnded();
};

#endif
