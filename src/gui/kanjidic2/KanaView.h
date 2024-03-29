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

#ifndef __GUI_KANAVIEW_H
#define __GUI_KANAVIEW_H

#include "core/Preferences.h"
#include "core/TextTools.h"
#include "gui/EntriesViewHelper.h"

#include <QAbstractTableModel>
#include <QFont>
#include <QTableView>

class KanaModel : public QAbstractTableModel {
    Q_OBJECT
  public:
    typedef enum { Hiragana = 0, Katakana = 1 } Mode;

  private:
    QFont _font;
    bool _showObsolete;
    Mode _mode;
    TextTools::KanaTable *_kanaTable;

  public:
    KanaModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;

    bool showObsolete() const { return _showObsolete; }
    void setShowObsolete(bool show);
    Mode mode() const { return _mode; }
    void setMode(Mode newMode);
    TextTools::KanaTable *kanaTable() const { return _kanaTable; }
    const QFont &font() const { return _font; }
    void setFont(const QFont &font) { _font = font; }
};

class KanaView : public QTableView {
    Q_OBJECT
  private:
    KanaModel _model;
    EntriesViewHelper _helper;

  protected:
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    virtual void startDrag(Qt::DropActions supportedActions);
    void updateLayout();

  public:
    KanaView(QWidget *parent = 0, bool viewOnly = false);
    EntriesViewHelper *helper() { return &_helper; }

    bool showObsolete() const { return _model.showObsolete(); }
    KanaModel::Mode mode() const { return _model.mode(); }

    static PreferenceItem<QString> characterFont;

  public slots:
    void setShowObsolete(bool show);
    void setMode(int newMode);
    void setCharacterFont(const QFont &font);

  signals:
    void entrySelected(const EntryPointer &entry);
};

#endif // __GUI_KANATABLE_H
