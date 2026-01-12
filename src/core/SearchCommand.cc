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

QRegularExpression SearchCommand::_singleWordMatch(singleWordString,
                                                   QRegularExpression::UseUnicodePropertiesOption);
QRegularExpression SearchCommand::_quotedWordsMatch(quotedWordsString,
                                                    QRegularExpression::UseUnicodePropertiesOption);
QRegularExpression SearchCommand::_argMatch(argString,
                                            QRegularExpression::UseUnicodePropertiesOption);
QRegularExpression SearchCommand::_commandMatch(":(\\w+)(?:=(?:(?:" + argString +
                                                    QString::fromUtf8(")(?:[,、](?:") + argString +
                                                    "))*))?",
                                                QRegularExpression::UseUnicodePropertiesOption);

SearchCommand SearchCommand::fromString(const QString &string) {
    QRegularExpressionMatch match = _commandMatch.match(
        string, 0, QRegularExpression::NormalMatch, QRegularExpression::AnchorAtOffsetMatchOption);

    if (!match.hasMatch()) {
        qDebug("Cannot match command string!");
        return SearchCommand::invalid();
    }

    QStringList caps;
    SearchCommand ret(match.captured(1));
    int pos = string.indexOf('=');
    if (pos != -1) {
        QRegularExpressionMatchIterator i = _argMatch.globalMatch(string.mid(pos));

        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            // Whether we matched a single word or a string
            if (match.captured(1) == "")
                ret.addArgument(match.captured(2));
            else
                ret.addArgument(match.captured(1));
        }
    }
    return ret;
}

bool SearchCommand::operator==(const SearchCommand &c) const {
    return command() == c.command() && args() == c.args();
}
