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

#include <QtDebug>

#include "core/TextTools.h"
#include "core/kanjidic2/KanjiVGParser.h"

QRegularExpression KanjiVGParser::versionRegExp(
    "This file has been generated on (\\d\\d\\d\\d\\-\\d\\d\\-\\d\\d)");

#define TAG_KANJIVG kanjivg
#define TAG_KANJI kanji
#define ATTR_ID "id"
#define TAG_GROUP g
#define ATTR_ELEMENT "kvg:element"
#define ATTR_ORIGINAL "kvg:original"
#define ATTR_RADICAL "kvg:radical"
#define ATTR_PART "kvg:part"
#define ATTR_NUMBER "kvg:number"
#define TAG_PATH path
#define ATTR_D "d"

// Used to support the recursivity of the strokegr tag
bool KanjiVGParser::parse_strokegr(QXmlStreamReader &reader, KanjiVGItem &kanji,
                                   QStack<KanjiVGGroupItem *> gStack, quint8 &strokeCounter) {
    TAG_BEGIN(TAG_GROUP)
    TAG_PRE(TAG_GROUP)
    // For now we do not consider groups without any element
    bool hasElement(HAS_ATTR(ATTR_ELEMENT));
    bool alreadyInStack(false);
    if (hasElement) {
        int element(TextTools::singleCharToUnicode(ATTR(ATTR_ELEMENT)));
        int original(TextTools::singleCharToUnicode(ATTR(ATTR_ORIGINAL)));
        int part(ATTR(ATTR_PART).toInt());
        int number(ATTR(ATTR_NUMBER).toInt());
        KanjiVGGroupItem *group = 0;
        // Part > 1, we must find a
        // group for which element and number match
        // (in the case there is no number, both numbers
        // will be zero anyway)
        if (part > 1) {
            foreach (const KanjiVGGroupItem &tGroup, kanji.groups) {
                if (tGroup.element == element && tGroup.number == number) {
                    // Foreach won't let us loop on a non-const reference
                    group = const_cast<KanjiVGGroupItem *>(&tGroup);
                    break;
                }
            }
        } else {
            // Otherwise the group is allocated
            kanji.groups << KanjiVGGroupItem();
            group = &kanji.groups.last();
        }
        if (!group) {
            qDebug("Warning - orphan group found for kanji %x", kanji.id);
            // To prevent the group to be popped from the stack, since we
            // won't push it
            hasElement = false;
        } else {
            group->element = element;
            group->original = original;
            group->number = number;
            if (HAS_ATTR(ATTR_RADICAL)) {
                QString rad(ATTR(ATTR_RADICAL));
                if (rad == "general")
                    group->radicalType = KanjiVGGroupItem::GENERAL;
                else if (rad == "tradit")
                    group->radicalType = KanjiVGGroupItem::TRADIT;
                else if (rad == "nelson")
                    group->radicalType = KanjiVGGroupItem::NELSON;
                else
                    qDebug("Unknown radical type: %s", rad.toLatin1().constData());
            }
            if (gStack.isEmpty())
                group->isRoot = true;
            // Do not push the group if it is already in the stack
            alreadyInStack = gStack.contains(group);
            if (!alreadyInStack)
                gStack << group;
        }
    }
    if (!parse_strokegr(reader, kanji, gStack, strokeCounter))
        return false;
    if (hasElement && !alreadyInStack)
        gStack.pop();
    DONE TAG_PRE(TAG_PATH) QString path(ATTR(ATTR_D));
    kanji.strokes << KanjiVGStrokeItem();
    KanjiVGStrokeItem &stroke = kanji.strokes.last();
    stroke.path = path;
    foreach (KanjiVGGroupItem *group, gStack)
        group->pathsIndexes << strokeCounter;
    ++strokeCounter;
    TAG_BEGIN(TAG_PATH)
    ENDTAG
    TAG_POST
    return true;
}

bool KanjiVGParser::parse(QXmlStreamReader &reader) {
    reader.setNamespaceProcessing(false);
    DOCUMENT_BEGIN(reader)
    TAG(TAG_KANJIVG)
    TAG_PRE(TAG_KANJI)
    KanjiVGItem kanji;
    quint8 strokeCounter(0);
    int id(ATTR(ATTR_ID).mid(QString("kvg:kanji_").size()).toInt(0, 16));
    bool shallInsert(TextTools::isJapaneseChar(TextTools::unicodeToSingleChar(id)));
    kanji.id = id;
    TAG_BEGIN(TAG_KANJI)
    if (shallInsert) {
        TAG_PRE(TAG_GROUP)
        int element(TextTools::singleCharToUnicode(ATTR(ATTR_ELEMENT)));
        int original(TextTools::singleCharToUnicode(ATTR(ATTR_ORIGINAL)));
        QStack<KanjiVGGroupItem *> gStack;

        kanji.groups << KanjiVGGroupItem();
        KanjiVGGroupItem *group = &kanji.groups.last();
        group->element = element;
        group->original = original;
        if (HAS_ATTR(ATTR_RADICAL)) {
            QString rad(ATTR(ATTR_RADICAL));
            if (rad == "general")
                group->radicalType = KanjiVGGroupItem::GENERAL;
            else if (rad == "tradit")
                group->radicalType = KanjiVGGroupItem::TRADIT;
            else if (rad == "nelson")
                group->radicalType = KanjiVGGroupItem::NELSON;
            else
                qDebug("Unknown radical type: %s", rad.toLatin1().constData());
        }
        if (!parse_strokegr(reader, kanji, gStack, strokeCounter))
            return false;
        DONE TAG_PRE(TAG_PATH) QString path(ATTR(ATTR_D));
        kanji.strokes << KanjiVGStrokeItem();
        KanjiVGStrokeItem &stroke = kanji.strokes.last();
        stroke.path = path;
        ++strokeCounter;
        TAG_BEGIN(TAG_PATH)
        ENDTAG
    }
    TAG_POST
    onItemParsed(kanji);
    DONE ENDTAG COMMENT if (!gotVersion) {
        QRegularExpressionMatch match = versionRegExp.match(TEXT);
        if (match.hasMatch()) {
            _version = match.captured(1);
            gotVersion = true;
        }
    }
    DONE DOCUMENT_END
}
