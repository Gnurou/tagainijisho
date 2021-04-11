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

#include "core/TextTools.h"
#include "core/kanjidic2/Kanjidic2Entry.h"
#include "core/EntriesCache.h"
#include "gui/kanjidic2/KanaView.h"
#include "gui/EntryFormatter.h"

#include <QHeaderView>
#include <QDrag>
#include <QMimeData>

PreferenceItem<QString> KanaView::characterFont("kanjidic2/kanaSelector", "characterFont", QFont("Noto Sans JP", 15).toString());

KanaModel::KanaModel(QObject *parent) : QAbstractTableModel(parent), _showObsolete(false), _mode(Hiragana), _kanaTable(&TextTools::hiraganaTable)
{
	_font.fromString(KanaView::characterFont.value());
}

int KanaModel::rowCount(const QModelIndex &parent) const
{
	return KANASTABLE_NBROWS;
}

int KanaModel::columnCount(const QModelIndex &parent) const
{
	return KANASTABLE_NBCOLS;
}

void KanaModel::setShowObsolete(bool show)
{
	_showObsolete = show;
	emit layoutChanged();
}

void KanaModel::setMode(Mode newMode)
{
	_mode = newMode;
	if (_mode == Hiragana) _kanaTable = &TextTools::hiraganaTable;
	else _kanaTable = &TextTools::katakanaTable;
	emit layoutChanged();
}

QVariant KanaModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) return QVariant();

	if (index.row() >= rowCount()) return QVariant();
	if (index.column() >= columnCount()) return QVariant();

	QChar c((*_kanaTable)[index.row()][index.column()]);
	TextTools::KanaInfo info(TextTools::kanaInfo(c));
	if (c.unicode() == 0 || (!showObsolete() && info.usage == TextTools::KanaInfo::Rare)) return QVariant();

	switch (role) {
	case Qt::DisplayRole:
		return QString(c);
	case Qt::TextAlignmentRole:
		return Qt::AlignCenter;
	case Qt::FontRole:
		return _font;
	case Entry::EntryRefRole:
		return QVariant::fromValue(EntryRef(KANJIDIC2ENTRY_GLOBALID, c.unicode()));
	default:
		break;
	}

	EntryPointer entry = EntryRef(KANJIDIC2ENTRY_GLOBALID, c.unicode()).get();
	switch (role) {
	case Qt::BackgroundRole:
		if (!entry || !entry->trained()) return QVariant();
		else return EntryFormatter::scoreColor(*entry);
	case Entry::EntryRole:
		return QVariant::fromValue(entry);
	default:
		return QVariant();
	}
}

static QStringList hLabels(QStringList() << "A" << "I" << "U" << "E" << "O");
static QStringList vLabels(QStringList() << "" << "K" << "G" << "S" << "Z" << "T" << "D" << "N" << "H"
					<< "B" << "P" << "M" << "Y" << "R" << "W" << "V" << "N");

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
	default:
		return QVariant();
	}
}

Qt::ItemFlags KanaModel::flags(const QModelIndex &index) const
{
	QChar c((*_kanaTable)[index.row()][index.column()]);
	TextTools::KanaInfo info(TextTools::kanaInfo(c));
	if (c.unicode() == 0 || (!showObsolete() && info.usage == TextTools::KanaInfo::Rare)) return Qt::NoItemFlags;
	else return QAbstractTableModel::flags(index) | Qt::ItemIsDragEnabled;
}

QMimeData *KanaModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	foreach (QModelIndex index, indexes) {
		if (index.isValid()) {
			stream << data(index, Entry::EntryRefRole).value<EntryRef>();
		}
	}

	mimeData->setData("tagainijisho/entry", encodedData);
	return mimeData;
}

KanaView::KanaView(QWidget *parent, bool viewOnly) : QTableView(parent), _helper(this, 0, true, viewOnly)
{
	setModel(&_model);

	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	setShowGrid(false);

	resizeColumnsToContents();
	resizeRowsToContents();

	// Add the select all action to the context menu
	QMenu *contextMenu = helper()->contextMenu();
	QAction *selectAllAction = new QAction(tr("Select All"), contextMenu);
	selectAllAction->setShortcut(QKeySequence::SelectAll);
	connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));
	QAction *firstAction = contextMenu->actions().isEmpty() ? 0 : contextMenu->actions()[0];
	contextMenu->insertAction(firstAction, selectAllAction);

	updateLayout();
}

void KanaView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	QTableView::selectionChanged(selected, deselected);
	if (selected.isEmpty()) return;
	QModelIndex index(selected.indexes().last());
	EntryPointer entry(qvariant_cast<EntryPointer>(model()->data(index, Entry::EntryRole)));
	if (entry) emit entrySelected(entry);
}

void KanaView::updateLayout()
{
	for (int i = 0; i < _model.rowCount(); ++i) {
		bool empty = true;
		for (int j = 0; j < _model.columnCount(); ++j) {
			QChar c = (*_model.kanaTable())[i][j];
			if (c.unicode() == 0) continue;
			TextTools::KanaInfo info(TextTools::kanaInfo(c));
			if (!showObsolete() && info.usage == TextTools::KanaInfo::Rare) continue;
			empty = false;
		}
		setRowHidden(i, empty);
	}
}

void KanaView::setShowObsolete(bool show)
{
	_model.setShowObsolete(show);
	updateLayout();
}

void KanaView::setMode(int newMode)
{
	_model.setMode(static_cast<KanaModel::Mode>(newMode));
	updateLayout();
}

void KanaView::startDrag(Qt::DropActions supportedActions)
{
	QModelIndexList indexes = selectedIndexes();
	if (indexes.count() > 0) {
		QMimeData *data(model()->mimeData(indexes));
		if (!data) return;
		QDrag *drag = new QDrag(this);
		drag->setMimeData(data);
		drag->exec(supportedActions, Qt::MoveAction);
	}
}

void KanaView::setCharacterFont(const QFont &font)
{
	_model.setFont(font);
	resizeColumnsToContents();
	resizeRowsToContents();
}
