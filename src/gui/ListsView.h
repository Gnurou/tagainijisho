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

#ifndef __GUI_LISTSVIEW_H
#define __GUI_LISTSVIEW_H

#include "core/EntriesCache.h"
#include <QTreeWidget>
#include <QTreeView>
#include <QMimeData>

#include <QAbstractItemModel>

class EntryListModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const { return 1; }
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	
	//virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
	virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
	virtual QStringList mimeTypes() const;
	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
	virtual Qt::DropActions supportedDropActions() const { return Qt::CopyAction | Qt::MoveAction; }
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
};

class EntryListView : public QTreeView
{
	Q_OBJECT
public:
	EntryListView(QWidget *parent = 0);

protected slots:
	virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

signals:
	void listSelected(int rowIndex);
	void entrySelected(EntryPointer<Entry> entry);
};

class ListTreeItem : public QTreeWidgetItem
{
private:
	int _rowId, _type, _id;
	
public:
	typedef enum { ListType = (QTreeWidgetItem::UserType), EntryType = (QTreeWidgetItem::UserType + 1) } Type;
	/// Creates an entry of list type
	ListTreeItem(int rowId, const QString &label);
	ListTreeItem(int rowId, const Entry *entry);
	void populate();
	int rowId() const { return _rowId; }
	int entryType() const { return _type; }
	int entryId() const { return _id; }
};

class ListsView : public QTreeWidget
{
	Q_OBJECT
private:
	
public:
	ListsView(QWidget *parent = 0);
	bool populateRoot();
	
protected:
	virtual QStringList mimeTypes () const;
	virtual QMimeData *mimeData(const QList<QTreeWidgetItem *> items) const;
	virtual bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);
	
public slots:
	void newList();
	void deleteCurrentItem();

protected slots:
	void onItemSelectionChanged();

signals:
	void listSelected(int rowId);
	void entrySelected(EntryPointer<Entry> entry);
};

#endif
