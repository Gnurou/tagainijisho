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
#include "core/Database.h"
#include "core/TextTools.h"

#include <QXmlStreamReader>
#include <QLocale>

#include <QtDebug>

typedef struct {
	int id;
	int grade;
	int strCount;
	int freq;
	int jlpt;
	QList<QString> skip;
	QList<QString> readings;
	QList<QString> meanings;
	QList<QString> nanori;
} Kanji;

#define __IGNORE if (ttype == QXmlStreamReader::Comment || ttype == QXmlStreamReader::DTD) continue;

#define PROCESS_BEGIN(tag, reader) \
	while (!reader.atEnd()) {     \
		QXmlStreamReader::TokenType ttype = reader.readNext(); \
		__IGNORE \
		if (ttype == QXmlStreamReader::EndElement && reader.name() == #tag) return false;

#define PROCESS_END return true; \
	} \
	return true;

#define DOCUMENT_BEGIN(reader) \
	if (reader.readNext() != QXmlStreamReader::StartDocument) return true; \
	while (!reader.atEnd()) {     \
		QXmlStreamReader::TokenType ttype = reader.readNext(); \
		__IGNORE
		
#define DOCUMENT_END \
		} \
		return reader.tokenType() != QXmlStreamReader::EndDocument;

#define CHARACTERS if (ttype == QXmlStreamReader::Characters) {
#define TEXT reader.text()
#define IGNORE_CHARACTERS CHARACTERS DONE

#define TAG(n) if (ttype == QXmlStreamReader::StartElement && reader.name() == #n) {
#define DONE continue; }
#define PASS(n) do reader.readNext(); while (!reader.atEnd() && !(reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == #n));
#define IGNORE_OTHER_TAGS if (ttype == QXmlStreamReader::StartElement) { \
	QStringRef tName(reader.name()); \
	do reader.readNext(); while (!reader.atEnd() && !(reader.tokenType() == QXmlStreamReader::EndElement && reader.name() == tName)); \
	continue; \
	}

#define PROCESS(part, ...) { if (process##_##part(__VA_ARGS__)) return true; }

static bool process_literal(QXmlStreamReader &reader, Kanji &kanji)
{
	PROCESS_BEGIN(literal, reader)
		CHARACTERS
			int kanjiCode = TextTools::singleCharToUnicode(TEXT.toString());
			kanji.id = kanjiCode;
		DONE
	PROCESS_END
}

static bool process_character(QXmlStreamReader &reader, Kanji &kanji)
{
	PROCESS_BEGIN(character, reader)
		TAG(literal)
			PROCESS(literal, reader, kanji)
		DONE
		IGNORE_OTHER_TAGS
		IGNORE_CHARACTERS
	PROCESS_END
}

static bool process_header(QXmlStreamReader &reader)
{
	PROCESS_BEGIN(header, reader)
		TAG(file_version)
			PASS(file_version)
		DONE
		TAG(database_version)
			PASS(database_version)
		DONE
		TAG(date_of_creation)
			PASS(date_of_creation)
		DONE
		IGNORE_CHARACTERS
	PROCESS_END
}

static bool process_kanjidic2(QXmlStreamReader &reader)
{
	PROCESS_BEGIN(kanjidic2, reader)
		TAG(character)
			Kanji kanji;
			PROCESS(character, reader, kanji)
		DONE
		TAG(header)
			PROCESS(header, reader);
		DONE
		IGNORE_CHARACTERS
	PROCESS_END
}

static bool process_main(QXmlStreamReader &reader)
{
	DOCUMENT_BEGIN(reader)
		TAG(kanjidic2)
			PROCESS(kanjidic2, reader)
		DONE
		IGNORE_CHARACTERS
	DOCUMENT_END
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QFile file("3rdparty/kanjidic2.xml");
	QXmlStreamReader reader(&file);
	file.open(QFile::ReadOnly | QFile::Text);
	if (process_main(reader)) {
		return 1;
	}
	return 0;
}
