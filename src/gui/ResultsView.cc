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

#include "gui/TagsDialogs.h"
#include "gui/ResultsList.h"
#include "gui/ResultsView.h"
#include "gui/EditEntryNotesDialog.h"

#include <QtDebug>

#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QDateTime>
#include <QItemSelection>
#include <QPainter>
#include <QApplication>
#include <QFontMetrics>
#include <QProgressDialog>
#include <QPixmap>
#include <QScrollBar>

ResultsViewFonts *ResultsViewFonts::_instance = 0;
PreferenceItem<int> ResultsView::displayMode("mainWindow/resultsView", "displayMode", ResultsViewFonts::TwoLines);
PreferenceItem<bool> ResultsView::smoothScrolling("mainWindow/resultsView", "smoothScrolling", true);

PreferenceItem<QString> ResultsViewFonts::textFont("mainWindow/resultsView", "textFont", "");
PreferenceItem<QString> ResultsViewFonts::kanaFont("mainWindow/resultsView", "kanaFont", "");
PreferenceItem<QString> ResultsViewFonts::kanjiFont("mainWindow/resultsView", "kanjiFont", QFont("Helvetica", 15).toString());

void ResultsViewEntryMenu::connectToResultsView(const ResultsView *const view)
{
	connect(&addToStudyAction, SIGNAL(triggered()),
			view, SLOT(studySelected()));
	connect(&removeFromStudyAction, SIGNAL(triggered()),
			view, SLOT(unstudySelected()));
	connect(&alreadyKnownAction, SIGNAL(triggered()),
	        view, SLOT(markAsKnown()));
	connect(&resetTrainingAction, SIGNAL(triggered()),
	        view, SLOT(resetTraining()));
	connect(&setTagsAction, SIGNAL(triggered()),
	        view, SLOT(setTags()));
	connect(&addTagsAction, SIGNAL(triggered()),
	        view, SLOT(addTags()));
	connect(&setNotesAction, SIGNAL(triggered()),
	        view, SLOT(addNote()));
	connect(this, SIGNAL(tagsHistorySelected(const QStringList &)),
			view, SLOT(addTags(const QStringList &)));
}

EntryDelegate::EntryDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
	updateFonts();
}

QSize EntryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	int maxHeight;
	if (displayMode == ResultsViewFonts::OneLine) maxHeight = qMax(QFontMetrics(kanjiFont).height(), qMax(QFontMetrics(kanaFont).height(), QFontMetrics(textFont).height()));
	else maxHeight = qMax(QFontMetrics(kanjiFont).height(), QFontMetrics(kanaFont).height()) + QFontMetrics(textFont).height();
	return QSize(300, maxHeight);
}

void EntryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QRect rect = option.rect.adjusted(2, 0, -2, 0);
	painter->save();

	Entry *entry = index.data(ResultsList::EntryRole).value<Entry *>();

	QColor textColor;
	if (option.state & QStyle::State_Selected) {
		if (QApplication::style()->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus, &option) && !(option.state & QStyle::State_HasFocus)) textColor = option.palette.color(QPalette::Inactive, QPalette::Text);
		else if (!(option.state & QStyle::State_HasFocus)) textColor = option.palette.color(QPalette::Inactive, QPalette::HighlightedText);
		else textColor = option.palette.color(QPalette::Active, QPalette::HighlightedText);
	}
	else {
		// If the entry is trained, the background color is fixed because we know the background color
		if (entry->trained()) textColor = Qt::black;
		else textColor = option.palette.color(QPalette::Text);
	}

	// Draw the background
	QStyleOptionViewItemV4 opt = option;
	initStyleOption(&opt, index);
	QStyle *style = QApplication::style();
	style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter);

	int topLineAscent;
	if (displayMode == ResultsViewFonts::OneLine) {
		topLineAscent = qMax(qMax(QFontMetrics(kanjiFont).ascent(), QFontMetrics(kanaFont).ascent()), QFontMetrics(textFont).ascent());
	} else {
		topLineAscent = qMax(QFontMetrics(kanjiFont).ascent(), QFontMetrics(kanaFont).ascent());
	}
	QRect bbox;
	painter->setPen(textColor);
	painter->setFont(kanjiFont);
	bbox = painter->boundingRect(rect, 0, entry->writings()[0]);
	painter->drawText(QPoint(rect.left(), rect.top() + topLineAscent),
	                  entry->writings()[0]);
	QString s = " ";
	for (int i = 1; i < entry->writings().size(); i++) {
		s += entry->writings()[i];
		if (i < entry->writings().size() - 1) s += ", ";
	}
	if (!entry->readings().isEmpty()) {
		if (entry->writings().size() > 1) s += " ";
		s += "(" + entry->readings().join(", ") + ")";
	}
	if (displayMode == ResultsViewFonts::OneLine)
		s += ": ";
	painter->setFont(kanaFont);
	s = QFontMetrics(kanaFont).elidedText(s, Qt::ElideRight, rect.width() - bbox.width());
	QRect rect2(rect);
	rect2.setLeft(bbox.right());
	QRect bbox2 = painter->boundingRect(rect2, 0, s);
	painter->drawText(QPoint(bbox.right(), rect.top() + topLineAscent), s);

	s.clear();
	if (entry->meanings().size() == 1) {
		s = entry->meanings()[0];
		if (!s.isEmpty()) s[0] = s[0].toUpper();
	}
	else for (int i = 0; i < entry->meanings().size(); i++) {
		s += QString("(%1) %2 ").arg(i + 1).arg(entry->meanings()[i]);
	}
	painter->setFont(textFont);
	if (displayMode == ResultsViewFonts::OneLine) {
		s = QFontMetrics(textFont).elidedText(s, Qt::ElideRight, rect.width() - (bbox.width() + bbox2.width()));
		painter->drawText(QPoint(bbox2.right(), rect.top() + topLineAscent), s);
	} else {
		s = QFontMetrics(textFont).elidedText(s, Qt::ElideRight, rect.width());
		painter->drawText(QPoint(rect.left(), rect.top() +
				qMax(QFontMetrics(kanjiFont).height(), QFontMetrics(kanaFont).height()) + QFontMetrics(textFont).ascent()), s);
	}

	// Now display tag and notes icons if the entry has any.
	int iconPos = rect.right() - 5;
	if (!entry->notes().isEmpty()) {
		iconPos -= _notesIcon.width() + 5;
		painter->drawPixmap(iconPos, rect.top(), _notesIcon);
	}
	if (!entry->tags().isEmpty()) {
		iconPos -= _tagsIcon.width() + 5;
		painter->drawPixmap(iconPos, rect.top(), _tagsIcon);
	}
	painter->restore();
}

void EntryDelegate::updateFonts()
{
	kanjiFont = ResultsViewFonts::font(ResultsViewFonts::Kanji);
	kanaFont = ResultsViewFonts::font(ResultsViewFonts::Kana);
	textFont = ResultsViewFonts::font(ResultsViewFonts::DefaultText);
	displayMode = (ResultsViewFonts::DisplayMode) ResultsView::displayMode.value();

	_tagsIcon.load(":/images/icons/tags.png");
	_tagsIcon = _tagsIcon.scaledToHeight(15);
	_notesIcon.load(":/images/icons/notes.png");
	_notesIcon = _notesIcon.scaledToHeight(15);
}

ResultsView::ResultsView(QWidget *parent, bool viewOnly) : QListView(parent), entryMenu(), contextMenu()
{
	// Is the fonts manager instance already running?
	if (!ResultsViewFonts::_instance) ResultsViewFonts::_instance = new ResultsViewFonts();
	connect(ResultsViewFonts::_instance, SIGNAL(fontsHaveChanged()), this, SLOT(updateFonts()));

	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setUniformItemSizes(true);
	setAlternatingRowColors(true);
	selectAllAction = contextMenu.addAction(tr("Select All"));
	selectAllAction->setShortcuts(QKeySequence::SelectAll);
	connect(selectAllAction, SIGNAL(triggered()),
			this, SLOT(selectAll()));
	if (!viewOnly) {
		contextMenu.addSeparator();
		entryMenu.connectToResultsView(this);
		entryMenu.populateMenu(&contextMenu);
	}
	setItemDelegate(new EntryDelegate(this));
	updateFonts();
	setSmoothScrolling(smoothScrolling.value());
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
	QModelIndexList selection = selectionModel()->selectedIndexes();
	selectAllAction->setEnabled(model()->rowCount() > 0);
	QList<const Entry *> selectedEntries;
	foreach(const QModelIndex &index, selection)
		selectedEntries << qVariantValue<Entry *>(index.data(ResultsList::EntryRole));
	entryMenu.updateStatus(selectedEntries);
	contextMenu.exec(mapToGlobal(event->pos()));
}

