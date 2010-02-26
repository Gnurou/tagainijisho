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

PreferenceItem<bool> ResultsView::smoothScrollingSetting("mainWindow/resultsView", "smoothScrolling", true);
PreferenceItem<QString> ResultsView::textFontSetting("mainWindow/resultsView", "textFont", "");
PreferenceItem<QString> ResultsView::kanaFontSetting("mainWindow/resultsView", "kanaFont", "");
PreferenceItem<QString> ResultsView::kanjiFontSetting("mainWindow/resultsView", "kanjiFont", QFont("Helvetica", 15).toString());
PreferenceItem<int> ResultsView::displayModeSetting("mainWindow/resultsView", "displayMode", EntryDelegateLayout::TwoLines);

ResultsView::ResultsView(QWidget *parent, EntryDelegateLayout *delegateLayout, bool viewOnly) : QListView(parent), helper(this), contextMenu()
{
	// If no delegate layout has been specified, let's use our private one...
	if (!delegateLayout) delegateLayout = new EntryDelegateLayout(static_cast<EntryDelegateLayout::DisplayMode>(displayModeSetting.value()), textFontSetting.value(), kanjiFontSetting.value(), kanaFontSetting.value(), this);
	connect(delegateLayout, SIGNAL(layoutHasChanged()), this, SLOT(updateLayout()));
	_delegateLayout = delegateLayout;

	setUniformItemSizes(true);
	setAlternatingRowColors(true);
	selectAllAction = contextMenu.addAction(tr("Select All"));
	selectAllAction->setShortcuts(QKeySequence::SelectAll);
	connect(selectAllAction, SIGNAL(triggered()),
			this, SLOT(selectAll()));
	setItemDelegate(new EntryDelegate(delegateLayout, this));
	// If the view is editable, the helper menu shall be enabled
	if (!viewOnly) {
		contextMenu.addSeparator();
		helper.populateMenu(&contextMenu);
	}
	updateLayout();
	setSmoothScrolling(smoothScrollingSetting.value());

	// Automatically update the view if the configuration changes
	connect(&smoothScrollingSetting, SIGNAL(valueChanged(QVariant)), this, SLOT(updateConfig(QVariant)));
	connect(&textFontSetting, SIGNAL(valueChanged(QVariant)), delegateLayout, SLOT(updateConfig(QVariant)));
	connect(&kanaFontSetting, SIGNAL(valueChanged(QVariant)), delegateLayout, SLOT(updateConfig(QVariant)));
	connect(&kanjiFontSetting, SIGNAL(valueChanged(QVariant)), delegateLayout, SLOT(updateConfig(QVariant)));
	connect(&displayModeSetting, SIGNAL(valueChanged(QVariant)), delegateLayout, SLOT(updateConfig(QVariant)));
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
	selectAllAction->setEnabled(model()->rowCount() > 0);
	QList<EntryPointer> _selectedEntries(helper.selectedEntries());
	// This is stupid, but const-safety forces us here
	QList<ConstEntryPointer> selectedEntries;
	foreach (const EntryPointer &entry, _selectedEntries) selectedEntries << entry;
	helper.updateStatus(selectedEntries);
	contextMenu.exec(mapToGlobal(event->pos()));
}

void ResultsView::updateLayout()
{
	// This is needed to force a redraw - but we loose the selection.
	QAbstractItemModel *m = model();
	setModel(0);
	setModel(m);
}

void ResultsView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	QListView::selectionChanged(selected, deselected);
	emit listSelectionChanged(selected, deselected);
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

void ResultsView::updateConfig(const QVariant &value)
{
	PreferenceRoot *from = qobject_cast<PreferenceRoot *>(sender());
	if (!from) return;
	setProperty(from->name().toLatin1().constData(), value);
}
