/*
 *  Copyright (C) 2010 Alexandre Courbot
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

#ifndef __GUI_KANJI_RADICALS_H
#define __GUI_KANJI_RADICALS_H

#include <QHash>
#include <QList>

/**
 * Provides a singleton that gives the list of kanji related
 * to a given radical code, and vice-versa.
 */
class KanjiRadicals {
  private:
    QHash<uint, quint8> kanji2rad;
    /// Sometimes a radical can have additional kanjis representing it.
    QHash<quint8, QList<uint>> rad2kanji;

    KanjiRadicals();

  public:
    static const KanjiRadicals &instance();
    quint8 kanji2Rad(uint kanji) const { return kanji2rad[kanji]; }
    const QList<uint> rad2Kanji(quint8 rad) const { return rad2kanji[rad]; }
};

#endif
