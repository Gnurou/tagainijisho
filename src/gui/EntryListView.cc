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

#include <QtDebug>

#include "core/Database.h"
#include "gui/EntryListModel.h"
#include "gui/EntryListView.h"

#include <QInputDialog>
#include <QMessageBox>

PreferenceItem<bool> EntryListView::smoothScrolling("mainWindow/lists", "smoothScrolling", true);
PreferenceItem<QString> EntryListView::textFont("mainWindow/lists", "textFont", "");
PreferenceItem<QString> EntryListView::kanaFont("mainWindow/lists", "kanaFont", "");
PreferenceItem<QString> EntryListView::kanjiFont("mainWindow/lists", "kanjiFont", QFont("Helvetica", 12).toString());
PreferenceItem<int> EntryListView::displayMode("mainWindow/lists", "displayMode", EntryDelegateLayout::OneLine);

EntryListView::EntryListView(QWidget *parent) : QTreeView(parent), helper(this), _newListAction(QIcon(":/images/icons/document-new.png"), tr("New list"), 0), _deleteSelectionAction(QIcon(":/images/icons/delete.png"), tr("Delete"), 0)
{
	EntryDelegateLayout *delegateLayout = new EntryDelegateLayout(static_cast<EntryDelegateLayout::DisplayMode>(displayMode.value()), textFont.value(), kanjiFont.value(), kanaFont.value(), this);
	delegate = new EntryDelegate(delegateLayout, this);
	setItemDelegate(delegate);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	scroller.activateOn(this);
	helper.populateMenu(&contextMenu);
	connect(&_newListAction, SIGNAL(triggered()), this, SLOT(newList()));
	connect(&_deleteSelectionAction, SIGNAL(triggered()), this, SLOT(deleteSelectedItems()));
}

void EntryListView::contextMenuEvent(QContextMenuEvent *event)
{
	QList<EntryPointer> _selectedEntries(helper.selectedEntries());
	// This is stupid, but const-safety forces us here
	QList<ConstEntryPointer> selectedEntries;
	foreach (const EntryPointer &entry, _selectedEntries) selectedEntries << entry;
	helper.updateStatus(selectedEntries);
	contextMenu.exec(mapToGlobal(event->pos()));
}

void EntryListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	QTreeView::selectionChanged(selected, deselected);
	if (selected.isEmpty()) return;
	QModelIndex index(selected.indexes().last());
	// Use the model data directly!
	EntryPointer entry(qvariant_cast<EntryPointer>(model()->data(index, ResultsList::EntryRole)));
	if (entry) emit entrySelected(entry);
}

void EntryListView::startDrag(Qt::DropActions supportedActions)
{
	QModelIndexList indexes = selectedIndexes();
	if (indexes.count() > 0) {
		QMimeData *data(model()->mimeData(indexes));
		if (!data) return;
		QDrag *drag = new QDrag(this);
		drag->setMimeData(data);
		drag->exec(supportedActions, Qt::MoveAction);
	}
}

// TODO Allow to create lists elsewhere than on the root
void EntryListView::newList()
{
	int idx = model()->rowCount(QModelIndex());
	if (!model()->insertRows(idx, 1, QModelIndex())) {
		QMessageBox::information(this, tr("Unable to create list"), tr("A database error occured while trying to add the list."));
		return;
	}
	QModelIndex index(model()->index(idx, 0, QModelIndex()));
	if (index.isValid()) {
		setCurrentIndex(index);
		edit(index);
	}
}

// TODO won't work if parent and children are selected
// BUG totally buggy in case of multiple selection - that's
// because the indexes change as they are deleted!
void EntryListView::deleteSelectedItems()
{
	if (QMessageBox::question(this, tr("Confirm deletion"), tr("This will delete the selected lists items and lists, including all their children. Continue?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) return;
	QModelIndexList selection(selectionModel()->selectedIndexes());
	bool success = true;
	foreach (const QModelIndex &index, selection) {
		if (!model()->removeRow(index.row(), index.parent())) success = false;
	}
	if (!success) QMessageBox::information(this, tr("Removal failed"), tr("A database error has occured while trying to remove the selected items:\n\n%1\n\n Some of them may be remaining.").arg(Database::lastError().text()));
}

