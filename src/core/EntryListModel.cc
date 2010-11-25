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

#include "core/Database.h"
#include "core/EntryListModel.h"
#include "core/EntryListCache.h"

#include <QFont>
#include <QFontMetrics>
#include <QSize>
#include <QPalette>

/*#define TRANSACTION Database::connection()->transaction()
#define ROLLBACK Database::connection()->rollback()
#define COMMIT Database::connection()->commit()

#define EXEC(q) if (!q.exec()) { qDebug() << __FILE__ << __LINE__ << "Cannot execute query:" << q.lastError().message(); return false; }
#define EXEC_T(q) if (!q.exec()) { qDebug() << __FILE__ << __LINE__ << "Cannot execute query:" << q.lastError().message(); goto transactionFailed; }
*/

void EntryListModel::setRoot(quint64 rootId)
{
	// Nothing changes?
	if (rootId == _rootId) return;

#if QT_VERSION >= 0x040600
	beginResetModel();
#endif
	_rootId = rootId;
#if QT_VERSION >= 0x040600
	endResetModel();
#else
	reset();
#endif
	emit rootHasChanged(rootId);
}

#define LISTFORINDEX(list, index) const EntryList &list = *EntryListCache::get(index.isValid() ? index.internalId() : 0)

QModelIndex EntryListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (column > 0) return QModelIndex();

	LISTFORINDEX(list, parent);
	quint64 listId;
	// If the parent is valid, then fetch its real list
	if (parent.isValid()) {
		EntryListData cEntry(list[parent.row()]);
		// Parent will always be a list
		Q_ASSERT(cEntry.isList());
		listId = cEntry.id;
	} else listId = 0;

	return indexFromList(listId, row);
}

QModelIndex EntryListModel::indexFromList(quint64 listId, quint64 position) const
{
	const EntryList *list = EntryListCache::get(listId);
	if (!list || position > list->size()) return QModelIndex();

	// FIXME Qt is wrong here - internalId() returns a qint64, so this function should take a qint64 too!
	return createIndex(position, 0, (quint32)list->listId());
}
	
QModelIndex EntryListModel::parent(const QModelIndex &idx) const
{
	// Invalid index has no parent
	if (!idx.isValid()) return QModelIndex();
	LISTFORINDEX(list, idx);
	// Root list has no parent
	if (list.listId() == 0) return QModelIndex();
	// We need to construct an index by determining the parent list of idx and the position of idx within it
	else {
		// We have the id of the list - all we need to do is find
		// which list contains it.
		QPair<const EntryList *, quint32> container = EntryListCache::getOwner(list.listId());
		// TODO implement parent fetching in the lists cache!
		return createIndex(container.second, 0, container.first ? container.first->listId() : 0);
	}
}

int EntryListModel::rowCount(const QModelIndex &index) const
{
	// Asking for size of root?
	if (!index.isValid()) return EntryListCache::get(0)->size();
	else {
		LISTFORINDEX(list, index);
		EntryListData cEntry(list[index.row()]);
		// Not a list? No child!
		if (!cEntry.isList()) return 0;
		else return EntryListCache::get(cEntry.id)->size();
	}
}

QVariant EntryListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.column() != 0) return QVariant();

	LISTFORINDEX(list, index);
	EntryListData cEntry(list[index.row()]);

	switch (role) {
		case Qt::DisplayRole:
		case Qt::EditRole:
		{
			if (cEntry.isList()) return EntryListCache::get(cEntry.id)->label();
			EntryPointer entry(cEntry.entryRef().get());
			if (!entry) return QVariant();
			else return entry->shortVersion(Entry::TinyVersion);
		}
		case Qt::BackgroundRole:
		{
			if (cEntry.isList()) return QPalette().button();
			EntryPointer entry(cEntry.entryRef().get());
			if (!entry || !entry->trained()) return QVariant();
			else return entry->scoreColor();
		}
		case Qt::FontRole:
		{
			if (cEntry.isList()) {
				QFont font;
				font.setPointSize(font.pointSize() + 1);
				font.setItalic(true);
				return font;
			}
			else return QVariant();
		}
		case Qt::SizeHintRole:
			if (cEntry.isList()) {
				QFont font;
				font.setPointSize(font.pointSize() + 1);
				font.setItalic(true);
				return QSize(300, QFontMetrics(font).height());
			}
			else return QVariant();
		case Entry::EntryRole:
		{
			if (cEntry.isList()) return QVariant();
			EntryPointer entry(cEntry.entryRef().get());
			if (!entry) return QVariant();
			else return QVariant::fromValue(entry);
		}
		case Entry::EntryRefRole:
		{
			if (cEntry.isList()) return QVariant();
			return QVariant::fromValue(cEntry.entryRef());
		}
		default:
			return QVariant();
	}
}

