/*
 *  Copyright (C) 2026  Alex Ehlke
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

#ifndef __GUI_LOOKUP_URL_H
#define __GUI_LOOKUP_URL_H

#include <QString>
#include <QUrl>

/// External lookup routes, independent of the dictionary and window lifetime.
struct LookupUrl {
    enum Type { Invalid, Search, Word, Kanji };
    Type type = Invalid;
    QString text;
    quint32 entryId = 0;

    static LookupUrl parse(const QUrl &url);
};

#endif
