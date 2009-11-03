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

#include "core/EntrySearcherManager.h"
#include "gui/SearchWidget.h"

#include <QtDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>

PreferenceItem<int> SearchWidget::resultsPerPagePref("mainWindow/resultsView", "resultsPerPage", 50);
PreferenceItem<int> SearchWidget::historySize("mainWindow/resultsView", "historySize", 100);
PreferenceItem<QByteArray> SearchWidget::splitterState("mainWindow", "splitterGeometry", "");

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent), _history(historySize.value()), queryInProgress(false), query(), queryPending(false), pageNbr(0), totalResults(-1), showAllResultsRequested(false)
{
	_searchBar = new SearchBar(this);

	setResultsPerPage(resultsPerPagePref.value());

	nbResultsLabel = new QLabel(this);

	_results = new ResultsList(this);

	_resultsView = new ResultsView(false, this);
	_resultsView->setModel(_results);
	_detailedView = new ToolBarDetailedView(this);
	splitter = new QSplitter(Qt::Vertical, this);
	splitter->addWidget(_resultsView);
	splitter->addWidget(_detailedView);

	prevButton = new QPushButton(QIcon(":/images/icons/go-previous.png"), 0, this);
	prevButton->setToolTip(tr("Previous search"));
	prevButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	prevButton->setEnabled(false);
	connect(prevButton, SIGNAL(clicked()), this, SLOT(goPrev()));
	nextButton = new QPushButton(QIcon(":/images/icons/go-next.png"), 0, this);
	nextButton->setToolTip(tr("Next search"));
	nextButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	nextButton->setEnabled(false);
	connect(nextButton, SIGNAL(clicked()), this, SLOT(goNext()));

	previousPageButton = new QPushButton(QIcon(":/images/icons/arrow-left.png"), "", this);
	previousPageButton->setToolTip(tr("Previous page"));
	previousPageButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	previousPageButton->setEnabled(false);
	nextPageButton = new QPushButton(QIcon(":/images/icons/arrow-right.png"), "", this);
	nextPageButton->setToolTip(tr("Next page"));
	nextPageButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	nextPageButton->setEnabled(false);
	showAllResultsButton = new QPushButton(QIcon(":/images/icons/arrow-down-double.png"), "", this);
	showAllResultsButton->setToolTip(tr("Show all results"));
	showAllResultsButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	showAllResultsButton->setEnabled(false);

	QHBoxLayout *hLayout = new QHBoxLayout();
	hLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->addWidget(prevButton);
	hLayout->addWidget(nextButton);
	hLayout->addWidget(nbResultsLabel);
	hLayout->addWidget(previousPageButton);
	hLayout->addWidget(nextPageButton);
	hLayout->addWidget(showAllResultsButton);

	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->addWidget(_searchBar);
	vLayout->addLayout(hLayout);
	vLayout->addWidget(splitter);

	// Stop current search
	connect(_searchBar, SIGNAL(stopSearch()), this, SLOT(stopSearch()));

	// Pages navigation
	connect(nextPageButton, SIGNAL(clicked()), this, SLOT(nextPage()));
	connect(previousPageButton, SIGNAL(clicked()), this, SLOT(previousPage()));
	connect(showAllResultsButton, SIGNAL(clicked()), this, SLOT(scheduleShowAllResults()));

	// Start of search
	connect(_searchBar, SIGNAL(startSearch(const QString &)), this, SLOT(search()));

	// Now on to the query/result logic
	// Results emitted by a query are sent to the results list
	connect(&query, SIGNAL(foundEntry(EntryPointer<Entry>)), _results, SLOT(addResult(EntryPointer<Entry>)));
	// When results are added in the results list, update the view
	connect(_results, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateNbResultsDisplay()));
	// React when we know how many results there are in the query
	connect(&query, SIGNAL(nbResults(unsigned int)), this, SLOT(showNbResults(unsigned int)));
	// When the search is over, inform the search bar
	connect(&query, SIGNAL(lastResult()), this, SLOT(currentPageReceived()));

	connect(&query, SIGNAL(firstResult()), _results, SLOT(startReceive()));
	connect(&query, SIGNAL(lastResult()), _results, SLOT(endReceive()));
	connect(&query, SIGNAL(aborted()), _results, SLOT(endReceive()));
	connect(&query, SIGNAL(error()), _results, SLOT(endReceive()));

	// Display selected items in the results view
	connect(_resultsView, SIGNAL(listSelectionChanged(QItemSelection,QItemSelection)), this, SLOT(display(QItemSelection,QItemSelection)));

	connect(&query, SIGNAL(aborted()), this, SLOT(queryError()));
	connect(&query, SIGNAL(error()), this, SLOT(queryError()));

	if (splitterState.isDefault()) splitter->setStretchFactor(1, 2);
	else splitter->restoreState(splitterState.value());
}

