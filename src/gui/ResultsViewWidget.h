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

#ifndef __GUI_RESULTSVIEWWIDGET_H
#define __GUI_RESULTSVIEWWIDGET_H

#include "gui/ResultsList.h"
#include "gui/ui_ResultsViewWidget.h"

#include <QWidget>
#include <QMovie>

class ResultsViewWidget : public QWidget, public Ui::ResultsViewWidget
{
	Q_OBJECT
private:
	QMovie *searchAnim;
	ResultsList *_results;
	
	int totalResults;
	bool showAllResultsTriggered;
	
private slots:
	void stopAndResetSearchAnim();
	void onNewSearch();
	void onSearchStarted();
	void onSearchEnded();
		
protected slots:
	/// Activate/deactivate the navigation buttons according
	/// to the current position and total number of results
	/// in the search
	void updateNavigationButtons();
	/// Update the label displaying results range and
	/// total number of results
	void updateNbResultsDisplay();
	
public:
	ResultsViewWidget(QWidget *parent);
	
	void setModel(ResultsList *rList);
	ResultsView *resultsView() { return _resultsView; }
	
public slots:
	/// Set the total number of results of a search.
	void showNbResults(unsigned int nbResults);
	
	void nextPage();
	void previousPage();
	void scheduleShowAllResults();
};

#endif
