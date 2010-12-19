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

#include "core/EntryListModel.h"
#include "gui/EntryListWidget.h"

#include <QToolButton>

EntryListWidget::EntryListWidget(QWidget *parent) : SearchFilterWidget(parent)
{
	setupUi(this);
	_titleLabel->setVisible(false);
	QFont titleFont;
	titleFont.setPointSize(titleFont.pointSize() + 2);
	titleFont.setBold(true);
	_titleLabel->setFont(titleFont);
	
	QToolBar *_toolBar = entryListView()->helper()->defaultToolBar(this);

	_toolBar->addAction(entryListView()->goUpAction());
	_toolBar->addAction(entryListView()->newListAction());
	_toolBar->addAction(entryListView()->deleteSelectionAction());
	
	vLayout->insertWidget(0, _toolBar);
	
	connect(entryListView(), SIGNAL(rootHasChanged(QModelIndex)), this, SLOT(onModelRootChanged(QModelIndex)));
}

void EntryListWidget::onModelRootChanged(const QModelIndex &idx)
{
	if (!idx.isValid()) {
		_titleLabel->setText("");
		_titleLabel->setVisible(false);
	} else {
		_titleLabel->setText(idx.data(Qt::DisplayRole).toString());
		_titleLabel->setVisible(true);
	}
}
