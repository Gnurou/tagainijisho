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

#include "gui/ListsView.h"

#include <QSqlQuery>

ListTreeItem::ListTreeItem(int rowId, Type type) : QTreeWidgetItem(type)
{
}

void ListTreeItem::populate() {
	if (type() == ListType) {
		QSqlQuery query;
		query.prepare("select rowid, position, type from lists where parent = ? order by position");
		query.addBindValue(rowId());
		query.exec();
		int cpt = 0;
		while (query.next()) {
			if (cpt != query.value(1).toInt()) {
				qDebug() << "List index inconsistency";
			}
			if (query.value(2).isNull())
				insertChild(cpt, new ListTreeItem(query.value(0).toInt(), ListType));
			else
				insertChild(cpt, new ListTreeItem(query.value(0).toInt(), EntryType));
			++cpt;
		}
	}
}

ListsView::ListsView(QWidget *parent) : QTreeWidget(parent)
{
}