Qt::ItemFlags EntryListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags ret(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);

	if (index.isValid()) {
		LISTFORINDEX(list, index);
		EntryListData cEntry(list[index.row()]);
		if (cEntry.isList()) ret |= Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
	}
	else ret |= Qt::ItemIsDropEnabled;
	return ret;
}
bool EntryListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	return false;

	/*
	if (role == Qt::EditRole && index.isValid()) {
		if (!TRANSACTION) return false;
		{
			SQLite::Query query(Database::connection());
			query.prepare("select type from lists where rowid = ?");
			query.bindValue(index.internalId());
			query.exec();
			if (query.next() && query.valueIsNull(0)) {
				query.prepare("update listsLabels set label = ? where rowid = ?");
				query.bindValue(value.toString());
				query.bindValue(index.internalId());
				EXEC_T(query);
			}
		}
		if (!COMMIT) goto transactionFailed;
		// Invalidate the cache entry
		EntryListCache::instance().invalidate(index.internalId());
		return true;
	}
	return false;
transactionFailed:
	ROLLBACK;
	EntryListCache::instance().invalidateAll();
	return false; */
}
	
// TODO How to handle the beginInsertRows if the transaction failed and endInsertRows is not called?
bool EntryListModel::insertRows(int row, int count, const QModelIndex & parent)
{
	return false;
	/*
	const QModelIndex realParent(parent.isValid() ? parent : index(rootId()));
	if (!TRANSACTION) return false;
	beginInsertRows(parent, row, row + count - 1);
	{
		SQLite::Query query(Database::connection());
		// First update the positions of items located after the new lists
		if (!moveRows(row, count, realParent, query)) goto transactionFailed;
		// Then insert the lists themselves with a default name
		for (int i = 0; i < count; i++) {
			query.prepare("insert into lists values(?, ?, NULL, NULL)");
			if (realParent.isValid()) query.bindValue(realParent.internalId());
			else query.bindNullValue();
			query.bindValue(row + i);
			EXEC_T(query);
			int rowId = query.lastInsertId();
			query.prepare("insert into listsLabels(docid, label) values(?, ?)");
			query.bindValue(rowId);
			query.bindValue(tr("New list"));
			EXEC_T(query);
		}
	}
	if (!COMMIT) goto transactionFailed;
	EntryListCache::instance().invalidateAll();
	endInsertRows();
	return true;
transactionFailed:
	ROLLBACK;
	EntryListCache::instance().invalidateAll();
	return false;*/
}

bool EntryListModel::removeRows(int row, int count, const QModelIndex &parent)
{
	// TODO use global transaction
	// TODO remove lists recursively if a list is removed
	LISTFORINDEX(parentList, parent);
	EntryListData cEntry(parentList[parent.row()]);
	// The list from which we are actually removing
	EntryList &list = *EntryListCache::get(cEntry.id);
	beginRemoveRows(parent, row, row + count - 1);
	while (count--) {
		list.remove(row);
	}
	endRemoveRows();
	return true;
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
			itemsStream << (quint64)index.internalId() << (quint64)index.row();
			
			// If the item is an entry, add it
			LISTFORINDEX(list, index);
			EntryListData cEntry(list[index.row()]);
			if (!cEntry.isList()) entriesStream << cEntry.entryRef();
			// TODO in case of a list, add all the items the list contains
		}
	}
	if (!entriesEncodedData.isEmpty()) mimeData->setData("tagainijisho/entry", entriesEncodedData);
	if (!itemsEncodedData.isEmpty()) mimeData->setData("tagainijisho/listitem", itemsEncodedData);

	return mimeData;
}

