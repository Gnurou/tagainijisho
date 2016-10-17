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

#include "gui/ResultsList.h"
#include "gui/EntryFormatter.h"

#include <QtDebug>
#include <QDataStream>
#include <QColor>

ResultsList::ResultsList(QObject *parent) : QAbstractListModel(parent), entries(), displayedUntil(0), dbThread(), query(&dbThread)
{
	connect(&timer, SIGNAL(timeout()),
		this, SLOT(updateViews()));
	timer.setInterval(100);
	
	// Results emitted by a query are added to us
	connect(&query, SIGNAL(result(EntryRef)), this, SLOT(addResult(EntryRef)));
	connect(&query, SIGNAL(firstResult()), this, SLOT(startReceive()));
	connect(&query, SIGNAL(completed()), this, SLOT(endReceive()));
	connect(&query, SIGNAL(aborted()), this, SLOT(endReceive()));
	connect(&query, SIGNAL(error(QString)), this, SLOT(endReceive()));
}

ResultsList::~ResultsList()
{
	abortSearch();
	clear();
}

QVariant ResultsList::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) return QVariant();

	if (index.row() >= entries.size()) return QVariant();

	if (role == Entry::EntryRefRole) return QVariant::fromValue(entries[index.row()]);
	EntryPointer entry(entries[index.row()].get());
//	EntryPointer entry(0);

	switch (role) {
	case Qt::BackgroundRole:
		if (!entry.data() || !entry->trained()) return QVariant();
		else return EntryFormatter::scoreColor(*entry);
	case Entry::EntryRole:
		return QVariant::fromValue(entry);
	case Qt::DisplayRole:
		if (entry.data()) return entry->shortVersion();
		else return "";
	default:
		return QVariant();
	}
}

QVariant ResultsList::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole) return QVariant();

	return QString("%1").arg(section);
}

void ResultsList::addResult(EntryRef entry)
{
	entries << entry;
}

void ResultsList::onEntryChanged(const EntryPointer &entry)
{
	int idx = entries.indexOf(EntryRef(entry));
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
	emit queryEnded();	
}

void ResultsList::clear()
{
	if (entries.isEmpty()) return;

	timer.stop();
	beginRemoveRows(QModelIndex(), 0, entries.size() - 1);
	// This is preferred to clear() because lists memory
	// usage never shrinks
	entries = QList<EntryRef>();
	endRemoveRows();
	displayedUntil = 0;
}

Qt::ItemFlags ResultsList::flags(const QModelIndex &index) const
{
	Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

	if (index.isValid()) return Qt::ItemIsDragEnabled | defaultFlags;
	else return defaultFlags;
}

QMimeData *ResultsList::mimeData(const QModelIndexList &indexes) const
{  
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	foreach (QModelIndex index, indexes) {
		if (index.isValid()) {
			stream << data(index, Entry::EntryRefRole).value<EntryRef>();
		}
	}

	mimeData->setData("tagainijisho/entry", encodedData);
	return mimeData;
}

void ResultsList::search(const QueryBuilder &qBuilder)
{
	// Stop the running query, if any
	abortSearch();
	
	// Clear the current set of results
	clear();
	
	// And start the query!
	query.exec(qBuilder.buildSqlStatement());
	emit queryStarted();
}

void ResultsList::abortSearch()
{
	query.abort();
	// Flush all the entries the results list may be receiving
	QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
	emit queryEnded();
}
