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

#include "core/jmdict/JMdictParser.h"

bool JMdictParser::parse(QXmlStreamReader &reader)
{
	DOCUMENT_BEGIN(reader)
		TAG(JMdict)
			TAG_PRE(entry)
			JMdictItem entry;
			TAG_BEGIN(entry)
				TAG(ent_seq)
					CHARACTERS
						entry.id = TEXT.toInt();
					DONE
				ENDTAG
				TAG_PRE(r_ele)
				JMdictKanaReadingItem kReading;
				TAG_BEGIN(r_ele)
					TAG(reb)
					ENDTAG
				ENDTAG
				TAG_PRE(k_ele)
				JMdictKanjiReadingItem kReading;
				TAG_BEGIN(k_ele)
					TAG(keb)
					ENDTAG
				ENDTAG
				TAG_PRE(sense)
				JMdictSenseItem sense;
				TAG_BEGIN(sense)
					TAG(gloss)
					ENDTAG
					TAG(pos)
					ENDTAG
					TAG(field)
					ENDTAG
					TAG(misc)
					ENDTAG
					TAG(dial)
					ENDTAG
					TAG(lsource)
					ENDTAG
				ENDTAG
			TAG_POST
			onItemParsed(entry);
			DONE
		ENDTAG
	DOCUMENT_END
}
