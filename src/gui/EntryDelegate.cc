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

EntryDelegateLayout::EntryDelegateLayout(QObject* parent, EntryDelegateLayout::DisplayMode displayMode, const QString& textFont, const QString& kanjiFont, const QString& kanaFont) : QObject(parent), _displayMode(displayMode)
{
	setTextFont(textFont);
	setKanjiFont(kanjiFont);
	setKanaFont(kanaFont);
}

void EntryDelegateLayout::setFont(FontRole role, const QFont &font)
{
	_font[role] = font;
	emit layoutHasChanged();
}

void EntryDelegateLayout::setDisplayMode(DisplayMode mode)
{
	_displayMode = mode;
	emit layoutHasChanged();
}

void EntryDelegateLayout::updateConfig(const QVariant &value)
{
	PreferenceRoot *from = qobject_cast<PreferenceRoot *>(sender());
	if (!from) return;
	setProperty(from->name().toLatin1().constData(), value);
}

EntryDelegate::EntryDelegate(EntryDelegateLayout* dLayout, QObject* parent) : QStyledItemDelegate(parent), layout(dLayout), _hiddenIcons(0)
{
	_tagsIcon.load(":/images/icons/tags.png");
	_tagsIcon = _tagsIcon.scaledToHeight(15);
	_notesIcon.load(":/images/icons/notes.png");
	_notesIcon = _notesIcon.scaledToHeight(15);
	_listsIcon.load(":/images/icons/list.png");
	_listsIcon = _listsIcon.scaledToHeight(15);
}

QSize EntryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	int maxHeight = -1;
	if (index.isValid()) {
		QVariant sizeHint(index.model()->data(index, Qt::SizeHintRole));
		if (!sizeHint.isNull() && sizeHint.type() == QVariant::Size) maxHeight = sizeHint.toSize().height();
	}
	if (maxHeight < 0) {
		if (layout->displayMode() == EntryDelegateLayout::OneLine) maxHeight = qMax(QFontMetrics(layout->kanjiFont()).height(), qMax(QFontMetrics(layout->kanaFont()).height(), QFontMetrics(layout->textFont()).height()));
		else maxHeight = qMax(QFontMetrics(layout->kanjiFont()).height(), QFontMetrics(layout->kanaFont()).height()) + QFontMetrics(layout->textFont()).height();
	}
	// This margin is added by the paint method
	maxHeight += 4;
	return QSize(300, maxHeight);
}

void EntryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	EntryPointer entry = index.data(Entry::EntryRole).value<EntryPointer>();
	if (!entry) { QStyledItemDelegate::paint(painter, option, index); return; }

	QRect rect = option.rect.adjusted(2, 2, -2, 2);
	//QRect rect = option.rect.adjusted(2, 0, -2, 0);
	painter->save();

	QColor textColor;
	if (option.state & QStyle::State_Selected) {
		if (QApplication::style()->styleHint(QStyle::SH_ItemView_ChangeHighlightOnFocus, &option) && !(option.state & QStyle::State_HasFocus)) textColor = option.palette.color(QPalette::Inactive, QPalette::Text);
		else if (!(option.state & QStyle::State_HasFocus)) textColor = option.palette.color(QPalette::Inactive, QPalette::HighlightedText);
		else textColor = option.palette.color(QPalette::Active, QPalette::HighlightedText);
	}
	else {
		textColor = option.palette.color(QPalette::Text);
	}

	// Draw the background
	QStyleOptionViewItem opt = option;
	initStyleOption(&opt, index);
	QStyle *style = QApplication::style();
	style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter);

	int topLineAscent;
	if (layout->displayMode() == EntryDelegateLayout::OneLine) {
		topLineAscent = qMax(qMax(QFontMetrics(layout->kanjiFont()).ascent(), QFontMetrics(layout->kanaFont()).ascent()), QFontMetrics(layout->textFont()).ascent());
	} else {
		topLineAscent = qMax(QFontMetrics(layout->kanjiFont()).ascent(), QFontMetrics(layout->kanaFont()).ascent());
	}
	QRect bbox;
	painter->setPen(textColor);
	painter->setFont(layout->kanjiFont());
	QString mainRepr(entry->mainRepr());
	QStringList writings(entry->writings());
	QStringList readings(entry->readings());
	bbox = painter->boundingRect(rect, 0, mainRepr);
	painter->drawText(QPoint(rect.left(), rect.top() + topLineAscent),
	          mainRepr);
	// Used for alternate writings and readings
	QString s = " ";
	if (layout->displayMode() == EntryDelegateLayout::OneLine) {
		// The first reading is only needed if the main representation is part of the writings
		if (!readings.isEmpty() && writings.contains(mainRepr)) s += "(" + readings[0] + "): ";
	} else {
		writings.removeAll(mainRepr);
		readings.removeAll(mainRepr);
		s += writings.join(", ");
		if (!readings.isEmpty()) {
			if (!writings.isEmpty()) s += " ";
			s += "(" + readings.join(", ") + ")";
		}
	}
	painter->setFont(layout->kanaFont());
	s = QFontMetrics(layout->kanaFont()).elidedText(s, Qt::ElideRight, rect.width() - bbox.width());
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
	painter->setFont(layout->textFont());
	if (layout->displayMode() == EntryDelegateLayout::OneLine) {
		s = QFontMetrics(layout->textFont()).elidedText(s, Qt::ElideRight, rect.width() - (bbox.width() + bbox2.width()));
		painter->drawText(QPoint(bbox2.right(), rect.top() + topLineAscent), s);
	} else {
		s = QFontMetrics(layout->textFont()).elidedText(s, Qt::ElideRight, rect.width());
		painter->drawText(QPoint(rect.left(), rect.top() +
				qMax(QFontMetrics(layout->kanjiFont()).height(), QFontMetrics(layout->kanaFont()).height()) + QFontMetrics(layout->textFont()).ascent()), s);
	}

	// Now display property icons if the entry has any.
	int iconPos = rect.right() - 5;
	if (!entry->notes().isEmpty() && !isHidden(NOTES_ICON)) {
		iconPos -= _notesIcon.width() + 5;
		painter->drawPixmap(iconPos, rect.top(), _notesIcon);
	}
	if (!entry->tags().isEmpty() && !isHidden(TAGS_ICON)) {
		iconPos -= _tagsIcon.width() + 5;
		painter->drawPixmap(iconPos, rect.top(), _tagsIcon);
	}
	if (!entry->lists().isEmpty() && !isHidden(LISTS_ICON)) {
		iconPos -= _listsIcon.width() + 5;
		painter->drawPixmap(iconPos, rect.top(), _listsIcon);
	}
	painter->restore();
}
