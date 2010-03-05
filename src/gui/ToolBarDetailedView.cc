/*
 *  Copyright (C) 2008, 2009  Alexandre Courbot
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

#include "gui/ToolBarDetailedView.h"

#include <QVBoxLayout>
#include <QApplication>

EntryDragButton::EntryDragButton(const SingleEntryView *view, QWidget *parent) : QToolButton(parent), _view(view)
{
	setAutoRaise(true);
	setIcon(QPixmap(":/images/icons/move.png"));
	setToolTip(tr("Drag the currently displayed entry"));
	connect(_view, SIGNAL(entrySet(Entry*)), this, SLOT(onViewEntryChanged(Entry*)));
	onViewEntryChanged(_view->entry().data());
}

void EntryDragButton::onViewEntryChanged(Entry *newEntry)
{
	setEnabled(newEntry != 0);
}

void EntryDragButton::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton) {
		_dragStarted = true;
		_dragPos = e->pos();
	}
	e->accept();
}

void EntryDragButton::mouseMoveEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::LeftButton) && _dragStarted) {
		if ((e->pos() - _dragPos).manhattanLength() >= QApplication::startDragDistance()) {
			_dragStarted = false;
			QDrag *drag = new QDrag(this);
			QMimeData *data = new QMimeData();
			QByteArray encodedData;
			QDataStream stream(&encodedData, QIODevice::WriteOnly);
			stream << _view->entry()->type() << _view->entry()->id();
			data->setData("tagainijisho/entry", encodedData);
			drag->setMimeData(data);
			drag->exec(Qt::CopyAction, Qt::CopyAction);
		}
	}
	e->accept();
}

void EntryDragButton::mouseReleaseEvent(QMouseEvent *e)
{
	_dragStarted = false;
	e->accept();
}

ToolBarDetailedView::ToolBarDetailedView(QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	_toolBar = new QToolBar(this);
	_toolBar->layout()->setContentsMargins(0, 0, 0, 0);
	_toolBar->setStyleSheet("QToolBar { background: none; border-style: none; border-width: 0px; margin: 0px; padding: 0px; }");
	_detailedView = new DetailedView(this);
	_detailedView->populateToolBar(_toolBar);
	_toolBar->addSeparator();
	dragButton = new EntryDragButton(_detailedView->entryView(), this);
	_toolBar->addWidget(dragButton);
	layout->addWidget(_toolBar);
	layout->addWidget(_detailedView);
}
