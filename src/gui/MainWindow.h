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

	QMap<QString, SearchFilterWidget *> _searchFilterWidgets;
	SearchBuilder _searchBuilder;
	ResultsList *_results;
	QueryBuilder _queryBuilder;
	
	EntryListModel _listModel;
	
	/**
	 * Run the search without touching the history.
	 */
	void _search(const QString &commands);
	
	/**
	 * Present the user with a config dialog to setup printing. Returns true
	 * if the user confirmed the dialog, false if he cancelled it.
	 */
	bool askForPrintOptions(QPrinter &printer, const QString &title = tr("Print"));
	
protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);

private slots:
	void populateMenu(QMenu *menu, int parentId);

protected slots:
	/// Start a search with the content of the search field
	void search(const QString &commands);
	/// Display the latest selected result in the detailed view
	void display(const QItemSelection &selected, const QItemSelection &deselected);

	void goPrev();
	void goNext();
	void resetSearch();
	
	void exportUserData();
	void importUserData();

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

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	static MainWindow *instance() { return _instance; }
	
	QToolBar *toolBar() { return _toolsToolBar; }

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
	
	SearchBuilder *searchBuilder() { return &_searchBuilder; }
	ResultsList *resultsList() { return _results; }
	ResultsView *resultsView() { return _resultsView->resultsView(); }
	DetailedView *detailedView() { return _detailedView->detailedView(); }

	const QueryBuilder &queryBuilder() const { return _queryBuilder; }
	
	void addSearchFilter(SearchFilterWidget *widget);
	SearchFilterWidget *getSearchFilter(const QString &name);
	void removeSearchFilterWidget(const QString &name);
};

#endif
