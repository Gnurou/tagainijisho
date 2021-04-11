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

#include "gui/TagsDialogs.h"
#include "gui/ResultsView.h"

#include <QtDebug>

#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QDateTime>
#include <QItemSelection>
#include <QPainter>
#include <QApplication>
#include <QFontMetrics>
#include <QPixmap>
#include <QScrollBar>
#include <QDrag>
#include <QGuiApplication>

PreferenceItem<bool> ResultsView::smoothScrollingSetting("mainWindow/resultsView", "smoothScrolling", true);
PreferenceItem<QString> ResultsView::textFontSetting("mainWindow/resultsView", "textFont", "");
PreferenceItem<QString> ResultsView::kanaFontSetting("mainWindow/resultsView", "kanaFont", QFont("Noto Sans JP").toString());
PreferenceItem<QString> ResultsView::kanjiFontSetting("mainWindow/resultsView", "kanjiFont", QFont("Noto Sans JP", 15).toString());
PreferenceItem<int> ResultsView::displayModeSetting("mainWindow/resultsView", "displayMode", EntryDelegateLayout::TwoLines);

ResultsView::ResultsView(QWidget *parent, EntryDelegateLayout *delegateLayout, bool viewOnly) : QListView(parent), _helper(this, delegateLayout, false, viewOnly)
{
	setUniformItemSizes(true);
	setAlternatingRowColors(true);

	// Set the delegate
	setItemDelegate(new EntryDelegate(helper()->delegateLayout(), this));

	// Add the select all action to the context menu
	QMenu *contextMenu = helper()->contextMenu();
	selectAllAction = new QAction(tr("Select All"), contextMenu);
	selectAllAction->setShortcut(QKeySequence::SelectAll);
	connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));
	QAction *firstAction = contextMenu->actions().isEmpty() ? 0 : contextMenu->actions()[0];
	contextMenu->insertAction(firstAction, selectAllAction);

	// If no delegate has been given, consider this view is ruled by the default settings
	if (!delegateLayout) {
		_helper.setPreferenceHandler(EntriesViewHelper::TextFont, &textFontSetting);
		_helper.setPreferenceHandler(EntriesViewHelper::KanaFont, &kanaFontSetting);
		_helper.setPreferenceHandler(EntriesViewHelper::KanjiFont, &kanjiFontSetting);
		_helper.setPreferenceHandler(EntriesViewHelper::DisplayMode, &displayModeSetting);
	}

	// Scrolling
	setSmoothScrolling(smoothScrollingSetting.value());
	connect(&smoothScrollingSetting, SIGNAL(valueChanged(QVariant)), &_helper, SLOT(updateConfig(QVariant)));
}

void ResultsView::setSmoothScrolling(bool value)
{
	if (value) {
		setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
		_charm.activateOn(this);
	}
	else {
		_charm.deactivate();
		setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
	}
}

void ResultsView::contextMenuEvent(QContextMenuEvent *event)
{
	QListView::contextMenuEvent(event);
	selectAllAction->setEnabled(model()->rowCount() > 0);
	// Display of context menu is done in helper
}

void ResultsView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	QListView::selectionChanged(selected, deselected);
	emit listSelectionChanged(selected, deselected);
	if (selected.isEmpty()) {
		// Check the current selection - if we have only one index, emit it as selected
		QModelIndexList selection(selectionModel()->selectedIndexes());
		if (selection.size() != 1) return;
		EntryPointer entry(qvariant_cast<EntryPointer>(model()->data(selection[0], Entry::EntryRole)));
		if (entry) emit entrySelected(entry);
	} else {
		// Otherwise emit the last index to have been selected
		QModelIndex index(selected.indexes().last());
		EntryPointer entry(qvariant_cast<EntryPointer>(model()->data(index, Entry::EntryRole)));
		if (entry) emit entrySelected(entry);
	}
}

void ResultsView::startDrag(Qt::DropActions supportedActions)
{
	QModelIndexList indexes = selectedIndexes();
	if (indexes.count() > 0) {
		QMimeData *data(model()->mimeData(indexes));
		if (!data) return;
		QDrag *drag = new QDrag(this);
		drag->setMimeData(data);
		drag->exec(supportedActions, Qt::CopyAction);
	}
}
