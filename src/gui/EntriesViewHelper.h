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
#include "gui/EntryDelegate.h"

#include <QMenu>

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
public:
	typedef enum { TextFont, KanaFont, KanjiFont, DisplayMode, MAX_PREF} Preference;

private:
	QAbstractItemView *_client;
	EntryDelegateLayout *_delegateLayout;
	QMenu _entriesMenu;
	bool _workOnSelection;
	QAction _actionPrint, _actionPrintPreview, _actionPrintBooklet, _actionPrintBookletPreview, _actionExportTab;
	QVector<PreferenceRoot *> prefRefs;
	QMenu _contextMenu;

	/**
	 * Parses all the given indexes recursively and returns then in the right
	 * order.
	 */
	QModelIndexList getAllIndexes(const QModelIndexList& indexes, QSet<QModelIndex>& alreadyIn);
	QModelIndexList getAllIndexes(const QModelIndexList& indexes);
	
	QModelIndexList getEntriesToProcess(bool limitToSelection = false);
	
public slots:
	/**
	 * Used to update the view in case the layout changed.
	 */
	void updateLayout();
	void updateConfig(const QVariant &value);


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
	 * @arg workOnSelection defines how the print and export actions will performed. If
	 * set, printing and export will only be available if a selection is active,
	 * and will perform on this selection exclusively. If not set (the default),
	 * these actions will take place on the whole model, with an option to restrict
	 * them to the current selection.
	 *
	 * @arg viewOnly sets whether the view this helper acts on is not editable. If so,
	 * options that let the displayed entries be modified will be omited.
	 */
	EntriesViewHelper(QAbstractItemView* client, EntryDelegateLayout* delegateLayout = 0, bool workOnSelection = false, bool viewOnly = false);
	
	QAbstractItemView *client() const { return _client; }
	/**
	 * Returns the entries option menu (export, print, ...)
	 */
	QMenu *entriesMenu() { return &_entriesMenu; }
	void setPreferenceHandler(Preference pref, PreferenceRoot *ref);
	EntryDelegateLayout *delegateLayout() { return _delegateLayout; }
	/**
	 * Returns the context menu (study, tags, notes, ...)
	 */
	QMenu *contextMenu() { return &_contextMenu; }

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
