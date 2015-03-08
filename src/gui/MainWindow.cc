/*
 *  Copyright (C) 2008-2013  Alexandre Courbot
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

#include "tagaini_config.h"

#include "core/Paths.h"
#include "core/TextTools.h"
#include "core/Database.h"
#include "core/Plugin.h"
#include "core/EntrySearcherManager.h"
#include "gui/UpdateChecker.h"
#include "gui/SavedSearchesOrganizer.h"
#include "gui/TrainSettings.h"
#include "gui/PreferencesWindow.h"
#include "gui/YesNoTrainer.h"
#include "gui/ScrollBarSmoothScroller.h"
#include "gui/TextFilterWidget.h"
#include "gui/MainWindow.h"
#include "gui/ui_AboutDialog.h"

#include <QtDebug>

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QApplication>
#include <QInputDialog>
#include <QDesktopServices>
#include "sqlite/Query.h"
#include <QTextBrowser>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QDataStream>
#include <QDockWidget>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopWidget>

/// State version of the main window - should be increated every time the docks or statusbars change
#define MAINWINDOW_STATE_VERSION 0

MainWindow *MainWindow::_instance = 0;
UpdateChecker *_updateChecker = 0;
UpdateChecker *_betaUpdateChecker = 0;

PreferenceItem<QString> MainWindow::applicationFont("mainWindow", "defaultFont", "");
PreferenceItem<bool> MainWindow::autoCheckUpdates("mainWindow", "autoCheckUpdates", true);
PreferenceItem<bool> MainWindow::autoCheckBetaUpdates("mainWindow", "autoCheckBetaUpdates", false);
PreferenceItem<int> MainWindow::updateCheckInterval("mainWindow", "updateCheckInterval", 3);

PreferenceItem<QByteArray> MainWindow::windowGeometry("mainWindow", "geometry", "");
PreferenceItem<QByteArray> MainWindow::windowState("mainWindow", "state", "");

PreferenceItem<QDateTime> MainWindow::lastUpdateCheck("mainWindow", "lastUpdateCheck", QDateTime(QDate(2000, 1, 1)));
PreferenceItem<bool> MainWindow::donationReminderDisplayed("mainWindow", "donationReminderDisplayed", false, true);
PreferenceItem<QDateTime> MainWindow::firstRunTime("mainWindow", "firstRunTime", QDateTime::currentDateTime(), true);

DockTitleBar::DockTitleBar(QWidget *widget, QDockWidget *parent) : QWidget(parent)
{
	QHBoxLayout *hLayout = new QHBoxLayout(this);
	hLayout->addWidget(widget);
	hLayout->addStretch();
	QIcon icon(static_cast<QApplication *>(QApplication::instance())->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	QToolButton *ttButton = new QToolButton(parent);
	ttButton->setIcon(icon);
	ttButton->setAutoRaise(true);
	ttButton->setMaximumSize(icon.actualSize(QSize(16, 16)) + QSize(1, 1));
	connect(ttButton, SIGNAL(clicked()), parent, SLOT(close()));
	hLayout->addWidget(ttButton);
	setAttribute(Qt::WA_MacMiniSize);

	int left, right, top, bottom;
	hLayout->getContentsMargins(&left, &top, &right, &bottom);
	hLayout->setContentsMargins(left, 0, right, 0);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _clipboardEnabled(false)
{
	_instance = this;
	
	setupUi(this);

	// Setup the corners
	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	
	// Strangely this is not done properly by Qt designer...
	connect(_setsMenu, SIGNAL(aboutToShow()), this, SLOT(populateSavedSearchesMenu()));
	
	// Reset search action
	_searchWidget->resetSearchAction()->setShortcut(QKeySequence("Ctrl+R"));
	_searchMenu->addAction(_searchWidget->resetSearchAction());
	setupClipboardSearchShortcut();
	_searchMenu->addSeparator();
	setupSearchWidget();
	setupListWidget();
	
	// Updates checker
	_updateChecker = new UpdateChecker("/updates/latestversion.php", this);
	_betaUpdateChecker = new UpdateChecker("/updates/latestbetaversion.php", this);
	// Message has to be queued here because of a bug in Qt 4.4 that will
	// make the QHttp::requestFinished signal being emitted twice if the
	// slot does not return after a couple of seconds. This problem should be
	// fixed in Qt 4.5.
	connect(_updateChecker, SIGNAL(updateAvailable(const QString &)),
		this, SLOT(updateAvailable(const QString &)), Qt::QueuedConnection);
	connect(_betaUpdateChecker, SIGNAL(updateAvailable(const QString &)),
		this, SLOT(betaUpdateAvailable(const QString &)), Qt::QueuedConnection);
	connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(updateCheck()));
	// Check every 10 minutes if the update check delay expired
	_updateTimer.setInterval(600000);
	_updateTimer.setSingleShot(false);
	_updateTimer.start();

	// And perform our startup checks
	donationReminderCheck();
	updateCheck();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	windowState.set(saveState(MAINWINDOW_STATE_VERSION));
	windowGeometry.set(saveGeometry());
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupSearchWidget()
{
	// Steal the tool bar and set it as our dock title bar widget
	QWidget *filtersToolBar = static_cast<QBoxLayout *>(searchWidget()->layout())->takeAt(0)->widget();
	DockTitleBar *dBar = new DockTitleBar(filtersToolBar, _searchDockWidget);
	dBar->setAttribute(Qt::WA_MacMiniSize);
	_searchDockWidget->setTitleBarWidget(dBar);
	
	connect(searchWidget()->resultsView(), SIGNAL(entrySelected(EntryPointer)), detailedView(), SLOT(display(EntryPointer)));

	// Focus on the text input on startup
	actionFocus_text_search->trigger();
	
	// Toggle action
	QAction *action = _searchDockWidget->toggleViewAction();
	action->setShortcut(QKeySequence("F2"));
	action->setIcon(QIcon(":/images/icons/ldap_lookup.png"));
	_searchMenu->addAction(action);
}

void MainWindow::setupListWidget()
{
	entryListWidget()->entryListView()->setModel(&_listModel);

	// Steal the toolbar
	QWidget *toolsBar = static_cast<QBoxLayout *>(entryListWidget()->layout())->takeAt(0)->widget();
	DockTitleBar *dBar = new DockTitleBar(toolsBar, _listDockWidget);
	dBar->setAttribute(Qt::WA_MacMiniSize);
	_listDockWidget->setTitleBarWidget(dBar);
	// Not visible on first start
	_listDockWidget->setVisible(false);
	
	connect(_entryListWidget->entryListView(), SIGNAL(entrySelected(EntryPointer)), detailedView(), SLOT(display(EntryPointer)));
	
	// Toggle action
	QAction *action = _listDockWidget->toggleViewAction();
	action->setShortcut(QKeySequence("F3"));
	action->setIcon(QIcon(":/images/icons/list.png"));
	_searchMenu->addAction(action);
}

void MainWindow::setupClipboardSearchShortcut()
{
	// Auto-clipboard search action
	QAction *_enableClipboardInputAction = new QAction(QIcon(":/images/icons/clipboard-search.png"), tr("Auto-search on clipboard content"), this);
	_enableClipboardInputAction->setCheckable(true);
	connect(_enableClipboardInputAction, SIGNAL(toggled(bool)), this, SLOT(enableClipboardInput(bool)));
	_searchMenu->addAction(_enableClipboardInputAction);
}

void MainWindow::updateCheck()
{
	if (autoCheckUpdates.value()) {
		QDateTime dt(lastUpdateCheck.value());
		if (!dt.isValid() || dt.addDays(updateCheckInterval.value()) <= QDateTime::currentDateTime()) {
			_updateChecker->checkForUpdates();
			if (autoCheckBetaUpdates.value()) _betaUpdateChecker->checkForUpdates();
			lastUpdateCheck.set(QDateTime::currentDateTime());
		}
	}
}

void MainWindow::donationReminderCheck()
{
	if (!donationReminderDisplayed.value()) {
		// Display the reminder if two weeks passed since the first usage
		if (QDateTime::currentDateTime() > firstRunTime.value().addDays(14)) {
			QMessageBox messageBox(QMessageBox::Information, tr("Please consider supporting Tagaini Jisho"),
					tr("You have now been using Tagaini Jisho for a short while. If you like it, please show your support by making a donation that expresses your appreciation of this software. While Tagaini is free, its development has a cost and your support will have a positive influence on its future. You can donate via Paypal or credit card.\n\nThis message is a one-time reminder and will not appear anymore - you can still donate later by choosing the corresponding option in the Help menu."),
					QMessageBox::NoButton, this);
			QPushButton donateButton(tr("Donate!"));
			donateButton.setIcon(QIcon(QPixmap(":/images/icons/donate.png")));
			messageBox.addButton(&donateButton, QMessageBox::AcceptRole);
			QPushButton laterButton(tr("Nah"));
			messageBox.addButton(&laterButton, QMessageBox::RejectRole);
			if (messageBox.exec() == QMessageBox::AcceptRole) {
				donate();
			}
			
			donationReminderDisplayed.setValue(true);
		}
	}
}

void MainWindow::exportUserData()
{
	QString to(QFileDialog::getSaveFileName(this, tr("Export user data..."), "export.dat"));
	if (to.isEmpty()) return;

	QFile dataFile(Database::instance()->userDBFile());

        QFile(to).remove(); // QFile::copy does not overwrite, so delete it first (if existent).
        if (!dataFile.copy(to)) {
            QMessageBox::critical(this, tr("Export user data..."), tr("Error: Could not export database! Please verify that you have write permissions on the target file."));
        }
}

void MainWindow::importUserData()
{
	QString from(QFileDialog::getOpenFileName(this, tr("Import user data...")));
	if (from.isEmpty()) return;

	QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("About to replace user data!"), tr("<b>WARNING:</b> All the user data (study list, tags, notes, training, ...) is going to be replaced by the selected user data file. Are you <b>sure</b> you want to do this?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if (ret != QMessageBox::Ok) return;

	// TODO Check whether the file is valid!
	QFile dataFile(Database::defaultDBFile());
	QFile(from).copy(dataFile.fileName() + ".import");

	ret = QMessageBox::information(this, tr("Please restart Tagaini Jisho"), tr("The imported data will be available the next time you start Tagaini Jisho. All changes performed from now will be lost. Do you want to exit Tagaini Jisho now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	if (ret == QMessageBox::Yes) close();
}

void MainWindow::preferences()
{
	PreferencesWindow *prefsWindow = new PreferencesWindow(this);
	prefsWindow->exec();
	delete prefsWindow;
}

void MainWindow::donate()
{
	QDesktopServices::openUrl(QUrl("http://www.tagaini.net/donate"));
}

void MainWindow::about()
{
#ifndef VERSION
#error No version defined - the -DVERSION=<version> flag must be set!
#endif
	QString message = QString(
		"<p>Copyright (C) 2008-2015 Alexandre Courbot.</p>"
		"<p align=\"center\"><a href=\"http://www.tagaini.net\">http://www.tagaini.net</a></p><p>This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under the conditions of the <a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">GNU General Public License, version 3.0</a>, or (at your option) any later version.</p><hr/>"
		);
	QString credits = "<p>Tagaini Jisho uses data from various sources:</p>";

	foreach (const Plugin *plugin, Plugin::plugins()) {
		credits += plugin->pluginInfo();
	}
	credits += "<p>Fugue Icons Copyright (C) 2010 <a href=\"http://p.yusukekamiyamane.com/\">Yusuke Kamiyamane</a>, licensed under a <a href=\"http://creativecommons.org/licenses/by/3.0/\">Creative Commons Attribution 3.0 license</a>.</p>";

	Ui::AboutDialog aboutDialogUI;
	QDialog aboutDialog;
	aboutDialogUI.setupUi(&aboutDialog);
	aboutDialog.setWindowIcon(windowIcon());
	aboutDialogUI.title->setText(aboutDialogUI.title->text() + " " + VERSION);
	aboutDialogUI.logo->setPixmap(aboutDialogUI.logo->pixmap()->scaledToWidth(75, Qt::SmoothTransformation));
	aboutDialogUI.credits->setHtml(message + credits);
	aboutDialogUI.credits->viewport()->setAutoFillBackground(false);
	ScrollBarSmoothScroller scroller;
	scroller.setScrollBar(aboutDialogUI.credits->verticalScrollBar());
	connect(aboutDialogUI.credits, SIGNAL(anchorClicked(QUrl)), this, SLOT(openUrl(QUrl)));

	QString authors =
		"<h2>Authors</h2>"
		"<p><b>Alexandre Courbot</b> project lead, programming.</p>"
		"<p><b>Axel Bodart</b> CMake improvements, web site guru, Mac release manager.</p>"
		"<p><b>Neil Caldwell</b> documentation.</p>"
		"<h2>Thanks to</h2>"
		"<p><b>Philip Seyfi</b> application icon.</p>"
		"<p><b>Tracy Poff</b> bug reports, documentation fixes.</p>"
		"<p>All the translators on <a href=\"https://www.transifex.net/projects/p/tagaini-jisho\">Transifex</a>.</p>"
		"<p>All the persons who donated and contributed directly or indirectly to this software!</p>"
		"";
	aboutDialogUI.authors->setHtml(authors);
	aboutDialogUI.authors->viewport()->setAutoFillBackground(false);
	ScrollBarSmoothScroller scroller2;
	scroller2.setScrollBar(aboutDialogUI.authors->verticalScrollBar());
	connect(aboutDialogUI.authors, SIGNAL(anchorClicked(QUrl)), this, SLOT(openUrl(QUrl)));
	aboutDialog.exec();
}

void MainWindow::openUrl(const QUrl &url)
{
	QDesktopServices::openUrl(url);
}

void MainWindow::manual()
{
	// First check if we have an installation prefix
#ifdef DATA_DIR
	QString f = QDir(DATA_DIR).filePath("doc/manual.html");
#else
	QString f = QDir(QCoreApplication::applicationDirPath()).filePath("doc/manual.html");
#endif
	QDesktopServices::openUrl(f);
}

void MainWindow::bugReport()
{
	QDesktopServices::openUrl(QUrl("https://github.com/Gnurou/tagainijisho/issues"));
}

void MainWindow::featureRequest()
{
	QDesktopServices::openUrl(QUrl("https://github.com/Gnurou/tagainijisho/issues"));
}

void MainWindow::askQuestion()
{
	QDesktopServices::openUrl(QUrl("http://groups.google.com/group/tagaini-jisho"));
}

void MainWindow::updateAvailable(const QString &version)
{
	QMessageBox messageBox(QMessageBox::Information, tr("An update is available!"),
			tr("Version %1 of Tagaini Jisho is available. Do you want to download it now?").arg(version),
			QMessageBox::NoButton, this);
	QPushButton downloadButton(tr("Let's go!"));
	messageBox.addButton(&downloadButton, QMessageBox::AcceptRole);
	QPushButton laterButton(tr("Maybe later"));
	messageBox.addButton(&laterButton, QMessageBox::RejectRole);
	if (messageBox.exec() == QMessageBox::AcceptRole) {
		QDesktopServices::openUrl(QUrl("http://www.tagaini.net/download"));
	}
	_updateTimer.stop();
}

void MainWindow::betaUpdateAvailable(const QString &version)
{
	QMessageBox messageBox(QMessageBox::Information, tr("A development update is available!"),
			tr("Development version %1 of Tagaini Jisho is available. Do you want to download it now?").arg(version),
			QMessageBox::NoButton, this);
	QPushButton downloadButton(tr("Let's go!"));
	messageBox.addButton(&downloadButton, QMessageBox::AcceptRole);
	QPushButton laterButton(tr("Maybe later"));
	messageBox.addButton(&laterButton, QMessageBox::RejectRole);
	if (messageBox.exec() == QMessageBox::AcceptRole) {
		QDesktopServices::openUrl(QUrl("http://www.tagaini.net/download"));
	}
	_updateTimer.stop();
}

void MainWindow::populateMenu(QMenu *menu, int parentId)
{
	SQLite::Query query(Database::connection());
	query.exec(QString("SELECT label, state, rowid FROM sets WHERE parent %1 ORDER BY position").arg(parentId == 0 ? "is null" : QString("= %1").arg(parentId)));
	while (query.next()) {
		// A folder
		if (query.valueIsNull(1)) {
			QMenu *subMenu = menu->addMenu(QIcon(":/images/icons/folder.png"), query.valueString(0));
			connect(subMenu, SIGNAL(aboutToShow()), this, SLOT(populateSubMenu()));
			subMenu->setProperty("T_rowid", query.valueInt(2));
			if (!parentId) _rootMenus << subMenu;
		}
		// A set
		else {
			QAction *action = menu->addAction(query.valueString(0), this, SLOT(onSavedSearchSelected()));
			action->setProperty("T_state", query.valueBlob(1));
			if (!parentId) _rootActions << action;
		}
	}
}

void MainWindow::populateSavedSearchesMenu()
{
	// First clear previous actions and menus
	foreach (QAction *action, _rootActions) delete action;
	_rootActions.clear();
	foreach (QMenu *menu, _rootMenus) delete menu;
	_rootMenus.clear();

	populateMenu(_setsMenu, 0);
}

void MainWindow::populateSubMenu()
{
	QMenu *menu(qobject_cast<QMenu *>(sender()));
	// Should never happen...
	if (!menu) return;
	int rowId(menu->property("T_rowid").toInt());
	menu->clear();

	populateMenu(menu, rowId);
	menu->addSeparator();
	QAction *action = menu->addAction(tr("Save current search here..."), this, SLOT(newSavedSearch()));
	action->setProperty("T_rowid", rowId);
	action = menu->addAction(tr("Create new folder here..."), this, SLOT(newSavedSearchesFolder()));
	action->setProperty("T_rowid", rowId);
}

void MainWindow::newSavedSearch()
{
	QAction *action(qobject_cast<QAction *>(sender()));
	// Should never happen
	if (!action) return;
	int parentId(action->property("T_rowid").toInt());

	bool ok;
	QString setName(QInputDialog::getText(this, tr("Save current search"), tr("Please enter a name for this search:"), QLineEdit::Normal, tr("Unnamed"), &ok));
	if (!ok) return;

	QByteArray curState;
	QDataStream ds(&curState, QIODevice::WriteOnly);
	ds << QVariant(searchWidget()->searchBuilder()->getState());
	SQLite::Query query(Database::connection());
	query.prepare(QString("INSERT INTO sets VALUES(NULL, %2, ifnull((SELECT max(position) + 1 FROM sets WHERE parent %1), 0), ?, ?)").arg(!parentId ? "is null" : QString("= %1").arg(parentId)).arg(!parentId ? "null" : QString::number(parentId)));
	query.bindValue(setName);
	query.bindValue(curState);
	// TODO error handling
	query.exec();
}

void MainWindow::newSavedSearchesFolder()
{
	QAction *action(qobject_cast<QAction *>(sender()));
	// Should never happen
	if (!action) return;
	int parentId(action->property("T_rowid").toInt());

	bool ok;
	QString setName(QInputDialog::getText(this, tr("New folder"), tr("Please enter a name for this folder"), QLineEdit::Normal, tr("Unnamed folder"), &ok));
	if (!ok) return;

	SQLite::Query query(Database::connection());
	query.prepare(QString("INSERT INTO sets VALUES(NULL, %2, ifnull((SELECT max(position) + 1 FROM sets WHERE parent %1), 0), ?, null)").arg(!parentId ? "is null" : QString("= %1").arg(parentId)).arg(!parentId ? "null" : QString::number(parentId)));
	query.bindValue(setName);
	// TODO error handling
	query.exec();
}

void MainWindow::onSavedSearchSelected()
{
	QAction *action = qobject_cast<QAction *>(sender());
	//  Should never happen, but hey.
	if (!action) return;
	QByteArray bState(action->property("T_state").toByteArray());
	QDataStream ds(&bState, QIODevice::ReadOnly);
	QMap<QString, QVariant> state(QVariant(ds).toMap());
	searchWidget()->searchBuilder()->restoreState(state);
	searchWidget()->searchBuilder()->runSearch();
	if (_searchDockWidget->isHidden()) _searchDockWidget->setHidden(false);
}

void MainWindow::organizeSavedSearches()
{
	SavedSearchesOrganizer organizer;
	organizer.exec();
}

void MainWindow::trainSettings()
{
	TrainSettings settings;
	if (settings.exec() == QDialog::Accepted) {
		settings.applySettings();
	}
}

// SearchWidget stuff
void MainWindow::display(const QModelIndex &clicked)
{
	QAbstractItemView *view(qobject_cast<QAbstractItemView *>(sender()));
	if (view == searchWidget()->resultsView() && !searchWidget()->resultsView()->selectionModel()->isSelected(clicked)) return;
	else if (view == _entryListWidget->entryListView() && !_entryListWidget->entryListView()->selectionModel()->isSelected(clicked)) return;
	EntryPointer entry(clicked.data(Entry::EntryRole).value<EntryPointer>());
	// Do not redisplay an entry if it is already shown
	if (_detailedView->detailedView()->entryView()->entry() == entry) return;
	if (entry) _detailedView->detailedView()->display(entry);
}

void MainWindow::resetSearch()
{
	searchWidget()->resetSearch();
}

void MainWindow::focusTextSearch()
{
	TextFilterWidget *tWidget = qobject_cast<TextFilterWidget *>(searchWidget()->getSearchFilter("searchtext"));
	if (!tWidget) return;
	searchWidget()->searchFilters()->showWidget(tWidget);
	// showWidget is supposed to do this already, but somehow this is still necessary.
	tWidget->setFocus();
	tWidget->searchField()->lineEdit()->selectAll();
}

void MainWindow::focusResultsList()
{
	searchWidget()->resultsView()->setFocus();
}

void MainWindow::enableClipboardInput(bool enable)
{
	QClipboard *clipboard = QApplication::clipboard();
	if (enable && !_clipboardEnabled) {
		_clipboardEnabled = true;
		connect(clipboard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(onClipboardChanged(QClipboard::Mode)));
	}
	else if (!enable && _clipboardEnabled ) {
		disconnect(clipboard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(onClipboardChanged(QClipboard::Mode)));
		_clipboardEnabled = false;
	}
}

void MainWindow::onClipboardChanged(QClipboard::Mode mode)
{
	TextFilterWidget *tfw = qobject_cast<TextFilterWidget *>(searchWidget()->getSearchFilter("searchtext"));
	if (!tfw) return;
	QClipboard *clipboard = QApplication::clipboard();
	QString cText(clipboard->text(mode));
	QString cText2;
	// Remove any space between Japanese content
	bool lastCharJapanese = true;
	bool lastCharSpace = false;
	foreach (QChar c, cText) {
		bool isJapanese = TextTools::isJapaneseChar(c);
		bool isSpace = c.isSpace();
		if (lastCharSpace) {
			// Space between japanese chars, skip it.
			if (isJapanese && lastCharJapanese) cText2 += c;
			// Otherwise include the space
			else { cText2 += QString(" ") + c; }
		}
		else if (!isSpace) cText2 += c;
		if (!isSpace) lastCharJapanese = isJapanese;
		lastCharSpace = isSpace;
	}
	if (cText2.isEmpty() || cText2 == tfw->text()) return;
	tfw->setText(cText2);
}

void MainWindow::fitToScreen(QWidget *widget)
{
	QRect screenRect = QApplication::desktop()->availableGeometry(widget);
	QRect popupRect(widget->geometry());
	if (screenRect.left() > popupRect.left()) popupRect.moveLeft(screenRect.left());
	if (screenRect.top() > popupRect.top()) popupRect.moveTop(screenRect.top());
	if (screenRect.right() < popupRect.right()) popupRect.moveRight(screenRect.right());
	if (screenRect.bottom() < popupRect.bottom()) popupRect.moveBottom(screenRect.bottom());
	widget->setGeometry(popupRect);
}

void MainWindow::restoreWholeState()
{
	// Geometry & state
	restoreGeometry(windowGeometry.value());
	restoreState(windowState.value(), MAINWINDOW_STATE_VERSION);
}
