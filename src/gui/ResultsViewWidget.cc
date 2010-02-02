/*
 *  Copyright (C) 2009,2009,2010  Alexandre Courbot
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

#include "gui/ResultsViewWidget.h"

ResultsViewWidget::ResultsViewWidget(QWidget *parent) : QWidget(parent), _results(0), totalResults(-1), showAllResultsTriggered(false)
{
	setupUi(this);
	
	// Search animation
	int searchAnimSize = showAllResultsButton->height();
	searchAnim = new QMovie(":/images/search.gif", "gif", this);
	if (searchAnimSize < 35) searchAnim->setScaledSize(QSize(searchAnimSize, searchAnimSize));
	searchAnim->jumpToFrame(0);
	searchActiveAnimation->setMovie(searchAnim);
	
	connect(nextPageButton, SIGNAL(clicked()), this, SLOT(nextPage()));
	connect(previousPageButton, SIGNAL(clicked()), this, SLOT(previousPage()));
	connect(showAllResultsButton, SIGNAL(clicked()), this, SLOT(scheduleShowAllResults()));
}

void ResultsViewWidget::setModel(ResultsList *rList)
{
	if (_results) {
		disconnect(searchActiveAnimation, SIGNAL(clicked()), _results, SLOT(abortSearch()));
		disconnect(_results, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowsRemoved(QModelIndex, int, int)));
		disconnect(_results, SIGNAL(nbResults(unsigned int)), this, SLOT(showNbResults(unsigned int)));
		disconnect(_results, SIGNAL(queryEnded()), this, SLOT(stopAndResetSearchAnim()));
		disconnect(_results, SIGNAL(queryEnded()), this, SLOT(onSearchEnded()));
		disconnect(_results, SIGNAL(queryStarted()), searchAnim, SLOT(start()));
		disconnect(_results, SIGNAL(queryStarted()), this, SLOT(onSearchStarted()));
		disconnect(_results, SIGNAL(newSearch()), this, SLOT(onNewSearch()));
		disconnect(_results, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateNbResultsDisplay()));
	}
	_results = rList;
	if (_results) {
		connect(_results, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(updateNbResultsDisplay()));
		connect(_results, SIGNAL(newSearch()), this, SLOT(onNewSearch()));
		connect(_results, SIGNAL(queryStarted()), this, SLOT(onSearchStarted()));
		connect(_results, SIGNAL(queryStarted()), searchAnim, SLOT(start()));
		connect(_results, SIGNAL(queryEnded()), this, SLOT(onSearchEnded()));
		connect(_results, SIGNAL(queryEnded()), this, SLOT(stopAndResetSearchAnim()));
		connect(_results, SIGNAL(nbResults(unsigned int)), this, SLOT(showNbResults(unsigned int)));
		connect(_results, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowsRemoved(QModelIndex, int, int)));
		connect(searchActiveAnimation, SIGNAL(clicked()), _results, SLOT(abortSearch()));
	}
	_resultsView->setModel(rList);
}

void ResultsViewWidget::stopAndResetSearchAnim()
{
	searchAnim->stop();
	searchAnim->jumpToFrame(0);
}

void ResultsViewWidget::onRowsRemoved(const QModelIndex &parent, int start, int end)
{
	if (_results->rowCount() == 0) {
		showAllResultsButton->setEnabled(false);
		nextPageButton->setEnabled(false);
		previousPageButton->setEnabled(false);
		totalResults = 0;
		updateNbResultsDisplay();
	}
}

void ResultsViewWidget::onNewSearch()
{
	totalResults = -1;
	showAllResultsTriggered = false;
}

void ResultsViewWidget::onSearchStarted()
{
	updateNbResultsDisplay();
	updateNavigationButtons();
}

void ResultsViewWidget::onSearchEnded()
{
	updateNavigationButtons();
}

void ResultsViewWidget::nextPage()
{
	_results->nextPage();
}

void ResultsViewWidget::previousPage()
{
	_results->previousPage();
}

void ResultsViewWidget::scheduleShowAllResults()
{
	showAllResultsTriggered = true;
	showAllResultsButton->setEnabled(false);
	nextPageButton->setEnabled(false);
	previousPageButton->setEnabled(false);
	_results->scheduleShowAllResults();
}

void ResultsViewWidget::showNbResults(unsigned int nbResults)
{
	totalResults = nbResults;
	updateNbResultsDisplay();
	updateNavigationButtons();
}

void ResultsViewWidget::updateNavigationButtons()
{
	nextPageButton->setEnabled(!showAllResultsTriggered && (totalResults > (_results->pageNbr() * _results->resultsPerPage()) + _results->nbResults()));
	previousPageButton->setEnabled(!showAllResultsTriggered && _results->pageNbr() > 0);
	showAllResultsButton->setEnabled(!showAllResultsTriggered && (totalResults == -1 || totalResults > _results->nbResults()));
}


void ResultsViewWidget::updateNbResultsDisplay()
{
	if (_results->nbResults() == 0) nbResultsLabel->clear();
	else nbResultsLabel->setText(QString(tr("Results %1 - %2 of %3")).arg(_results->nbResults() > 0 || _results->pageNbr() > 0 ? QString::number(_results->pageNbr() * _results->resultsPerPage() + 1) : "0").arg(QString::number(_results->pageNbr() * _results->resultsPerPage() + _results->nbResults())).arg(totalResults > -1 ? QString::number(totalResults) : QString("??")));
}
