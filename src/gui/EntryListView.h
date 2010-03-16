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
	EntryDelegateLayout *_delegateLayout;
	EntryDelegate *delegate;
	EntriesViewHelper _helper;
	QMenu contextMenu;
	QAction _newListAction, _rightClickNewListAction;
	QAction _deleteSelectionAction;

private slots:
	/**
	 * Used to update the view in case the layout changed.
	 */
	void updateLayout();
	void updateConfig(const QVariant &value);

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	virtual void startDrag(Qt::DropActions supportedActions);

protected slots:
	virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	void rightClickNewList();

public:
	EntryListView(QWidget* parent = 0, EntryDelegateLayout* delegateLayout = 0, bool viewOnly = false);
	EntryDelegateLayout *delegateLayout() { return _delegateLayout; }
	EntriesViewHelper *helper() { return &_helper; }
	
	bool smoothScrolling() const { return verticalScrollMode() == QAbstractItemView::ScrollPerPixel; }
	void setSmoothScrolling(bool value);
	Q_PROPERTY(bool smoothScrolling READ smoothScrolling WRITE setSmoothScrolling);

	QAction *newListAction(const QModelIndex &parent = QModelIndex()) { return &_newListAction; }
	QAction *deleteSelectionAction() { return &_deleteSelectionAction; }

	static PreferenceItem<bool> smoothScrollingSetting;
	static PreferenceItem<QString> kanjiFontSetting;
	static PreferenceItem<QString> kanaFontSetting;
	static PreferenceItem<QString> textFontSetting;
	static PreferenceItem<int> displayModeSetting;

public slots:
	void newList(const QModelIndex &parent = QModelIndex());
	void deleteSelectedItems();
	
signals:
	void selectionHasChanged(const QItemSelection &selected, const QItemSelection &deselected);
	void listSelected(int rowIndex);
	void entrySelected(EntryPointer entry);
};

#endif
