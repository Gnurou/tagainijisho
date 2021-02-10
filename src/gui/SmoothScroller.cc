/*
 *  Copyright (C) 2009  Alexandre Courbot
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

#include "gui/SmoothScroller.h"

#include <QtDebug>

#include <QWidget>
#include <QEvent>
#include <QListView>
#include <QScrollBar>
#include <QWheelEvent>

#define TIMER_DELAY 20
#define STEP_SIZE 120

SmoothScroller::SmoothScroller(QObject *parent) : QObject(parent), _timer(), _user(0), _state(Stopped), _dest(0), _steps(0)
{
}

SmoothScroller::~SmoothScroller()
{
}

void SmoothScroller::timerEvent(QTimerEvent *event)
{
	int sbValue(_user->verticalScrollBar()->value());
	if (_dest == sbValue) {
		_timer.stop();
		event->accept();
		return;
	}
	int move(qAbs((sbValue - _dest) / 3));
	if (move == 0) move = 1;
	if (_dest < sbValue) sbValue -= move;
	else if (_dest > sbValue) sbValue += move;
	_user->verticalScrollBar()->setValue(sbValue);
	event->accept();
}

void SmoothScroller::activateOn(QAbstractScrollArea *scrollArea)
{
	if (_user == scrollArea) return;

	if (_user) deactivate();
	_user = scrollArea;
	_user->viewport()->installEventFilter(this);
	_dest = _user->verticalScrollBar()->value();
	connect(_user->verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(scrollBarReleased()));
	connect(_user->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollBarValueChanged(int)));
}

void SmoothScroller::deactivate()
{
	if (!_user) return;
	disconnect(_user->verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(scrollBarReleased()));
	disconnect(_user->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrollBarValueChanged(int)));
	_user->viewport()->removeEventFilter(this);
	_user = 0;
	_dest = 0;
}

void SmoothScroller::scrollBarReleased()
{
	int oldDest = _dest;
	_dest = _user->verticalScrollBar()->value();
	QListView *listView(qobject_cast<QListView *>(_user));
	if (listView && listView->uniformItemSizes()) {
		int itemSize(listView->itemDelegate()->sizeHint(QStyleOptionViewItem(), listView->rootIndex()).height());
		if (_dest % itemSize != 0) {
			if (_dest > oldDest) _dest = qMin(_dest + itemSize - (_dest % itemSize), _user->verticalScrollBar()->maximum());
			else _dest = qMax(_dest - _dest % itemSize, _user->verticalScrollBar()->minimum());
		}
	}
	if (_dest != oldDest && !_timer.isActive()) _timer.start(TIMER_DELAY, this);
}

void SmoothScroller::scrollBarValueChanged(int value)
{
	if (_user->verticalScrollBar()->isSliderDown()) return;
	if (!_timer.isActive()) _dest = value;
/*	if (!value) {
		QListView *listView(qobject_cast<QListView *>(_user));
		if (listView && !listView->model()->rowCount() ) _dest = 0;
	}*/
	// Did we change the value ourselves?
//	if (_timer.isActive() || _user->verticalScrollBar()->isSliderDown()) return;
//	if (value != _dest) {
//		_dest = value;
//		_timer.start(TIMER_DELAY, this);
//	}
}

bool SmoothScroller::eventFilter(QObject *src, QEvent *event)
{
	if (event->type() != QEvent::Wheel) return false;
	QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
	_steps += -wheelEvent->angleDelta().y();
	int scrollValue;
	QListView *listView(qobject_cast<QListView *>(_user));
	if (listView && listView->uniformItemSizes()) {
		// If we have a uniform list, set the step size to the items size
		// TODO cumulatively store the deltas until the ideal value is reached
		int itemSize(listView->itemDelegate()->sizeHint(QStyleOptionViewItem(), listView->rootIndex()).height());
		int displayedItems = listView->height() / itemSize;
		int nbItems = displayedItems > 3 ? 3 : displayedItems;
		scrollValue = (_steps / STEP_SIZE) * itemSize * nbItems;
		if (_dest % itemSize) {
			if (scrollValue > 0) scrollValue -= _dest % itemSize;
			else scrollValue -= _dest % itemSize - itemSize;
		}
	} else scrollValue = (_steps / STEP_SIZE) * _user->verticalScrollBar()->singleStep() * 2;
	_steps %= STEP_SIZE;
	_dest = _dest + scrollValue;
	if (_dest < _user->verticalScrollBar()->minimum()) _dest = _user->verticalScrollBar()->minimum();
	else if (_dest > _user->verticalScrollBar()->maximum()) _dest = _user->verticalScrollBar()->maximum();
	if (!_timer.isActive() && _dest != _user->verticalScrollBar()->value()) _timer.start(TIMER_DELAY, this);
	return true;
}
