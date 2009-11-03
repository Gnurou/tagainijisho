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

#ifndef __GUI_CLICKABLE_LABEL_H
#define __GUI_CLICKABLE_LABEL_H

#include <QLabel>
#include <QMouseEvent>

class ClickableLabel : public QLabel {
	Q_OBJECT
private:

protected:
	void mousePressEvent(QMouseEvent *event);

public:
	ClickableLabel(QWidget *parent = 0) : QLabel(parent) {}
signals:
	void clicked();
};

#endif
