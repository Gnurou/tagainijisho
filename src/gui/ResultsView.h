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

class ResultsViewFonts : public QObject
{
	Q_OBJECT
public:
	typedef enum { DefaultText, Kana, Kanji, MAX_FONTS } FontRole;
	typedef enum { OneLine = 0, TwoLines, MAX_MODES } DisplayMode;
private:
	static ResultsViewFonts *_instance;

	QFont _font[MAX_FONTS];

	void _setFont(FontRole role, const QFont &font);
	void _fontsChanged();
	QFont _defaultFont(FontRole role) const;

public:
	ResultsViewFonts(QWidget *parent = 0);

	static void setFont(FontRole role, const QFont &font) { _instance->_setFont(role, font); }
	static void fontsChanged() { _instance->_fontsChanged(); }
	static const QFont &font(FontRole role) { return _instance->_font[role]; }

	static PreferenceItem<QString> kanjiFont;
	static PreferenceItem<QString> kanaFont;
	static PreferenceItem<QString> textFont;

signals:
	void fontsHaveChanged();

friend class ResultsView;
};

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
	ResultsViewEntryMenu entryMenu;
	QMenu contextMenu;
	QAction *selectAllAction;
	SmoothScroller _charm;

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
	ResultsView(QWidget *parent = 0, bool viewOnly = false);
	void contextMenuEvent(QContextMenuEvent *event);
	virtual void setModel(QAbstractItemModel *model);

	void setSmoothScrolling(bool value);

	static PreferenceItem<int> displayMode;
	static PreferenceItem<bool> smoothScrolling;

public slots:
	void updateFonts();

signals:
	// Used to abstract the selection model which may not be consistent
	// if the user changes fonts
	void listSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
};

#endif
