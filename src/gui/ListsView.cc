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

#include "gui/ListsView.h"

#include <QInputDialog>
#include <QSqlQuery>
#include <QSqlError>

ListTreeItem::ListTreeItem(int rowId, const QString &label) : QTreeWidgetItem(ListType), _rowId(rowId), _type(-1), _id(-1)
{
	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
	setFlags(flags);
	setText(0, label);
	
	// TODO should rather be done incrementally, when the expand button is clicked
	populate();
}

ListTreeItem::ListTreeItem(int rowId, const Entry *entry) : QTreeWidgetItem(EntryType), _rowId(rowId), _type(entry->type()), _id(entry->id())
{
	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
	setFlags(flags);
	setText(0, entry->shortVersion());
}

void ListTreeItem::populate() {
	if (type() == ListType) {
		QSqlQuery query;
		query.prepare("select lists.rowid, type, id, label from lists left join listsLabels on lists.rowid = listsLabels.rowid where parent = ? order by position");
		query.addBindValue(rowId());
		query.exec();
		while (query.next()) {
			if (query.value(1).isNull()) insertChild(childCount(), new ListTreeItem(query.value(0).toInt(), query.value(3).toString()));
			else {
				EntryPointer<Entry> entry(EntriesCache::get(query.value(1).toInt(), query.value(2).toInt()));
				if (entry.data() == 0) continue;
				insertChild(childCount(), new ListTreeItem(query.value(0).toInt(), entry.data()));
			}
		}
	}
}

// TODO: handle double-click to change the list name
ListsView::ListsView(QWidget *parent) : QTreeWidget(parent)
{
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
	populateRoot();
}

bool ListsView::populateRoot()
{
	QSqlQuery query;
	query.exec("select lists.rowid, type, id, label from lists left join listsLabels on lists.rowid = listsLabels.rowid where parent is null order by position");
	while (query.next()) {
		ListTreeItem *item;
		if (query.value(1).isNull()) item = new ListTreeItem(query.value(0).toInt(), query.value(3).toString());
		else {
			EntryPointer<Entry> entry(EntriesCache::get(query.value(1).toInt(), query.value(2).toInt()));
			if (entry.data() == 0) continue;
			item = new ListTreeItem(query.value(0).toInt(), entry.data());
		}
		addTopLevelItem(item);
	}
}

Qt::DropActions ListsView::supportedDropActions() const
{
	return Qt::CopyAction;
}

QStringList ListsView::mimeTypes () const
{
	QStringList types;
	types << "tagainijisho/entry";
	return types;
}

QMimeData *ListsView::mimeData(const QList<QTreeWidgetItem *> items) const
{
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	foreach (const QTreeWidgetItem * it, items) {
		const ListTreeItem *item = static_cast<const ListTreeItem *>(it);
		if (!item) continue;
		if (item->type() == ListTreeItem::EntryType) {
			EntryPointer<Entry> entry(EntriesCache::get(item->entryType(), item->entryId()));
			if (!entry.data()) continue;
			stream << entry->type() << entry->id();
		} else {
			// TODO encode data for lists items
		}
	}

	mimeData->setData("tagainijisho/entry", encodedData);
	return mimeData;
}

bool ListsView::dropMimeData(QTreeWidgetItem *_parent, int index, const QMimeData *data, Qt::DropAction action)
{
	ListTreeItem *parent = static_cast<ListTreeItem *>(_parent);

	// Data dropped from the list entry can only be handled if we have a parent
	if (parent) {
		QByteArray ba = data->data("tagainijisho/entry");
		QDataStream ds(&ba, QIODevice::ReadOnly);
		while (!ds.atEnd()) {
			int _t, _id;
			ds >> _t;
			ds >> _id;

			EntryPointer<Entry> entry(EntriesCache::get(_t, _id));
			if (entry.data() == 0) continue;
			
			QSqlQuery query;
			// TODO use a transaction here!
			// TODO Update the positions of elements that are below us
			
			// Update the positions of elements that have been moved by our drop
			query.prepare("update lists set position = position + 1 where parent = ? and position >= ?");
			query.addBindValue(parent->rowId());
			query.addBindValue(index);
			if (!query.exec()) {
				qDebug() << "Cannot insert entry into list:" << query.lastError().text();
				continue;
			}
			// Insert our element
			query.prepare("insert into lists values(?, ?, ?, ?)");
			query.addBindValue(parent->rowId());
			query.addBindValue(index);
			query.addBindValue(_t);
			query.addBindValue(_id);
			if (!query.exec()) {
				qDebug() << "Cannot insert entry into list:" << query.lastError().text();
				continue;
			}
			int rowId = query.lastInsertId().toInt();
			
			// Finally add the entry to our model
			parent->insertChild(index, new ListTreeItem(rowId, entry.data()));
			parent->setExpanded(true);
		}
	}
	
	// TODO handle the case where we move lists and entries
	return true;
}

void ListsView::newList()
{
	QString label(QInputDialog::getText(this, tr("New list..."), tr("Please enter the name of the new list:")));
	if (label.isEmpty()) return;
	QSqlQuery query;
	ListTreeItem *parent = 0;
	QList<QTreeWidgetItem *> selection(selectedItems());
	if (!selection.isEmpty()) {
		parent = static_cast<ListTreeItem *>(selection[0]);
		if (parent->type() != ListTreeItem::ListType) parent = 0;
	}
	query.prepare("insert into lists values(?, ?, NULL, NULL)");
	// Add the parent and position arguments
	query.addBindValue(!parent ? QVariant() : parent->rowId());
	query.addBindValue(!parent ? topLevelItemCount() : parent->childCount());
	// TODO do that with a transaction
	if (!query.exec()) {
		qDebug() << "Cannot create list:" << query.lastError().text();
		return;
	}
	int rowId(query.lastInsertId().toInt());
	query.prepare("insert into listsLabels(docid, label) values(?, ?)");
	query.addBindValue(rowId);
	query.addBindValue(label);
	if (!query.exec()) {
		qDebug() << "Cannot create list:" << query.lastError().text();
		return;
	}
	// Now insert the item into the view
	ListTreeItem *item = new ListTreeItem(rowId, label);
	if (!parent) addTopLevelItem(item);
	else {
		parent->addChild(item);
		parent->setExpanded(true);
	}
}

void ListsView::deleteCurrentItem()
{
}

void ListsView::onItemSelectionChanged()
{
	QList<QTreeWidgetItem *> selection(selectedItems());
	if (selection.isEmpty()) return;
	ListTreeItem *item = static_cast<ListTreeItem *>(selection[0]);
	if (item->type() == ListTreeItem::ListType) emit listSelected(item->rowId());
	else {
		EntryPointer<Entry> entry(EntriesCache::get(item->entryType(), item->entryId()));
		if (entry.data()) emit entrySelected(entry);
	}
}