void ResultsView::studySelected()
{
	QModelIndexList selection = selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Marking entries..."), tr("Abort"), 0, selection.size(), this);
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		Entry *entry = qVariantValue<Entry *>(index.data(ResultsList::EntryRole));
		entry->addToTraining();
		update(index);
	}
}

void ResultsView::unstudySelected()
{
	QModelIndexList selection = selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Marking entries..."), tr("Abort"), 0, selection.size(), this);
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		Entry *entry = qVariantValue<Entry *>(index.data(ResultsList::EntryRole));
		entry->removeFromTraining();
		update(index);
	}
}

void ResultsView::markAsKnown()
{
	QModelIndexList selection = selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Marking entries..."), tr("Abort"), 0, selection.size(), this);
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		Entry *entry = qVariantValue<Entry *>(index.data(ResultsList::EntryRole));
		if (!entry->alreadyKnown()) entry->setAlreadyKnown();
		update(index);
	}
}

void ResultsView::resetTraining()
{
	QModelIndexList selection = selectionModel()->selectedIndexes();
	// Progress bar
	QProgressDialog progressDialog(tr("Resetting entries..."), tr("Abort"), 0, selection.size(), this);
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	foreach (const QModelIndex &index, selection) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		Entry *entry = qVariantValue<Entry *>(index.data(ResultsList::EntryRole));
		entry->resetScore();
		update(index);
	}
}

void ResultsView::setTags()
{
	QModelIndexList selection = selectionModel()->selectedIndexes();
	QList<Entry *> entries;
	for (int i = 0; i < selection.size(); i++)
		entries << qVariantValue<Entry *>(selection[i].data(ResultsList::EntryRole));

	TagsDialogs::setTagsDialog(entries, this);
}

void ResultsView::addTags()
{
	QModelIndexList selection = selectionModel()->selectedIndexes();
	QList<Entry *> entries;
	for (int i = 0; i < selection.size(); i++)
		entries << qVariantValue<Entry *>(selection[i].data(ResultsList::EntryRole));

	TagsDialogs::addTagsDialog(entries, this);
}

void ResultsView::addTags(const QStringList &tags)
{
	QModelIndexList selection = selectionModel()->selectedIndexes();
	QList<Entry *> entries;
	for (int i = 0; i < selection.size(); i++)
		entries << qVariantValue<Entry *>(selection[i].data(ResultsList::EntryRole));

	// Progress bar
	QProgressDialog progressDialog(tr("Adding tags..."), tr("Abort"), 0, selection.size(), this);
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);
	int i = 0;
	foreach (Entry *entry, entries) {
		if (progressDialog.wasCanceled()) break;
		progressDialog.setValue(i++);
		entry->addTags(tags);
	}
}

void ResultsView::addNote()
{
	QModelIndexList selection = selectionModel()->selectedIndexes();
	if (selection.size() != 1) return;
	Entry *entry = qVariantValue<Entry *>(selection[0].data(ResultsList::EntryRole));
	EditEntryNotesDialog dialog(*entry, this);
	dialog.exec();
}

void ResultsView::updateFonts()
{
	EntryDelegate *delegate = qobject_cast<EntryDelegate *>(itemDelegate());
	delegate->updateFonts();
	// This is needed to force a redraw - but we loose the selection.
	QAbstractItemModel *m = model();
	setModel(0);
	setModel(m);
}

void ResultsView::setModel(QAbstractItemModel *model)
{
	QItemSelectionModel *oldSelModel = selectionModel();
	QListView::setModel(model);
	if (oldSelModel) delete oldSelModel;

	// TODO use selectionChanged instead!
	connect(selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
			this, SIGNAL(listSelectionChanged(QItemSelection,QItemSelection)));
}

ResultsViewFonts::ResultsViewFonts(QWidget *parent) : QObject(parent)
{
	// Init fonts and colors with the default values
	_font[DefaultText].fromString(textFont.value());
	_font[Kanji].fromString(kanjiFont.value());
	_font[Kana].fromString(kanaFont.value());
}

void ResultsViewFonts::_setFont(FontRole role, const QFont &font)
{
	_font[role] = font;
}

void ResultsViewFonts::_fontsChanged()
{
	emit fontsHaveChanged();
}
