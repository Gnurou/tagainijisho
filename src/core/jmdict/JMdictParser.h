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

#ifndef __CORE_JMDICT_PARSER
#define __CORE_JMDICT_PARSER

#include "core/XmlParserHelper.h"

#include <QStringList>
#include <QMap>

class JMdictKanjiWritingItem {
public:
	QString writing;
	// TODO Frequency not yet calculated
	int frequency;
	
	JMdictKanjiWritingItem() : writing(), frequency(0) {}
};

class JMdictKanaReadingItem {
public:
	QString reading;
	bool noKanji;
	// TODO Frequency not yet calculated
	int frequency;
	QList<quint8> restrictedTo;
	
	JMdictKanaReadingItem() : reading(), noKanji(false), frequency(0) {}
};

class JMdictSenseItem {
public:
	// TODO sizes should be adapted
	quint64 pos;
	quint64 field;
	quint64 misc;
	quint64 dialect;
	QList<quint8> restrictedToKanji;
	QList<quint8> restrictedToKana;
	/// Maps a language to its glosses
	QMap<QString, QStringList> gloss;
	
	JMdictSenseItem() : pos(0), field(0), misc(0), dialect(0) {}
};

class JMdictItem {
public:
	int id;
	int frequency;
	QList<JMdictKanjiWritingItem> kanji;
	QList<JMdictKanaReadingItem> kana;
	QList<JMdictSenseItem> senses;
	
	JMdictItem() : id(0), frequency(0) {}
};

class JMdictParser {
private:
	//static const QStringList _validReadings;
	
public:
	QStringList languages;
	
	JMdictParser();
	virtual ~JMdictParser() {}
	bool parse(QXmlStreamReader &reader);
	
	// This method can be overloaded by subclasses in order to implement
	// a behavior when an item is finished being parsed.
	// Returns true if the processing completed successfully, false otherwise
	virtual bool onItemParsed(JMdictItem &entry) { return true; }
};

#endif
