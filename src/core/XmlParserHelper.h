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

#ifndef __CORE_XMLPARSERHELPER
#define __CORE_XMLPARSERHELPER

#include <QXmlStreamReader>

bool skipTag(QXmlStreamReader &reader, QStringView tag);

#define HAS_ATTR(attr) reader.attributes().hasAttribute(attr)
#define ATTR(attr) reader.attributes().value(attr).toString()

// Open the parsing loop, and check whether we reached the end of the tag
// already
#define __TAG_BEGIN(tag)                                                                           \
    while (!reader.atEnd()) {                                                                      \
        reader.readNext();                                                                         \
        if (reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == tag)            \
            break;

#define _TAG_BEGIN(tag) __TAG_BEGIN(u## #tag)
#define TAG_BEGIN(tag) _TAG_BEGIN(tag)

// Skip all tags and characters that we did not treat, return an error if an
// unexpected token type is met
#define TAG_POST                                                                                   \
    if (reader.tokenType() == QXmlStreamReader::Comment ||                                         \
        reader.tokenType() == QXmlStreamReader::DTD)                                               \
        continue;                                                                                  \
    if (reader.tokenType() == QXmlStreamReader::StartElement) {                                    \
        if (!skipTag(reader, reader.name()))                                                       \
            return false;                                                                          \
        continue;                                                                                  \
    }                                                                                              \
    if (reader.tokenType() == QXmlStreamReader::Characters)                                        \
        continue;                                                                                  \
    qDebug("Parser error (%d): %s", reader.tokenType(),                                            \
           reader.errorString().toUtf8().constData());                                             \
    return false;                                                                                  \
    }

bool skipTag(QXmlStreamReader &reader, QStringView tag);

#define DOCUMENT_BEGIN(reader)                                                                     \
    if (reader.readNext() != QXmlStreamReader::StartDocument)                                      \
        return false;                                                                              \
    while (!reader.atEnd()) {                                                                      \
        reader.readNext();

#define DOCUMENT_END                                                                               \
    }                                                                                              \
    return reader.tokenType() == QXmlStreamReader::EndDocument;

#define CHARACTERS if (reader.tokenType() == QXmlStreamReader::Characters) {
#define COMMENT if (reader.tokenType() == QXmlStreamReader::Comment) {
#define DOCUMENT_TYPE_DEFINITION if (reader.tokenType() == QXmlStreamReader::DTD) {
#define TEXT reader.text().toString()
#define DONE                                                                                       \
    continue;                                                                                      \
    }

#define __TAG_PRE(tag)                                                                             \
    if (reader.tokenType() == QXmlStreamReader::StartElement && reader.name() == tag) {
#define _TAG_PRE(tag) __TAG_PRE(u## #tag)
#define TAG_PRE(tag) _TAG_PRE(tag)

#define TAG(tag)                                                                                   \
    TAG_PRE(tag)                                                                                   \
    TAG_BEGIN(tag)

#define ENDTAG                                                                                     \
    TAG_POST                                                                                       \
    DONE

#define PROCESS(part, ...)                                                                         \
    {                                                                                              \
        if (process##_##part(__VA_ARGS__))                                                         \
            return false;                                                                          \
    }

#endif
