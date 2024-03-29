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

#ifndef __GUI_ENTRYLISTWIDGET_H
#define __GUI_ENTRYLISTWIDGET_H

#include "gui/SearchFilterWidget.h"
#include "gui/ui_EntryListWidget.h"

class EntryListWidget : public SearchFilterWidget, private Ui::EntryListWidget {
    Q_OBJECT
  protected:
    virtual void _reset() {}

  public:
    EntryListWidget(QWidget *parent = 0);
    virtual QString name() const { return "lists"; }
    virtual QString currentTitle() const { return tr("Lists"); }
    virtual QString currentCommand() const { return ""; }
    EntryListView *entryListView() const { return _lists; }

  public slots:
    void onModelRootChanged(const QModelIndex &idx);
};

#endif
