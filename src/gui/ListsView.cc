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

#include "core/EntriesCache.h"
#include "gui/ListsView.h"

#include <QInputDialog>
#include <QSqlQuery>
#include <QSqlError>

#define EXEC(q) if (!q.exec()) { qDebug() << __FILE__ << __LINE__ << "Cannot execute query:" << q.lastError().text(); return false; }

// TODO Drag'n drop within the list - by implementing new MIME type and/or using move actions
// TODO Allow edition of lists names
// TODO Allow creation of new lists
// TODO Deletion of lists and items

// TODO the model should cache rowid, type, id and label into the indexes using a dedicated structure

QModelIndex EntryListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (column > 0) return QModelIndex();
	
	QSqlQuery query;
	if (!parent.isValid()) {
		query.prepare("select rowid from lists where parent is null and position = ?");
		query.addBindValue(row);
	}
	else {
		query.prepare("select rowid from lists where parent = ? and position = ?");
		quint64 parentRow = parent.internalId();
		query.addBindValue(parentRow);  
		query.addBindValue(row);  
	}
	query.exec();
	if (query.next()) return createIndex(row, column, query.value(0).toInt());
	else return QModelIndex();
	
}
	
QModelIndex EntryListModel::parent(const QModelIndex &index) const
{
	if (!index.isValid()) return QModelIndex();
	QSqlQuery query;
	query.prepare("select lists.parent, t2.position from lists join lists as t2 on lists.parent = t2.rowid where lists.rowid = ?");
	query.addBindValue(index.internalId());
	query.exec();
	if (query.next()) return createIndex(query.value(1).toInt(), 0, query.value(0).toInt());
	else return QModelIndex();
}

int EntryListModel::rowCount(const QModelIndex &parent) const
{
	QSqlQuery query;
	if (!parent.isValid()) {
		query.prepare("select count(*) from lists where parent is null");
	} else {
		query.prepare("select count(*) from lists where parent = ?");
		query.addBindValue(parent.internalId());
	}
	query.exec();
	if (query.next()) return query.value(0).toInt();
	return -1;
}
	
QVariant EntryListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) return QVariant();
	if (role == Qt::DisplayRole) {
		if (index.column() != 0) return QVariant();
		QSqlQuery query;
		query.prepare("select type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.rowid = ?");
		query.addBindValue(index.internalId());
		query.exec();
		if (query.next()) {
			if (query.value(0).isNull()) return query.value(2).toString();
			else {
				EntryPointer<Entry> entry(EntriesCache::get(query.value(0).toInt(), query.value(1).toInt()));
				if (!entry.data()) return QVariant();
				else return entry->shortVersion(Entry::TinyVersion);
			}
		}
	}
	return QVariant();
}

/*
bool EntryListModel::insertRows(int row, int count, const QModelIndex & parent = QModelIndex())
{
	beginInsertRows(parent, row, row + count -1);
	QSqlQuery query;
	QString queryText("update lists set position = position + co!!!!unt where parent %1 and position >= ?");
	if (!parent.isValid()) {
		query.prepare(queryText.arg("is null"));
	} else {
		query.prepare(queryText.arg("= ?"));
		query.addBindValue(parent.internalId());
	}
	query.addBindValue(row);
	EXEC(q);
	endInsertRows();
	return true;
}*/

bool EntryListModel::removeRows(int row, int count, const QModelIndex &parent)
{
	// TODO use a transaction!
	beginRemoveRows(parent, row, row + count - 1);
	QSqlQuery query;
	// Remove requested rows from the database
	QString queryString("delete from lists where parent %1 and position between ? and ?");
	if (!parent.isValid()) query.prepare(queryString.arg("is null"));
	else {
		query.prepare(queryString.arg("= ?"));
		query.addBindValue(parent.internalId());
	}
	query.addBindValue(row);
	query.addBindValue(row + count - 1);
	EXEC(query);
	// Update the positions of items that were after the ones we removed
	queryString = "update lists set position = position - ? where parent %1 and position >= ?";
	if (!parent.isValid()) query.prepare(queryString.arg("is null"));
	else query.prepare(queryString.arg("= ?"));
	query.addBindValue(count);
	if (!parent.isValid()) query.addBindValue(parent.internalId());
	query.addBindValue(row + count);
	EXEC(query);
	endRemoveRows();
	return true;
}
	
Qt::ItemFlags EntryListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags ret(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
	
	QSqlQuery query;
	query.prepare("select type from lists where rowid = ?");
	query.addBindValue(index.internalId());
	query.exec();
	if (query.next() && query.value(0).isNull()) ret |= Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
	
	return ret;
}

