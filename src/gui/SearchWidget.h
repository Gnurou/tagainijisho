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


#ifndef __GUI_SEARCH_WIDGET_H
#define __GUI_SEARCH_WIDGET_H

#include "core/QueryBuilder.h"
#include "core/Query.h"
#include "core/ResultsList.h"
#include "gui/ResultsView.h"
#include "gui/SearchFilterWidget.h"
#include "gui/SearchBuilder.h"
#include "gui/AbstractHistory.h"

#include "gui/ui_SearchWidget.h"

#include <QWidget>

/**
 * A widget that features all the necessary to search entries and display search results.
 *
 * It features a toolbar from which filters can be accessed, and a results view that displays
 * the entries set corresponding to the current filters state. It also handles history and
 * various operations the user may want to perform on the set of displayed entries, like
 * printing or exporting.
 */
class SearchWidget : public QWidget, private Ui::SearchWidget
{
Q_OBJECT
private:
	QToolBar *_filtersToolBar;
	AbstractHistory<QMap<QString, QVariant>, QList<QMap<QString, QVariant> > > _history;

	QMap<QString, SearchFilterWidget *> _searchFilterWidgets;
	SearchBuilder _searchBuilder;
	ResultsList *_results;
	QueryBuilder _queryBuilder;

protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);

	/// Run the search without touching the history.
	void _search(const QString &commands);

protected slots:
	/// Start a search with the given commands
	void search(const QString &commands);

public:
	SearchWidget(QWidget *parent = 0);

	SearchBuilder *searchBuilder() { return &_searchBuilder; }
	ResultsList *resultsList() { return _results; }
	ResultsView *resultsView() { return _resultsView->resultsView(); }
	QueryBuilder *queryBuilder() { return &_queryBuilder; }
	MultiStackedWidget *searchFilters() { return _searchFilters; }
	
	void addSearchFilter(SearchFilterWidget *widget);
	SearchFilterWidget *getSearchFilter(const QString &name);
	void removeSearchFilterWidget(const QString &name);

	static PreferenceItem<int> historySize;

public slots:
	void resetSearch();

	void goPrev();
	void goNext();
	
signals:
};

#endif
