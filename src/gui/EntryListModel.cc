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

#include "gui/EntryListModel.h"

#include <QSqlError>

#define TRANSACTION QSqlDatabase::database().transaction()
#define ROLLBACK QSqlDatabase::database().rollback()
#define COMMIT QSqlDatabase::database().commit()

#define EXEC(q) if (!q.exec()) { qDebug() << __FILE__ << __LINE__ << "Cannot execute query:" << q.lastError().text(); return false; }
#define EXEC_T(q) if (!q.exec()) { qDebug() << __FILE__ << __LINE__ << "Cannot execute query:" << q.lastError().text(); goto transactionFailed; }

const EntryListModelCache &EntryListModel::getFromCache(int rowId) const
{
	if (!rowIdCache.contains(rowId)) {
		QSqlQuery query;
		query.prepare("select lists.rowid, parent, position, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.rowid = ?");
		query.addBindValue(rowId);
		query.exec();
		EntryListModelCache nCache;
		// Invalid model
		if (!query.next()) {
			nCache.rowId = -1;
			nCache.parent = -1;
			nCache.position = -1;
			nCache.type = -1;
			nCache.id = -1;
		} else {
			nCache.rowId = query.value(0).toInt();
			nCache.parent = query.value(1).isNull() ? -1 : query.value(1).toInt();
			nCache.position = query.value(2).toInt();
			nCache.type = query.value(3).isNull() ? -1 : query.value(3).toInt();
			nCache.id = query.value(4).toInt();
			nCache.label = query.value(5).toString();
		}
		rowIdCache[rowId] = nCache;
		rowParentCache[QPair<int, int>(nCache.position, nCache.parent)] = nCache;
	}
	return rowIdCache[rowId];
}

const EntryListModelCache &EntryListModel::getFromCache(int row, int parent) const
{
	QPair<int, int> key(row, parent);
	if (!rowParentCache.contains(key)) {
		QSqlQuery query;
		query.prepare(QString("select lists.rowid, parent, position, type, id, label from lists left join listsLabels on lists.rowid == listsLabels.rowid where lists.position = ? and lists.parent %1").arg(parent == -1 ? "is null" : "= ?"));
		query.addBindValue(row);
		if (parent != -1) query.addBindValue(parent);
		query.exec();
		EntryListModelCache nCache;
		// Invalid model
		if (!query.next()) {
			nCache.rowId = -1;
			nCache.parent = -1;
			nCache.position = -1;
			nCache.type = -1;
			nCache.id = -1;
		} else {
			nCache.rowId = query.value(0).toInt();
			nCache.parent = query.value(1).isNull() ? -1 : query.value(1).toInt();
			nCache.position = query.value(2).toInt();
			nCache.type = query.value(3).isNull() ? -1 : query.value(3).toInt();
			nCache.id = query.value(4).toInt();
			nCache.label = query.value(5).toString();
		}
		rowIdCache[nCache.rowId] = nCache;
		rowParentCache[key] = nCache;
	}
	return rowParentCache[key];
}

void EntryListModel::invalidateCache(int rowId) const
{
	if (rowIdCache.contains(rowId));
	const EntryListModelCache &cEntry = rowIdCache[rowId];
	rowParentCache.remove(QPair<int, int>(cEntry.position, cEntry.parent));
	rowIdCache.remove(rowId);
}

void EntryListModel::invalidateCache() const
{
	rowParentCache.clear();
	rowIdCache.clear();
}

QModelIndex EntryListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (column > 0) return QModelIndex();
	const EntryListModelCache &cEntry = getFromCache(row, parent.isValid() ? parent.internalId() : -1);
	if (cEntry.rowId == -1) return QModelIndex();
	else return createIndex(row, column, cEntry.rowId);
}
	
QModelIndex EntryListModel::index(int rowId) const
{
	// Invalid items have no parent
	if (rowId == -1) return QModelIndex();
	const EntryListModelCache &cEntry = getFromCache(rowId);
	if (cEntry.rowId == -1) return QModelIndex();
	return createIndex(cEntry.position, 0, rowId);
}

QModelIndex EntryListModel::parent(const QModelIndex &idx) const
{
	if (!idx.isValid()) return QModelIndex();
	const EntryListModelCache &cEntry = getFromCache(idx.internalId());
	if (cEntry.rowId == -1) return QModelIndex();
	else return index(cEntry.parent);
}

// TODO not cached!
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
		const EntryListModelCache &cEntry = getFromCache(index.internalId());
		if (cEntry.rowId == -1) return QVariant();
		if (cEntry.type == -1) return cEntry.label;
		else {
			EntryPointer<Entry> entry(EntriesCache::get(cEntry.type, cEntry.id));
			if (!entry.data()) return QVariant();
			else return entry->shortVersion(Entry::TinyVersion);
		}
	}
	return QVariant();
}

