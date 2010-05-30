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
#include "gui/ScrollBarSmoothScroller.h"
#include "gui/EntryDelegate.h"
#include "gui/EntriesViewHelper.h"

#include <QTreeWidget>
#include <QTreeView>
#include <QAction>
#include <QDragEnterEvent>
#include <QDragMoveEvent>

class EntryListView : public QTreeView
{
	Q_OBJECT
private:
	ScrollBarSmoothScroller scroller;
	EntriesViewHelper _helper;
	QAction _setAsRootAction, _newListAction, _rightClickNewListAction, _deleteSelectionAction, _goUpAction;
	
private slots:
	void rightClickNewList();

protected:
	virtual void startDrag(Qt::DropActions supportedActions);
	virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

public:
	EntryListView(QWidget* parent = 0, EntryDelegateLayout* delegateLayout = 0, bool viewOnly = false);
	EntriesViewHelper *helper() { return &_helper; }
	
	virtual void setModel(QAbstractItemModel *model);
	
	bool smoothScrolling() const { return verticalScrollMode() == QAbstractItemView::ScrollPerPixel; }
	void setSmoothScrolling(bool value);
	Q_PROPERTY(bool smoothScrolling READ smoothScrolling WRITE setSmoothScrolling);

	QAction *newListAction() { return &_newListAction; }
	QAction *deleteSelectionAction() { return &_deleteSelectionAction; }
	QAction *goUpAction() { return &_goUpAction; }
	
	static PreferenceItem<bool> smoothScrollingSetting;
	static PreferenceItem<QString> kanjiFontSetting;
	static PreferenceItem<QString> kanaFontSetting;
	static PreferenceItem<QString> textFontSetting;
	static PreferenceItem<int> displayModeSetting;

public slots:
	void setSelectedAsRoot();
	void newList(const QModelIndex &parent = QModelIndex());
	void deleteSelectedItems();
	void goUp();
	void onModelRootChanged(int rootId);
	
signals:
	void selectionHasChanged(const QItemSelection &selected, const QItemSelection &deselected);
	void entrySelected(const EntryPointer &entry);
};

#endif
