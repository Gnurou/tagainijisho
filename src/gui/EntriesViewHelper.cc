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

#include "gui/EntriesViewHelper.h"
// TODO For entry role - get rid of this
#include "gui/ResultsList.h"
#include "gui/EntryMenu.h"
#include "gui/EditEntryNotesDialog.h"
#include "gui/TagsDialogs.h"

#include <QAction>
#include <QProgressDialog>

EntriesViewHelper::EntriesViewHelper(QAbstractItemView *parent) : EntryMenu(parent), _client(parent)
{
	connect(&addToStudyAction, SIGNAL(triggered()),
			this, SLOT(studySelected()));
	connect(&removeFromStudyAction, SIGNAL(triggered()),
			this, SLOT(unstudySelected()));
	connect(&alreadyKnownAction, SIGNAL(triggered()),
	        this, SLOT(markAsKnown()));
	connect(&resetTrainingAction, SIGNAL(triggered()),
	        this, SLOT(resetTraining()));
	connect(&setTagsAction, SIGNAL(triggered()),
	        this, SLOT(setTags()));
	connect(&addTagsAction, SIGNAL(triggered()),
	        this, SLOT(addTags()));
	connect(&setNotesAction, SIGNAL(triggered()),
	        this, SLOT(addNote()));
	connect(this, SIGNAL(tagsHistorySelected(const QStringList &)),
			this, SLOT(addTags(const QStringList &)));
}

QList<EntryPointer> EntriesViewHelper::selectedEntries() const
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	QList<EntryPointer> selectedEntries;
	foreach(const QModelIndex &index, selection) {
		EntryPointer entry(qVariantValue<EntryPointer>(index.data(ResultsList::EntryRole)));
		if (entry) selectedEntries << entry;
	}
	return selectedEntries;
}

void EntriesViewHelper::studySelected()
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Marking entries..."), tr("Abort"), 0, selection.size(), client());
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		EntryPointer entry = qVariantValue<EntryPointer>(index.data(ResultsList::EntryRole));
		if (!entry) continue;
		entry->addToTraining();
		client()->update(index);
	}
}

void EntriesViewHelper::unstudySelected()
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Marking entries..."), tr("Abort"), 0, selection.size(), client());
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		EntryPointer entry = qVariantValue<EntryPointer>(index.data(ResultsList::EntryRole));
		if (!entry) continue;
		entry->removeFromTraining();
		client()->update(index);
	}
}

void EntriesViewHelper::markAsKnown()
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Marking entries..."), tr("Abort"), 0, selection.size(), client());
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		EntryPointer entry = qVariantValue<EntryPointer>(index.data(ResultsList::EntryRole));
		if (!entry) continue;
		if (!entry->alreadyKnown()) continue;
		entry->setAlreadyKnown();
		client()->update(index);
	}
}

void EntriesViewHelper::resetTraining()
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Resetting entries..."), tr("Abort"), 0, selection.size(), client());
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		EntryPointer entry = qVariantValue<EntryPointer>(index.data(ResultsList::EntryRole));
		if (!entry) continue;
		entry->resetScore();
		client()->update(index);
	}
}

void EntriesViewHelper::setTags()
{
	TagsDialogs::setTagsDialog(selectedEntries(), client());
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	foreach (const QModelIndex &index, selection)
		client()->update(index);
}

void EntriesViewHelper::addTags()
{
	TagsDialogs::addTagsDialog(selectedEntries(), client());
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	foreach (const QModelIndex &index, selection)
		client()->update(index);
}

void EntriesViewHelper::addTags(const QStringList &tags)
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Adding tags..."), tr("Abort"), 0, selection.size(), client());
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);
	
	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		EntryPointer entry = qVariantValue<EntryPointer>(index.data(ResultsList::EntryRole));
		if (!entry) continue;
		entry->addTags(tags);
		client()->update(index);
	}
}

void EntriesViewHelper::addNote()
{
	QModelIndexList selection = client()->selectionModel()->selectedIndexes();
	if (selection.size() != 1) return;
	EntryPointer entry = qVariantValue<EntryPointer >(selection[0].data(ResultsList::EntryRole));
	if (!entry) return;
	EditEntryNotesDialog dialog(*entry, client());
	if (dialog.exec() != QDialog::Accepted) return;
	foreach (const QModelIndex &index, selection)
		client()->update(index);
}
