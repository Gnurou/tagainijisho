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
#ifndef __GUI_ELIDEDPUSHBUTTON_H
#define __GUI_ELIDEDPUSHBUTTON_H

#include <QDebug>

#include <QPushButton>
#include <QString>
#include <QFontMetrics>

template <class T>
class ElidedPushButton : public T
{
private:
	int _maxTextWidth;

protected:
	QString textToDisplay(const QString &str) {
		if (maxTextWidth() == -1) return str;
		else return QFontMetrics(T::font()).elidedText(str, Qt::ElideRight, maxTextWidth(), Qt::TextShowMnemonic);
	}

public:
	ElidedPushButton(QWidget *parent = 0) : T(parent), _maxTextWidth(-1) {}

	int maxTextWidth() const { return _maxTextWidth; }
	void setMaxTextWidth(int width) { _maxTextWidth = width; }

	void setText(const QString &str) {
		QString realText(textToDisplay(str));
		T::setText(realText);
		if (realText != str) {
			T::setToolTip(str);
		}
		else T::setToolTip("");
	}
};

#endif
