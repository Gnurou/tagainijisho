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

#ifndef __GUI_ENTRYDELEGATE_H
#define __GUI_ENTRYDELEGATE_H

#include "core/Entry.h"
#include "gui/ResultsList.h"
// TODO Not good!
#include "gui/ResultsView.h"

#include <QStyledItemDelegate>

class EntryDelegateLayout : public QObject
{
	Q_OBJECT
public:
	typedef enum { DefaultText, Kana, Kanji, MAX_FONTS } FontRole;
	typedef enum { OneLine = 0, TwoLines, MAX_MODES } DisplayMode;
private:
	static EntryDelegateLayout *_instance;

	QFont _font[MAX_FONTS];

	void _setFont(FontRole role, const QFont &font);
	void _fontsChanged();
	QFont _defaultFont(FontRole role) const;

public:
	EntryDelegateLayout(QWidget *parent = 0);

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

class EntryDelegate : public QStyledItemDelegate
{
	Q_OBJECT
protected:
	QFont textFont, kanaFont, kanjiFont;
	EntryDelegateLayout::DisplayMode displayMode;
	QPixmap _tagsIcon;
	QPixmap _notesIcon;

public:
	EntryDelegate(QObject *parent = 0);
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const;
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void updateLayout();
};

#endif
