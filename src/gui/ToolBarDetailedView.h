/*
 *  Copyright (C) 2008, 2009  Alexandre Courbot
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

#ifndef __GUI_TOOLBARDETAILEDVIEW_H_
#define __GUI_TOOLBARDETAILEDVIEW_H_

#include <QToolBar>
#include <QToolButton>

#include "gui/DetailedView.h"

class EntryDragButton : public QToolButton {
    Q_OBJECT
  private:
    const SingleEntryView *const _view;
    bool _dragStarted;
    QPoint _dragPos;

  protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

  protected slots:
    void onViewEntryChanged(Entry *newEntry);

  public:
    EntryDragButton(const SingleEntryView *view, QWidget *parent = 0);
};

/// TODO remove and layout things manually?
class ToolBarDetailedView : public QWidget {
    Q_OBJECT
  private:
    QToolBar *_toolBar;
    DetailedView *_detailedView;
    EntryDragButton *dragButton;

  public:
    ToolBarDetailedView(QWidget *parent = 0);
    DetailedView *detailedView() { return _detailedView; }
    QToolBar *toolBar() { return _toolBar; }
};

#endif
