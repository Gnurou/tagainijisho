/*
 *  Copyright (C) 2008/2009/2010  Alexandre Courbot
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

#include "core/Entry.h"
#include "gui/TagsDialogs.h"
#include "gui/EntryMenu.h"

#include <QtDebug>

#include <QModelIndex>
#include <QModelIndexList>
#include <QMenu>

EntryMenu::EntryMenu(QObject *parent) : QObject(parent), copyWritingAction(0), copyReadingAction(0), addToStudyAction(QIcon(":/images/icons/flag-blue.png"), tr("Add to &study list"), 0), removeFromStudyAction(QIcon(":/images/icons/flag-black.png"), tr("Remove from &study list"), 0), alreadyKnownAction(QIcon(":/images/icons/flag-green.png"), tr("Already &known"), 0), resetTrainingAction(QIcon(":/images/icons/flag-red.png"), tr("&Reset score"), 0), setTagsAction(QIcon(":/images/icons/tags.png"), tr("Set &tags..."), 0), addTagsAction(QIcon(":/images/icons/tags-add.png"), tr("&Add tags..."), 0), setNotesAction(QIcon(":/images/icons/notes.png"), tr("Edit &notes..."), 0), lastTagsMenu(tr("Recently added tags..."), 0)
{
	lastTagsMenu.setIcon(QIcon(":/images/icons/tags-add.png"));
	setEnabledAll(false);
	connect(&lastTagsMenu, SIGNAL(aboutToShow()), this, SLOT(makeLastTagsMenu()));
}

void EntryMenu::populateMenu(QMenu *menu)
{
	menu->addAction(&copyWritingAction);
	menu->addAction(&copyReadingAction);
	menu->addSeparator();
	menu->addAction(&addToStudyAction);
	menu->addAction(&removeFromStudyAction);
	menu->addAction(&alreadyKnownAction);
	menu->addAction(&resetTrainingAction);
	menu->addAction(&setTagsAction);
	menu->addAction(&addTagsAction);
	menu->addMenu(&lastTagsMenu);
	menu->addAction(&setNotesAction);
}

void EntryMenu::populateToolBar(QToolBar *bar)
{
	bar->addAction(&addToStudyAction);
	bar->addAction(&removeFromStudyAction);
	bar->addAction(&alreadyKnownAction);
	bar->addAction(&resetTrainingAction);
	bar->addSeparator();
	bar->addAction(&setTagsAction);
	bar->addAction(&addTagsAction);
//	bar->addMenu(&lastTagsMenu);
	bar->addAction(&setNotesAction);
}

void EntryMenu::setEnabledAll(bool enabled)
{
	addToStudyAction.setVisible(true);
	removeFromStudyAction.setVisible(true);
	addToStudyAction.setEnabled(enabled);
	removeFromStudyAction.setEnabled(enabled);
	alreadyKnownAction.setEnabled(enabled);
	resetTrainingAction.setEnabled(enabled);
	setTagsAction.setEnabled(enabled);
	addTagsAction.setEnabled(enabled);
	setNotesAction.setEnabled(enabled);
	lastTagsMenu.setEnabled(enabled);
	copyWritingAction.setVisible(false);
	copyReadingAction.setVisible(false);
}

void EntryMenu::updateStatus(const QList<ConstEntryPointer>& entries)
{
	// Change the mark to study label according to the selection properties:
	bool allMarked = true;
	bool allUnmarked = true;
	bool allAlreadyKnown = true;
	bool hasSelection = entries.size() != 0;
	bool oneEntry = entries.size() == 1;

	foreach(ConstEntryPointer entry, entries) {
		if (!entry->trained()) allMarked = false;
		else allUnmarked = false;
		if (!entry->alreadyKnown()) allAlreadyKnown = false;
	}

	// Decide which of add to or remove from study action is visible
	addToStudyAction.setVisible(!allMarked | !hasSelection);
	removeFromStudyAction.setVisible(!allUnmarked);

	addToStudyAction.setEnabled(hasSelection);
	// Always enabled if visible
	removeFromStudyAction.setEnabled(true);
	alreadyKnownAction.setEnabled(hasSelection && !allAlreadyKnown);
	resetTrainingAction.setEnabled(!allUnmarked);
	setTagsAction.setEnabled(hasSelection);
	addTagsAction.setEnabled(hasSelection);
	setNotesAction.setEnabled(oneEntry);
	lastTagsMenu.setEnabled(hasSelection && !TagsDialogs::lastAddedTags.isEmpty());

	if (oneEntry) {
		ConstEntryPointer entry(entries[0]);
		const QStringList& writings(entry->writings());
		const QStringList& readings(entry->readings());

		if (!writings.isEmpty()) {
			copyWritingAction.setVisible(true);
			copyWritingAction.setText(tr("Copy \"%1\" to clipboard").arg(writings[0]));
		} else {
			copyWritingAction.setVisible(false);
		}
		if (!readings.isEmpty()) {
			copyReadingAction.setVisible(true);
			copyReadingAction.setText(tr("Copy \"%1\" to clipboard").arg(readings[0]));
		} else {
			copyReadingAction.setVisible(false);
		}
	} else {
		updateStatusMultiSelect(entries.size());
	}
}

void EntryMenu::updateStatus(const ConstEntryPointer& entry)
{
	QList<ConstEntryPointer> entries;
	if (entry) entries << entry;
	updateStatus(entries);
}

void EntryMenu::updateStatusMultiSelect(int numSelectedEntries) {
	copyWritingAction.setVisible(true);
	copyReadingAction.setVisible(true);
	copyWritingAction.setText(tr("Copy writings to clipboard"));
	copyReadingAction.setText(tr("Copy readings to clipboard"));
}

void EntryMenu::makeLastTagsMenu()
{
	lastTagsMenu.clear();
	for (int i = TagsDialogs::lastAddedTags.size(); i > 0 ; i--) {
		QAction *action = lastTagsMenu.addAction(tr("Add \"%1\"").arg(TagsDialogs::lastAddedTags[i - 1].join(", ")));
		action->setProperty("TJtagsListIndex", i - 1);
		connect(action, SIGNAL(triggered()), this, SLOT(onLastTagsActionTriggered()));
	}
}

void EntryMenu::onLastTagsActionTriggered()
{
	int tagsListIndex = qobject_cast<QAction *>(sender())->property("TJtagsListIndex").toInt();
	emit tagsHistorySelected(TagsDialogs::lastAddedTags[tagsListIndex]);
}
