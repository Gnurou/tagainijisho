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

#include "gui/EntryListModel.h"
#include "gui/EntryListView.h"

#include <QInputDialog>
#include <QSqlDatabase>
#include <QMessageBox>

EntryListView::EntryListView(QWidget *parent) : QTreeView(parent)
{
	EntryDelegateLayout *delegateLayout = new EntryDelegateLayout(EntryDelegateLayout::OneLine, "", "", "", this);
	delegate = new EntryDelegate(delegateLayout, this);
	setItemDelegate(delegate);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	scroller.activateOn(this);
}

void EntryListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	if (selected.isEmpty()) return;
	QModelIndex index(selected.indexes().last());
	QSqlQuery query;
	query.prepare("select type, id from lists where rowid = ?");
	query.addBindValue(index.internalId());
	query.exec();
	if (query.next()) {
		if (query.value(0).isNull()) emit listSelected(index.internalId());
		else {
			EntryPointer entry(EntriesCache::get(query.value(0).toInt(), query.value(1).toInt()));
			if (entry) emit entrySelected(entry);
		}
	}
}

/*
 * This part is tricky - we want the drag and drop of list items to appear as move events instead of copy, because
 * this is what actually happens when a list item is drag'n dropped. On the other hand, the action received by
 * the drop handler must be a copy, otherwise the list will try to remove the row itself (the row within the
 * database is updated, not inserted/deleted). By forcing the type of the QDragEnterEvent and QDragMoveEvent
 * (but not of the QDropEvent), we obtain the desired effect.
 */
void EntryListView::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData *mimeData = event->mimeData();
	if (mimeData->hasFormat("tagainijisho/listitem")) {
		if (event->proposedAction() == Qt::MoveAction) event->acceptProposedAction();
		else { event->setDropAction(Qt::MoveAction); event->accept(); }
	}
	QTreeView::dragEnterEvent(event);
}

void EntryListView::dragMoveEvent(QDragMoveEvent *event)
{  
	const QMimeData *mimeData = event->mimeData();
	if (mimeData->hasFormat("tagainijisho/listitem")) {
		if (event->proposedAction() == Qt::MoveAction) event->acceptProposedAction();
		else { event->setDropAction(Qt::MoveAction); event->accept(); }
	}
	QTreeView::dragMoveEvent(event);
}

// TODO Allow to create lists elsewhere than on the root
void EntryListView::newList()
{
	int idx = model()->rowCount(QModelIndex());
	if (!model()->insertRows(idx, 1, QModelIndex()))
		QMessageBox::information(this, tr("Unable to create list"), tr("A database error occured while trying to add the list."));
}

void EntryListView::deleteSelectedItems()
{
	if (QMessageBox::question(this, tr("Confirm deletion"), tr("This will delete the selected lists items and lists, including all their children. Continue?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) return;
	QModelIndexList selection(selectionModel()->selectedIndexes());
	bool success = true;
	foreach (const QModelIndex &index, selection) {
		if (!model()->removeRow(index.row(), index.parent())) success = false;
	}
	if (!success) QMessageBox::information(this, tr("Removal failed"), tr("A database error occured while trying to remove the selected items. Some of them may be remaining."));
}