bool EntryListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::EditRole && index.isValid()) {
		if (!TRANSACTION) return false;
		{
			QSqlQuery query;
			query.prepare("select type from lists where rowid = ?");
			query.addBindValue(index.internalId());
			query.exec();
			if (query.next() && query.value(0).isNull()) {
				query.prepare("update listsLabels set label = ? where rowid = ?");
				query.addBindValue(value.toString());
				query.addBindValue(index.internalId());
				EXEC_T(query);
			}
		}
		if (!COMMIT) goto transactionFailed;
		// Invalidate the cache entry
		invalidateCache(index.internalId());
		return true;
	}
	return false;
transactionFailed:
	ROLLBACK;
	return false;
}
	
bool EntryListModel::moveRows(int row, int delta, const QModelIndex &parent, QSqlQuery &query)
{
	QString queryText("update lists set position = position + ? where parent %1 and position >= ?");
	if (!parent.isValid()) query.prepare(queryText.arg("is null"));
	else query.prepare(queryText.arg("= ?"));
	query.addBindValue(delta);
	if (parent.isValid()) query.addBindValue(parent.internalId());
	query.addBindValue(row);
	EXEC(query);
	return true;
}

// TODO How to handle the beginInsertRows if the transaction failed and endInsertRows is not called?
bool EntryListModel::insertRows(int row, int count, const QModelIndex & parent)
{
	if (!TRANSACTION) return false;
	{
		QSqlQuery query;
		// First update the positions of items located after the new lists
		if (!moveRows(row, count, parent, query)) goto transactionFailed;
		// Then insert the lists themselves with a default name
		for (int i = 0; i < count; i++) {
			query.prepare("insert into lists values(?, ?, NULL, NULL)");
			query.addBindValue(parent.isValid() ? parent.internalId() : QVariant(QVariant::Int));
			query.addBindValue(row + i);
			EXEC_T(query);
			int rowId = query.lastInsertId().toInt();
			query.prepare("insert into listsLabels(docid, label) values(?, ?)");
			query.addBindValue(rowId);
			query.addBindValue(tr("New list"));
			EXEC_T(query);
		}
	}
	beginInsertRows(parent, row, row + count -1);
	if (!COMMIT) goto transactionFailed;
	invalidateCache();
	endInsertRows();
	return true;
transactionFailed:
	ROLLBACK;
	return false;
}

bool EntryListModel::_removeRows(int row, int count, const QModelIndex &parent)
{
	QSqlQuery query;
	// Get the list of items to remove
	QString queryString("select rowid from lists where parent %1 and position between ? and ?");
	if (!parent.isValid()) query.prepare(queryString.arg("is null"));
	else {
		query.prepare(queryString.arg("= ?"));
		query.addBindValue(parent.internalId());
	}
	query.addBindValue(row);
	query.addBindValue(row + count - 1);
	EXEC(query);
	QList<int> ids;
	while (query.next()) ids << query.value(0).toInt();
	// Recursively remove any child the items may have
	foreach (int id, ids) {
		QModelIndex idx(index(id));
		if (!idx.isValid()) continue;
		int childsNbr(rowCount(idx));
		if (childsNbr > 0 && !_removeRows(0, childsNbr, idx)) return false;
	}
	// Remove the listsLabels entries
	QStringList strIds;
	foreach (int id, ids) strIds << QString::number(id);
	
	beginRemoveRows(parent, row, row + count - 1);
	query.prepare(QString("delete from listsLabels where rowid in (%1)").arg(strIds.join(", ")));
	EXEC(query);
	// Remove the lists entries
	query.prepare(QString("delete from lists where rowid in (%1)").arg(strIds.join(", ")));
	EXEC(query);
	// Update the positions of items that were after the ones we removed
	if (!moveRows(row + count, -count, parent, query)) return false;
	invalidateCache();
	endRemoveRows();
	return true;
}

bool EntryListModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (!TRANSACTION) return false;
	if (!_removeRows(row, count, parent)) return true;
	if (!COMMIT) goto transactionFailed;
	return true;
transactionFailed:
	ROLLBACK;
	return false;
}

Qt::ItemFlags EntryListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags ret(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
	
	if (index.isValid()) {
		const EntryListModelCache &cEntry = getFromCache(index.internalId());
		if (cEntry.rowId == -1) return Qt::ItemIsEnabled;
		if (cEntry.type == -1) ret |= Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
	}
	else ret |= Qt::ItemIsDropEnabled;
	return ret;
}

