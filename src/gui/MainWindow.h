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
#include "gui/EntryListWidget.h"

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
	static PreferenceItem<QByteArray> splitterState;
	
	bool _clipboardEnabled;
	
	EntryListModel _listModel;
	EntryListWidget *_entryListWidget;
	
private slots:
	void populateMenu(QMenu *menu, int parentId);
	void resetSearch();

	void onClipboardChanged();
	void onClipboardSelectionChanged();
	void enableClipboardInput(bool enable);

protected slots:
	/// Display the latest selected result in the detailed view
	void display(const QItemSelection &selected, const QItemSelection &deselected);

	void focusTextSearch();
	
	void exportUserData();
	void importUserData();

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
	
	SearchWidget *searchWidget() { return _searchWidget; }
	DetailedView *detailedView() { return _detailedView->detailedView(); }
	EntryListWidget *entryListWidget() { return _entryListWidget; }
	
	/**
	 * Ensures the widget fits within the screen, i.e. adjust its position
	 * if it is out of bounds
	 */
	static void fitToScreen(QWidget *widget);
};

#endif
