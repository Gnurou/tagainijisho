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
#include <QLabel>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QTextCharFormat>
#include <QPixmap>

/**
 * A list view suitable for displaying entries that come as the result of a query.
 */
class ResultsView : public QListView
{
	Q_OBJECT
protected:
	EntriesViewHelper _helper;
	QAction *selectAllAction;
	SmoothScroller _charm;
	
	void contextMenuEvent(QContextMenuEvent *event);
	virtual void startDrag(Qt::DropActions supportedActions);
	/**
	 * Reimplemented to emit the listSelectionChanged signal
	 */
	void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

public:
	ResultsView(QWidget* parent = 0, EntryDelegateLayout* delegateLayout = 0, bool viewOnly = false);
	EntriesViewHelper *helper() { return &_helper; }

	bool smoothScrolling() const { return verticalScrollMode() == QAbstractItemView::ScrollPerPixel; }
	void setSmoothScrolling(bool value);
	Q_PROPERTY(bool smoothScrolling READ smoothScrolling WRITE setSmoothScrolling);

	static PreferenceItem<bool> smoothScrollingSetting;
	static PreferenceItem<QString> kanjiFontSetting;
	static PreferenceItem<QString> kanaFontSetting;
	static PreferenceItem<QString> textFontSetting;
	static PreferenceItem<int> displayModeSetting;
	
signals:
	// Used to abstract the selection model which may not be consistent
	// if the user changes fonts
	void listSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
	void entrySelected(const EntryPointer &entry);
};

#endif
