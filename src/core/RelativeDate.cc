/*
 *  Copyright (C) 2009  Alexandre Courbot
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

#include "core/RelativeDate.h"
#include "core/Preferences.h"

#include <QRegExp>

PreferenceItem<int> RelativeDate::firstDayOfWeek("", "firstDayOfWeek", Qt::Monday);

RelativeDate::RelativeDate() : _type(NotSet), _ago(0) {}

RelativeDate::RelativeDate(const QString &string) { setDateString(string); }

QString RelativeDate::dateString() const {
    switch (dateType()) {
    case DaysAgo:
        if (ago() == 0)
            return "today";
        if (ago() == 1)
            return "yesterday";
        return QString("%1 days ago").arg(ago());
    case WeeksAgo:
        if (ago() == 0)
            return "this week";
        if (ago() == 1)
            return "last week";
        return QString("%1 weeks ago").arg(ago());
    case MonthsAgo:
        if (ago() == 0)
            return "this month";
        if (ago() == 1)
            return "last month";
        return QString("%1 months ago").arg(ago());
    case YearsAgo:
        if (ago() == 0)
            return "this year";
        if (ago() == 1)
            return "last year";
        return QString("%1 years ago").arg(ago());
    case AbsoluteDate:
        return absoluteDate().toString(Qt::ISODate);
    case NotSet:
    default:
        return "";
    }
}

void RelativeDate::setDateString(const QString &date) {
    static QRegExp isoDatePattern("\\d\\d\\d\\d-\\d\\d-\\d\\d");
    static QRegExp relativeDatePattern("(\\d+) (days|weeks|months|years) ago");
    static QRegExp naturalReadingPattern("(this|last) (week|month|year)");

    if (isoDatePattern.exactMatch(date)) {
        setDateType(AbsoluteDate);
        setAbsoluteDate(QDate::fromString(date, Qt::ISODate));
    } else if (naturalReadingPattern.exactMatch(date)) {
        QString when(naturalReadingPattern.cap(1));
        QString what(naturalReadingPattern.cap(2));
        if (when == "this")
            setAgo(0);
        else
            setAgo(1);
        if (what == "week")
            setDateType(WeeksAgo);
        else if (what == "month")
            setDateType(MonthsAgo);
        else if (what == "year")
            setDateType(YearsAgo);
    } else if (relativeDatePattern.exactMatch(date)) {
        setAgo(relativeDatePattern.cap(1).toInt());
        QString what(relativeDatePattern.cap(2));
        if (what == "days")
            setDateType(DaysAgo);
        else if (what == "weeks")
            setDateType(WeeksAgo);
        else if (what == "months")
            setDateType(MonthsAgo);
        else if (what == "years")
            setDateType(YearsAgo);
    } else if (date == "today" || date == "yesterday") {
        setDateType(DaysAgo);
        if (date == "today")
            setAgo(0);
        else
            setAgo(1);
    } else
        setDateType(NotSet);
}

QString RelativeDate::translatedDateString() const {
    switch (dateType()) {
    case DaysAgo:
        if (ago() == 0)
            return tr("today");
        if (ago() == 1)
            return tr("yesterday");
        return tr("%1 days ago").arg(ago());
    case WeeksAgo:
        if (ago() == 0)
            return tr("this week");
        if (ago() == 1)
            return tr("last week");
        return tr("%1 weeks ago").arg(ago());
    case MonthsAgo:
        if (ago() == 0)
            return tr("this month");
        if (ago() == 1)
            return tr("last month");
        return tr("%1 months ago").arg(ago());
    case YearsAgo:
        if (ago() == 0)
            return tr("this year");
        if (ago() == 1)
            return tr("last year");
        return tr("%1 years ago").arg(ago());
    case AbsoluteDate:
        return absoluteDate().toString(Qt::DefaultLocaleShortDate);
    case NotSet:
    default:
        return "";
    }
}

QDate RelativeDate::date() const {
    QDate today(QDate::currentDate());
    switch (dateType()) {
    case DaysAgo:
        return today.addDays(-ago());
    case WeeksAgo:
        return today.addDays((-today.dayOfWeek() + firstDayOfWeek.value()) % -7)
            .addDays(-7 * ago());
    case MonthsAgo:
        return today.addDays(-today.day() + 1).addMonths(-ago());
    case YearsAgo:
        return QDate(today.year(), 1, 1).addYears(-ago());
    case AbsoluteDate:
        return absoluteDate();
    case NotSet:
    default:
        return QDate();
    }
}
