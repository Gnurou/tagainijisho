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

#include <QStringList>
#include <QMap>

class Kanjidic2Item {
public:
	int id;
	int grade;
	int stroke_count;
	int freq;
	int jlpt;
	QString skip;
	QString fourCorner;
	// TODO Add a rmgroup parameter to allow grouping of readings and meanings
	// Associate the reading types to the readings
	QMap<QString, QStringList> readings;
	// Associate the language to the meaning
	QMap<QString, QStringList> meanings;
	QStringList nanori;
	
	Kanjidic2Item() : id(0), grade(0), stroke_count(0), freq(0), jlpt(0) {}
};

class Kanjidic2Parser {
private:
	static const QStringList _validReadings;
	
public:
	static QStringList languages;
	
	Kanjidic2Parser() {}
	virtual ~Kanjidic2Parser() {}
	bool parse(QXmlStreamReader &reader);
	
	// This method can be overloaded by subclasses in order to implement
	// a behavior when an item is finished being parsed.
	// Returns true if the processing completed successfully, false otherwise
	virtual bool onItemParsed(Kanjidic2Item &kanji) { return true; }
};

#endif
