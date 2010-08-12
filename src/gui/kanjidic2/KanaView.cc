/*
 *  Copyright (C) 2010  Alexandre Courbot
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

#include <QtDebug>

#include "gui/kanjidic2/KanaView.h"
#include "core/TextTools.h"
#include "core/kanjidic2/Kanjidic2Entry.h"
#include "core/EntriesCache.h"

#include <QHeaderView>

KanaModel::KanaModel(QObject *parent) : QAbstractTableModel(parent)
{
	_font.setPointSize(_font.pointSize() * 2);
}

int KanaModel::rowCount(const QModelIndex &parent) const
{
	return KANASTABLE_NBROWS;
}

int KanaModel::columnCount(const QModelIndex &parent) const
{
	return KANASTABLE_NBCOLS;
}

QVariant KanaModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) return QVariant();

	if (index.row() >= rowCount()) return QVariant();
	if (index.column() >= columnCount()) return QVariant();

	QChar c(TextTools::kanasTable[index.row()][index.column()]);
	EntryPointer entry = EntryRef(KANJIDIC2ENTRY_GLOBALID, c.unicode()).get();

	switch (role) {
	case Qt::BackgroundRole:
		if (!entry || !entry->trained()) return QVariant();
		else return entry->scoreColor();
	case Entry::EntryRole:
		return QVariant::fromValue(entry);
	case Qt::DisplayRole:
		if (c.unicode() != 0) return QString(c);
		else return QVariant();
	case Qt::TextAlignmentRole:
		return Qt::AlignCenter;
	case Qt::FontRole:
		return _font;
	default:
		return QVariant();
	}
}

static QStringList hLabels(QStringList() << "a" << "i" << "u" << "e" << "o");
static QStringList vLabels(QStringList() << "" << "" << "k" << "g" << "s" << "z" << "t" << "d" << "n" << "h"
					<< "b" << "p" << "m" << "y" << "r" << "w" << "v" << "n");

QVariant KanaModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	switch (role) {
	case Qt::DisplayRole:
		switch (orientation) {
		case Qt::Horizontal:
			return hLabels[section];
		case Qt::Vertical:
			return vLabels[section];
		}
		return "";
	case Qt::SizeHintRole:
	{
		QFontMetrics fm(_font);
		return QSize(fm.maxWidth(), fm.height());
	}
	case Qt::TextAlignmentRole:
		return Qt::AlignCenter;
	case Qt::FontRole:
		return _font;
	default:
		return QVariant();
	}
}

Qt::ItemFlags KanaModel::flags(const QModelIndex &index) const
{
	QChar c(TextTools::kanasTable[index.row()][index.column()]);
	if (c.unicode() == 0) return 0;
	else return QAbstractTableModel::flags(index);
}

KanaView::KanaView(QWidget *parent) : QTableView(parent), _helper(this)
{
	setModel(&_model);

	horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	verticalHeader()->setResizeMode(QHeaderView::Fixed);
	setShowGrid(false);

	resizeColumnsToContents();
	resizeRowsToContents();
}

void KanaView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	QTableView::selectionChanged(selected, deselected);
	if (selected.isEmpty()) return;
	QModelIndex index(selected.indexes().last());
	EntryPointer entry(qvariant_cast<EntryPointer>(model()->data(index, Entry::EntryRole)));
	if (entry) emit entrySelected(entry);
}
