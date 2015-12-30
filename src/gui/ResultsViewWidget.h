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

class ResultsViewWidget : public QWidget, private Ui::ResultsViewWidget
{
	Q_OBJECT
private:
	ResultsList *_results;
	
protected slots:
	void onSearchStarted();
	void onSearchFinished();
	void updateResultsCount();

public:
	ResultsViewWidget(QWidget *parent);
	
	void setModel(ResultsList *rList);
	ResultsView *resultsView() { return _resultsView; }
	QHBoxLayout *buttonsLayout() { return _buttonsLayout; }
};

#endif
