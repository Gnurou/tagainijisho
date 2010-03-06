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

#include "tagaini_config.h"

#include "core/Paths.h"
#include "core/TextTools.h"
#include "core/Database.h"
#include "core/Plugin.h"
#include "core/EntrySearcherManager.h"
#include "gui/UpdateChecker.h"
#include "gui/SetsOrganizer.h"
#include "gui/TrainSettings.h"
#include "gui/PreferencesWindow.h"
#include "gui/YesNoTrainer.h"
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
#include <QSqlQuery>
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
#include <QClipboard>

/// State version of the main window - should be increated every time the docks or statusbars change
#define MAINWINDOW_STATE_VERSION 0

MainWindow *MainWindow::_instance = 0;
UpdateChecker *_updateChecker = 0;
UpdateChecker *_betaUpdateChecker = 0;

PreferenceItem<QString> MainWindow::applicationFont("", "defaultFont", "");
PreferenceItem<QString> MainWindow::guiLanguage("", "guiLanguage", "");
PreferenceItem<bool> MainWindow::autoCheckUpdates("", "autoCheckUpdates", true);
PreferenceItem<bool> MainWindow::autoCheckBetaUpdates("", "autoCheckBetaUpdates", false);
PreferenceItem<int> MainWindow::updateCheckInterval("", "updateCheckInterval", 3);

PreferenceItem<QByteArray> MainWindow::windowGeometry("mainWindow", "geometry", "");
PreferenceItem<QByteArray> MainWindow::windowState("mainWindow", "state", "");
PreferenceItem<QByteArray> MainWindow::splitterState("mainWindow", "splitterGeometry", "");

/*void SearchFilterDock::closeEvent(QCloseEvent *event)
{
	SearchFilterWidget *sfw = qobject_cast<SearchFilterWidget *>(widget());
	if (sfw) sfw->reset();
	QDockWidget::closeEvent(event);
}*/

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
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _clipboardEnabled(false)
{
	_instance = this;
	
	setupUi(this);
	// Strangely this is not done properly by Qt designer...
	connect(_setsMenu, SIGNAL(aboutToShow()), this, SLOT(populateSetsMenu()));
	
	// Steal the tool bar and set it as our dock title bar widget
	QWidget *filtersToolBar = static_cast<QBoxLayout *>(searchWidget()->layout())->takeAt(0)->widget();
	DockTitleBar *dBar = new DockTitleBar(filtersToolBar, searchDockWidget);
	searchDockWidget->setTitleBarWidget(dBar);
	// TODO Save space, otherwise the title bar may become too big
	//filtersToolBar->setMaximumHeight(dBar->height() / 2);
	dBar->layout()->setContentsMargins(0, 0, 0, 0);

	// Focus on the text input on startup
	actionFocus_text_search->trigger();
	
	// Auto-clipboard search action
	QAction *_enableClipboardInputAction = new QAction(tr("Auto-search on clipboard content"), this);
	_enableClipboardInputAction->setCheckable(true);
	connect(_enableClipboardInputAction, SIGNAL(toggled(bool)), this, SLOT(enableClipboardInput(bool)));
	_searchMenu->addAction(_enableClipboardInputAction);
	
	// List widget
	_entryListWidget = new EntryListWidget(this);
	_entryListWidget->entryListView()->setModel(&_listModel);
	connect(_entryListWidget->entryListView(), SIGNAL(entrySelected(EntryPointer)), detailedView(), SLOT(display(EntryPointer)));
	QDockWidget *dWidget = new QDockWidget(_entryListWidget->currentTitle(), this);
	dWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
	dBar = new DockTitleBar(new QLabel(tr("Lists"), dWidget), dWidget);
	dWidget->setTitleBarWidget(dBar);
	
	dWidget->setWidget(_entryListWidget);
	addDockWidget(Qt::LeftDockWidgetArea, dWidget);
	dWidget->setObjectName(_entryListWidget->currentTitle() + "Dock");
	_searchMenu->addSeparator();
	QAction *action = searchDockWidget->toggleViewAction();
	action->setShortcut(QKeySequence("F2"));
	_searchMenu->addAction(action);
	action = dWidget->toggleViewAction();
	action->setShortcut(QKeySequence("F3"));
	_searchMenu->addAction(action);
	
	// Geometry & state
	restoreGeometry(windowGeometry.value());
	restoreState(windowState.value(), MAINWINDOW_STATE_VERSION);
	// Splitter layout
	//if (splitterState.isDefault()) {
		//splitter->setStretchFactor(0, 1);
		//splitter->setStretchFactor(1, 3);
	//}
	//else splitter->restoreState(splitterState.value());
	

	// Docks
	/*
	setTabPosition(Qt::TopDockWidgetArea, QTabWidget::North);
	QDockWidget *textWidget = addSearchFilter(new TextFilterWidget(this), Qt::TopDockWidgetArea);
	addSearchFilter(new StudyFilterWidget(this), textWidget);
	addSearchFilter(new JLPTFilterWidget(this), textWidget);
	addSearchFilter(new TagsFilterWidget(this), textWidget);
	addSearchFilter(new NotesFilterWidget(this), textWidget);
	EntryListWidget *elWidget = new EntryListWidget(this);
	elWidget->entryListView()->setModel(&_listModel);
	connect(elWidget->entryListView(), SIGNAL(entrySelected(EntryPointer<Entry>)), detailedView(), SLOT(display(EntryPointer<Entry>)));
	addSearchFilter(elWidget, Qt::LeftDockWidgetArea);
	textWidget->raise();
	*/
	// TODO dirty fix!
	//restoreState(windowState.value(), MAINWINDOW_STATE_VERSION);
	// Setup the corners
	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	
	// Display selected items in the results view
	connect(searchWidget()->resultsView(), SIGNAL(listSelectionChanged(QItemSelection,QItemSelection)), this, SLOT(display(QItemSelection,QItemSelection)));
		
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
	// And perform our startup check
	updateCheck();
}

