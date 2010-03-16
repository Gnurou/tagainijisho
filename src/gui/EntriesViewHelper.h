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

#ifndef __GUI_ENTRIES_VIEW_HELPER_H
#define __GUI_ENTRIES_VIEW_HELPER_H

#include "gui/EntryMenu.h"

/**
 * Provides a set of functions commonly used by views on entry lists.
 *
 * For instance, a right-click menu is automatically available with
 * frequently-used actions (study, tags, ...) to apply to the current
 * selection.
 */
class EntriesViewHelper : public EntryMenu
{
	Q_OBJECT
private:
	QAbstractItemView *_client;
	QMenu _entriesMenu;
	bool _workOnSelection;
	QAction _actionPrint, _actionPrintPreview, _actionPrintBooklet, _actionPrintBookletPreview, _actionExportTab;

protected slots:
	void studySelected();
	void unstudySelected();
	void markAsKnown();
	void resetTraining();
	void setTags();
	void addTags();
	void addTags(const QStringList &tags);
	void addNote();
	
	/**
	 * Present the user with a config dialog to setup printing. Returns true
	 * if the user confirmed the dialog, false if he cancelled it.
	 */
	bool askForPrintOptions(QPrinter &printer, const QString &title = tr("Print"));
	
public:
	/**
	 * workOnSelection defines how the print and export actions will performed. If
	 * set, printing and export will only be available if a selection is active,
	 * and will perform on this selection exclusively. If not set (the default),
	 * these actions will take place on the whole model, with an option to restrict
	 * them to the current selection.
	 */
	EntriesViewHelper(QAbstractItemView *client, bool workOnSelection = false);
	QAbstractItemView *client() const { return _client; }
	QMenu *entriesMenu() { return &_entriesMenu; }

	/**
	 * Returns a list of the currently selected entries.
	 *
	 * Using this method should be preferred over parsing the
	 * selection manually because it only return entries - if
	 * items of another kind are also selected, they are simply
	 * ignored.
	 */
	QList<EntryPointer> selectedEntries() const;
	
public slots:
	void print();
	void printPreview();
	void printBooklet();
	void printBookletPreview();

	void tabExport();
};

#endif
