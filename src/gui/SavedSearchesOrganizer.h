/*
 *  Copyright (C) 2009  Alexandre Courbot
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

#ifndef __GUI_SETSORGANIZER_H
#define __GUI_SETSORGANIZER_H

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QTreeWidget>

class SavedSearchTreeItem : public QTreeWidgetItem {
  private:
    enum {
        SavedSearchType = (QTreeWidgetItem::UserType),
        FolderType = (QTreeWidgetItem::UserType + 1)
    };
    int _setId;
    int _position;
    /**
     * This member is only used when the copy constructor is called, and
     * contains the parent of the copied item (which is not reflected in the
     * copy). In any other case, it is null and can be ignored.
     *
     * The parent is not normally copied to avoid the item re-appearing under
     * its previous parent during a drag'n drop operation.
     */
    QTreeWidgetItem *_parentCopy;

  public:
    SavedSearchTreeItem(int setId, int position, bool isFolder, const QString &label);
    SavedSearchTreeItem(const SavedSearchTreeItem &other);
    virtual ~SavedSearchTreeItem();
    void setData(int column, int role, const QVariant &value);
    int setId() const { return _setId; }
    int position() const { return _position; }
    void setPosition(int newPos) { _position = newPos; }
    bool isFolder() const { return type() == FolderType; }
    QTreeWidgetItem *parentCopy() const { return _parentCopy; }
};

class SavedSearchesTreeWidget : public QTreeWidget {
    Q_OBJECT
  private:
    QStringList _mimeTypes;
    /// Return all the childs of the given parent item correctly typed. Returns
    /// top-level items if parent is null
    QList<SavedSearchTreeItem *> childsOf(SavedSearchTreeItem *parent);

  public:
    SavedSearchesTreeWidget(QWidget *parent = 0);

  protected:
    virtual void dropEvent(QDropEvent *event);
    virtual QStringList mimeTypes() const { return _mimeTypes; }
    virtual bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data,
                              Qt::DropAction action);
    virtual QMimeData *mimeData(const QList<QTreeWidgetItem *> items) const;
    virtual void contextMenuEvent(QContextMenuEvent *event);
    void deleteSavedSearch(SavedSearchTreeItem *item);
    void populateFolder(SavedSearchTreeItem *parent) const;

  protected slots:
    void deleteSelection();

  public slots:
    void populateRoot();
};

#include "gui/ui_SavedSearchesOrganizer.h"

class SavedSearchesOrganizer : public QDialog, private Ui::SavedSearchesOrganizer {
    Q_OBJECT
  public:
    SavedSearchesOrganizer(QWidget *parent = 0);
};

#endif
