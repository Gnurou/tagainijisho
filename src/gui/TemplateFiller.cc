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

#include "gui/TemplateFiller.h"
#include "core/Entry.h"

#include <QMetaObject>
#include <QRegularExpression>
#include <QtDebug>

QString TemplateFiller::fill(const QString &tmpl, const EntryFormatter *formatter,
                             const EntryPointer &entry) {
    // For 'T' option
    int tablePos = -1;
    int colCpt = 0;
    int tmplPos = 0;

    QString ret;
    QRegularExpression funcMatch("\\$\\$(\\w+)(?:\\[([^\\]]+)\\]){0,1}");
    QRegularExpressionMatchIterator matches = funcMatch.globalMatch(tmpl);
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        int matchPos = match.capturedStart();
        QString whole = match.captured(0);
        QString meth = match.captured(1);
        QStringList options = match.captured(2).split(',', Qt::SkipEmptyParts);
        QString repl;
        // Try to invoke the format method
        QMetaObject::invokeMethod(const_cast<EntryFormatter *>(formatter),
                                  QString("format" + meth).toLatin1().constData(),
                                  Qt::DirectConnection, Q_RETURN_ARG(QString, repl),
                                  Q_ARG(ConstEntryPointer, entry));
        // Process options
        foreach (const QString &option, options)
            switch (option[0].toLatin1()) {
            // If the result if empty, remove the block which tag is given
            case 'R': {
                if (!repl.isEmpty())
                    break;
                QString tag(option.mid(1));
                // Extend the match position to the tag we want to remove
                int nPos = tmpl.lastIndexOf("<" + tag, matchPos);
                if (nPos == -1)
                    break;
                matchPos = nPos;
                nPos = tmpl.indexOf("</" + tag, matchPos);
                if (nPos != -1)
                    nPos = tmpl.indexOf(">", nPos);
                if (nPos == -1)
                    break;
                ++nPos;
                whole = tmpl.mid(matchPos, nPos - matchPos);
                break;
            }
            // Output the result as a table cell according to the given number
            // of columns
            case 'T': {
                // Do not output cell for empty string
                if (repl.isEmpty())
                    break;
                int tPos = tmpl.lastIndexOf("<table", matchPos);
                // Found matching table?
                if (tPos != -1) {
                    int maxCols = option.mid(1).toInt();
                    QString t("<td>%1</td>");
                    if (tPos != tablePos || ++colCpt >= maxCols)
                        colCpt = 0;
                    if (colCpt == 0)
                        t = "<tr>" + t;
                    else if (colCpt == maxCols - 1)
                        t += "</tr>";
                    repl = t.arg(repl);
                }
                tablePos = tPos;
                break;
            }
            default:
                break;
            }
        ret += tmpl.mid(tmplPos, matchPos - tmplPos);
        ret += repl;
        tmplPos = match.capturedEnd();
        // If we did not output anything, remove ending newline of space
        if (repl.isEmpty())
            while (matchPos < ret.size() && (ret[matchPos] == '\n' || ret[matchPos] == ' '))
                ret.remove(matchPos, 1);
    }
    ret += tmpl.chopped(tmplPos);
    return ret;
}

QString TemplateFiller::extract(const QString &tmpl, const QStringList &parts,
                                bool includeRootText) {
    QString ret;
    int cPos = -0;
    QRegularExpression partMatch("<!-- *PART *: *(\\w+) *-->"), closePartMatch("<!-- */PART *-->");

    // Find an opening part tag
    QRegularExpressionMatchIterator matches = partMatch.globalMatch(tmpl);
    while ((matches.hasNext())) {
        QRegularExpressionMatch match = matches.next();
        int pos = match.capturedStart();
        // If the root text is to be included, do so
        if (includeRootText)
            ret += tmpl.mid(cPos, pos - cPos);

        // Find the associated closing tag
        QRegularExpressionMatch closeMatch = closePartMatch.match(tmpl, pos);
        // No closing tag, assume end of document
        if (!closeMatch.hasMatch())
            cPos = tmpl.size() - 1;
        // Otherwise remove trailing spaces and end of line after closing tag
        else {
            cPos = closeMatch.capturedEnd();
            while (cPos < tmpl.size() && (tmpl[cPos] == '\n' || tmpl[cPos] == ' '))
                ++cPos;
        }

        // Now decide whether or not to output this part
        QString partName(match.captured(1));
        if (parts.contains(partName))
            ret += tmpl.mid(pos, cPos - pos);

        pos = cPos;
    }
    // Finally include trailing text if needed
    if (includeRootText)
        ret += tmpl.mid(cPos);

    return ret;
}
