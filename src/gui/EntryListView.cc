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
#include <QSqlError>
#include <QMessageBox>

PreferenceItem<bool> EntryListView::smoothScrolling("mainWindow/lists", "smoothScrolling", true);
PreferenceItem<QString> EntryListView::textFont("mainWindow/lists", "textFont", "");
PreferenceItem<QString> EntryListView::kanaFont("mainWindow/lists", "kanaFont", "");
PreferenceItem<QString> EntryListView::kanjiFont("mainWindow/lists", "kanjiFont", QFont("Helvetica", 12).toString());
PreferenceItem<int> EntryListView::displayMode("mainWindow/lists", "displayMode", EntryDelegateLayout::OneLine);

EntryListView::EntryListView(QWidget *parent) : QTreeView(parent)
{
	EntryDelegateLayout *delegateLayout = new EntryDelegateLayout(static_cast<EntryDelegateLayout::DisplayMode>(displayMode.value()), textFont.value(), kanjiFont.value(), kanaFont.value(), this);
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
	if (!success) QMessageBox::information(this, tr("Removal failed"), tr("A database error has occured while trying to remove the selected items:\n\n%1\n\n Some of them may be remaining.").arg(QSqlDatabase::database().lastError().text()));
}

