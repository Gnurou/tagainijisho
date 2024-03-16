/*
 *  Copyright (C) 2011  Alexandre Courbot
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
#include "gui/TJLineEdit.h"

#include <QStyle>
#include <QToolButton>

TJLineEdit::TJLineEdit(QWidget *parent) : QLineEdit(parent) {
    _resetButton = new QToolButton(this);
    QPixmap pixmap(":/images/icons/edit-clear-locationbar-rtl.png");
    _resetButton->setIcon(QIcon(pixmap));
    _resetButton->setIconSize(pixmap.size());
    _resetButton->setCursor(Qt::ArrowCursor);
    _resetButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    _resetButton->hide();
    connect(_resetButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(this, SIGNAL(textChanged(const QString &)), this,
            SLOT(updateResetButton(const QString &)));

    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    setStyleSheet(QString("QLineEdit { padding-right: %1px; } ")
                      .arg(_resetButton->sizeHint().width() + frameWidth + 1));
}

void TJLineEdit::resizeEvent(QResizeEvent *event) {
    QSize sz = _resetButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    _resetButton->move(rect().right() - frameWidth - sz.width(),
                       (rect().bottom() + 1 - sz.height()) / 2);
}

void TJLineEdit::updateResetButton(const QString &text) {
    _resetButton->setVisible(!text.isEmpty());
}
