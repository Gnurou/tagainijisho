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

#include "gui/kanjidic2/KanaTable.h"
#include "core/TextTools.h"

#include <QHeaderView>

KanaTable::KanaTable(QWidget *parent) : QTableWidget(parent)
{
	QStringList hLabels(QStringList() << "a" << "i" << "u" << "e" << "o");
	QStringList vLabels(QStringList() << "" << "" << "k" << "g" << "s" << "z" << "t" << "d" << "n" << "h"
						<< "b" << "p" << "m" << "y" << "r" << "w" << "v" << "n");

	setRowCount(vLabels.size());
	setColumnCount(hLabels.size());
	setHorizontalHeaderLabels(hLabels);
	setVerticalHeaderLabels(vLabels);

	QFont f(font());
	f.setPointSize(f.pointSize() * 2);

	horizontalHeader()->setResizeMode(QHeaderView::Fixed);
	horizontalHeader()->setFont(f);
	verticalHeader()->setResizeMode(QHeaderView::Fixed);
	verticalHeader()->setFont(f);
	setShowGrid(false);


	for (int i = 0; i < KANASTABLE_NBROWS; i++) {
		bool added(false);
		for (int j = 0; j < KANASTABLE_NBCOLS; j++) {
			bool selectable = true;
			QChar c(TextTools::kanasTable[i][j]);
			if (c.unicode() == 0) selectable = false;
			const TextTools::KanaInfo kInfo(TextTools::kanaInfo(c));
			if (kInfo.size == TextTools::KanaInfo::Small || kInfo.usage == TextTools::KanaInfo::Rare) selectable = false;
			QTableWidgetItem *item = new QTableWidgetItem(selectable ? QString(c) : "");
			item->setTextAlignment(Qt::AlignCenter);
			item->setFont(f);
			if (!selectable) item->setFlags(0);
			setItem(i, j, item);
			if (selectable) added = true;
		}
		// If no character is added to the row, hide it
		if (!added) setRowHidden(i, true);
	}
	resizeColumnsToContents();
	resizeRowsToContents();
}
