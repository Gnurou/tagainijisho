/*
 *  Copyright (C) 2008/2009/2010  Alexandre Courbot
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

#ifndef __GUI_RESULTS_VIEW_H
#define __GUI_RESULTS_VIEW_H

#include "core/Preferences.h"
#include "gui/EntryMenu.h"
#include "gui/SmoothScroller.h"
#include "gui/EntryDelegate.h"
#include "gui/EntriesViewHelper.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QListView>
#include <QMenu>
#include <QLabel>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QTextCharFormat>
#include <QPixmap>

class ResultsView : public QListView
{
	Q_OBJECT
protected:
	EntryDelegateLayout *_delegateLayout;
	EntriesViewHelper helper;
	QMenu contextMenu;
	QAction *selectAllAction;
	SmoothScroller _charm;
	
	virtual void startDrag(Qt::DropActions supportedActions);
	/**
	 * Reimplemented to emit the listSelectionChanged signal
	 */
	void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

public:
	ResultsView(QWidget* parent = 0, EntryDelegateLayout* delegateLayout = 0, bool viewOnly = false);
	void contextMenuEvent(QContextMenuEvent *event);

	void setSmoothScrolling(bool value);
	EntryDelegateLayout *delegateLayout() { return _delegateLayout; }

	static PreferenceItem<bool> smoothScrolling;
	static PreferenceItem<QString> kanjiFont;
	static PreferenceItem<QString> kanaFont;
	static PreferenceItem<QString> textFont;
	static PreferenceItem<int> displayMode;
	
public slots:
	/**
	 * Used to update the view in case the layout changed.
	 */
	void updateLayout();

signals:
	// Used to abstract the selection model which may not be consistent
	// if the user changes fonts
	void listSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
};

#endif
