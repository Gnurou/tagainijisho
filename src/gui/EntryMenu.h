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

#ifndef __GUI_ENTRY_MENU_H
#define __GUI_ENTRY_MENU_H

#include "core/EntriesCache.h"

#include <QAbstractItemView>
#include <QAction>
#include <QCoreApplication>
#include <QMenu>
#include <QObject>
#include <QToolBar>

class Entry;
class QMenu;

/**
 * Offers a standard set of actions that should always be available whenever
 * an entry is displayed. Inheritors can connect slots to the actions to
 * implement the desired behavior.
 */
class EntryMenu : public QObject {
    Q_OBJECT
  protected:
    QAction copyWritingAction;
    QAction copyReadingAction;
    QAction addToStudyAction;
    QAction removeFromStudyAction;
    QAction alreadyKnownAction;
    QAction resetTrainingAction;
    QAction setTagsAction;
    QAction addTagsAction;
    QAction setNotesAction;
    QMenu lastTagsMenu;

  protected slots:
    void makeLastTagsMenu();
    void onLastTagsActionTriggered();

  public:
    EntryMenu(QObject *parent = 0);

    /**
     * Populates the given menu with the entry-related options
     */
    void populateMenu(QMenu *menu);
    /**
     * Populates the given toolbar with the entry-related options
     */
    void populateToolBar(QToolBar *bar);

    void setEnabledAll(bool enabled);

    /**
     * Enable/disable items according to the properties of the
     * entries list given as argument.
     */
    void updateStatus(const QList<ConstEntryPointer> &entries);
    /**
     * Shortcut method.
     */
    void updateStatus(const ConstEntryPointer &entry);
    /**
     * Enable/disable items based on when multiple entries
     * have been selected. Avoids the need for creating the
     * list of entries with large selections.
     */
    void updateStatusMultiSelect(int numSelectedEntries);

  signals:
    /**
     * Raised when a menu entry of the added tags history has
     * been selected. The string list contains the tags to
     * be added to the selected entry/entries.
     */
    void tagsHistorySelected(const QStringList &tags);
};

#endif
