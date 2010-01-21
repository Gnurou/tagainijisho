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

#include "core/jmdict/JMdictParser.h"

JMdictParser::JMdictParser()
{
	languages << "en";
}

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
				TAG_PRE(k_ele)
				entry.kanji << JMdictKanjiWritingItem();
				JMdictKanjiWritingItem &kWriting = entry.kanji.last();
				TAG_BEGIN(k_ele)
					TAG(keb)
						CHARACTERS
						kWriting.writing = TEXT;
						DONE
					ENDTAG
				ENDTAG
				TAG_PRE(r_ele)
				entry.kana << JMdictKanaReadingItem();
				JMdictKanaReadingItem &kReading = entry.kana.last();
				TAG_BEGIN(r_ele)
					TAG(reb)
						CHARACTERS
						kReading.reading = TEXT;
						DONE
					ENDTAG
					TAG_PRE(re_nokanji)
						kReading.noKanji = true;
					TAG_BEGIN(re_nokanji)
					ENDTAG
					TAG(re_restr)
						CHARACTERS
						QString writing(TEXT);
						// Find the index of the writing that matches the given parameter
						int idx = 0;
						foreach (const JMdictKanjiWritingItem &kWriting, entry.kanji) {
							if (kWriting.writing == writing) break;
							++idx;
						}
						if (idx == entry.kanji.size()) {
							qDebug("Entry %d: re_restr without a matching keb", entry.id);
						}
						else kReading.restrictedTo << idx;
						DONE
					ENDTAG
				ENDTAG
				TAG_PRE(sense)
				entry.senses << JMdictSenseItem();
				JMdictSenseItem &sense = entry.senses.last();
				TAG_BEGIN(sense)
					TAG_PRE(gloss)
					QString lang;
					if (HAS_ATTR("xml:lang")) {
						lang = ATTR("xml:lang").mid(0, 2);
						// Turn the 3-letters convention into a 2-letters one for the
						// sake of consistency
						if (lang == "sp") lang = "es";
						else if (lang == "ge") lang = "de";
					}
					else lang = "en";
					bool shallInsert = languages.contains(lang);
					TAG_BEGIN(gloss)
						CHARACTERS
						if (shallInsert) sense.gloss[lang] << TEXT;
						DONE
					ENDTAG
					// TODO
					TAG(pos)
					ENDTAG
					TAG(field)
					ENDTAG
					TAG(misc)
					ENDTAG
					TAG(dial)
					ENDTAG
					TAG(stagk)
						CHARACTERS
						QString writing(TEXT);
						// Find the index of the writing that matches the given parameter
						int idx = 0;
						foreach (const JMdictKanjiWritingItem &kWriting, entry.kanji) {
							if (kWriting.writing == writing) break;
							++idx;
						}
						if (idx == entry.kanji.size()) {
							qDebug("Entry %d: stagk without a matching keb", entry.id);
						}
						else sense.restrictedToKanji << idx;
						DONE
					ENDTAG
					TAG(stagr)
						CHARACTERS
						QString reading(TEXT);
						// Find the index of the writing that matches the given parameter
						int idx = 0;
						foreach (const JMdictKanaReadingItem &kReading, entry.kana) {
							if (kReading.reading == reading) break;
							++idx;
						}
						if (idx == entry.kana.size()) {
							qDebug("Entry %d: stagr without a matching reb", entry.id);
						}
						else sense.restrictedToKana << idx;
						DONE
					ENDTAG
					TAG(lsource)
					ENDTAG
				TAG_POST
				// We are not interested in senses for which there is no gloss!
				if (sense.gloss.isEmpty()) entry.senses.removeLast();
				DONE
			TAG_POST
			onItemParsed(entry);
			DONE
		ENDTAG
	DOCUMENT_END
}
