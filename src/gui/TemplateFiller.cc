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

#include "core/Entry.h"
#include "gui/TemplateFiller.h"

#include <QRegExp>
#include <QMetaObject>

#include <QtDebug>

QString TemplateFiller::fill(const QString &tmpl, const EntryFormatter *formatter, const EntryPointer &entry)
{
    QString ret(tmpl);
    QRegExp funcMatch("\\$\\$(\\w+)(?:\\[([^\\]]+)\\]){0,1}");
    int pos = 0, matchPos;
    while ((matchPos = funcMatch.indexIn(ret, pos)) != -1) {
        QString whole = funcMatch.cap(0);
        QString meth = funcMatch.cap(1);
        QStringList options = funcMatch.cap(2).split(',');
		QString repl;
		// Try to invoke the format method
		QMetaObject::invokeMethod(const_cast<EntryFormatter *>(formatter), QString("format" + meth).toLatin1().constData(), Qt::DirectConnection, Q_RETURN_ARG(QString, repl), Q_ARG(ConstEntryPointer, entry));
        ret.replace(matchPos, whole.size(), repl);
        pos = matchPos;
    }
    return ret;
}
