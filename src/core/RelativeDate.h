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

#ifndef __CORE_RELATIVEDATE_H
#define __CORE_RELATIVEDATE_H

#include "Preferences.h"

#include <QCoreApplication>
#include <QDate>
#include <QString>

/**
 * A date class that allows to express either absolute dates (ala QDate) or
 * dates relative from the current day (x days ago, weeks ago, ...).
 */
class RelativeDate {
    Q_DECLARE_TR_FUNCTIONS(RelativeDate)
  public:
    typedef enum {
        NotSet = 0,
        DaysAgo = 1,
        WeeksAgo = 2,
        MonthsAgo = 3,
        YearsAgo = 4,
        AbsoluteDate = 255
    } DateType;

  private:
    DateType _type;
    QDate _absoluteDate;
    qint64 _ago;

  public:
    static PreferenceItem<int> firstDayOfWeek;
    RelativeDate();
    /**
     * Constructs a relative date according to the string
     * representation given as parameter.
     */
    RelativeDate(const QString &string);

    DateType dateType() const { return _type; }
    void setDateType(int type) { _type = (DateType)type; }
    bool isSet() const { return dateType() != NotSet; }

    const QDate &absoluteDate() const { return _absoluteDate; }
    void setAbsoluteDate(const QDate &date) { _absoluteDate = date; }

    qint64 ago() const { return _ago; }
    void setAgo(qint64 value) { _ago = value; }

    /**
     * Returns a string representing the current date, i.e. either an absolute
     * date in ISO format, or "xx days/weeks/months/years ago", or an empty
     * string if no date is set.
     */
    QString dateString() const;
    /**
     * Sets all the date parameters from a single string built upon the model of
     * getDate().
     */
    void setDateString(const QString &date);

    /**
     * Returns a translated string of the relative date held. This should only
     * be used for display purpose ; any attempt to use this string as an
     * argument to setDateString() will fail!
     */
    QString translatedDateString() const;

    /**
     * Returns the actual date represented by this relative date,
     * according to the current date. If the string does not comply
     * to the format, nothing happens.
     */
    QDate date() const;
};

#endif // RELATIVEDATE_H
