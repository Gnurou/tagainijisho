/*
 *  Copyright (C) 2010  Alexandre Courbot
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

#include "KanaSelector.h"

#include <QBoxLayout>
#include <QToolBar>

KanaSelector::KanaSelector(QWidget *parent) : QWidget(parent) {
    setupUi(this);

    QToolBar *_toolBar = kanaView()->helper()->defaultToolBar(this);
    // Steal the widgets from the 1st layout and insert them into the
    // toolbar
    QBoxLayout *_layout = static_cast<QBoxLayout *>(layout());
    QBoxLayout *_layout2 = static_cast<QBoxLayout *>(_layout->itemAt(0)->layout());
    while (_layout2->count())
        _toolBar->addWidget(_layout2->takeAt(0)->widget());
    delete _layout2;

    _layout->insertWidget(0, _toolBar);
}

KanaSelector::~KanaSelector() {}