bool EntryListModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &_parent)
{
	// TODO Handle transactions!
	
	if (!_parent.isValid()) return false;
	if (action == Qt::IgnoreAction) return true;
	if (column == -1) column = 0;
	if (column > 0) return false;

	LISTFORINDEX(parentList, _parent);
	EntryListData cEntry(parentList[_parent.row()]);
	EntryList &list = *EntryListCache::get(cEntry.id);

	// If dropped on a list, append the entries
	if (row == -1) row = rowCount(_parent);

	emit layoutAboutToBeChanged();

	// If we have list items, we must move the items instead of inserting them
	if (data->hasFormat("tagainijisho/listitem")) {
		// ids of the items we move
		QList<QPair<quint64, quint64> > ids;
		// Fetch the ids of the rows that moved from
		// the stream
		QByteArray ba = data->data("tagainijisho/listitem");
		QDataStream ds(&ba, QIODevice::ReadOnly);
		while (!ds.atEnd()) {
			quint64 listId, pos;
			ds >> listId;
			ds >> pos;
			ids << QPair<quint64, quint64>(listId, pos);
		}
		emit layoutAboutToBeChanged();
		int cpt = 0;
		typedef QPair<quint64, quint64> qpp;
		// TODO Take the destination list size change into account!
		// TODO Also, the positions within source lists may not be valid anymore
		// as the items move.
		// TODO Instead of deleting/creating new nodes and making ids turn, why not
		// change their properties directly? Need support from the lists layer
		foreach (const qpp &id, ids) {
			// Needed to update persistent indexes
			QModelIndex srcIndex(indexFromList(id.first, id.second));
			// Get the data from the source and remove it from the tree
			EntryList &srcList = *EntryListCache::get(id.first);
			EntryListData eData = srcList[id.second];
			srcList.remove(id.second);

			// Insert the data into the destination
			list.insert(eData, row + cpt);

			// Don't forget to change any persistent index - views use them at least to keep track
			// of the current selection
			changePersistentIndex(srcIndex, indexFromList(list.listId(), row + cpt));

			cpt++;
		}
		emit layoutChanged();

	}
	// No list data, we probably dropped from the results view or something -
	// add the entries to the list
	else if (data->hasFormat("tagainijisho/entry")) {
		QByteArray ba = data->data("tagainijisho/entry");
		QDataStream ds(&ba, QIODevice::ReadOnly);
		QList<EntryRef> entries;

		while (!ds.atEnd()) {
			EntryRef entryRef;
			ds >> entryRef;
			entries << entryRef;
		}

		// Insert rows must be done on what the view thinks is the parent
		beginInsertRows(_parent, row, row + entries.size() - 1);
		int cpt = 0;
		foreach (const EntryRef &entry, entries) {
			EntryListData eData;
			eData.type = entry.type();
			eData.id = entry.id();
			list.insert(eData, row + cpt++);

			// Now add the list to the entry if it is loaded
			//if (entry.isLoaded()) entry.get()->lists() << list.listId();
		}
		endInsertRows();
	}
	return true;

	/*

	// If dropped on the root, update the parent to the real one
	// Direct affectation of model indexes does not seem to copy the internal id
	const QModelIndex parent(_parent.isValid() ? _parent : index(rootId()));

	// If we have list items, we must move the items instead of inserting them
	if (data->hasFormat("tagainijisho/listitem")) {
		int realRow(row);

		// First check if we are not dropping a parent into one of its children
		foreach (int rowid, rowIds) {
			QModelIndex idx(index(rowid));
			QModelIndex p(parent);
			while (p.isValid()) {
				if (p == idx) return false;
				p = p.parent();
			}
		}
	
		// Do the move
		int itemCpt = 0;
		if (!TRANSACTION) return false;
		emit layoutAboutToBeChanged();
		{
			SQLite::Query query(Database::connection());
			foreach (int rowid, rowIds) {
				// First get the model index
				QModelIndex idx(index(rowid));
				// and its parent
				QModelIndex idxParent(realParent(idx));
				// If the destination parent is the same as the source and the destination row superior, we must decrement
				// the latter because the source has not been moved yet.
				if (idxParent == parent && row > idx.row() && realRow > 0) --realRow;
				// Do not bother if the position did not change
				if (idxParent == parent && realRow == idx.row()) continue;
				
				// Update rows position after the one we move
				if (!moveRows(idx.row() + 1, -1, idxParent, query)) goto transactionFailed;
				// Update rows position after the one we insert
				if (!moveRows(realRow + itemCpt, 1, parent, query)) goto transactionFailed;
				// And do the move
				query.prepare("update lists set parent = ?, position = ? where rowid = ?");
				if (parent.isValid()) query.bindValue(parent.internalId());
				else query.bindNullValue();
				query.bindValue(realRow + itemCpt);
				query.bindValue(rowid);
				EXEC_T(query);
				EntryListCache::instance().invalidateAll();
				// Don't forget to change any persistent index - views use them at least to keep track
				// of the current selection
				changePersistentIndex(idx, index(rowid));
				++itemCpt;
			}
		}
		if (!COMMIT) goto transactionFailed;
		EntryListCache::instance().invalidateAll();
		emit layoutChanged();
		return true;
	}
	// No list data, we probably dropped from the results view or something -
	// add the entries to the list
	else if (data->hasFormat("tagainijisho/entry")) {
	
		if (!TRANSACTION) return false;
		{
			SQLite::Query query(Database::connection());
			// Start by moving the rows after the destination
			if (!moveRows(row, entries.size(), parent, query)) goto transactionFailed;
			// And insert the new rows at the right position
			query.prepare("insert into lists values(?, ?, ?, ?)");
			for (int i = 0; i < entries.size(); ++i) {
				if (parent.isValid()) query.bindValue(parent.internalId());
				else query.bindNullValue();
				query.bindValue(row + i);
				query.bindValue(entries[i].type());
				query.bindValue(entries[i].id());
				EXEC_T(query);
				ids << query.lastInsertId();
			}
		}
		// Insert rows must be done on what the view thinks is the parent
		beginInsertRows(_parent, row, row + entries.size() - 1);
		if (!COMMIT) goto transactionFailed;
		EntryListCache::instance().invalidateAll();
		endInsertRows();
		// Now add the list to the entries, if necessary
		int i = 0;
		foreach (EntryRef ref, entries) {
			if (ref.isLoaded()) ref.get()->lists() << ids[i];
			++i;
		}
		return true;
	}
	else return false;
transactionFailed:
	ROLLBACK;
	EntryListCache::instance().invalidateAll();
	return false;*/
}
