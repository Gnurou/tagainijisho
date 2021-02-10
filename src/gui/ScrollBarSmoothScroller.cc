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
#include <QApplication>

ScrollBarSmoothScroller::ScrollBarSmoothScroller(QObject *parent) : QObject(parent), _scrollee(0), _delta(0)
{
	_timer.setInterval(20);
	_timer.setSingleShot(false);
	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateAnimationState()));
}

ScrollBarSmoothScroller::ScrollBarSmoothScroller(QScrollBar *bar, QObject *parent) : QObject(parent), _scrollee(0), _delta(0)
{
	_timer.setInterval(20);
	_timer.setSingleShot(false);
	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateAnimationState()));
	setScrollBar(bar);
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
		_delta += wEvent->angleDelta().y();
		int steps = _delta / 120;
		_delta %= 120;
		if (!_timer.isActive()) _destination = _scrollee->value();
		_destination -= steps * _scrollee->singleStep() * QApplication::wheelScrollLines();
		if (_destination < _scrollee->minimum()) _destination = _scrollee->minimum();
		else if (_destination > _scrollee->maximum()) _destination = _scrollee->maximum();
		if (!_timer.isActive()) _timer.start();
		event->accept();
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
			_destination = _scrollee->sliderPosition();
			_scrollee->setSliderPosition(_scrollee->value());
			if (!_timer.isActive()) _timer.start();
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
	int factor = qMax(1, qAbs(_destination - pos) / 3);
	if (_destination > pos) pos += factor;
	else pos -= factor;
	_scrollee->setValue(pos);
}
