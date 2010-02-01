/*
 *  Copyright (C) 2008, 2009  Alexandre Courbot
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

#ifndef __GUI_MAIN_WINDOW_H_
#define __GUI_MAIN_WINDOW_H_

#include "gui/ui_MainWindow.h"
#include "core/Preferences.h"
#include "core/QueryBuilder.h"
#include "core/Query.h"
#include "gui/EntryListModel.h"
#include "gui/ResultsList.h"
#include "gui/ResultsView.h"
#include "gui/AbstractHistory.h"
#include "gui/ToolBarDetailedView.h"
#include "gui/SearchFilterWidget.h"
#include "gui/SearchBuilder.h"
#include "gui/MultiStackedWidget.h"

#include <QSplitter>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMap>
#include <QActionGroup>
#include <QDialog>
#include <QSpinBox>
#include <QTimer>

/*class SearchFilterDock : public QDockWidget
{
	Q_OBJECT
protected:
	/// Resets the state of the search widget when the dock is closed
	virtual void closeEvent(QCloseEvent *event);

public:
	SearchFilterDock(QWidget *parent = 0) : QDockWidget(parent) {}
};*/

class MainWindow : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT
private:
	static PreferenceItem<QByteArray> windowGeometry;
	static PreferenceItem<QByteArray> windowState;
	static MainWindow *_instance;

	// Used by sets
	QList<QAction *> _rootActions;
	QList<QMenu *> _rootMenus;
	
	QTimer _updateTimer;

	// SearchWidget stuff
	static PreferenceItem<QByteArray> splitterState;
	AbstractHistory<QMap<QString, QVariant>, QList<QMap<QString, QVariant> > > _history;

	MultiStackedWidget *_searchFilters;
	QMap<QString, SearchFilterWidget *> _searchFilterWidgets;
	SearchBuilder _searchBuilder;
	ResultsList *_results;
	QueryBuilder _queryBuilder;
	int totalResults;
	bool showAllResultsTriggered;
	
	EntryDelegate *delegate;

	EntryListModel _listModel;
	


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
	// End of SearchWidget stuff
	
protected:
	/**
	 * Present the user with a config dialog to setup printing. Returns true
	 * if the user confirmed the dialog, false if he cancelled it.
	 */
	bool askForPrintOptions(QPrinter &printer, const QString &title = tr("Print"));
	/**
	 * Prints all the given entries (pre-printed into QPictures) on one page. The
	 * entries must all fit within the range 0..height, and height must not be higher
	 * than the available height of the painter.
	 */
	void printPageOfEntries(const QList<QPicture> &entries, QPainter *painter, int height);

private slots:
	void populateMenu(QMenu *menu, int parentId);

protected slots:
	// SearchWidget stuff
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

	/// Replay the previous search in the history
	void goPrev();
	/// Replay the next search in the history
	void goNext();
	// End of SearchWidget stuff
	
	
	void exportUserData();
	void importUserData();
	/**
	 * Print the job on the given printer, using the default print font.
	 * An alternative font can be specified if font is not null.
	 */
	void prepareAndPrintJob(QPrinter *printer, const QFont *font = 0);
	void prepareAndPrintBookletJob(QPrinter *printer);
	void print();
	void printPreview();
	void printBooklet();
	void printBookletPreview();
	void tabExport();
	void preferences();

	void organizeSets();

	void about();
	void donate();
	void manual();
	void bugReport();
	void featureRequest();
	void askQuestion();
	void updateAvailable(const QString &version);
	void betaUpdateAvailable(const QString &version);
	void updateCheck();

	void populateSetsMenu();
	void populateSubMenu();
	void newSet();
	void newSetsFolder();
	void onSetSelected();

	void trainSettings();

	void openUrl(const QUrl &url);
	
public slots:
	void nextPage();
	void previousPage();
	void scheduleShowAllResults();

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	static MainWindow *instance() { return _instance; }
	
	QToolBar *toolBar() { return _toolBar; }

	QMenu *fileMenu() { return _fileMenu; }
	QMenu *searchMenu() { return _searchMenu; }
	QMenu *trainMenu() { return _trainMenu; }
	QMenu *helpMenu() { return _helpMenu; }

	static PreferenceItem<QString> applicationFont;
	static PreferenceItem<QString> guiLanguage;
	static PreferenceItem<bool> autoCheckUpdates;
	static PreferenceItem<bool> autoCheckBetaUpdates;
	static PreferenceItem<int> updateCheckInterval;
	static PreferenceItem<QDateTime> lastUpdateCheck;
	static PreferenceItem<int> historySize;
	
	// SearchWidget stuff
	ResultsList *resultsList() { return _results; }
	ResultsView *resultsView() { return _resultsView; }
	DetailedView *detailedView() { return _detailedView->detailedView(); }

	const QueryBuilder &queryBuilder() const { return _queryBuilder; }
	
	void addSearchFilter(SearchFilterWidget *widget);
	SearchFilterWidget *getSearchFilter(const QString &name);
	void removeSearchFilterWidget(const QString &name);
};

#endif
