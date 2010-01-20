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

class JMdictItem {
};

class JMdictParser {
private:
	static const QStringList _validReadings;
	
public:
	static QStringList languages;
	
	JMdictParser() {}
	virtual ~JMdictParser() {}
	bool parse(QXmlStreamReader &reader);
	
	// This method can be overloaded by subclasses in order to implement
	// a behavior when an item is finished being parsed.
	// Returns true if the processing completed successfully, false otherwise
	virtual bool onItemParsed(JMdictItem &kanji) { return true; }
};

#endif
