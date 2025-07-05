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

#ifndef __CORE_KANJIVGPARSER_H
#define __CORE_KANJIVGPARSER_H

#include "core/XmlParserHelper.h"

#include <QMap>
#include <QRegExp>
#include <QStack>
#include <QStringList>

class KanjiVGGroupItem {
  public:
    typedef enum { NONE, GENERAL, TRADIT, NELSON, JIS } RadicalType;
    int number;
    int element;
    int original;
    bool isRoot;
    RadicalType radicalType;
    // List of stroke numbers that are part of this group
    QList<quint8> pathsIndexes;
    KanjiVGGroupItem() : number(0), element(0), original(0), isRoot(false), radicalType(NONE) {}
};

class KanjiVGStrokeItem {
  public:
    int type;
    QString path;

    KanjiVGStrokeItem() : type(0) {}
};

class KanjiVGItem {
  public:
    int id;
    QList<KanjiVGGroupItem> groups;
    QList<KanjiVGStrokeItem> strokes;
    KanjiVGItem() : id(0) {}
};

class KanjiVGParser {
  private:
    static QRegExp versionRegExp;

    QString _version;
    bool gotVersion;
    bool parse_strokegr(QXmlStreamReader &reader, KanjiVGItem &kanji,
                        QStack<KanjiVGGroupItem *> gStack, quint8 &strokeCounter);

  public:
    KanjiVGParser() : gotVersion(false) {}
    virtual ~KanjiVGParser() {}
    bool parse(QXmlStreamReader &reader);
    const QString &version() const { return _version; }

    // This method can be overloaded by subclasses in order to implement
    // a behavior when an item is finished being parsed.
    // Returns true if the processing completed successfully, false otherwise
    virtual bool onItemParsed(KanjiVGItem &kanji) { return true; }
};

#endif
