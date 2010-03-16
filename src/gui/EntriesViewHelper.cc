/*
 *  Copyright (C) 2010  Alexandre Courbot
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

#include "gui/EntriesViewHelper.h"
#include "gui/EntryMenu.h"
#include "gui/EditEntryNotesDialog.h"
#include "gui/TagsDialogs.h"
#include "gui/EntriesPrinter.h"

#include <QAction>
#include <QProgressDialog>
#include <QFile>
#include <QPrintDialog>
#include <QFileDialog>
#include <QMessageBox>

EntriesViewHelper::EntriesViewHelper(QAbstractItemView *parent) : EntryMenu(parent), _client(parent), _actionPrint(QIcon(":/images/icons/print.png"), tr("&Print..."), 0), _actionPrintPreview(QIcon(":/images/icons/print.png"), tr("Print p&review..."), 0), _actionPrintBooklet(QIcon(":/images/icons/print.png"), tr("Print &booklet..."), 0), _actionPrintBookletPreview(QIcon(":/images/icons/print.png"), tr("Booklet p&review..."), 0), _actionExportTab(QIcon(":/images/icons/document-export.png"), tr("&Export..."), 0)
{
	connect(&addToStudyAction, SIGNAL(triggered()),
			this, SLOT(studySelected()));
	connect(&removeFromStudyAction, SIGNAL(triggered()),
			this, SLOT(unstudySelected()));
	connect(&alreadyKnownAction, SIGNAL(triggered()),
	        this, SLOT(markAsKnown()));
	connect(&resetTrainingAction, SIGNAL(triggered()),
	        this, SLOT(resetTraining()));
	connect(&setTagsAction, SIGNAL(triggered()),
	        this, SLOT(setTags()));
	connect(&addTagsAction, SIGNAL(triggered()),
	        this, SLOT(addTags()));
	connect(&setNotesAction, SIGNAL(triggered()),
	        this, SLOT(addNote()));
	connect(this, SIGNAL(tagsHistorySelected(const QStringList &)),
			this, SLOT(addTags(const QStringList &)));

	connect(&_actionPrint, SIGNAL(triggered()), this, SLOT(print()));
	connect(&_actionPrintBooklet, SIGNAL(triggered()), this, SLOT(printBooklet()));
	connect(&_actionPrintPreview, SIGNAL(triggered()), this, SLOT(printPreview()));
	connect(&_actionPrintBookletPreview, SIGNAL(triggered()), this, SLOT(printBookletPreview()));
	connect(&_actionExportTab, SIGNAL(triggered()), this, SLOT(tabExport()));
		
	_entriesMenu.addAction(&_actionPrint);
	_entriesMenu.addAction(&_actionPrintPreview);
	_entriesMenu.addAction(&_actionPrintBooklet);
	_entriesMenu.addAction(&_actionPrintBookletPreview);
	_entriesMenu.addSeparator();
	_entriesMenu.addAction(&_actionExportTab);
}

QList<EntryPointer> EntriesViewHelper::selectedEntries() const
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	QList<EntryPointer> selectedEntries;
	foreach(const QModelIndex &index, selection) {
		EntryPointer entry(qVariantValue<EntryPointer>(index.data(Entry::EntryRole)));
		if (entry) selectedEntries << entry;
	}
	return selectedEntries;
}

void EntriesViewHelper::studySelected()
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Marking entries..."), tr("Abort"), 0, selection.size(), client());
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		EntryPointer entry = qVariantValue<EntryPointer>(index.data(Entry::EntryRole));
		if (!entry) continue;
		entry->addToTraining();
		client()->update(index);
	}
}

void EntriesViewHelper::unstudySelected()
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Marking entries..."), tr("Abort"), 0, selection.size(), client());
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		EntryPointer entry = qVariantValue<EntryPointer>(index.data(Entry::EntryRole));
		if (!entry) continue;
		entry->removeFromTraining();
		client()->update(index);
	}
}

void EntriesViewHelper::markAsKnown()
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Marking entries..."), tr("Abort"), 0, selection.size(), client());
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		EntryPointer entry = qVariantValue<EntryPointer>(index.data(Entry::EntryRole));
		if (!entry) continue;
		if (!entry->alreadyKnown()) continue;
		entry->setAlreadyKnown();
		client()->update(index);
	}
}

void EntriesViewHelper::resetTraining()
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Resetting entries..."), tr("Abort"), 0, selection.size(), client());
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		EntryPointer entry = qVariantValue<EntryPointer>(index.data(Entry::EntryRole));
		if (!entry) continue;
		entry->resetScore();
		client()->update(index);
	}
}

void EntriesViewHelper::setTags()
{
	TagsDialogs::setTagsDialog(selectedEntries(), client());
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	foreach (const QModelIndex &index, selection)
		client()->update(index);
}

void EntriesViewHelper::addTags()
{
	TagsDialogs::addTagsDialog(selectedEntries(), client());
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	foreach (const QModelIndex &index, selection)
		client()->update(index);
}

void EntriesViewHelper::addTags(const QStringList &tags)
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Adding tags..."), tr("Abort"), 0, selection.size(), client());
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);
	
	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		EntryPointer entry = qVariantValue<EntryPointer>(index.data(Entry::EntryRole));
		if (!entry) continue;
		entry->addTags(tags);
		client()->update(index);
	}
}

void EntriesViewHelper::addNote()
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	if (selection.size() != 1) return;
	EntryPointer entry = qVariantValue<EntryPointer >(selection[0].data(Entry::EntryRole));
	if (!entry) return;
	EditEntryNotesDialog dialog(*entry, client());
	if (dialog.exec() != QDialog::Accepted) return;
	foreach (const QModelIndex &index, selection)
		client()->update(index);
}

bool EntriesViewHelper::askForPrintOptions(QPrinter &printer, const QString &title)
{
	QPrintDialog printDialog(&printer, client());
	printDialog.setWindowTitle(title);
	printDialog.setOptions(QAbstractPrintDialog::PrintToFile | QAbstractPrintDialog::PrintPageRange | QAbstractPrintDialog::PrintSelection);
	if (printDialog.exec() != QDialog::Accepted) return false;
	return true;
}

void EntriesViewHelper::print()
{
	QPrinter printer;
	if (!askForPrintOptions(printer)) return;
	QModelIndexList selIndexes;
	if (printer.printRange() == QPrinter::Selection) selIndexes = client()->selectionModel()->selectedIndexes();
	EntriesPrinter(client()->model(), selIndexes, client()).print(&printer);
}

void EntriesViewHelper::printPreview()
{
	QPrinter printer;
	EntriesPrinter(client()->model(), QModelIndexList(), client()).printPreview(&printer);
}

void EntriesViewHelper::printBooklet()
{
	QPrinter printer;
	if (!askForPrintOptions(printer, tr("Booklet print"))) return;
	QModelIndexList selIndexes;
	if (printer.printRange() == QPrinter::Selection) selIndexes = client()->selectionModel()->selectedIndexes();
	EntriesPrinter(client()->model(), selIndexes, client()).printBooklet(&printer);
}

void EntriesViewHelper::printBookletPreview()
{
	QPrinter printer;
	EntriesPrinter(client()->model(), QModelIndexList(), client()).printBookletPreview(&printer);
}

void EntriesViewHelper::tabExport()
{
	QString exportFile = QFileDialog::getSaveFileName(0, tr("Export to tab-separated file..."));
	if (exportFile.isEmpty()) return;
	QFile outFile(exportFile);
	if (!outFile.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(0, tr("Cannot write file"), QString(tr("Unable to write file %1.")).arg(exportFile));
		return;
	}

	QModelIndexList selection(client()->selectionModel()->selectedIndexes());
	const QAbstractItemModel *model = client()->model();
	
	// Build the list of entries to export
	QList<ConstEntryPointer> entries;
	// No selection specified, we export all the model
	if (selection.isEmpty()) {
		// Parse the model and print all its content
		for (int i = 0; i < model->rowCount(); i++) {
			ConstEntryPointer entry(qVariantValue<EntryPointer>(model->index(i, 0, QModelIndex()).data(Entry::EntryRole)));
			if (entry) entries << entry;
		}
	// Selection specified, we limit ourselves to it
	} else {
		foreach (const QModelIndex &idx, selection) {
			ConstEntryPointer entry(qVariantValue<EntryPointer>(idx.data(Entry::EntryRole)));
			if (entry) entries << entry;
		}
	}
	
	// Dummy entry to notify Anki that tab is our delimiter
	//outFile.write("\t\t\n");
	foreach (ConstEntryPointer entry, entries) {
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