void SearchWidget::display(const QItemSelection &selected, const QItemSelection &deselected)
{
	if (selected.isEmpty()) return;
	Entry *entry = qVariantValue<Entry *>(selected.indexes()[0].data(ResultsList::EntryRole));
	_detailedView->detailedView()->display(entry);
}

void SearchWidget::search()
{
	QString text = _searchBar->text();
	if (!(text.isEmpty() || text == ":jmdict" || text == ":kanjidic")) _history.add(_searchBar->getState());
	_search();
}

void SearchWidget::_search()
{
	_results->clear();
	QString searchText = _searchBar->text();

	if (searchText.isEmpty() || searchText == ":jmdict" || searchText == ":kanjidic") {
		_queryBuilder.clear();
		_searchBar->searchCompleted();
		totalResults = -1;
		pageNbr = 0;
		updateNbResultsDisplay();
		updateNavigationButtons();
		return;
	}

	showAllResultsRequested = false;
	// If the query is already running, interrupt it
	if (queryInProgress) query.abort();
	_queryBuilder.clear();
	query.clear();

	prevButton->setEnabled(_history.hasPrevious());
	nextButton->setEnabled(_history.hasNext());

	// If we cannot build a valid query, no need to continue
	if (!EntrySearcherManager::instance().buildQuery(searchText, _queryBuilder)) {
		_searchBar->searchCompleted();
		return;
	}

	// Otherwise, the query is ready to be run - but we may have to delay
	// it if we haven't received all the signals from the previous query
	// yet. This is to avoid problems such as result from the previous
	// query appearing in this query's results.
	if (queryInProgress) {
		// Setting queryPending to true will make the query be launched
		// once all the pending signals from the previous one are
		// received.
		queryPending = true;
	}
	// Clear to go? Just run the query then!
	else {
		startPreparedQuery();
	}
}

void SearchWidget::startPreparedQuery()
{
	// Clear all the current data
	_results->clear();
	totalResults = -1;
	pageNbr = 0;
	updateNbResultsDisplay();
	previousPageButton->setEnabled(false);
	nextPageButton->setEnabled(false);
	showAllResultsButton->setEnabled(true);

	// And run!
	queryInProgress = true;
	_searchBar->searchStarted();
	query.prepare(_queryBuilder);
	query.fetch(0, _resultsPerPage);
}

void SearchWidget::showNbResults(unsigned int nbResults)
{
	totalResults = nbResults;
	updateNbResultsDisplay();
	updateNavigationButtons();
}

void SearchWidget::updateNavigationButtons()
{
	nextPageButton->setEnabled((totalResults > (pageNbr * _resultsPerPage) + _results->nbResults()));
	previousPageButton->setEnabled(pageNbr > 0);
	showAllResultsButton->setEnabled(totalResults > _results->nbResults());
}

