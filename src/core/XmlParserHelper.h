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

bool skipTag(QXmlStreamReader& reader, const QStringRef &tag);

#define HAS_ATTR(x) reader.attributes().hasAttribute(x)
#define ATTR(x) reader.attributes().value(x).toString()

// Open the parsing loop, and check whether we reached the end of the tag already
#define __TAG_BEGIN(tag) \
	while (!reader.atEnd()) {     \
		reader.readNext(); \
		if (reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == tag) break;

#define TAG_BEGIN(tag) __TAG_BEGIN(#tag)

// Skip all tags and characters that we did not treat, return an error if an unexpected token type is met
#define TAG_POST if (reader.tokenType() == QXmlStreamReader::Comment || reader.tokenType() == QXmlStreamReader::DTD) continue; \
	if (reader.tokenType() == QXmlStreamReader::StartElement) { if (!skipTag(reader, reader.name())) return false; continue; } \
	if (reader.tokenType() == QXmlStreamReader::Characters) continue; \
	return false; \
	}

bool skipTag(QXmlStreamReader& reader, const QStringRef &tag);

#define DOCUMENT_BEGIN(reader) \
	if (reader.readNext() != QXmlStreamReader::StartDocument) return false; \
	while (!reader.atEnd()) {     \
		reader.readNext();
		
#define DOCUMENT_END } \
		return reader.tokenType() == QXmlStreamReader::EndDocument;

#define CHARACTERS if (reader.tokenType() == QXmlStreamReader::Characters) {
#define COMMENT if (reader.tokenType() == QXmlStreamReader::Comment) {
#define TEXT reader.text().toString()
#define DONE continue; }

#define TAG_PRE(n) if (reader.tokenType() == QXmlStreamReader::StartElement && reader.name() == #n) {

#define TAG(n) TAG_PRE(n) \
	TAG_BEGIN(n)
	
#define ENDTAG TAG_POST \
	DONE

#define PROCESS(part, ...) { if (process##_##part(__VA_ARGS__)) return false; }

#endif
