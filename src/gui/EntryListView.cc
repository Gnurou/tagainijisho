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
#include "core/EntryListCache.h"
#include "gui/EntryListView.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QToolBar>
#include <QDrag>

PreferenceItem<bool> EntryListView::smoothScrollingSetting("mainWindow/lists", "smoothScrolling", true);
PreferenceItem<QString> EntryListView::textFontSetting("mainWindow/lists", "textFont", "");
PreferenceItem<QString> EntryListView::kanaFontSetting("mainWindow/lists", "kanaFont", "");
PreferenceItem<QString> EntryListView::kanjiFontSetting("mainWindow/lists", "kanjiFont", QFont("Helvetica", 15).toString());
PreferenceItem<int> EntryListView::displayModeSetting("mainWindow/lists", "displayMode", EntryDelegateLayout::OneLine);

EntryListView::EntryListView(QWidget *parent, EntryDelegateLayout* delegateLayout, bool viewOnly) : QTreeView(parent), _helper(this, delegateLayout, true, viewOnly), _newListAction(QIcon(":/images/icons/document-new.png"), tr("New list..."), 0), _rightClickNewListAction(_newListAction.icon(), _newListAction.text(), 0), _deleteSelectionAction(QIcon(":/images/icons/delete.png"), tr("Delete"), 0), _renameListAction(QIcon(), tr("Rename list..."), 0), _goUpAction(QIcon(":/images/icons/go-up.png"), tr("Go up"), 0)
{
	setHeaderHidden(true);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	EntryDelegate *delegate = new EntryDelegate(helper()->delegateLayout(), this);
	delegate->setHidden(EntryDelegate::LISTS_ICON, true);
	setItemDelegate(delegate);

	// Add actions to the context menu
	QMenu *contextMenu = helper()->contextMenu();
	// If the view is editable, the helper menu shall be augmented with edit options
	if (!viewOnly) {
		contextMenu->addAction(&_rightClickNewListAction);
		contextMenu->addAction(&_renameListAction);
		contextMenu->addAction(&_deleteSelectionAction);
	}
	connect(&_newListAction, SIGNAL(triggered()), this, SLOT(newList()));
	connect(&_rightClickNewListAction, SIGNAL(triggered()), this, SLOT(rightClickNewList()));
	_deleteSelectionAction.setEnabled(false);
	connect(&_deleteSelectionAction, SIGNAL(triggered()), this, SLOT(deleteSelectedItems()));
	_goUpAction.setEnabled(false);
	connect(&_renameListAction, SIGNAL(triggered()), this, SLOT(editSelectedList()));
	connect(&_goUpAction, SIGNAL(triggered()), this, SLOT(goUp()));
	connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(setRootIndex(QModelIndex)));

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
}

void EntryListView::setModel(QAbstractItemModel *newModel)
{
	QTreeView::setModel(newModel);
	_goUpAction.setEnabled(false);
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

void EntryListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	QTreeView::selectionChanged(selected, deselected);
	
	// Update the status of the right-click new list action
	QModelIndexList selection(selected.indexes());
	_rightClickNewListAction.setEnabled(selection.size() <= 1 && (selection.size() == 0 || !selection[0].data(Entry::EntryRole).isValid()));
	_renameListAction.setEnabled(selection.size() == 1 && !selection[0].data(Entry::EntryRole).isValid());
	
	_deleteSelectionAction.setEnabled(!selected.isEmpty());
	emit selectionHasChanged(selected, deselected);
	if (selected.isEmpty()) return;
	QModelIndex index(selected.indexes().last());
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

void EntryListView::newList(QModelIndex parent)
{
	if (parent == QModelIndex()) {
		parent = rootIndex();
	}
	int idx = model()->rowCount(parent);
	if (!model()->insertRows(idx, 1, parent)) {
		QMessageBox::information(this, tr("Unable to create list"), tr("A database error occured while trying to add the list."));
		return;
	}
	QModelIndex index(model()->index(idx, 0, parent));
	if (index.isValid()) {
		setExpanded(index, true);
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

bool EntryListView::deleteEntries(const QModelIndex &index)
{
	if (!index.isValid()) return true;
	EntryListModel *m(qobject_cast<EntryListModel *>(model()));
	int nRows = m->rowCount(index);
	// Delete all children
	while (nRows--)
		if (!deleteEntries(index.child(0, 0))) return false;

	// Remove ourselves from the model
	EntryRef ref(index.data(Entry::EntryRefRole).value<EntryRef>());
	if (ref.isLoaded()) {
		EntryPointer e(ref.get());
		e->removeFromList(m->rowIdFromIndex(index));
	}

	return model()->removeRow(index.row(), index.parent());
}

void EntryListView::deleteSelectedItems()
{
	if (QMessageBox::question(this, tr("Confirm deletion"), tr("This will delete the selected lists items and lists, including all their children. Continue?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) return;
	QModelIndexList selection(selectionModel()->selectedIndexes());

	// Build a persistent list of all the indexes to remove as we are going to mess with their order
	QList<QPersistentModelIndex> perList;
	foreach (const QModelIndex &idx, selection) {
		perList << QPersistentModelIndex(idx);
	}

	// TODO progress bar
	if (!EntryListCache::connection()->transaction()) goto failure_1;
	foreach (const QPersistentModelIndex &index, perList) {
		if (!deleteEntries(index)) goto failure_2;
	}
	if (!EntryListCache::connection()->commit()) goto failure_2;

	return;

failure_2:
	EntryListCache::connection()->rollback();
failure_1:
	QMessageBox::information(this, tr("Removal failed"), tr("A database error has occured while trying to remove the selected items:\n\n%1\n\n Some of them may be remaining.").arg(Database::lastError().message()));
}

void EntryListView::goUp()
{
	setRootIndex(rootIndex().parent());
}

void EntryListView::setRootIndex(const QModelIndex &idx)
{
	// Entries can not become roots
	if (idx.data(Entry::EntryRefRole).isValid()) return;
	// Necessary to avoid a Qt bug in QTreeView::scrollContentsBy that
	// requests the size of row 0, resulting in a bug if we are opening
	// an empty list while the view is scrolled down.
	scrollToTop();
	clearSelection();
	QTreeView::setRootIndex(idx);
	_goUpAction.setEnabled(idx.isValid());
	emit rootHasChanged(idx);
}

void EntryListView::editSelectedList()
{
	QModelIndexList selection(selectionModel()->selectedIndexes());
	if (selection.size() == 1) {
		QModelIndex idx(selection[0]);
		if (!idx.data(Entry::EntryRole).isValid()) edit(idx);
	}
}