MainWindow::~MainWindow()
{
	//splitterState.set(splitter->saveState());
	windowState.set(saveState(MAINWINDOW_STATE_VERSION));
	windowGeometry.set(saveGeometry());
}

PreferenceItem<QDateTime> MainWindow::lastUpdateCheck("", "lastUpdateCheck", QDateTime(QDate(2000, 1, 1)));
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

void MainWindow::exportUserData()
{
	QString to(QFileDialog::getSaveFileName(this, tr("Export user data...")));
	if (to.isEmpty()) return;

	QFile dataFile(QDir(userProfile()).absoluteFilePath("user.db"));
	dataFile.copy(to);
}

void MainWindow::importUserData()
{
	QString from(QFileDialog::getOpenFileName(this, tr("Import user data...")));
	if (from.isEmpty()) return;

	QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("About to replace user data!"), tr("<b>WARNING:</b> All the user data (study list, tags, notes, training, ...) is going to be replaced by the selected user data file. Are you <b>sure</b> you want to do this?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if (ret != QMessageBox::Ok) return;

	// TODO Check whether the file is valid!
	QFile dataFile(QDir(userProfile()).absoluteFilePath("user.db"));
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
	QString message = QString("<p>Copyright (C) 2008, 2009, 2010 Alexandre Courbot.</p><p align=\"center\"><a href=\"http://www.tagaini.net\">http://www.tagaini.net</a></p><p>This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under the conditions of the <a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">GNU General Public Licence, version 3.0</a>.</p><hr/>");
	QString credits = "<p>Tagaini Jisho uses data from various sources:</p>";

	foreach (const Plugin *plugin, Plugin::plugins()) {
		credits += plugin->pluginInfo();
	}
	Ui::AboutDialog aboutDialogUI;
	QDialog aboutDialog;
	SmoothScroller scroller;
	aboutDialogUI.setupUi(&aboutDialog);
	aboutDialogUI.title->setText(aboutDialogUI.title->text() + " " + QUOTEMACRO(VERSION));
	aboutDialogUI.logo->setPixmap(aboutDialogUI.logo->pixmap()->scaledToWidth(75, Qt::SmoothTransformation));
	aboutDialogUI.credits->setHtml(message + credits);
	aboutDialogUI.credits->viewport()->setAutoFillBackground(false);
	scroller.activateOn(aboutDialogUI.credits);
	connect(aboutDialogUI.credits, SIGNAL(anchorClicked(QUrl)), this, SLOT(openUrl(QUrl)));
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
	QDesktopServices::openUrl(QUrl("file://" + QDir(QUOTEMACRO(DATA_DIR)).filePath("doc/manual.html")));
#else
	QDesktopServices::openUrl(QUrl("file://" + QDir(QCoreApplication::applicationDirPath()).filePath("doc/manual.html")));
#endif
}

void MainWindow::bugReport()
{
	QDesktopServices::openUrl(QUrl("https://bugs.launchpad.net/tagaini-jisho"));
}

void MainWindow::featureRequest()
{
	QDesktopServices::openUrl(QUrl("https://blueprints.launchpad.net/tagaini-jisho"));
}

void MainWindow::askQuestion()
{
	QDesktopServices::openUrl(QUrl("https://answers.launchpad.net/tagaini-jisho"));
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
		QDesktopServices::openUrl(QUrl("http://www.tagaini.net"));
	}
	else _updateTimer.stop();
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
		QDesktopServices::openUrl(QUrl("http://www.tagaini.net"));
	}
	else _updateTimer.stop();
}

void MainWindow::populateMenu(QMenu *menu, int parentId)
{
	QSqlQuery query;
	query.exec(QString("SELECT label, state, rowid FROM sets WHERE parent %1 ORDER BY position").arg(parentId == 0 ? "is null" : QString("= %1").arg(parentId)));
	while (query.next()) {
		// A folder
		if (query.value(1).isNull()) {
			QMenu *subMenu = menu->addMenu(QIcon(":/images/icons/folder.png"), query.value(0).toString());
			connect(subMenu, SIGNAL(aboutToShow()), this, SLOT(populateSubMenu()));
			subMenu->setProperty("T_rowid", query.value(2).toInt());
			if (!parentId) _rootMenus << subMenu;
		}
		// A set
		else {
			QAction *action = menu->addAction(query.value(0).toString(), this, SLOT(onSetSelected()));
			action->setProperty("T_state", query.value(1).toByteArray());
			if (!parentId) _rootActions << action;
		}
	}
}