bool EntryListModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	if (action == Qt::IgnoreAction) return true;
	if (column > 0) return false;
	
	if (data->hasFormat("tagainijisho/entry")) {
		if (!parent.isValid()) return false;
		{
			QSqlQuery query;
			query.prepare("select type from lists where rowid = ?");
			query.addBindValue(parent.internalId());
			query.exec();
			if (!query.next() && !query.value(0).isNull()) return false;
		}
		QByteArray ba = data->data("tagainijisho/entry");
		QDataStream ds(&ba, QIODevice::ReadOnly);
		QList<QPair<int, int> > entries;
		while (!ds.atEnd()) {
			int type, id;
			ds >> type >> id;
			entries << QPair<int, int>(type, id);
		}
		beginInsertRows(parent, row, row + entries.size() - 1);
		// TODO use a transaction!
		QSqlQuery query;
		query.prepare("update lists set position = position + ? where parent = ? and position >= ?");
		query.addBindValue(entries.size());
		query.addBindValue(parent.internalId());
		query.addBindValue(row);
		EXEC(query);
		QStringList vals;
		for (int i = 0; i < entries.size(); ++i) vals << QString("%1, %2, %3, %4").arg(parent.internalId()).arg(row + i).arg(entries[i].first).arg(entries[i].second);
		query.prepare(QString("insert into lists values(%1)").arg(vals.join(", ")));
		EXEC(query);
		endInsertRows();
	}
	
	return false;
}

QStringList EntryListModel::mimeTypes() const
{
	QStringList ret;
	ret << "tagainijisho/entry";
	return ret;
}

QMimeData *EntryListModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	
	foreach (const QModelIndex &index, indexes) {
		if (index.isValid()) {
			QSqlQuery query;
			query.prepare("select type, id from lists where rowid = ?");
			query.addBindValue(index.internalId());
			query.exec();
			if (!query.next() || query.value(0).isNull()) continue;
			int type(query.value(0).toInt());
			int id(query.value(1).toInt());
			stream << type << id;
		}
	}
	mimeData->setData("tagainijisho/entry", encodedData);
	return mimeData;
}


EntryListView::EntryListView(QWidget *parent) : QTreeView(parent)
{
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
			EntryPointer<Entry> entry(EntriesCache::get(query.value(0).toInt(), query.value(1).toInt()));
			if (entry.data()) emit entrySelected(entry);
		}
	}
}














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
	return true;
}

QStringList ListsView::mimeTypes () const
{
	QStringList types;
	types << "tagainijisho/entry";
	types << "tagainijisho/listviewitems";
	return types;
}

QMimeData *ListsView::mimeData(const QList<QTreeWidgetItem *> items) const
{
	QByteArray entryEncodedData, listItemEncodedData;

	QDataStream entryStream(&entryEncodedData, QIODevice::WriteOnly);
	QDataStream listItemStream(&listItemEncodedData, QIODevice::WriteOnly);

	foreach (const QTreeWidgetItem * it, items) {
		const ListTreeItem *item = static_cast<const ListTreeItem *>(it);
		if (!item) continue;
		if (item->type() == ListTreeItem::EntryType) {
			EntryPointer<Entry> entry(EntriesCache::get(item->entryType(), item->entryId()));
			if (!entry.data()) continue;
			entryStream << entry->type() << entry->id();
			
			listItemStream << (quint64)item;
		} else {
			// TODO encode data for lists items
		}
	}

	QMimeData *mimeData = new QMimeData();
	mimeData->setData("tagainijisho/entry", entryEncodedData);
	mimeData->setData("tagainijisho/listviewitems", listItemEncodedData);
	return mimeData;
}

bool ListsView::dropMimeData(QTreeWidgetItem *_parent, int index, const QMimeData *data, Qt::DropAction action)
{
	ListTreeItem *parent = static_cast<ListTreeItem *>(_parent);

	// Data dropped from the list entry can only be handled if we have a parent
	if (parent) {
		// First check if we have been sent list view items - in this case this
		// means we have to move then
		if (data->hasFormat("tagainijisho/listviewitems")) {
			QByteArray ba = data->data("tagainijisho/listviewitems");
			QDataStream ds(&ba, QIODevice::ReadOnly);
			while (!ds.atEnd()) {
				// Ouch! This is so bad...
				quint64 _ptr;
				ListTreeItem *item;
				ds >> _ptr;
				item = (ListTreeItem *)_ptr;
				
				if (!item) continue;
				int parentRow = parent ? parent->rowId() : -1;
				int prevPos = parent ? indexOfTopLevelItem(item) : parent->indexOfChild(item);
				// Update the positions of the items that are after our previous position
				// one we are moving
				// TODO Embed that into a transaction!
				QSqlQuery query;
				query.prepare(QString("update lists set position = position - 1 where parent %1 and position > ?").arg(parentRow == -1 ? "is null" : "= ?"));
				if (parentRow != -1) query.addBindValue(parentRow);
				query.addBindValue(prevPos);
				EXEC(query);
				// Update the positions of the items that are after our new position
				query.prepare(QString("update lists set position = position + 1 where parent %1 and position >= ?").arg(parentRow == -1 ? "is null" : "= ?"));
				if (parentRow != -1) query.addBindValue(parentRow);
				query.addBindValue(index);
				EXEC(query);
				// Finally update our position
				query.prepare("update lists set position = ? where rowid = ?");
				query.addBindValue(index);
				query.addBindValue(item->rowId());
				EXEC(query);
				// And update our model
				//if (!parent) move
			}
		}
		// Just regular entry items - we can add them directly
		else if (data->hasFormat("tagainijisho/entry")) {
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
