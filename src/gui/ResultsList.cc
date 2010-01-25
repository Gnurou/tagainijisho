/*
 *  Copyright (C) 2008  Alexandre Courbot
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

#include "gui/EntryFormatter.h"
#include "gui/ResultsList.h"

#include <QtDebug>

ResultsList::ResultsList(QObject *parent) : QAbstractListModel(parent), entries(), displayedUntil(0)
{
	connect(&timer, SIGNAL(timeout()),
		this, SLOT(updateViews()));
	timer.setInterval(100);
}

ResultsList::~ResultsList()
{
	clear();
}

QVariant ResultsList::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) return QVariant();

	if (index.row() >= entries.size()) return QVariant();

	if (role == Qt::BackgroundRole) {
		Entry *entry = entries.at(index.row()).data();
		if (!entry->trained()) return QVariant();
		const EntryFormatter *formatter(EntryFormatter::getFormatter(entry));
		if (formatter) return formatter->scoreColor(entry);
		return QVariant();
	}

	if (role == EntryRole) return QVariant::fromValue(entries[index.row()].data());
	else if (role == Qt::DisplayRole) return entries[index.row()]->shortVersion();

	return QVariant();
}

QVariant ResultsList::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole) return QVariant();

	return QString("%1").arg(section);
}

void ResultsList::addResult(EntryPointer<Entry> entry)
{
	entries << entry;
}

void ResultsList::onEntryChanged(Entry *entry)
{
	int idx = entries.indexOf(EntryPointer<Entry>(entry));
	QModelIndex itemIndex = createIndex(idx, 0);
	emit dataChanged(itemIndex, itemIndex);
}

void ResultsList::updateViews()
{
	// TODO Acquire mutex on entries to ensure consistency despite of
	// multithreading?
	if (displayedUntil < entries.size()) {
		beginInsertRows(QModelIndex(), displayedUntil, entries.size());
		endInsertRows();
		displayedUntil = entries.size();
	}
}

void ResultsList::startReceive()
{
	timer.start();
}

void ResultsList::endReceive()
{
	timer.stop();
	updateViews();
}

void ResultsList::clear()
{
	if (entries.isEmpty()) return;

	beginRemoveRows(QModelIndex(), 0, entries.size() - 1);
	entries.clear();
	endRemoveRows();
	displayedUntil = 0;
	timer.stop();
}

Qt::ItemFlags ResultsList::flags(const QModelIndex &index) const
{
	Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

	if (index.isValid()) return Qt::ItemIsDragEnabled | defaultFlags;
	else return defaultFlags;
}

QStringList ResultsList::mimeTypes() const
{
	QStringList types;
	types << "tagainijisho/entry";
	return types;
}
 
QMimeData *ResultsList::mimeData(const QModelIndexList &indexes) const
{  
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	foreach (QModelIndex index, indexes) {
		if (index.isValid()) {
			Entry *entry = data(index, EntryRole).value<Entry *>();
			stream << entry->type() << entry->id();
		}
	}

	mimeData->setData("tagainijisho/entry", encodedData);
	return mimeData;
}
