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

#ifndef __GUI_ENTRYLISTMODEL_H
#define __GUI_ENTRYLISTMODEL_H

#include "core/EntriesCache.h"

#include <QAbstractItemModel>
#include <QSqlQuery>
#include <QMimeData>

typedef struct {
	int rowId;
	int parent;
	int position;
	int type;
	int id;
	QString label;
} EntryListModelCache;

class EntryListModel : public QAbstractItemModel
{
	Q_OBJECT
private:
	mutable QHash<int, EntryListModelCache> rowIdCache;
	mutable QHash<QPair<int, int>, EntryListModelCache> rowParentCache;

	const EntryListModelCache &getFromCache(int rowid) const;
	const EntryListModelCache &getFromCache(int row, int parent) const;
	/// Invalidates the cache for the specified row id
	void invalidateCache(int rowId) const;
	/// Invalidates the whole cache
	void invalidateCache() const;

	/**
	 * Move all rows in parent with a position >= row by adding delta to their position.
	 * Returns true upon success.
	 */
	bool moveRows(int row, int delta, const QModelIndex &parent, QSqlQuery &query);
	/// Private version of removeRows that do not start a transaction
	bool _removeRows(int row, int count, const QModelIndex &parent);

public:
	EntryListModel(QObject *parent = 0) : QAbstractItemModel(parent) {}

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex index(int rowId) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const { return 1; }
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	
	virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
	virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
	virtual QStringList mimeTypes() const;
	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
	virtual Qt::DropActions supportedDropActions() const { return Qt::CopyAction | Qt::MoveAction; }
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
};

#endif
