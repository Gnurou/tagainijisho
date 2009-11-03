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
#include "gui/HexSpinBox.h"

HexSpinBox::HexSpinBox(QWidget *parent) : QSpinBox(parent)
{
	_validator = new QRegExpValidator(QRegExp("(0x)?[0-9a-fA-f]{0,6}"), this);
}

QValidator::State HexSpinBox::validate(QString &input, int &pos) const
{
	QValidator::State valid = _validator->validate(input, pos);
	if (valid == QValidator::Invalid) return QValidator::Invalid;
	QString cleanInput(input);
	if (input.startsWith("0x")) cleanInput.remove(0, 2);
	int value = cleanInput.toInt(0, 16);
	if (value < minimum() || value > maximum()) return QValidator::Invalid;
	return valid;
}

int HexSpinBox::valueFromText(const QString &text) const
{
	if (text.isEmpty()) return 0;
	return text.toInt(0, 16);
}

QString HexSpinBox::textFromValue(int val) const
{
	if (val == 0) return "";
	return QString::number(val, 16);
}
