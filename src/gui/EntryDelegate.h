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

class EntryDelegate : public QStyledItemDelegate
{
	Q_OBJECT
protected:
	QFont textFont, kanaFont, kanjiFont;
	ResultsViewFonts::DisplayMode displayMode;
	QPixmap _tagsIcon;
	QPixmap _notesIcon;

public:
	EntryDelegate(QObject *parent = 0);
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const;
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void updateFonts();
};

#endif
