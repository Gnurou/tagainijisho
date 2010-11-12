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

EntryListView::EntryListView(QWidget *parent, EntryDelegateLayout* delegateLayout, bool viewOnly) : QTreeView(parent), _helper(this, delegateLayout, true, viewOnly), _setAsRootAction(tr("Set as root"), 0), _newListAction(QIcon(":/images/icons/document-new.png"), tr("New list..."), 0), _rightClickNewListAction(_newListAction.icon(), _newListAction.text(), 0), _deleteSelectionAction(QIcon(":/images/icons/delete.png"), tr("Delete"), 0), _goUpAction(QIcon(":/images/icons/go-up.png"), tr("Go up"), 0)
{
	setHeaderHidden(true);
	EntryDelegate *delegate = new EntryDelegate(helper()->delegateLayout(), this);
	delegate->setHidden(EntryDelegate::LISTS_ICON, true);
	setItemDelegate(delegate);

	// Add the set as root option to the context menu
	QMenu *contextMenu = helper()->contextMenu();
	QAction *firstAction = contextMenu->actions().isEmpty() ? 0 : contextMenu->actions()[0];
	contextMenu->insertAction(firstAction, &_setAsRootAction);
	// If the view is editable, the helper menu shall be augmented with edit options
	if (!viewOnly) {
		contextMenu->addAction(&_rightClickNewListAction);
		contextMenu->addAction(&_deleteSelectionAction);
	}
	_setAsRootAction.setEnabled(false);
	connect(&_setAsRootAction, SIGNAL(triggered()), this, SLOT(setSelectedAsRoot()));
	connect(&_newListAction, SIGNAL(triggered()), this, SLOT(newList()));
	connect(&_rightClickNewListAction, SIGNAL(triggered()), this, SLOT(rightClickNewList()));
	_deleteSelectionAction.setEnabled(false);
	connect(&_deleteSelectionAction, SIGNAL(triggered()), this, SLOT(deleteSelectedItems()));
	_goUpAction.setEnabled(false);
	connect(&_goUpAction, SIGNAL(triggered()), this, SLOT(goUp()));

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
	QAbstractItemModel *oldModel = model();
	if (oldModel && oldModel->metaObject()->indexOfSignal(QMetaObject::normalizedSignature(SIGNAL(rootHasChanged(int))).constData() + 1) != -1) disconnect(oldModel, SIGNAL(rootHasChanged(int)), this, SLOT(onModelRootChanged(int)));
	QTreeView::setModel(newModel);
	_setAsRootAction.setEnabled(false);
	_goUpAction.setEnabled(false);
	if (newModel && newModel->metaObject()->indexOfSignal(QMetaObject::normalizedSignature(SIGNAL(rootHasChanged(int))).constData() + 1) != -1) connect(newModel, SIGNAL(rootHasChanged(int)), this, SLOT(onModelRootChanged(int)));
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
	_setAsRootAction.setEnabled(selection.size() == 1 && !selection[0].data(Entry::EntryRole).isValid());
	
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

void EntryListView::newList(const QModelIndex &parent)
{
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
	if (!success) QMessageBox::information(this, tr("Removal failed"), tr("A database error has occured while trying to remove the selected items:\n\n%1\n\n Some of them may be remaining.").arg(Database::lastError().message()));
}

void EntryListView::setSelectedAsRoot()
{
	QModelIndexList selection(selectionModel()->selectedIndexes());
	if (!selection.isEmpty() && selection.size() == 1) {
		EntryListModel *myModel = qobject_cast<EntryListModel *>(model());
		if (!myModel) return;
		int id = selection[0].internalId();
		myModel->setRoot(id);
	}
}

void EntryListView::goUp()
{
	EntryListModel *myModel = qobject_cast<EntryListModel *>(model());
	if (!myModel || myModel->rootId() == 0) return;

	//QModelIndex idx(myModel->index(myModel->rootId()));
	//QModelIndex parent(myModel->realParent(idx));
	//int parentId = parent.isValid() ? parent.internalId() : 0;
	
	//myModel->setRoot(parentId);
}

void EntryListView::onModelRootChanged(int rootId)
{
	_setAsRootAction.setEnabled(false);
	_goUpAction.setEnabled(rootId != 0);
	emit rootHasChanged(rootId);
}
