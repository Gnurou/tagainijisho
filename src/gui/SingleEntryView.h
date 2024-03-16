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

#ifndef __GUI_SINGLE_ENTRY_VIEW_H
#define __GUI_SINGLE_ENTRY_VIEW_H

#include "EntryMenu.h"
#include "core/EntriesCache.h"

#include <QObject>

/**
 * Implements behavior relevant to all views involving a single entry,
 * that is a holder for the pointer to that entry, plus menu items
 * that are useful to manipulate it.
 */
class SingleEntryView : public EntryMenu {
    Q_OBJECT
  private:
    EntryPointer _entry;

  protected slots:
    virtual void copyWriting();
    virtual void copyReading();
    virtual void addToStudy();
    virtual void removeFromStudy();
    virtual void alreadyKnown();
    virtual void resetTraining();
    virtual void setTags();
    virtual void addTags();
    virtual void setNotes();
    virtual void setTagsFromHistory(const QStringList &tags);

  public:
    SingleEntryView(QObject *parent = 0);

    const EntryPointer &entry() const { return _entry; }
    void setEntry(const EntryPointer &entry);
    void populateMenu(QMenu *menu) { EntryMenu::populateMenu(menu); }
    void populateToolBar(QToolBar *bar) { EntryMenu::populateToolBar(bar); }

  public slots:
    /**
     * Update the menu options according to the current entry.
     * to be called if the entry state has changed somehow.
     */
    void updateMenuEntries();

  signals:
    /**
     * Emited every time the entry status has been changed.
     */
    void entryChanged(Entry *);

    /**
     * Emited when the watched entry changes.
     */
    void entrySet(Entry *);
};

#endif