void MainWindow::populateSetsMenu()
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
	QAction *action = menu->addAction(tr("Create new set here..."), this, SLOT(newSet()));
	action->setProperty("T_rowid", rowId);
	action = menu->addAction(tr("Create new folder here..."), this, SLOT(newSetsFolder()));
	action->setProperty("T_rowid", rowId);
}

void MainWindow::newSet()
{
	QAction *action(qobject_cast<QAction *>(sender()));
	// Should never happen
	if (!action) return;
	int parentId(action->property("T_rowid").toInt());

	bool ok;
	QString setName(QInputDialog::getText(this, tr("New set"), tr("Please enter a name for this set:"), QLineEdit::Normal, tr("Unnamed set"), &ok));
	if (!ok) return;

	QByteArray curState;
	QDataStream ds(&curState, QIODevice::WriteOnly);
	ds << QVariant(searchWidget()->searchBuilder()->getState());
	QSqlQuery query;
	query.prepare(QString("INSERT INTO sets VALUES(%2, ifnull((SELECT max(position) + 1 FROM sets WHERE parent %1), 0), ?, ?)").arg(!parentId ? "is null" : QString("= %1").arg(parentId)).arg(!parentId ? "null" : QString::number(parentId)));
	query.addBindValue(setName);
	query.addBindValue(curState);
	// TODO error handling
	query.exec();
}

void MainWindow::newSetsFolder()
{
	QAction *action(qobject_cast<QAction *>(sender()));
	// Should never happen
	if (!action) return;
	int parentId(action->property("T_rowid").toInt());

	bool ok;
	QString setName(QInputDialog::getText(this, tr("New sets folder"), tr("Please enter a name for this folder"), QLineEdit::Normal, tr("Unnamed folder"), &ok));
	if (!ok) return;

	QSqlQuery query;
	query.prepare(QString("INSERT INTO sets VALUES(%2, ifnull((SELECT max(position) + 1 FROM sets WHERE parent %1), 0), ?, null)").arg(!parentId ? "is null" : QString("= %1").arg(parentId)).arg(!parentId ? "null" : QString::number(parentId)));
	query.addBindValue(setName);
	// TODO error handling
	query.exec();
}

void MainWindow::onSetSelected()
{
	QAction *action = qobject_cast<QAction *>(sender());
	//  Should never happen, but hey.
	if (!action) return;
	QByteArray bState(action->property("T_state").toByteArray());
	QDataStream ds(&bState, QIODevice::ReadOnly);
	QMap<QString, QVariant> state(QVariant(ds).toMap());
	searchWidget()->searchBuilder()->restoreState(state);
	searchWidget()->searchBuilder()->runSearch();
}

void MainWindow::organizeSets()
{
	SetsOrganizer organizer;
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
void MainWindow::display(const QItemSelection &selected, const QItemSelection &deselected)
{
	if (selected.isEmpty()) return;
	EntryPointer entry = qVariantValue<EntryPointer>(selected.indexes()[0].data(Entry::EntryRole));
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

void MainWindow::enableClipboardInput(bool enable)
{
	QClipboard *clipboard = QApplication::clipboard();
	if (enable && !_clipboardEnabled) {
		connect(clipboard, SIGNAL(dataChanged()), this, SLOT(onClipboardChanged()));
		connect(clipboard, SIGNAL(selectionChanged()), this, SLOT(onClipboardSelectionChanged()));
		_clipboardEnabled = true;
	}
	else if (!enable && _clipboardEnabled ) {
		disconnect(clipboard, SIGNAL(dataChanged()), this, SLOT(onClipboardChanged()));
		disconnect(clipboard, SIGNAL(selectionChanged()), this, SLOT(onClipboardSelectionChanged()));
		_clipboardEnabled = false;
	}
}

void MainWindow::onClipboardChanged()
{
	TextFilterWidget *tfw = qobject_cast<TextFilterWidget *>(searchWidget()->getSearchFilter("searchtext"));
	if (!tfw) return;
	QClipboard *clipboard = QApplication::clipboard();
	QString cText(clipboard->text(QClipboard::Clipboard));
	if (cText.isEmpty() || cText == tfw->text()) return;
	tfw->setText(cText);
}

void MainWindow::onClipboardSelectionChanged()
{
	TextFilterWidget *tfw = qobject_cast<TextFilterWidget *>(searchWidget()->getSearchFilter("searchtext"));
	if (!tfw) return;
	QClipboard *clipboard = QApplication::clipboard();
	QString cText(clipboard->text(QClipboard::Selection));
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
