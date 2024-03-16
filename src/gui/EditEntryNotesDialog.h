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

#ifndef __GUI_EDITENTRYNOTESDIALOG_H__
#define __GUI_EDITENTRYNOTESDIALOG_H__

#include "core/Entry.h"
#include "core/Preferences.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListView>
#include <QStyledItemDelegate>
#include <QTextEdit>

class EntryNotesModel : public QAbstractListModel {
    Q_OBJECT
  private:
    Entry &_entry;

  public:
    enum { NoteRole = Qt::UserRole };
    EntryNotesModel(Entry &entry, QObject *parent = 0)
        : QAbstractListModel(parent), _entry(entry) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const { return _entry.notes().size(); }
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    Entry &entry() { return _entry; }
};

class EditEntryNotesDialogEntryDelegate : public QStyledItemDelegate {
    Q_OBJECT
  public:
    EditEntryNotesDialogEntryDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {}
    virtual QString displayText(const QVariant &value, const QLocale &locale) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class EditEntryNotesDialog : public QDialog {
    Q_OBJECT
  private:
    static PreferenceItem<QByteArray> windowGeometry;

    EntryNotesModel *model;
    QListView *view;
    QTextEdit *textEdit;
    QDialogButtonBox *buttonBox;
    QPushButton *deleteButton;
    Entry::Note *currentNote;

  protected slots:
    void displayNote(const QItemSelection &selected, const QItemSelection &deselected);
    void newNote();
    void deleteNote();

  public:
    EditEntryNotesDialog(Entry &entry, QWidget *parent = 0);
    ~EditEntryNotesDialog();

    virtual void closeEvent(QCloseEvent *event);
};

#endif