void SearchWidget::nextPage()
{
	_results->clear();
	updateNbResultsDisplay();
	_searchBar->searchStarted();
	previousPageButton->setEnabled(false);
	nextPageButton->setEnabled(false);
	showAllResultsButton->setEnabled(true);
	queryInProgress = true;
	if (totalResults < ++pageNbr * _resultsPerPage) pageNbr = 0;
	query.fetch(pageNbr * _resultsPerPage, _resultsPerPage);
}

void SearchWidget::previousPage()
{
	_results->clear();
	updateNbResultsDisplay();
	_searchBar->searchStarted();
	previousPageButton->setEnabled(false);
	nextPageButton->setEnabled(false);
	showAllResultsButton->setEnabled(true);
	queryInProgress = true;
	if (totalResults < --pageNbr * _resultsPerPage) pageNbr = 0;
	query.fetch(pageNbr * _resultsPerPage, _resultsPerPage);
}

void SearchWidget::scheduleShowAllResults()
{
	if (!queryInProgress) showAllResults();
	else {
		// Schedule all the results to be shown once the current
		// request is finnished
		showAllResultsRequested = true;
		showAllResultsButton->setEnabled(false);
		// In addition, if the page being displayed is not the first
		// one, the request can be stopped at once.
		if (pageNbr !=0) stopSearch();
	}
}

void SearchWidget::showAllResults()
{
	_searchBar->searchStarted();
	previousPageButton->setEnabled(false);
	nextPageButton->setEnabled(false);
	showAllResultsButton->setEnabled(false);
	queryInProgress = true;
	// If we are on the first page, we can just
	// continue fetching all results
	if (pageNbr == 0)
		query.fetch(_resultsPerPage);
	// Otherwise we have to fetch from the beginning
	else {
		pageNbr = 0;
		_results->clear();
		query.fetch();
	}
}


void SearchWidget::updateNbResultsDisplay()
{
	if (_results->nbResults() == 0) nbResultsLabel->clear();
	else nbResultsLabel->setText(QString(tr("Results %1 - %2 of %3")).arg(_results->nbResults() > 0 || pageNbr > 0 ? QString::number(pageNbr * _resultsPerPage + 1) : "0").arg(QString::number(pageNbr * _resultsPerPage + _results->nbResults())).arg(totalResults > -1 ? QString::number(totalResults) : QString("??")));
}

void SearchWidget::goPrev()
{
	QMap<QString, QVariant> q;
	bool ok = _history.previous(q);
	if (ok) {
		_searchBar->restoreState(q);
		_search();
	}
}

void SearchWidget::goNext()
{
	QMap<QString, QVariant> q;
	bool ok = _history.next(q);
	if (ok) {
		_searchBar->restoreState(q);
		_search();
	}
}

void SearchWidget::currentPageReceived()
{
	queryEnded();
}

void SearchWidget::queryError()
{
	_searchBar->searchCompleted();
	queryInProgress = false;
	if (queryPending) {
		queryPending = false;
		startPreparedQuery();
	}
}

void SearchWidget::queryEnded()
{
	// Did the user press the show all results button while the
	// query was ongoing?
	if (showAllResultsRequested) {
		showAllResultsRequested = false;
		showAllResults();
	}
	else {
		_searchBar->searchCompleted();
		// Fix a bug that seems to occur sometimes with counting...
		if (!showAllResultsButton->isEnabled()) {
			if (totalResults != _results->nbResults())
				showNbResults(_results->nbResults());
		}
		updateNavigationButtons();
		queryInProgress = false;
		if (queryPending) {
			queryPending = false;
			startPreparedQuery();
		}
	}
}

void SearchWidget::stopSearch()
{
	query.abort();
	// Flush all the entries the results list may be receiving
	QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
	previousPageButton->setEnabled(false);
	nextPageButton->setEnabled(false);
	showAllResultsButton->setEnabled(false);
}

SearchWidget::~SearchWidget()
{
	stopSearch();
	splitterState.set(splitter->saveState());
}