QStringList EntryListModel::mimeTypes() const
{
	QStringList ret;
	ret << "tagainijisho/entry";
	ret << "tagainijisho/listitem";
	return ret;
}

QMimeData *EntryListModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();
	QByteArray entriesEncodedData;
	QDataStream entriesStream(&entriesEncodedData, QIODevice::WriteOnly);
	QByteArray itemsEncodedData;
	QDataStream itemsStream(&itemsEncodedData, QIODevice::WriteOnly);
	
	foreach (const QModelIndex &index, indexes) {
		if (index.isValid()) {
			// Add the item
			itemsStream << (int)index.internalId();
			
			// If the item is an entry, add it
			const EntryListModelCache &cEntry = getFromCache(index.internalId());
			if (cEntry.type != -1) entriesStream << cEntry.type << cEntry.id;
		}
	}
	if (!entriesEncodedData.isEmpty()) mimeData->setData("tagainijisho/entry", entriesEncodedData);
	if (!itemsEncodedData.isEmpty()) mimeData->setData("tagainijisho/listitem", itemsEncodedData);
	return mimeData;
}

// TODO Handle the case when a list is dropped into one of its children!
bool EntryListModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	if (action == Qt::IgnoreAction) return true;
	if (column == -1) column = 0;
	if (column > 0) return false;
	
	// If we have list items, we must move the items instead of inserting them
	if (data->hasFormat("tagainijisho/listitem")) {
		QByteArray ba = data->data("tagainijisho/listitem");
		QDataStream ds(&ba, QIODevice::ReadOnly);
		int i = 0;
		// If dropped on a list, append the entries
		if (row == -1) row = rowCount(parent);
		int realRow(row);
		
		if (!TRANSACTION) return false;
		emit layoutAboutToBeChanged();
		{
			QSqlQuery query;
			while (!ds.atEnd()) {
				int rowid;
				ds >> rowid;
				// First get the model index
				QModelIndex idx(index(rowid));
				// and its parent
				QModelIndex idxParent(idx.parent());
				// If the destination parent is the same as the source and the destination row superior, we must decrement
				// the latter because the source has not been moved yet.
				if (idxParent == parent && row > idx.row() && realRow > 0) --realRow;
				// Do not bother if the position did not change
				if (idxParent == parent && realRow == idx.row()) continue;
				
				// Update rows position after the one we move
				if (!moveRows(idx.row() + 1, -1, idxParent, query)) goto transactionFailed;
				// Update rows position after the one we insert
				if (!moveRows(realRow + i, 1, parent, query)) goto transactionFailed;
				// And do the move
				query.prepare("update lists set parent = ?, position = ? where rowid = ?");
				query.addBindValue(parent.isValid() ? parent.internalId() : QVariant(QVariant::Int));
				query.addBindValue(realRow + i);
				query.addBindValue(rowid);
				EXEC_T(query);
				invalidateCache(rowid);
				// Don't forget to change any persistent index - views use them at least to keep track
				// of the current selection
				changePersistentIndex(idx, index(rowid));
				++i;
			}
		}
		if (!COMMIT) goto transactionFailed;
		invalidateCache();
		emit layoutChanged();
	}
	// No list data, we probably dropped from the results view or something -
	// add the entries to the list
	else if (data->hasFormat("tagainijisho/entry")) {
		if (!parent.isValid()) return false;
		{
			const EntryListModelCache &cEntry = getFromCache(parent.internalId());
			if (cEntry.type != -1) return false;
		}
		QByteArray ba = data->data("tagainijisho/entry");
		QDataStream ds(&ba, QIODevice::ReadOnly);
		QList<QPair<int, int> > entries;
		while (!ds.atEnd()) {
			int type, id;
			ds >> type >> id;
			entries << QPair<int, int>(type, id);
		}
		// If dropped on a list, append the entries
		if (row == -1) row = rowCount(parent);
		if (!TRANSACTION) return false;
		{
			QSqlQuery query;
			// Start by moving the rows after the destination
			if (!moveRows(row, entries.size(), parent, query)) goto transactionFailed;
			// And insert the new rows at the right position
			query.prepare("insert into lists values(?, ?, ?, ?)");
			for (int i = 0; i < entries.size(); ++i) {
				query.addBindValue(parent.internalId());
				query.addBindValue(row + i);
				query.addBindValue(entries[i].first);
				query.addBindValue(entries[i].second);
				EXEC_T(query);
			}
		}
		beginInsertRows(parent, row, row + entries.size() - 1);
		if (!COMMIT) goto transactionFailed;
		invalidateCache();
		endInsertRows();
	}
	return true;
transactionFailed:
	ROLLBACK;
	return false;
}
