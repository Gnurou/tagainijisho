/*
 *  Copyright (C) 2010 Alexandre Courbot
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

#include "gui/ScrollBarSmoothScroller.h"

#include <QWheelEvent>

ScrollBarSmoothScroller::ScrollBarSmoothScroller(QObject *parent) : QObject(parent), _scrollee(0)
{
	_timer.setInterval(20);
	_timer.setSingleShot(false);
	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateAnimationState()));
}


void ScrollBarSmoothScroller::setScrollBar(QScrollBar *bar)
{
	if (_scrollee) {
		disconnect(_scrollee, SIGNAL(actionTriggered(int)), this, SLOT(onScrollBarAction(int)));
		_scrollee->removeEventFilter(this);
	}
	_scrollee = bar;
	if (_scrollee) {
		_scrollee->installEventFilter(this);
		connect(_scrollee, SIGNAL(actionTriggered(int)), this, SLOT(onScrollBarAction(int)));
		_destination = _scrollee->value();
	}
}

bool ScrollBarSmoothScroller::eventFilter(QObject *watched, QEvent *event)
{
	if (event->type() == QEvent::Wheel) {
		QWheelEvent *wEvent = static_cast<QWheelEvent *>(event);
		int wheelDelta = wEvent->delta();
		int steps = wheelDelta / 120;
		if (steps > 0) while (steps--) _scrollee->triggerAction(QAbstractSlider::SliderSingleStepSub);
		else if (steps < 0) while (steps++) _scrollee->triggerAction(QAbstractSlider::SliderSingleStepAdd);
		return true;
	}
	return false;
}

void ScrollBarSmoothScroller::onScrollBarAction(int action)
{
	switch (action) {
		case QAbstractSlider::SliderSingleStepAdd:
		case QAbstractSlider::SliderSingleStepSub:
		case QAbstractSlider::SliderPageStepAdd:
		case QAbstractSlider::SliderPageStepSub:
			qDebug() << _scrollee->singleStep() << _scrollee->sliderPosition() << _scrollee->value();
			_destination = _scrollee->sliderPosition();
			_scrollee->setSliderPosition(_scrollee->value());
			_timer.start();
			break;
		default:
			break;
	}
}

void ScrollBarSmoothScroller::updateAnimationState()
{
	int pos(_scrollee->value());
	if (_destination == pos) {
		_timer.stop();
		return;
	}
	if (qAbs(_destination - pos) == 1) pos = _destination;
	else pos += (_destination - pos) / 2;
	_scrollee->setValue(pos);
}
