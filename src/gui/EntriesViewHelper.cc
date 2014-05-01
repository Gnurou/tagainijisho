/*
 *  Copyright (C) 2010-2012  Alexandre Courbot
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

#include "core/Paths.h"
#include <core/Database.h>
#include "gui/EntriesViewHelper.h"
#include "gui/EntryMenu.h"
#include "gui/EditEntryNotesDialog.h"
#include "gui/TagsDialogs.h"
#include "gui/EntriesPrinter.h"
#include "gui/BatchHandler.h"

#include <QAction>
#include <QProgressDialog>
#include <QFile>
#include <QPrintDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QTreeView>
#include <QTableView>
#include <QContextMenuEvent>
#include <QLayout>
#include <QToolButton>
#include <QApplication>
#include <QClipboard>

EntriesViewHelper::EntriesViewHelper(QAbstractItemView* client, EntryDelegateLayout* delegateLayout, bool workOnSelection, bool viewOnly) : EntryMenu(client), _client(client), _entriesMenu(), _workOnSelection(workOnSelection), _actionPrint(QIcon(":/images/icons/print.png"), tr("&Print..."), 0), _actionPrintPreview(QIcon(":/images/icons/print.png"), tr("Print p&review..."), 0), _actionPrintBooklet(QIcon(":/images/icons/print.png"), tr("Print &booklet..."), 0), _actionPrintBookletPreview(QIcon(":/images/icons/print.png"), tr("Booklet pre&view..."), 0), _actionExportTab(QIcon(":/images/icons/document-export.png"), tr("Export as &TSV..."), 0), _actionExportJs(QIcon(":/images/icons/document-export.png"), tr("Export as &HTML..."), 0), prefRefs(MAX_PREF), _contextMenu()
{
	client->installEventFilter(this);
	client->viewport()->installEventFilter(this);
	
	// If no delegate layout has been specified, let's use our private one...
	if (!delegateLayout) delegateLayout = new EntryDelegateLayout(this);
	connect(delegateLayout, SIGNAL(layoutHasChanged()), this, SLOT(updateLayout()));
	_delegateLayout = delegateLayout;

	connect(&copyWritingAction, SIGNAL(triggered()), this, SLOT(copyWriting()));
	connect(&copyReadingAction, SIGNAL(triggered()), this, SLOT(copyReading()));
	connect(&addToStudyAction, SIGNAL(triggered()), this, SLOT(studySelected()));
	connect(&removeFromStudyAction, SIGNAL(triggered()), this, SLOT(unstudySelected()));
	connect(&alreadyKnownAction, SIGNAL(triggered()), this, SLOT(markAsKnown()));
	connect(&resetTrainingAction, SIGNAL(triggered()), this, SLOT(resetTraining()));
	connect(&setTagsAction, SIGNAL(triggered()), this, SLOT(setTags()));
	connect(&addTagsAction, SIGNAL(triggered()), this, SLOT(addTags()));
	connect(&setNotesAction, SIGNAL(triggered()), this, SLOT(addNote()));
	connect(this, SIGNAL(tagsHistorySelected(const QStringList &)), this, SLOT(addTags(const QStringList &)));

	connect(&_actionPrint, SIGNAL(triggered()), this, SLOT(print()));
	connect(&_actionPrintBooklet, SIGNAL(triggered()), this, SLOT(printBooklet()));
	connect(&_actionPrintPreview, SIGNAL(triggered()), this, SLOT(printPreview()));
	connect(&_actionPrintBookletPreview, SIGNAL(triggered()), this, SLOT(printBookletPreview()));
	connect(&_actionExportTab, SIGNAL(triggered()), this, SLOT(tabExport()));
	connect(&_actionExportJs, SIGNAL(triggered()), this, SLOT(jsExport()));
		
	_entriesMenu.addAction(&_actionPrint);
	_entriesMenu.addAction(&_actionPrintPreview);
	_entriesMenu.addAction(&_actionPrintBooklet);
	_entriesMenu.addAction(&_actionPrintBookletPreview);
	_entriesMenu.addSeparator();
	_entriesMenu.addAction(&_actionExportTab);
	_entriesMenu.addAction(&_actionExportJs);
	
	// If the view is editable, the helper menu shall be enabled
	if (!viewOnly) {
		_contextMenu.addSeparator();
		populateMenu(&_contextMenu);
		_contextMenu.addSeparator();
	}
}

void EntriesViewHelper::setPreferenceHandler(Preference pref, PreferenceRoot *ref)
{
	if (prefRefs[pref]) disconnect(prefRefs[pref], SIGNAL(valueChanged(QVariant)), _delegateLayout, SLOT(updateConfig(QVariant)));
	prefRefs[pref] = ref;
	if (ref) {
		_delegateLayout->setProperty(ref->name().toLatin1().constData(), ref->variantValue());
		connect(prefRefs[pref], SIGNAL(valueChanged(QVariant)), _delegateLayout, SLOT(updateConfig(QVariant)));
	}
}

QList<EntryPointer> EntriesViewHelper::selectedEntries() const
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	QProgressDialog progressDialog(tr("Selecting entries..."), tr("Abort"), 0, selection.size(), 0);
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Please wait..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	QList<EntryPointer> selectedEntries;
	int i = 0;
	int completed = true;
	foreach(const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) {
			completed = false;
			break;
		}
		progressDialog.setValue(i++);
		EntryPointer entry(index.data(Entry::EntryRole).value<EntryPointer>());
		if (entry) selectedEntries << entry;
	}
	if (!completed) return QList<EntryPointer>();
	else return selectedEntries;
}

void EntriesViewHelper::applyOnSelection(const BatchHandler &handler)
{
	QModelIndexList selection(client()->selectionModel()->selectedIndexes());

	BatchHandler::applyOnEntries(handler, selectedEntries());
	foreach (const QModelIndex &index, selection)
		client()->update(index);
}

struct StudyHandler : BatchHandler
{
	void apply(const EntryPointer &e) const
	{
		e->addToTraining();
	}
};

struct CopyWritingHandler : BatchHandler
{
	void apply(const EntryPointer &e) const
	{
		const QStringList& writings(e->writings());

		if (writings.isEmpty())
			return;

		QApplication::clipboard()->setText(writings[0]);
	}
};

struct CopyReadingHandler : BatchHandler
{
	void apply(const EntryPointer &e) const
	{
		const QStringList& readings(e->readings());

		if (readings.isEmpty())
			return;

		QApplication::clipboard()->setText(readings[0]);
	}
};

void EntriesViewHelper::copyWriting()
{
	applyOnSelection(CopyWritingHandler());
}

void EntriesViewHelper::copyReading()
{
	applyOnSelection(CopyReadingHandler());
}

void EntriesViewHelper::studySelected()
{
	applyOnSelection(StudyHandler());
}

struct UnstudyHandler : BatchHandler
{
	void apply(const EntryPointer &e) const
	{
		e->removeFromTraining();
	}
};

void EntriesViewHelper::unstudySelected()
{
	applyOnSelection(UnstudyHandler());
}

struct MarkAsKnownHandler : BatchHandler
{
	void apply(const EntryPointer &e) const
	{
		e->setAlreadyKnown();
	}
};

void EntriesViewHelper::markAsKnown()
{
	applyOnSelection(MarkAsKnownHandler());
}

struct ResetTrainingHandler : BatchHandler
{
	void apply(const EntryPointer &e) const
	{
		e->resetScore();
	}
};

void EntriesViewHelper::resetTraining()
{
	applyOnSelection(ResetTrainingHandler());
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

struct AddTagsListHandler : BatchHandler
{
	const QStringList &_tags;
	AddTagsListHandler(const QStringList &tags) : _tags(tags) {}

	void apply(const EntryPointer &e) const
	{
		e->addTags(_tags);
	}
};

void EntriesViewHelper::addTags(const QStringList &tags)
{
	applyOnSelection(AddTagsListHandler(tags));
}

void EntriesViewHelper::addNote()
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	if (selection.size() != 1) return;
	EntryPointer entry(selection[0].data(Entry::EntryRole).value<EntryPointer>());
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
	QAbstractPrintDialog::PrintDialogOptions options = QAbstractPrintDialog::PrintToFile | QAbstractPrintDialog::PrintPageRange;
	if (!_workOnSelection) options |= QAbstractPrintDialog::PrintSelection;
	printDialog.setOptions(options);
	if (printDialog.exec() != QDialog::Accepted) return false;
	return true;
}

static bool modelIndexLessThan(const QModelIndex &i1, const QModelIndex &i2)
{
	// Just in case we compare the same index - should not happen in real life
	if (i1 == i2) return false;

	// Build lists of the path that leads to both indexes from the root node
	QList<QModelIndex> l1, l2;
	QModelIndex p(i1);
	do {
		l1.prepend(p);
		if (!p.isValid()) break;
		p = p.parent();
	} while (true);
	p = i2;
	do {
		l2.prepend(p);
		if (!p.isValid()) break;
		p = p.parent();
	} while (true);

	// Check if one is the parent of another
	if (l2.contains(i1)) return true;
	if (l1.contains(i2)) return false;

	// Otherwise find the common lowest ancestor
	int lca = 0;
	while (l1[lca] == l2[lca]) lca++;

	// And check which child is lower than the other
	return l1[lca].row() < l2[lca].row();
}

QModelIndexList EntriesViewHelper::getAllIndexes(const QModelIndexList& indexes)
{
	QSet<QModelIndex> alreadyIn;
	QModelIndexList ret(getAllIndexes(indexes, alreadyIn));
	qStableSort(ret.begin(), ret.end(), modelIndexLessThan);
	return ret;
}

QModelIndexList EntriesViewHelper::getAllIndexes(const QModelIndexList& indexes, QSet<QModelIndex>& alreadyIn)
{
	QModelIndexList ret;
	
	foreach (const QModelIndex &idx, indexes) {
		// Should never happen
		if (!idx.isValid()) continue;
		if (alreadyIn.contains(idx)) continue;
		ret << idx;
		alreadyIn << idx;
		// Entries can be put directly into the return list
		ConstEntryPointer entry(idx.data(Entry::EntryRole).value<EntryPointer>());
		if (!entry) {
			// Non entries indexes must be list - see if they have children
			QModelIndexList childs;
			int childsCount = idx.model()->rowCount(idx);
			for (int i = 0; i < childsCount; i++) childs << idx.child(i, 0);
			ret += getAllIndexes(childs, alreadyIn);
		}
	}
	return ret;
}

QModelIndexList EntriesViewHelper::getEntriesToProcess(bool limitToSelection)
{
	QModelIndexList selIndexes;
	if (_workOnSelection || limitToSelection) selIndexes = client()->selectionModel()->selectedIndexes();
	else for (int i = 0; i < client()->model()->rowCount(QModelIndex()); i++) {
		for (int j = 0; j < client()->model()->columnCount(QModelIndex()); j++) {
			QModelIndex idx(client()->model()->index(i, j, QModelIndex()));
			if (idx.isValid()) selIndexes << idx;
		}
	}
	QModelIndexList entries(getAllIndexes(selIndexes));
	return entries;
}
	

void EntriesViewHelper::print()
{
	QPrinter printer;
	if (!askForPrintOptions(printer, tr("Print"))) return;
	EntriesPrinter(client()).print(getEntriesToProcess(printer.printRange() & QPrinter::Selection), &printer);
}

void EntriesViewHelper::printPreview()
{
	QPrinter printer;
	if (!workOnSelection() && !askForPrintOptions(printer, tr("Print preview"))) return;
	EntriesPrinter(client()).printPreview(getEntriesToProcess(printer.printRange() & QPrinter::Selection), &printer);
}

void EntriesViewHelper::printBooklet()
{
	QPrinter printer;
	if (!askForPrintOptions(printer, tr("Booklet print"))) return;
	EntriesPrinter(client()).printBooklet(getEntriesToProcess(printer.printRange() & QPrinter::Selection), &printer);
}

void EntriesViewHelper::printBookletPreview()
{
	QPrinter printer;
	if (!workOnSelection() && !askForPrintOptions(printer, tr("Booklet print preview"))) return;
	EntriesPrinter(client()).printBookletPreview(getEntriesToProcess(printer.printRange() & QPrinter::Selection), &printer);
}

void EntriesViewHelper::tabExport()
{
	QString exportFile = QFileDialog::getSaveFileName(0, tr("Export to tab-separated file..."), "export.tsv");
	if (exportFile.isEmpty()) return;
	QFile outFile(exportFile);
	if (!outFile.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(0, tr("Cannot write file"), QString(tr("Unable to write file %1.")).arg(exportFile));
		return;
	}

	QModelIndexList entries(getEntriesToProcess());

	// Dummy entry to notify Anki that tab is our delimiter
	//outFile.write("\t\t\n");
	foreach (const QModelIndex &idx, entries) {
		ConstEntryPointer entry(idx.data(Entry::EntryRole).value<EntryPointer>());
		// We cannot "export" lists due to the file purpose
		if (!entry) continue;
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

static QString escapeQuotes(const QString &str)
{
	QString ret(str);
	return ret.replace('"', "&quot;");
}

void EntriesViewHelper::jsExport()
{
	QString exportFile = QFileDialog::getSaveFileName(0, tr("Export to HTML flashcard file..."), "flashcard.html");
	if (exportFile.isEmpty()) return;
	QFile outFile(exportFile);
	if (!outFile.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(0, tr("Cannot write file"), QString(tr("Unable to write file %1!")).arg(exportFile));
		return;
	}

	QModelIndexList entries(getEntriesToProcess());
	QList<ConstEntryPointer> realEntries;

	foreach (const QModelIndex &idx, entries) {
		ConstEntryPointer entry(idx.data(Entry::EntryRole).value<EntryPointer>());
		// We cannot "export" lists due to the file purpose
		if (!entry) continue;
		realEntries << entry;
	}
	
	QFile tmplFile(lookForFile("export_template.html"));
	if (!tmplFile.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(0, tr("Cannot open template file"), QString(tr("Unable to open template file!")).arg(exportFile));
		return;
	}
	
	QString tmpl(QString::fromUtf8(tmplFile.readAll()));
	
	int idx = 0;
	QString data(QString("var entries = Array();\nfor (var i = 0; i < %1; i++) entries[i] = Array();\n").arg(realEntries.size()).toUtf8());
	foreach (const ConstEntryPointer &entry, realEntries) {
		QStringList readings = entry->readings();
		QStringList meanings = entry->meanings();
		QString mainRepr(escapeQuotes(entry->mainRepr()));
		readings.removeAll(mainRepr);
		QString reading;
		QString meaning;
		if (readings.size() > 0) reading = escapeQuotes(readings.join(", "));
		if (meanings.size() == 1) meaning = escapeQuotes(meanings[0]);
		else {
			int cpt = 1;
			foreach (const QString &str, meanings)
				meaning += QString(" (%1) %2").arg(cpt++).arg(escapeQuotes(str));
		}
		data += QString("entries[%1][0]=\"%2\";\nentries[%1][1]=\"%3\";\nentries[%1][2]=\"%4\";\n").arg(idx++).arg(mainRepr).arg(reading).arg(meaning);
	}
	
	tmpl.replace("__DATA__", data);
	outFile.write(tmpl.toUtf8());

	outFile.close();
}

bool EntriesViewHelper::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == client()) {
		switch (ev->type()) {
			case QEvent::ContextMenu:
			{
				QMenu *menu(contextMenu());
				if (menu->actions().isEmpty()) return true;
				QContextMenuEvent *cev(static_cast<QContextMenuEvent *>(ev));
				QList<ConstEntryPointer> cSelectedEntries;
				if (client()->selectionModel()->selectedIndexes().size() < 250) {
					QList<EntryPointer> _selectedEntries(selectedEntries());
					// This is stupid, but const-safety forces us here
					foreach (const EntryPointer &entry, _selectedEntries) cSelectedEntries << entry;
					updateStatus(cSelectedEntries);
				} else {
					setEnabledAll(true);
				}
				menu->exec(client()->mapToGlobal(cev->pos()));
				return false;
			}
			default:
				return false;
		}
	}
	else if (obj == client()->viewport()) {
		switch (ev->type()) {
			case QEvent::MouseButtonPress:
			{
				QMouseEvent *mev(static_cast<QMouseEvent *>(ev));
				QModelIndex clickedIndex;
				clickedIndex = client()->indexAt(mev->pos());
				QModelIndexList selectedIndexes(client()->selectionModel()->selectedIndexes());
				if (selectedIndexes.size() != 1 || !selectedIndexes.contains(clickedIndex)) return false;
				// If we arrive here we know we have clicked on the uniquely selected item.
				// Left button and no modifier? In any case clear the current selection, so that the
				// selection signal is emitted anyway.
				if (mev->button() == Qt::LeftButton && mev->modifiers() == Qt::NoModifier) {
					client()->selectionModel()->clear();
				}
				return false;
			}
			default:
				return false;
		}
	}
	return false;
}

void EntriesViewHelper::updateLayout()
{
	// This is needed to force a redraw - but we loose the selection.
	QAbstractItemModel *m = client()->model();
	client()->setModel(0);
	client()->setModel(m);
}

void EntriesViewHelper::updateConfig(const QVariant &value)
{
	PreferenceRoot *from = qobject_cast<PreferenceRoot *>(sender());
	if (!from) return;
	client()->setProperty(from->name().toLatin1().constData(), value);
}

QAction *EntriesViewHelper::entriesActions(QObject *parent)
{
	QAction *entriesMenuAction = new QAction(QIcon(":/images/icons/list-add.png"), "", parent);
	entriesMenuAction->setMenu(entriesMenu());
	return entriesMenuAction;
}

QToolBar *EntriesViewHelper::defaultToolBar(QWidget *parent)
{
	QToolBar *toolBar = new QToolBar(parent);
	toolBar->setAttribute(Qt::WA_MacMiniSize);
	toolBar->layout()->setContentsMargins(0, 0, 0, 0);
	toolBar->setStyleSheet("QToolBar { background: none; border-style: none; border-width: 0px; margin: 0px; padding: 0px; }");

	QAction *_entriesActions = entriesActions(toolBar);
	toolBar->addAction(_entriesActions);
	// Fix the behavior of the entries button
	QToolButton *tButton = qobject_cast<QToolButton *>(toolBar->widgetForAction(_entriesActions));
	if (tButton) tButton->setPopupMode(QToolButton::InstantPopup);

	return toolBar;
}
