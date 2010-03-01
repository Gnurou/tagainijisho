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

#include "core/EntrySearcherManager.h"
#include "gui/SearchWidget.h"
#include "gui/EntriesPrinter.h"
#include "gui/EntryTypeFilterWidget.h"
#include "gui/TextFilterWidget.h"
#include "gui/TagsFilterWidget.h"
#include "gui/NotesFilterWidget.h"
#include "gui/StudyFilterWidget.h"
#include "gui/JLPTFilterWidget.h"
#include "gui/ClickableLabel.h"

#include <QFile>
#include <QPrintDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>

PreferenceItem<int> SearchWidget::historySize("mainWindow/resultsView", "historySize", 100);

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent), _history(historySize.value())
{
	setupUi(this);
	
	// Setup our event listener
	//filtersScrollArea->viewport()->installEventFilter(this);
	_searchFilters->installEventFilter(this);
	
	// Add the prev/next search actions to the results view bar
	QToolButton *prevSearchButton = new QToolButton(this);
	prevSearchButton->setDefaultAction(actionPreviousSearch);
	_resultsView->buttonsLayout()->insertWidget(0, prevSearchButton);
	QToolButton *nextSearchButton = new QToolButton(this);
	nextSearchButton->setDefaultAction(actionNextSearch);
	_resultsView->buttonsLayout()->insertWidget(1, nextSearchButton);
	
	// Setup the results model and view
	_results = new ResultsList(this);
	_resultsView->setModel(_results);
	
	// Search builder
	connect(&_searchBuilder, SIGNAL(queryRequested(QString)), this, SLOT(search(QString)));
	
	_filtersToolBar = new QToolBar(this);
	_filtersToolBar->layout()->setContentsMargins(0, 0, 0, 0);
	_filtersToolBar->setStyleSheet("QToolBar { background: none; border-style: none; border-width: 0px; margin: 0px; padding: 0px; }");
	
	QMenu *entriesMenu = new QMenu(this);
	entriesMenu->addAction(actionPrint);
	entriesMenu->addAction(actionPrintPreview);
	entriesMenu->addAction(actionPrintBooklet);
	entriesMenu->addAction(actionBookletPreview);
	entriesMenu->addSeparator();
	entriesMenu->addAction(actionExport);
	QAction *entriesMenuAction = new QAction(QIcon(QPixmap(":/images/icons/list-add.png")), tr("Displayed entries..."), _filtersToolBar);
	entriesMenuAction->setMenu(entriesMenu);
	_filtersToolBar->addAction(entriesMenuAction);
	// Fix the behavior of the entries button
	QToolButton *tButton = qobject_cast<QToolButton *>(_filtersToolBar->widgetForAction(entriesMenuAction));
	if (tButton) tButton->setPopupMode(QToolButton::InstantPopup);
	
	// Search filters
	EntryTypeFilterWidget *typeFilter = new EntryTypeFilterWidget(this);
	_searchFilterWidgets[typeFilter->name()] = typeFilter;
	_filtersToolBar->addWidget(typeFilter);
	connect(typeFilter, SIGNAL(updateTitle(const QString &)), _searchFilters, SLOT(onTitleChanged(const QString &)));
	_searchBuilder.addSearchFilter(typeFilter);
	TextFilterWidget *textFilter = new TextFilterWidget(this);
	addSearchFilter(textFilter);
	addSearchFilter(new StudyFilterWidget(this));
	addSearchFilter(new JLPTFilterWidget(this));
	addSearchFilter(new TagsFilterWidget(this));
	addSearchFilter(new NotesFilterWidget(this));
	
	// Show the text search by default
	_searchFilters->showWidget(textFilter);

	// Insert the tool bar
	static_cast<QBoxLayout *>(layout())->insertWidget(0, _filtersToolBar);
}

void SearchWidget::search(const QString &commands)
{
	QString localCommands(commands.trimmed());
	if (!(localCommands.isEmpty() || localCommands == ":jmdict" || localCommands == ":kanjidic")) {
		_history.add(_searchBuilder.getState());
		_search(localCommands);
	}
	else {
		_results->abortSearch();
		_results->clear();
		_resultsView->showNbResults(0);
	}
}

