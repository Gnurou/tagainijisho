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

#ifndef __CORE_KANJIDIC2PARSER_H
#define __CORE_KANJIDIC2PARSER_H

#include "core/XmlParserHelper.h"

#include <QMap>
#include <QPair>
#include <QStringList>

class Kanjidic2Item {
  public:
    typedef enum { NONE, GENERAL, TRADIT, NELSON } RadicalType;
    uint id;
    int grade;
    int stroke_count;
    int freq;
    int jlpt;
    int heisig;
    QString skip;
    QString fourCorner;
    // TODO Add a rmgroup parameter to allow grouping of readings and meanings
    // Associate the reading types to the readings
    QMap<QString, QStringList> readings;
    // Associate the language to the meaning
    QMap<QString, QStringList> meanings;
    QStringList nanori;
    QList<QPair<quint8, RadicalType>> radicals;
    QString dictionaries;

    Kanjidic2Item() : id(0), grade(0), stroke_count(0), freq(0), jlpt(0), heisig(0) {}
};

class Kanjidic2Parser {
  private:
    static const QStringList _validReadings;
    QString _dateOfCreation;

  protected:
    QStringList languages;

  public:
    Kanjidic2Parser(const QStringList &langs) : languages(langs) {}
    virtual ~Kanjidic2Parser() {}
    bool parse(QXmlStreamReader &reader);
    const QString &dateOfCreation() const { return _dateOfCreation; }

    // This method can be overloaded by subclasses in order to implement
    // a behavior when an item is finished being parsed.
    // Returns true if the processing completed successfully, false otherwise
    virtual bool onItemParsed(Kanjidic2Item &entry) { return true; }

    // This method can be overloaded by subclasses in order to implement
    // a behavior when the DTD is read.
    // Returns true if the processing completed successfully, false otherwise
    virtual bool onDTD(const QString &dtd) { return true; }
};

#endif
