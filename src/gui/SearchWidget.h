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

#ifndef __GUI_SEARCHWIDGET_H_
#define __GUI_SEARCHWIDGET_H_

#include "core/QueryBuilder.h"
#include "core/Query.h"
#include "gui/SearchBar.h"
#include "gui/ResultsList.h"
#include "ResultsView.h"
#include "gui/AbstractHistory.h"
#include "gui/ToolBarDetailedView.h"

#include <QLabel>
#include <QSplitter>
#include <QPushButton>
#include <QList>

class SearchWidget : public QWidget
{
	Q_OBJECT
private:
	static PreferenceItem<QByteArray> splitterState;

	AbstractHistory<QMap<QString, QVariant>, QList<QMap<QString, QVariant> > > _history;

	SearchBar *_searchBar;
	ResultsList *_results;
	ResultsView *_resultsView;
	ToolBarDetailedView *_detailedView;

	QSplitter *splitter;
	EntryDelegate *delegate;
	QLabel *nbResultsLabel;

	QPushButton *nextPageButton;
	QPushButton *previousPageButton;
	QPushButton *showAllResultsButton;

	QPushButton *prevButton;
	QPushButton *nextButton;

	// Set to true if a query has been started and we haven't handled any
	// signal relative to its termination.
	bool queryInProgress;
	QueryBuilder _queryBuilder;
	Query query;
	// Set to true if the query should be automatically executed
	// when we receive an end-of-query event (abort, lastEntry, error)
	bool queryPending;

	int pageNbr;
	int totalResults;
	int _resultsPerPage;

	bool showAllResultsRequested;

	/**
	 * Actually run the query that has been prepared before. Do not call
	 * if queryInProgress is true!
	 */
	void startPreparedQuery();

	/**
	 * Called whenever we have received all the signals from a given
	 * query.
	 */
	void queryEnded();

	/**
	 * Run the search without touching the history.
	 */
	void _search(const QString &commands);

public:
	SearchWidget(QWidget *parent = 0);
	~SearchWidget();

	SearchBar *searchBar() { return _searchBar; }
	ResultsList *resultsList() { return _results; }
	ResultsView *resultsView() { return _resultsView; }
	DetailedView *detailedView() { return _detailedView->detailedView(); }
	int resultsPerPage() const { return _resultsPerPage; }
	void setResultsPerPage(int nbr) { _resultsPerPage = nbr; }
	const QueryBuilder &queryBuilder() const { return _queryBuilder; }

	static PreferenceItem<int> resultsPerPagePref;
	static PreferenceItem<int> historySize;

protected slots:
	/// Set the total number of results of a search.
	void showNbResults(unsigned int nbResults);
	/// Activate/deactivate the navigation buttons according
	/// to the current position and total number of results
	/// in the search
	void updateNavigationButtons();
	/// Indicates all the results in the current page have been received
	void currentPageReceived();
	/// Update the label displaying results range and
	/// total number of results
	void updateNbResultsDisplay();
	/// Start a search with the content of the search field
	void search(const QString &commands);
	/// Display the latest selected result in the detailed view
	void display(const QItemSelection &selected, const QItemSelection &deselected);

	/// Show all the query results as soon as possible
	void scheduleShowAllResults();


	/// Replay the previous search in the history
	void goPrev();
	/// Replay the next search in the history
	void goNext();

public slots:
	/// Jump to next results page
	void nextPage();
	/// Jump to previous results page
	void previousPage();
	/// Display all the results in a single page
	void showAllResults();

	/// Stop the current search - immediatly
	void stopSearch();
	/// What to do when the query encountered an error?
	void queryError();

friend class MainWindow;
};

#endif
