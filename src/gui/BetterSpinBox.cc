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

#include "gui/BetterSpinBox.h"

BetterSpinBox::BetterSpinBox(QWidget *parent) : QSpinBox(parent)
{
	_validator = new QRegExpValidator(QRegExp("[0-9a-fA-f]{0,6}"), this);
}

QValidator::State BetterSpinBox::validate(QString &input, int &pos) const
{
	QValidator::State valid = _validator->validate(input, pos);
	if (valid == QValidator::Invalid) return QValidator::Invalid;
	int value = input.toInt();
	if (value < minimum() || value > maximum()) return QValidator::Invalid;
	return valid;
}

int BetterSpinBox::valueFromText(const QString &text) const
{
	if (text.isEmpty()) return 0;
	return text.toInt();
}

QString BetterSpinBox::textFromValue(int val) const
{
	if (val == 0) return "";
	return QString::number(val, 10);
}
