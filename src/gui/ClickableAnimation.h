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

#ifndef __GUI_CLICKABLE_ANIMATION_H
#define __GUI_CLICKABLE_ANIMATION_H

#include <QWidget>
#include <QMouseEvent>
#include <QImage>
#include <QPixmap>
#include <QTimer>

class ClickableAnimation : public QWidget {
	Q_OBJECT
private:
	QImage _original;
	QList<QPixmap> _frames;
	QSize _size;

	quint16 currentFrame;
	QTimer timer;

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);

protected slots:
	void nextFrame();

public:
	ClickableAnimation(QWidget *parent = 0);
	bool setBaseImage(const QString &image);
	virtual QSize sizeHint() const;
	void setSize(const QSize &size) { _size = size; }

public slots:
	void start();
	void stop();
	bool jumpToFrame(quint16 frame);

signals:
	void clicked();
};

#endif
