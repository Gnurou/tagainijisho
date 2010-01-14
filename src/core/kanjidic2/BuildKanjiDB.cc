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

static bool process_literal(QXmlStreamReader &reader)
{
	while(!reader.atEnd()) {
		QXmlStreamReader::TokenType ttype = reader.readNext();
		if (ttype == QXmlStreamReader::Characters) {
			int kanjiCode = TextTools::singleCharToUnicode(reader.text().toString());
			qDebug() << TextTools::unicodeToSingleChar(kanjiCode);
		}
		else if (ttype == QXmlStreamReader::EndElement && reader.name() == "literal") return false;
	}
	return true;
}

static bool process_character(QXmlStreamReader &reader)
{
	QXmlStreamAttributes attrs = reader.attributes();
	while(!reader.atEnd()) {
		QXmlStreamReader::TokenType ttype = reader.readNext();
		if (ttype == QXmlStreamReader::EndElement && reader.name() == "character") {
			// Character is completely read, we can write it into the database and return
			return false;
		}
		else if (ttype == QXmlStreamReader::StartElement) {
			if (reader.name() == "literal") if (process_literal(reader)) return true;
		}
	}
	return true;
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QFile file("3rdparty/kanjidic2.xml");
	QXmlStreamReader reader(&file);
	file.open(QFile::ReadOnly | QFile::Text);

	while(!reader.atEnd()) {
		QXmlStreamReader::TokenType ttype = reader.readNext();
		if (ttype == QXmlStreamReader::StartElement) {
			if (reader.name() == "character") if (process_character(reader)) {
				qDebug() << "Error while processing";
			}
		}
	}
	
	return 0;
}
