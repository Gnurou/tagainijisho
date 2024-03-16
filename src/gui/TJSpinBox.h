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
#ifndef __GUI_TJSPINBOX_H
#define __GUI_TJSPINBOX_H

#include <QRegularExpressionValidator>
#include <QSpinBox>

class TJSpinBox : public QSpinBox {
    Q_OBJECT
  private:
    QRegularExpressionValidator *_validator;
    unsigned int _base;

  protected:
    virtual QValidator::State validate(QString &input, int &pos) const;
    virtual int valueFromText(const QString &text) const;
    virtual QString textFromValue(int val) const;

  public:
    TJSpinBox(QWidget *parent = 0, const QString &validRegExp = "[0-9]*", unsigned int base = 10);
    unsigned int base() const { return _base; }
};

#endif
