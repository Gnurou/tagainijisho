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

#ifndef __GUI_SMOOTHSCROLLER_H
#define __GUI_SMOOTHSCROLLER_H

#include <QObject>
#include <QBasicTimer>
#include <QAbstractScrollArea>

/**
 * This class allows smooth scrolling on list views and
 * other scrollable widgets. It is largely inspired from
 * the FlickCharm example available on Qt labs.
 *
 */
class SmoothScroller : public QObject
{
	Q_OBJECT
private:
	QBasicTimer _timer;
	QAbstractScrollArea *_user;
	enum { Stopped, Scrolling } _state;
	int _dest, _steps;

protected:
	void timerEvent(QTimerEvent *event);
protected slots:
	void scrollBarReleased();
	void scrollBarValueChanged(int value);

public:
	SmoothScroller(QObject *parent = 0);
	~SmoothScroller();
	void activateOn(QAbstractScrollArea *scrollArea);
	void deactivate();
	bool eventFilter(QObject *src, QEvent *event);
};

#endif
