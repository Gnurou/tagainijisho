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
#include "core/EntryListModel.h"
#include "gui/ToolBarDetailedView.h"

#include <QSplitter>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMap>
#include <QActionGroup>
#include <QDialog>
#include <QSpinBox>
#include <QTimer>
#include <QClipboard>

/*class SearchFilterDock : public QDockWidget
{
	Q_OBJECT
protected:
	/// Resets the state of the search widget when the dock is closed
	virtual void closeEvent(QCloseEvent *event);

public:
	SearchFilterDock(QWidget *parent = 0) : QDockWidget(parent) {}
};*/

class DockTitleBar : public QWidget
{
	Q_OBJECT
private:
public:
	DockTitleBar(QWidget *widget, QDockWidget *parent = 0);
};

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
	
	bool _clipboardEnabled;
	
	EntryListModel _listModel;
	
	void setupSearchWidget();
	void setupClipboardSearchShortcut();
	void setupListWidget();

private slots:
	void populateMenu(QMenu *menu, int parentId);
	void resetSearch();

	void onClipboardChanged(QClipboard::Mode mode);
	void enableClipboardInput(bool enable);

protected:
	virtual void closeEvent(QCloseEvent *event);

protected slots:
	/// Display the latest selected result in the detailed view
	void display(const QModelIndex& clicked);

	void focusTextSearch();
	void focusResultsList();
	
	void exportUserData();
	void importUserData();

	void preferences();

	void organizeSavedSearches();

	void about();
	void donate();
	void manual();
	void bugReport();
	void featureRequest();
	void askQuestion();
	void updateAvailable(const QString &version);
	void betaUpdateAvailable(const QString &version);
	void updateCheck();
	void donationReminderCheck();

	void populateSavedSearchesMenu();
	void populateSubMenu();
	void newSavedSearch();
	void newSavedSearchesFolder();
	void onSavedSearchSelected();

	void trainSettings();

	void openUrl(const QUrl &url);

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	static MainWindow *instance() { return _instance; }
	
	QMenu *fileMenu() { return _fileMenu; }
	QMenu *searchMenu() { return _searchMenu; }
	QMenu *trainMenu() { return _trainMenu; }
	QMenu *helpMenu() { return _helpMenu; }

	static PreferenceItem<QString> applicationFont;
	static PreferenceItem<bool> autoCheckUpdates;
	static PreferenceItem<bool> autoCheckBetaUpdates;
	static PreferenceItem<int> updateCheckInterval;
	static PreferenceItem<QDateTime> lastUpdateCheck;
	// Keep track of when the user started the program for the first time
	// so that we can bother him with a donation reminder
	static PreferenceItem<bool> donationReminderDisplayed;
	static PreferenceItem<QDateTime> firstRunTime;
	
	SearchWidget *searchWidget() { return _searchWidget; }
	EntryListWidget *entryListWidget() { return _entryListWidget; }
	DetailedView *detailedView() { return _detailedView->detailedView(); }
	
	QDockWidget *searchDockWidget() { return _searchDockWidget; }
	QDockWidget *listDockWidget() { return _listDockWidget; }
	
	/**
	 * Ensures the widget fits within the screen, i.e. adjust its position
	 * if it is out of bounds
	 */
	static void fitToScreen(QWidget *widget);

	void restoreWholeState();
};

#endif
