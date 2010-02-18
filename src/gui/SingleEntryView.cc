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

#include "gui/TagsDialogs.h"
#include "gui/SingleEntryView.h"
#include "gui/EditEntryNotesDialog.h"

SingleEntryView::SingleEntryView(QObject *parent) : EntryMenu(parent), _entry(0)
{
	connect(&addToStudyAction, SIGNAL(triggered()), this, SLOT(addToStudy()));
	connect(&removeFromStudyAction, SIGNAL(triggered()), this, SLOT(removeFromStudy()));
	connect(&alreadyKnownAction, SIGNAL(triggered()), this, SLOT(alreadyKnown()));
	connect(&resetTrainingAction, SIGNAL(triggered()), this, SLOT(resetTraining()));
	connect(&setTagsAction, SIGNAL(triggered()), this, SLOT(setTags()));
	connect(&addTagsAction, SIGNAL(triggered()), this, SLOT(addTags()));
	connect(&setNotesAction, SIGNAL(triggered()), this, SLOT(setNotes()));
	connect(this, SIGNAL(tagsHistorySelected(const QStringList &)),
	        this, SLOT(setTagsFromHistory(const QStringList &)));
}

void SingleEntryView::setEntry(Entry *entry)
{
	if (_entry) disconnect(_entry.data(), SIGNAL(entryChanged(Entry *)), this, SIGNAL(entryChanged(Entry *)));
	if (entry) connect(entry, SIGNAL(entryChanged(Entry *)), this, SIGNAL(entryChanged(Entry *)));
	_entry = entry;
	updateStatus(_entry);
}

void SingleEntryView::addToStudy()
{
	entry()->addToTraining();
}

void SingleEntryView::removeFromStudy()
{
	entry()->removeFromTraining();
}

void SingleEntryView::alreadyKnown()
{
	entry()->setAlreadyKnown();
}

void SingleEntryView::resetTraining()
{
	entry()->resetScore();
}

void SingleEntryView::setTags()
{
	QList<EntryPointer> list;
	list << entry();
	TagsDialogs::setTagsDialog(list);
}

void SingleEntryView::addTags()
{
	QList<EntryPointer> list;
	list << entry();
	TagsDialogs::addTagsDialog(list);
}

void SingleEntryView::setNotes()
{
	EditEntryNotesDialog dialog(*entry());
	dialog.exec();
}

void SingleEntryView::updateMenuEntries()
{
	EntryMenu::updateStatus(entry());
}

void SingleEntryView::setTagsFromHistory(const QStringList &tags)
{
	entry()->addTags(tags);
}

