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
#ifndef __GUI_KANJIDIC2_KANASELECTOR_H
#define __GUI_KANJIDIC2_KANASELECTOR_H

#include <QWidget>
#include "gui/kanjidic2/ui_KanaSelector.h"

class KanaView;

namespace Ui {
    class KanaSelector;
}

class KanaSelector : public QWidget, private Ui::KanaSelector
{
    Q_OBJECT

public:
    explicit KanaSelector(QWidget *parent = 0);
    ~KanaSelector();

    KanaView *kanaView() { return _kanaView; }
};

#endif // KANASELECTOR_H
