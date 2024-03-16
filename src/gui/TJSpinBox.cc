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
#include "gui/TJSpinBox.h"

TJSpinBox::TJSpinBox(QWidget *parent, const QString &validRegExp, unsigned int base)
    : QSpinBox(parent), _base(base) {
    _validator = new QRegularExpressionValidator(QRegularExpression(validRegExp), this);
}

QValidator::State TJSpinBox::validate(QString &input, int &pos) const {
    QString cleanInput(input);
    if (input.startsWith(prefix()))
        cleanInput.remove(0, prefix().size());
    QValidator::State valid = _validator->validate(cleanInput, pos);
    if (valid == QValidator::Invalid)
        return QValidator::Invalid;
    int value = cleanInput.toInt(0, base());
    if (value < minimum() || value > maximum())
        return QValidator::Invalid;
    return valid;
}

int TJSpinBox::valueFromText(const QString &text) const {
    if (text.isEmpty())
        return 0;
    return text.toInt(0, base());
}

QString TJSpinBox::textFromValue(int val) const {
    if (val == 0)
        return "";
    return QString::number(val, base());
}
