/*
 *  Copyright (C) 2008  Alexandre Courbot
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

#include "core/SearchCommand.h"

#include <QtDebug>

SearchCommand SearchCommand::_invalid("invalid");

static QString singleWordString = "(-?[\\w\\*\\?\\-\\._:]+)";
static QString quotedWordsString = "\"([^\"]*)\"";
static QString argString = singleWordString + "|" + quotedWordsString;

QRegExp SearchCommand::_singleWordMatch(singleWordString);
QRegExp SearchCommand::_quotedWordsMatch(quotedWordsString);
QRegExp SearchCommand::_argMatch(argString);
QRegExp SearchCommand::_commandMatch(":(\\w+)(?:=(?:(?:" + argString +
                                     QString::fromUtf8(")(?:[,、](?:") + argString + "))*))?");

SearchCommand SearchCommand::fromString(const QString &string) {
    if (!_commandMatch.exactMatch(string)) {
        qDebug("Cannot match command string!");
        return SearchCommand::invalid();
    }
    QStringList caps;
    SearchCommand ret(_commandMatch.cap(1));
    int pos = string.indexOf('=');
    if (pos != -1)
        while ((pos = _argMatch.indexIn(string, pos)) != -1) {
            // Whether we matched a single word or a string
            if (_argMatch.cap(1) == "")
                ret.addArgument(_argMatch.cap(2));
            else
                ret.addArgument(_argMatch.cap(1));
            pos += _argMatch.matchedLength();
        }
    return ret;
}

bool SearchCommand::operator==(const SearchCommand &c) const {
    return command() == c.command() && args() == c.args();
}
