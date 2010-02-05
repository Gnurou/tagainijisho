/*
 *  Copyright (C) 2008  Alexandre Courbot
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

#include "config.h"

#include "core/Paths.h"
#include "core/Database.h"
#include "core/Plugin.h"
#include "core/EntrySearcherManager.h"
#include "gui/EntryFormatter.h"
#include "gui/BookletPrinter.h"
#include "gui/UpdateChecker.h"
#include "gui/SetsOrganizer.h"
#include "gui/TrainSettings.h"
#include "gui/PreferencesWindow.h"
#include "gui/YesNoTrainer.h"
#include "gui/MainWindow.h"
#include "gui/ui_AboutDialog.h"
#include "gui/TextFilterWidget.h"
#include "gui/TagsFilterWidget.h"
#include "gui/NotesFilterWidget.h"
#include "gui/StudyFilterWidget.h"
#include "gui/JLPTFilterWidget.h"
#include "gui/EntryListWidget.h"
#include "gui/ClickableLabel.h"

#include <QtDebug>

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopServices>
#include <QFileDialog>
#include <QSqlQuery>
#include <QTextBrowser>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextEdit>
#include <QProgressDialog>
#include <QPicture>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QPrintPreviewDialog>
#include <QDataStream>
#include <QDockWidget>

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
PreferenceItem<int> MainWindow::historySize("mainWindow/resultsView", "historySize", 100);
PreferenceItem<QByteArray> MainWindow::splitterState("mainWindow", "splitterGeometry", "");

/*void SearchFilterDock::closeEvent(QCloseEvent *event)
{
	SearchFilterWidget *sfw = qobject_cast<SearchFilterWidget *>(widget());
	if (sfw) sfw->reset();
	QDockWidget::closeEvent(event);
}*/

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _history(historySize.value())
{
	_instance = this;
	
	setupUi(this);
	// Strangely this is not done properly by Qt designer...
	connect(_setsMenu, SIGNAL(aboutToShow()), this, SLOT(populateSetsMenu()));
	
	// Setup the results model and view
	_results = new ResultsList(this);
	_resultsView->setModel(_results);
	
	// Geometry & state
	restoreGeometry(windowGeometry.value());
	restoreState(windowState.value(), MAINWINDOW_STATE_VERSION);
	// Splitter layout
	if (splitterState.isDefault()) {
		splitter->setStretchFactor(0, 1);
		splitter->setStretchFactor(1, 3);
	}
	else splitter->restoreState(splitterState.value());
	
	// Search builder
	connect(&_searchBuilder, SIGNAL(queryRequested(QString)), this, SLOT(search(QString)));
	
	// Search filters
	SearchFilterWidget *textFilter = new TextFilterWidget(this);
	addSearchFilter(textFilter);
	addSearchFilter(new StudyFilterWidget(this));
	addSearchFilter(new JLPTFilterWidget(this));
	addSearchFilter(new TagsFilterWidget(this));
	addSearchFilter(new NotesFilterWidget(this));
	_searchFilters->showWidget(textFilter);

	// List widget
	EntryListWidget *elWidget = new EntryListWidget(this);
	elWidget->entryListView()->setModel(&_listModel);
	connect(elWidget->entryListView(), SIGNAL(entrySelected(EntryPointer<Entry>)), detailedView(), SLOT(display(EntryPointer<Entry>)));
	QDockWidget *dWidget = new QDockWidget(elWidget->currentTitle(), this);
	dWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dWidget->setWidget(elWidget);
	addDockWidget(Qt::LeftDockWidgetArea, dWidget);
	dWidget->setObjectName(elWidget->currentTitle() + "Dock");
	_searchMenu->addSeparator();
	_searchMenu->addAction(dWidget->toggleViewAction());
	

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
	restoreState(windowState.value(), MAINWINDOW_STATE_VERSION);
	
	// Display selected items in the results view
	connect(_resultsView->resultsView(), SIGNAL(listSelectionChanged(QItemSelection,QItemSelection)), this, SLOT(display(QItemSelection,QItemSelection)));
	
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
	splitterState.set(splitter->saveState());
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

#define PRINT_MINIMAL_SPACING 10

void MainWindow::printPageOfEntries(const QList<QPicture> &entries, QPainter *painter, int height)
{
	// First adjust the distance between entries
	int dist;
	if (entries.size() == 1) dist = 0;
	else {
		int totalHeight = 0;
		foreach(const QPicture &pic, entries) totalHeight += pic.height();
		// Use uniform repartition only if the space taken by the entries is more than 3/4 of the page
		if (totalHeight > height / 1.5) dist = (height - totalHeight) / entries.size() - 1;
		// Otherwise use a default space
		else dist = PRINT_MINIMAL_SPACING;
	}

	// Then print all entries drawn so far
	int pos = 0;
	foreach(const QPicture &pic, entries) {
		painter->save();
		painter->drawPicture(QPoint(0, pos), pic);
		painter->restore();
		pos += pic.height() + dist;
	}
}

bool MainWindow::askForPrintOptions(QPrinter &printer, const QString &title)
{
	QPrintDialog printDialog(&printer, this);
	printDialog.setWindowTitle(title);
	#if QT_VERSION >= 0x040500
	printDialog.setOptions(QAbstractPrintDialog::PrintToFile | QAbstractPrintDialog::PrintPageRange | QAbstractPrintDialog::PrintSelection);
	#endif
	if (printDialog.exec() != QDialog::Accepted) return false;
	return true;
}

void MainWindow::prepareAndPrintBookletJob(QPrinter *printer)
{
	BookletPrinter bPrinter(printer);
	bPrinter.setPrintRange(printer->printRange());
	bPrinter.setFromTo(printer->fromPage() * 8 - 7, printer->toPage() * 8);
	QFont textFont;
	textFont.setPointSize(15);
	prepareAndPrintJob(&bPrinter, &textFont);
}

void MainWindow::prepareAndPrintJob(QPrinter *printer, const QFont *font)
{
	int fromPage = -1, toPage = -1;
	bool selectionOnly = false;
	// Do we have a print range specified?
	if (printer->printRange() == QPrinter::PageRange) {
		fromPage = printer->fromPage();
		toPage = printer->toPage();
	}
	// Else are we supposed to print the selection only?
	else if (printer->printRange() == QPrinter::Selection) selectionOnly = true;

	// Setup the font
	QFont textFont;
	if (!font) {
		font = &textFont;
	}

	// Build the list of entries to print
	QList<const Entry *> entries;
	if (!selectionOnly) {
		const ResultsList *results = resultsList();
		for (int i = 0; i < results->nbResults(); i++)
			entries << results->getEntry(i).data();
	} else {
		QModelIndexList selection = resultsView()->selectionModel()->selection().indexes();
		for (int i = 0; i < selection.size(); i++)
			entries << qVariantValue<Entry *>(selection[i].data(ResultsList::EntryRole));
	}

	// Setup progress bar
	QProgressDialog progressDialog(tr("Preparing print job..."), tr("Abort"), 0, entries.size(), this);
	progressDialog.setMinimumDuration(50);
	progressDialog.setWindowTitle(tr("Printing..."));
	progressDialog.setWindowModality(Qt::WindowModal);
	progressDialog.show();

	// Print entries page by page
	int pageNbr = 1;
	QList<QPicture> waitingEntries;
	QPainter painter(printer);
	QRectF pageRect = painter.window();
	QRectF remainingSpace = pageRect;
	for (int i = 0; i < entries.size(); i++) {
		if (progressDialog.wasCanceled()) return;
		const Entry *const entry = entries[i];
		QRectF usedSpace;
		QPicture tPicture;
		QPainter picPainter(&tPicture);
		const EntryFormatter *formatter(EntryFormatter::getFormatter(entry));
		if (!formatter) continue;
		formatter->draw(entry, picPainter, pageRect, usedSpace, *font);
		if (!pageRect.contains(usedSpace)) {
			qDebug() << "Warning: entry does not fit on whole page, giving up this one...";
			continue;
		}
		picPainter.end();
		tPicture.setBoundingRect(usedSpace.toRect());

		// Do we need a new page here?
		if (remainingSpace.height() < usedSpace.height()) {
			// Print the current page
			if (fromPage == -1 || (pageNbr >= fromPage && pageNbr <= toPage)) {
				// If not on the first page, get a new page
				if (pageNbr > 1 && pageNbr > fromPage) printer->newPage();
				printPageOfEntries(waitingEntries, &painter, pageRect.height());
			}
			remainingSpace = pageRect;
			waitingEntries.clear();
			++pageNbr;
			// Optimize if we already reached the last page
			if (fromPage != -1 && pageNbr > toPage) return;
		}
		waitingEntries << tPicture;
		// Update remaining space, taking care to keep some white between entries
		remainingSpace.setTop(remainingSpace.top() + usedSpace.height() + PRINT_MINIMAL_SPACING);

		progressDialog.setValue(i);
	}

	if (fromPage == -1 || (pageNbr >= fromPage && pageNbr <= toPage)) {
		if (pageNbr > 1 && pageNbr > fromPage) printer->newPage();
		printPageOfEntries(waitingEntries, &painter, pageRect.height());
	}
}

void MainWindow::print()
{
	QPrinter printer;
	if (!askForPrintOptions(printer)) return;
	prepareAndPrintJob(&printer);
}

void MainWindow::printPreview()
{
	QPrinter tprinter;
	QPrintPreviewDialog dialog(&tprinter, this);
	dialog.setWindowTitle(tr("Print preview"));
	connect(&dialog, SIGNAL(paintRequested(QPrinter *)), this, SLOT(prepareAndPrintJob(QPrinter *)));
	dialog.exec();
}

void MainWindow::printBooklet()
{
	QPrinter printer;
	if (!askForPrintOptions(printer, tr("Booklet print"))) return;
	prepareAndPrintBookletJob(&printer);
}

void MainWindow::printBookletPreview()
{
	QPrinter tprinter;
	QPrintPreviewDialog dialog(&tprinter, this);
	dialog.setWindowTitle(tr("Booklet print preview"));
	connect(&dialog, SIGNAL(paintRequested(QPrinter *)), this, SLOT(prepareAndPrintBookletJob(QPrinter *)));
	dialog.exec();
}

void MainWindow::tabExport()
{
	QString exportFile = QFileDialog::getSaveFileName(0, tr("Export to tab-separated file..."));
	if (exportFile.isEmpty()) return;
	QFile outFile(exportFile);
	if (!outFile.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(0, tr("Cannot write file"), QString(tr("Unable to write file %1.")).arg(exportFile));
		return;
	}

	const ResultsList *results = resultsList();
	// Dummy entry to notify Anki that tab is our delimiter
	//outFile.write("\t\t\n");
	for (int i = 0; i < results->nbResults(); i++) {
		EntryPointer<Entry> entry = results->getEntry(i);
		QStringList writings = entry->writings();
		QStringList readings = entry->readings();
		QStringList meanings = entry->meanings();
		QString writing;
		QString reading;
		QString meaning;
		if (writings.size() > 0) writing = writings[0];
		if (readings.size() > 0) reading = readings[0];
		if (meanings.size() == 1) meaning += " " + meanings[0];
		else {
			int cpt = 1;
			foreach (const QString &str, meanings)
				meaning += QString(" (%1) %2").arg(cpt++).arg(str);
		}
		if (outFile.write(QString("%1\t%2\t%3\n").arg(writing).arg(readings.join(", ")).arg(meanings.join(", ")).toUtf8()) == -1) {
			QMessageBox::warning(0, tr("Error writing file"), QString(tr("Error while writing file %1.")).arg(exportFile));
			return;
		}
	}

	outFile.close();
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
	aboutDialogUI.setupUi(&aboutDialog);
	aboutDialogUI.title->setText(aboutDialogUI.title->text() + " " + QUOTEMACRO(VERSION));
	aboutDialogUI.logo->setPixmap(aboutDialogUI.logo->pixmap()->scaledToWidth(75, Qt::SmoothTransformation));
	aboutDialogUI.credits->setHtml(message + credits);
	aboutDialogUI.credits->viewport()->setAutoFillBackground(false);
	connect(aboutDialogUI.credits, SIGNAL(anchorClicked(QUrl)), this, SLOT(openUrl(QUrl)));
	aboutDialog.exec();
}

void MainWindow::openUrl(const QUrl &url)
{
	QDesktopServices::openUrl(url);
}

void MainWindow::manual()
{
	QDesktopServices::openUrl(QUrl("http://www.tagaini.net/manual"));
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
	ds << QVariant(_searchBuilder.getState());
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
	_searchBuilder.restoreState(state);
	_searchBuilder.runSearch();
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
	Entry *entry = qVariantValue<Entry *>(selected.indexes()[0].data(ResultsList::EntryRole));
	_detailedView->detailedView()->display(entry);
}

void MainWindow::search(const QString &commands)
{
	QString localCommands(commands.trimmed());
	if (!(localCommands.isEmpty() || localCommands == ":jmdict" || localCommands == ":kanjidic")) {
		_history.add(_searchBuilder.getState());
		_search(localCommands);
	}
	else {
		_results->clear();
		_results->abortSearch();
	}
}

void MainWindow::_search(const QString &commands)
{
	_queryBuilder.clear();
	
	actionPreviousSearch->setEnabled(_history.hasPrevious());
	actionNextSearch->setEnabled(_history.hasNext());

	// If we cannot build a valid query, no need to continue
	if (!EntrySearcherManager::instance().buildQuery(commands, _queryBuilder)) return;

	_results->search(_queryBuilder);
}

void MainWindow::goPrev()
{
	QMap<QString, QVariant> q;
	bool ok = _history.previous(q);
	if (ok) {
		_searchBuilder.restoreState(q);
		_search(_searchBuilder.commands());
	}
}

void MainWindow::goNext()
{
	QMap<QString, QVariant> q;
	bool ok = _history.next(q);
	if (ok) {
		_searchBuilder.restoreState(q);
		_search(_searchBuilder.commands());
	}
}

void MainWindow::addSearchFilter(SearchFilterWidget *sWidget)
{
	if (_searchBuilder.contains(sWidget->name())) return;
	_searchFilterWidgets[sWidget->name()] = sWidget;
	_filtersToolBar->addWidget(_searchFilters->addWidget(sWidget->currentTitle(), sWidget));
	connect(sWidget, SIGNAL(updateTitle(const QString &)), _searchFilters, SLOT(onTitleChanged(const QString &)));
	_searchBuilder.addSearchFilter(sWidget->name(), sWidget);
}

SearchFilterWidget *MainWindow::getSearchFilter(const QString &name)
{
	if (!_searchFilterWidgets.contains(name)) return 0;
	return _searchFilterWidgets[name];
}

void MainWindow::removeSearchFilterWidget(const QString &name)
{
	if (!_searchFilterWidgets.contains(name)) return;
	
	_searchFilterWidgets.remove(name);
	_searchBuilder.removeSearchFilter(name);
}
