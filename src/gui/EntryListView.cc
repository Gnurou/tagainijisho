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
#include "core/EntryListModel.h"
#include "gui/EntryListView.h"

#include <QInputDialog>
#include <QMessageBox>

PreferenceItem<bool> EntryListView::smoothScrollingSetting("mainWindow/lists", "smoothScrolling", true);
PreferenceItem<QString> EntryListView::textFontSetting("mainWindow/lists", "textFont", "");
PreferenceItem<QString> EntryListView::kanaFontSetting("mainWindow/lists", "kanaFont", "");
PreferenceItem<QString> EntryListView::kanjiFontSetting("mainWindow/lists", "kanjiFont", QFont("Helvetica", 12).toString());
PreferenceItem<int> EntryListView::displayModeSetting("mainWindow/lists", "displayMode", EntryDelegateLayout::OneLine);

EntryListView::EntryListView(QWidget *parent, EntryDelegateLayout* delegateLayout, bool viewOnly) : QTreeView(parent), _helper(this, true), _newListAction(QIcon(":/images/icons/document-new.png"), tr("New list..."), 0), _rightClickNewListAction(_newListAction.icon(), _newListAction.text(), 0), _deleteSelectionAction(QIcon(":/images/icons/delete.png"), tr("Delete"), 0)
{
	// If no delegate layout has been specified, let's use our private one...
	if (!delegateLayout) delegateLayout = new EntryDelegateLayout(static_cast<EntryDelegateLayout::DisplayMode>(displayModeSetting.value()), textFontSetting.value(), kanjiFontSetting.value(), kanaFontSetting.value(), this);
	connect(delegateLayout, SIGNAL(layoutHasChanged()), this, SLOT(updateLayout()));
	_delegateLayout = delegateLayout;
	delegate = new EntryDelegate(_delegateLayout, this);
	connect(_delegateLayout, SIGNAL(layoutHasChanged()), this, SLOT(updateLayout()));
	setItemDelegate(delegate);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setSmoothScrolling(smoothScrollingSetting.value());
	// If the view is editable, the helper menu shall be enabled
	if (!viewOnly) {
		_helper.populateMenu(&contextMenu);
		contextMenu.addSeparator();
		contextMenu.addAction(&_rightClickNewListAction);
		contextMenu.addAction(&_deleteSelectionAction);
	}
	setHeaderHidden(true);
	connect(&_newListAction, SIGNAL(triggered()), this, SLOT(newList()));
	connect(&_rightClickNewListAction, SIGNAL(triggered()), this, SLOT(rightClickNewList()));
	_deleteSelectionAction.setEnabled(false);
	connect(&_deleteSelectionAction, SIGNAL(triggered()), this, SLOT(deleteSelectedItems()));

	// Automatically update the view if the configuration changes
	connect(&smoothScrollingSetting, SIGNAL(valueChanged(QVariant)), this, SLOT(updateConfig(QVariant)));
	connect(&textFontSetting, SIGNAL(valueChanged(QVariant)), delegateLayout, SLOT(updateConfig(QVariant)));
	connect(&kanaFontSetting, SIGNAL(valueChanged(QVariant)), delegateLayout, SLOT(updateConfig(QVariant)));
	connect(&kanjiFontSetting, SIGNAL(valueChanged(QVariant)), delegateLayout, SLOT(updateConfig(QVariant)));
	connect(&displayModeSetting, SIGNAL(valueChanged(QVariant)), delegateLayout, SLOT(updateConfig(QVariant)));
}

void EntryListView::setSmoothScrolling(bool value)
{
	if (value) {
		setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
		scroller.setScrollBar(verticalScrollBar());
	}
	else {
		scroller.setScrollBar(0);
		setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
	}
}

void EntryListView::updateLayout()
{
	// This is needed to force a redraw - but we loose the selection.
	QAbstractItemModel *m = model();
	setModel(0);
	setModel(m);
}

void EntryListView::contextMenuEvent(QContextMenuEvent *event)
{
	QList<EntryPointer> _selectedEntries(_helper.selectedEntries());
	// This is stupid, but const-safety forces us here
	QList<ConstEntryPointer> selectedEntries;
	foreach (const EntryPointer &entry, _selectedEntries) selectedEntries << entry;
	_helper.updateStatus(selectedEntries);
	
	// Update the status of the right-click new list action
	QModelIndexList selection(selectionModel()->selectedIndexes());
	_rightClickNewListAction.setEnabled(selection.size() <= 1 && !selection[0].data(Entry::EntryRole).isValid());
	
	contextMenu.exec(mapToGlobal(event->pos()));
}

void EntryListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	QTreeView::selectionChanged(selected, deselected);
	_deleteSelectionAction.setEnabled(!selected.isEmpty());
	emit selectionHasChanged(selected, deselected);
	if (selected.isEmpty()) {
		return;
	}
	QModelIndex index(selected.indexes().last());
	// Use the model data directly!
	EntryPointer entry(qvariant_cast<EntryPointer>(model()->data(index, Entry::EntryRole)));
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

void EntryListView::newList(const QModelIndex &parent)
{
	int idx = model()->rowCount(parent);
	if (!model()->insertRows(idx, 1, parent)) {
		QMessageBox::information(this, tr("Unable to create list"), tr("A database error occured while trying to add the list."));
		return;
	}
	QModelIndex index(model()->index(idx, 0, parent));
	if (index.isValid()) {
		setCurrentIndex(index);
		edit(index);
	}
}

void EntryListView::rightClickNewList()
{
	QModelIndexList selection(selectionModel()->selectedIndexes());
	if (selection.isEmpty()) newList();
	else if (selection.size() == 1 && !selection[0].data(Entry::EntryRole).isValid()) newList(selection[0]);
}

void EntryListView::deleteSelectedItems()
{
	if (QMessageBox::question(this, tr("Confirm deletion"), tr("This will delete the selected lists items and lists, including all their children. Continue?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) return;
	QModelIndexList selection(selectionModel()->selectedIndexes());
	QList<QPersistentModelIndex> perList;
	foreach (const QModelIndex &idx, selection) {
		perList << QPersistentModelIndex(idx);
	}
	bool success = true;
	foreach (const QPersistentModelIndex &index, perList) {
		if (!index.isValid()) continue;
		if (!model()->removeRow(index.row(), index.parent())) success = false;
	}
	if (!success) QMessageBox::information(this, tr("Removal failed"), tr("A database error has occured while trying to remove the selected items:\n\n%1\n\n Some of them may be remaining.").arg(Database::lastError().text()));
}

void EntryListView::updateConfig(const QVariant &value)
{
	PreferenceRoot *from = qobject_cast<PreferenceRoot *>(sender());
	if (!from) return;
	setProperty(from->name().toLatin1().constData(), value);
}
