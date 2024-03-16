/*
 *  Copyright (C) 2008/2009  Alexandre Courbot
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

#ifndef __GUI_ENTRYTYPEFILTERWIDGET_H
#define __GUI_ENTRYTYPEFILTERWIDGET_H

#include "gui/SearchFilterWidget.h"

#include <QComboBox>

/**
 * Let the user select the entry type (vocabulary of kanji) to filter.
 */
class EntryTypeFilterWidget : public SearchFilterWidget {
    Q_OBJECT
  public:
    typedef enum { All = 0, Vocabulary = 1, Kanjis = 2 } Type;

  private:
    QComboBox *_comboBox;

  protected:
    virtual void _reset();

  public:
    EntryTypeFilterWidget(QWidget *parent = 0);
    virtual QString name() const { return "entrytypeselector"; }
    virtual QString currentTitle() const { return tr("Entry type filter"); }
    virtual QString currentCommand() const;

    int type() const { return _comboBox->currentIndex(); }
    void setType(int newType) { _comboBox->setCurrentIndex(newType); }
    Q_PROPERTY(int type READ type WRITE setType)

  private slots:
    void onComboBoxChanged(int index);
};

#endif
