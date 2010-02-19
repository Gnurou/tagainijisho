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

#ifndef __GUI_RESULTS_VIEW_H
#define __GUI_RESULTS_VIEW_H

#include "core/Preferences.h"
#include "gui/EntryMenu.h"
#include "gui/SmoothScroller.h"
#include "gui/EntryDelegate.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QListView>
#include <QMenu>
#include <QLabel>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QTextCharFormat>
#include <QPixmap>

class ResultsView;

class ResultsViewEntryMenu : public EntryMenu
{
	Q_OBJECT
public:
	void connectToResultsView(const ResultsView *const view);
};

class ResultsView : public QListView
{
	Q_OBJECT
protected:
	EntryDelegateLayout *_delegateLayout;
	ResultsViewEntryMenu entryMenu;
	QMenu contextMenu;
	QAction *selectAllAction;
	SmoothScroller _charm;
	
	virtual void startDrag(Qt::DropActions supportedActions);

protected slots:
	void studySelected();
	void unstudySelected();

	/**
	 * Add the study mark on the selected items, and
	 * set the training data to 20 success on 20 trials
	 * if score < 90.
	 */
	void markAsKnown();
	/**
	 * Reset the training scores on the selected items.
	 */
	void resetTraining();
	void setTags();
	void addTags();
	void addTags(const QStringList &tags);
	void addNote();

public:
	ResultsView(QWidget* parent = 0, EntryDelegateLayout* delegateLayout = 0, bool viewOnly = false);
	void contextMenuEvent(QContextMenuEvent *event);
	virtual void setModel(QAbstractItemModel *model);

	void setSmoothScrolling(bool value);
	EntryDelegateLayout *delegateLayout() { return _delegateLayout; }

	static PreferenceItem<bool> smoothScrolling;
	static PreferenceItem<QString> kanjiFont;
	static PreferenceItem<QString> kanaFont;
	static PreferenceItem<QString> textFont;
	static PreferenceItem<int> displayMode;

public slots:
	void updateLayout();

signals:
	// Used to abstract the selection model which may not be consistent
	// if the user changes fonts
	void listSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
};

#endif
