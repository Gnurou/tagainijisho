/*
 *  Copyright (C) 2008, 2009, 2010  Alexandre Courbot
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

#include "gui/EntryDelegate.h"

#include <QPainter>
#include <QApplication>

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
	Entry *entry = index.data(ResultsList::EntryRole).value<Entry *>();
	if (!entry) { QStyledItemDelegate::paint(painter, option, index); return; }

	QRect rect = option.rect.adjusted(2, 0, -2, 0);
	painter->save();

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
