/*
 *  Copyright (C) 2008  Alexandre Courbot
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
#include <QPainter>
#include "gui/ClickableAnimation.h"

#define NB_FRAMES 30
#define DEFAULT_SIZE 30
#define ROTATION_TIME 1500

ClickableAnimation::ClickableAnimation(QWidget *parent) : QWidget(parent), _size(DEFAULT_SIZE, DEFAULT_SIZE), currentFrame(0)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	connect(&timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
}

bool ClickableAnimation::setBaseImage(const QString &image)
{
	if (!_original.load(image)) return false;

	_frames.clear();
	for (int i = 0; i < NB_FRAMES; ++i) {
		QMatrix rotMatrix;
		rotMatrix.rotate((360 / NB_FRAMES) * i);
		QImage tmp(_original.transformed(rotMatrix));
		QRect tmpRect(tmp.rect());
		tmpRect.setSize(QSize(_original.width(), _original.height()));
		tmpRect.moveCenter(tmp.rect().center());
		_frames << QPixmap::fromImage(tmp.copy(tmpRect).scaled(_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	}
	return true;
}

QSize ClickableAnimation::sizeHint() const {
	return _size;
}

void ClickableAnimation::nextFrame()
{
	if (++currentFrame >= _frames.size()) {
		currentFrame = 0;
	}
	update();
}

void ClickableAnimation::paintEvent(QPaintEvent *event)
{
	if (_frames.isEmpty()) return;
	QPainter painter(this);
	painter.drawPixmap(0, 0, _frames[currentFrame]);
}

void ClickableAnimation::mousePressEvent(QMouseEvent *event)
{
	emit clicked();
	event->accept();
}

void ClickableAnimation::start()
{
	timer.start(ROTATION_TIME / NB_FRAMES);
}

void ClickableAnimation::stop()
{
	timer.stop();
}

bool ClickableAnimation::jumpToFrame(quint16 frame)
{
	if (frame > _frames.size()) return false;
	currentFrame = frame;
	update();
	return true;
}
