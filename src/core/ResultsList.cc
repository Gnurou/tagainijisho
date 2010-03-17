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

#include "core/ResultsList.h"

#include <QtDebug>

PreferenceItem<int> ResultsList::resultsPerPagePref("mainWindow/resultsView", "resultsPerPage", 50);

ResultsList::ResultsList(QObject *parent) : QAbstractListModel(parent), entries(), displayedUntil(0), query(), _pageNbr(0), totalResults(-1), showAllResultsRequested(false), _active(false)
{
	connect(&timer, SIGNAL(timeout()),
		this, SLOT(updateViews()));
	timer.setInterval(100);
	
	setResultsPerPage(resultsPerPagePref.value());
	
	// Results emitted by a query are added to us
	connect(&query, SIGNAL(foundEntry(EntryPointer)), this, SLOT(addResult(EntryPointer)));
	connect(&query, SIGNAL(firstResult()), this, SLOT(startReceive()));
	connect(&query, SIGNAL(lastResult()), this, SLOT(endReceive()));
	connect(&query, SIGNAL(aborted()), this, SLOT(endReceive()));
	connect(&query, SIGNAL(error()), this, SLOT(endReceive()));
	
	connect(&query, SIGNAL(nbResults(unsigned int)), this, SLOT(setNbResults(unsigned int)));

	connect(&query, SIGNAL(aborted()), this, SLOT(queryError()));
	connect(&query, SIGNAL(error()), this, SLOT(queryError()));
	
	// Update the number of items per page when the setting is changed
	connect(&resultsPerPagePref, SIGNAL(valueChanged(QVariant)), this, SLOT(onPrefValueChanged(QVariant)));
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

	if (role == Qt::BackgroundRole) {
		const EntryPointer &entry = entries.at(index.row());
		if (!entry->trained()) return QVariant();
		else return entry->scoreColor();
	}

	if (role == Entry::EntryRole) return QVariant::fromValue(entries[index.row()]);
	else if (role == Qt::DisplayRole) return entries[index.row()]->shortVersion();

	return QVariant();
}

QVariant ResultsList::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole) return QVariant();

	return QString("%1").arg(section);
}

void ResultsList::addResult(EntryPointer entry)
{
	entries << entry;
}

void ResultsList::onEntryChanged(const EntryPointer &entry)
{
	int idx = entries.indexOf(entry);
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
	
	// If we were ordered to show all results while the query was not over,
	// let's do it now.
	if (showAllResultsRequested) {
		showAllResultsRequested = false;
		scheduleShowAllResults();
	}
}

void ResultsList::clear()
{
	if (entries.isEmpty()) return;

	timer.stop();
	beginRemoveRows(QModelIndex(), 0, entries.size() - 1);
	// This is preferred to clear() because lists memory
	// usage never shrinks
	entries = QList<EntryPointer>();
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
			EntryPointer entry = data(index, Entry::EntryRole).value<EntryPointer>();
			stream << EntryRef(entry);
		}
	}

	mimeData->setData("tagainijisho/entry", encodedData);
	return mimeData;
}

void ResultsList::queryError()
{
	_active = false;
	emit queryEnded();
}

void ResultsList::nextPage()
{
	if (totalResults < (pageNbr() + 1) * resultsPerPage()) return;
	query.abort();
	QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
	clear();
	_active = true;
	query.fetch(++_pageNbr * resultsPerPage(), resultsPerPage());
	emit queryStarted();
}

void ResultsList::previousPage()
{
	if (pageNbr() == 0) return;
	query.abort();
	QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
	clear();
	_active = true;
	query.fetch(--_pageNbr * resultsPerPage(), resultsPerPage());
	emit queryStarted();
}

void ResultsList::scheduleShowAllResults()
{
	// If the current query completed, no problem!
	if (!query.isRunning()) {
		// First check if all the results are not already displayed
		if (pageNbr() == 0 && totalResults <= nbResults()) return;
		
		_active = true;
		// If we are on the first page, we can just continue the query this way
		if (pageNbr() == 0) {
			query.fetch(nbResults(), -1);
			emit queryStarted();
		}
		else {
			// Otherwise do the search from start again
			clear();
			_pageNbr = 0;
			totalResults = -1;
			query.fetch(0, -1);
			emit queryStarted();
		}
	}
	// Otherwise let's call ourselves again once it is done...
	else showAllResultsRequested = true;
}

void ResultsList::search(const QueryBuilder &qBuilder)
{
	// Stop the running query, if any
	abortSearch();
	
	// Clear the current set of results
	clear();
	_pageNbr = 0;
	totalResults = -1;
	
	// And start the query!
	query.prepare(qBuilder);
	_active = true;
	emit newSearch();
	emit queryStarted();
	query.fetch(0, resultsPerPage());
}

void ResultsList::abortSearch()
{
	_active = false;
	query.abort();
	// Flush all the entries the results list may be receiving
	QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
	// Clear the list of all received entries
	query.clear();
	emit queryEnded();
}

void ResultsList::onPrefValueChanged(QVariant newValue)
{
	setResultsPerPage(newValue.toInt());
}

void ResultsList::setNbResults(unsigned int nbRes)
{
	totalResults = nbRes;
	emit nbResults(totalResults);
}