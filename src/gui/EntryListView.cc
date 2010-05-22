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
#include <QToolBar>

PreferenceItem<bool> EntryListView::smoothScrollingSetting("mainWindow/lists", "smoothScrolling", true);
PreferenceItem<QString> EntryListView::textFontSetting("mainWindow/lists", "textFont", "");
PreferenceItem<QString> EntryListView::kanaFontSetting("mainWindow/lists", "kanaFont", "");
PreferenceItem<QString> EntryListView::kanjiFontSetting("mainWindow/lists", "kanjiFont", QFont("Helvetica", 12).toString());
PreferenceItem<int> EntryListView::displayModeSetting("mainWindow/lists", "displayMode", EntryDelegateLayout::OneLine);

EntryListView::EntryListView(QWidget *parent, EntryDelegateLayout* delegateLayout, bool viewOnly) : QTreeView(parent), _helper(this, delegateLayout, true, viewOnly), _newListAction(QIcon(":/images/icons/document-new.png"), tr("New list..."), 0), _rightClickNewListAction(_newListAction.icon(), _newListAction.text(), 0), _deleteSelectionAction(QIcon(":/images/icons/delete.png"), tr("Delete"), 0)
{
	setHeaderHidden(true);
	setItemDelegate(new EntryDelegate(helper()->delegateLayout(), this));

	// If the view is editable, the helper menu shall be augmented
	if (!viewOnly) {
		QMenu *contextMenu = helper()->contextMenu();
		contextMenu->addAction(&_rightClickNewListAction);
		contextMenu->addAction(&_deleteSelectionAction);
	}
	connect(&_newListAction, SIGNAL(triggered()), this, SLOT(newList()));
	connect(&_rightClickNewListAction, SIGNAL(triggered()), this, SLOT(rightClickNewList()));
	_deleteSelectionAction.setEnabled(false);
	connect(&_deleteSelectionAction, SIGNAL(triggered()), this, SLOT(deleteSelectedItems()));

	// If no delegate has been given, consider this view is ruled by the default settings
	if (!delegateLayout) {
		_helper.setPreferenceHandler(EntriesViewHelper::TextFont, &textFontSetting);
		_helper.setPreferenceHandler(EntriesViewHelper::KanaFont, &kanaFontSetting);
		_helper.setPreferenceHandler(EntriesViewHelper::KanjiFont, &kanjiFontSetting);
		_helper.setPreferenceHandler(EntriesViewHelper::DisplayMode, &displayModeSetting);
	}

	// Scrolling
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	setSmoothScrolling(smoothScrollingSetting.value());
	connect(&smoothScrollingSetting, SIGNAL(valueChanged(QVariant)), &_helper, SLOT(updateConfig(QVariant)));
	
	// Propagate the clicked signal as a selection signal
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(itemClicked(QModelIndex)));
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

/**
 * @bug The signal will be emitted twice when an unselected item is clicked - filtered by
 *      the detailed view
 */
void EntryListView::itemClicked(const QModelIndex &clicked)
{
	// Do not emit signal for entries that are not selected
	if (!selectionModel()->isSelected(clicked)) return;
	// Use the model data directly!
	EntryPointer entry(qvariant_cast<EntryPointer>(model()->data(clicked, Entry::EntryRole)));
	if (entry) emit entrySelected(entry);
}

void EntryListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	QTreeView::selectionChanged(selected, deselected);
	
	// Update the status of the right-click new list action
	QModelIndexList selection(selected.indexes());
	_rightClickNewListAction.setEnabled(selection.size() <= 1 && (selection.size() == 0 || !selection[0].data(Entry::EntryRole).isValid()));

	
	_deleteSelectionAction.setEnabled(!selected.isEmpty());
	emit selectionHasChanged(selected, deselected);
	if (selected.isEmpty()) return;
	QModelIndex index(selected.indexes().last());
	itemClicked(index);
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