void SearchWidget::_search(const QString &commands)
{
	_queryBuilder.clear();
	
	actionPreviousSearch->setEnabled(_history.hasPrevious());
	actionNextSearch->setEnabled(_history.hasNext());

	// If we cannot build a valid query, no need to continue
	if (!EntrySearcherManager::instance().buildQuery(commands, _queryBuilder)) return;

	_results->search(_queryBuilder);
}

void SearchWidget::goPrev()
{
	QMap<QString, QVariant> q;
	bool ok = _history.previous(q);
	if (ok) {
		_searchBuilder.restoreState(q);
		_search(_searchBuilder.commands());
	}
}

void SearchWidget::goNext()
{
	QMap<QString, QVariant> q;
	bool ok = _history.next(q);
	if (ok) {
		_searchBuilder.restoreState(q);
		_search(_searchBuilder.commands());
	}
}

void SearchWidget::resetSearch()
{
	_searchBuilder.reset();
}

bool SearchWidget::askForPrintOptions(QPrinter &printer, const QString &title)
{
	QPrintDialog printDialog(&printer, this);
	printDialog.setWindowTitle(title);
	#if QT_VERSION >= 0x040500
	printDialog.setOptions(QAbstractPrintDialog::PrintToFile | QAbstractPrintDialog::PrintPageRange | QAbstractPrintDialog::PrintSelection);
	#endif
	if (printDialog.exec() != QDialog::Accepted) return false;
	return true;
}

void SearchWidget::print()
{
	QPrinter printer;
	if (!askForPrintOptions(printer)) return;
	QModelIndexList selIndexes;
	if (printer.printRange() == QPrinter::Selection) selIndexes = resultsView()->selectionModel()->selectedIndexes();
	EntriesPrinter(resultsList(), selIndexes, this).print(&printer);
}

void SearchWidget::printPreview()
{
	QPrinter printer;
	EntriesPrinter(resultsList(), QModelIndexList(), this).printPreview(&printer);
}

void SearchWidget::printBooklet()
{
	QPrinter printer;
	if (!askForPrintOptions(printer, tr("Booklet print"))) return;
	QModelIndexList selIndexes;
	if (printer.printRange() == QPrinter::Selection) selIndexes = resultsView()->selectionModel()->selectedIndexes();
	EntriesPrinter(resultsList(), selIndexes, this).printBooklet(&printer);
}

void SearchWidget::printBookletPreview()
{
	QPrinter printer;
	EntriesPrinter(resultsList(), QModelIndexList(), this).printBookletPreview(&printer);
}

void SearchWidget::tabExport()
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
		ConstEntryPointer entry = results->getEntry(i);
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

void SearchWidget::addSearchFilter(SearchFilterWidget *sWidget)
{
	if (_searchBuilder.contains(sWidget->name())) return;
	_searchFilterWidgets[sWidget->name()] = sWidget;
	_filtersToolBar->addAction(new ToolBarWidget(_searchFilters->addWidget(sWidget->currentTitle(), sWidget), _filtersToolBar));
	connect(sWidget, SIGNAL(updateTitle(const QString &)), _searchFilters, SLOT(onTitleChanged(const QString &)));
	_searchBuilder.addSearchFilter(sWidget);
}

SearchFilterWidget *SearchWidget::getSearchFilter(const QString &name)
{
	if (!_searchFilterWidgets.contains(name)) return 0;
	return _searchFilterWidgets[name];
}

void SearchWidget::removeSearchFilterWidget(const QString &name)
{
	if (!_searchFilterWidgets.contains(name)) return;
	
	_searchFilterWidgets.remove(name);
	_searchBuilder.removeSearchFilter(name);
}

bool SearchWidget::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == _searchFilters) {
		if (event->type() == QEvent::Resize) {
			QResizeEvent *revt(static_cast<QResizeEvent *>(event));
			// The new height of the scroll area will depend on whether we need to display the horizontal scrollbar or not
			int newHeight = revt->size().height();
			if (revt->size().width() > filtersScrollArea->viewport()->width()) newHeight += filtersScrollArea->horizontalScrollBar()->height() + 2;
			filtersScrollArea->setMinimumHeight(newHeight);
			filtersScrollArea->setMaximumHeight(newHeight);
		}
	}
	return QWidget::eventFilter(obj, event);
}
