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

#ifndef __GUI_ENTRYLISTVIEW_H
#define __GUI_ENTRYLISTVIEW_H

#include "core/EntriesCache.h"
#include "gui/SmoothScroller.h"
#include "gui/EntryDelegate.h"
#include "gui/EntriesViewHelper.h"

#include <QTreeWidget>
#include <QTreeView>
#include <QAction>
#include <QMenu>
#include <QDragEnterEvent>
#include <QDragMoveEvent>

class EntryListView : public QTreeView
{
	Q_OBJECT
private:
	SmoothScroller scroller;
	EntryDelegate *delegate;
	EntriesViewHelper helper;
	QMenu contextMenu;
	QAction _newListAction;
	QAction _deleteSelectionAction;

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	virtual void startDrag(Qt::DropActions supportedActions);

protected slots:
	virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

public:
	EntryListView(QWidget *parent = 0);
	QAction *newListAction() { return &_newListAction; }
	QAction *deleteSelectionAction() { return &_deleteSelectionAction; }

	static PreferenceItem<bool> smoothScrolling;
	static PreferenceItem<QString> kanjiFont;
	static PreferenceItem<QString> kanaFont;
	static PreferenceItem<QString> textFont;
	static PreferenceItem<int> displayMode;

public slots:
	void newList();
	void deleteSelectedItems();
	
signals:
	void selectionHasChanged(const QItemSelection &selected, const QItemSelection &deselected);
	void listSelected(int rowIndex);
	void entrySelected(EntryPointer entry);
};

#endif
