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

#include "gui/LookupUrl.h"

#include <QRegularExpression>
#include <QUrlQuery>

LookupUrl LookupUrl::parse(const QUrl &url) {
    LookupUrl result;
    if (!url.isValid() || url.scheme() != "tagaini" || !url.userInfo().isEmpty() ||
        url.port() != -1 || url.hasFragment())
        return result;

    if (url.host() == "search" && url.path().isEmpty()) {
        const auto items = QUrlQuery(url).queryItems(QUrl::FullyDecoded);
        if (items.size() != 1 || items.first().first != "q" ||
            items.first().second.trimmed().isEmpty())
            return result;
        result.type = Search;
        result.text = items.first().second;
    } else if (url.host() == "word" && !url.hasQuery()) {
        static const QRegularExpression path("\\A/([0-9]+)\\z");
        const auto match = path.match(url.path());
        if (!match.hasMatch())
            return result;
        bool ok;
        const quint32 id = match.captured(1).toUInt(&ok);
        if (!ok || id == 0)
            return result;
        result.type = Word;
        result.entryId = id;
    } else if (url.host() == "kanji" && !url.hasQuery() && url.path().startsWith('/')) {
        // Count Unicode scalar values, not UTF-16 code units: rare kanji can
        // occupy a surrogate pair. Decode the path exactly once.
        const QString character = url.path().mid(1);
        const auto codepoints = character.toUcs4();
        if (codepoints.size() != 1 || character == "/" || !QChar::isPrint(codepoints.first()) ||
            QChar::isSpace(codepoints.first()))
            return result;
        result.type = Kanji;
        result.text = character;
        result.entryId = codepoints.first();
    }
    return result;
}
