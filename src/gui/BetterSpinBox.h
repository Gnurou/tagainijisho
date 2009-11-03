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
#ifndef __GUI_BETTERSPINBOX_H
#define __GUI_BETTERSPINBOX_H

#include <QSpinBox>
#include <QRegExpValidator>

class BetterSpinBox : public QSpinBox
{
	Q_OBJECT
private:
	QRegExpValidator *_validator;

protected:
	virtual QValidator::State validate(QString &input, int &pos) const;
	virtual int valueFromText(const QString &text) const;
	virtual QString textFromValue(int val) const;

public:
	BetterSpinBox(QWidget *parent = 0);
};

#endif
