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

ResultsViewWidget::ResultsViewWidget(QWidget *parent) : QWidget(parent), _results(0)
{
	setupUi(this);
}

void ResultsViewWidget::setModel(ResultsList *rList)
{
	if (_results) {
		disconnect(_results, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(updateResultsCount()));
		disconnect(_results, SIGNAL(queryEnded()), this, SLOT(onSearchFinished()));
		disconnect(_results, SIGNAL(queryStarted()), this, SLOT(onSearchStarted()));
	}
	_results = rList;
	if (_results) {
		connect(_results, SIGNAL(queryStarted()), this, SLOT(onSearchStarted()));
		connect(_results, SIGNAL(queryEnded()), this, SLOT(onSearchFinished()));
		connect(_results, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(updateResultsCount()));
	}
	_resultsView->setModel(rList);
}

void ResultsViewWidget::onSearchStarted()
{
	nbResultsLabel->setText(tr("Searching..."));
}

void ResultsViewWidget::onSearchFinished()
{
	nbResultsLabel->clear();
	updateResultsCount();
}

void ResultsViewWidget::updateResultsCount()
{
	int nbResults = _resultsView->model()->rowCount();
	if (nbResults == 0) nbResultsLabel->clear();
	else nbResultsLabel->setText(QString(tr("%1 Results")).arg(nbResults));
}
