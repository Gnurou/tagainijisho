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

#include "core/Preferences.h"

#include <QMainWindow>
#include <QMenu>
#include <QMap>
#include <QActionGroup>
#include <QDialog>
#include <QSpinBox>
#include <QTimer>

class SearchWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT
private:
	static PreferenceItem<QByteArray> windowGeometry;
	static MainWindow *_instance;

	SearchWidget *_searchWidget;
	QMenu *_fileMenu;
	QMenu *_searchMenu;
	QMenu *_resultsMenu;
	QMenu *_setsMenu;
	QList<QAction *> _rootActions;
	QList<QMenu *> _rootMenus;
	QMenu *_trainMenu;
	QMenu *_helpMenu;

	QMenu *_tagsListMenu;
	QActionGroup _tagsListMenuActionGroup;
	QMap<QAction *, QString> _tagsListAction;

	QTimer _updateTimer;

	void createMenus();

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

	SearchWidget *searchWidget() { return _searchWidget; }

	static PreferenceItem<QDateTime> lastUpdateCheck;
};

#endif
