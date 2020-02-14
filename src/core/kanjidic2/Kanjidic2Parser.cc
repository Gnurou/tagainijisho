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

#include "core/TextTools.h"
#include "core/kanjidic2/Kanjidic2Parser.h"

const QStringList Kanjidic2Parser::_validReadings(QString("ja_on,ja_kun").split(','));

bool Kanjidic2Parser::parse(QXmlStreamReader &reader)
{
	DOCUMENT_BEGIN(reader)
		DOCUMENT_TYPE_DEFINITION
		if (!onDTD(TEXT)) return false;
		DONE
		TAG(kanjidic2)
			TAG_PRE(character)
			Kanjidic2Item kanji;
			TAG_BEGIN(character)
				TAG(literal)
				CHARACTERS
					uint kanjiCode = TextTools::singleCharToUnicode(TEXT);
					kanji.id = kanjiCode;
				DONE
				ENDTAG
				TAG(radical)
					TAG_PRE(rad_value)
						QPair<quint8, Kanjidic2Item::RadicalType> rad;
						QString rad_type(ATTR("rad_type"));
						if (rad_type == "classical") rad.second = Kanjidic2Item::GENERAL;
						else if (rad_type == "nelson_c") rad.second = Kanjidic2Item::NELSON;
					TAG_BEGIN(rad_value)
					CHARACTERS
						rad.first = TEXT.toUInt();
						kanji.radicals << rad;
					DONE
					ENDTAG
				ENDTAG
				TAG(misc)
					TAG(grade)
					CHARACTERS
						kanji.grade = TEXT.toUInt();
					DONE
					ENDTAG
					TAG(stroke_count)
					CHARACTERS
						kanji.stroke_count = TEXT.toUInt();
					DONE
					ENDTAG
					TAG_PRE(freq)
						uint curFreq;
					TAG_BEGIN(freq)
					CHARACTERS
						curFreq = TEXT.toUInt();
						kanji.freq = curFreq;
					DONE
					ENDTAG
					TAG(jlpt)
					CHARACTERS
						kanji.jlpt = TEXT.toUInt();
					DONE
					ENDTAG
				ENDTAG
				TAG(dic_number)
					TAG_PRE(dic_ref)
						QString dr_type(ATTR("dr_type"));
					TAG_BEGIN(dic_ref)
					CHARACTERS
						if (dr_type == "heisig") kanji.heisig = TEXT.toUInt();
						kanji.dictionaries += dr_type;
						kanji.dictionaries += '\t';
						kanji.dictionaries += TEXT;
						kanji.dictionaries += '\n';
					DONE
					ENDTAG
				ENDTAG
				TAG(reading_meaning)
					TAG(rmgroup)
						TAG_PRE(reading)
							QString r_type(ATTR("r_type"));
						TAG_BEGIN(reading)
						CHARACTERS
							if (_validReadings.contains(r_type)) kanji.readings[r_type] << TEXT;
						DONE
						ENDTAG
						TAG_PRE(meaning)
							QString lang;
							if (HAS_ATTR("m_lang")) lang = ATTR("m_lang");
							else lang = "en";
						TAG_BEGIN(meaning)
							if (languages.contains(lang)) kanji.meanings[lang] << TEXT;
						CHARACTERS
						DONE
						ENDTAG
					ENDTAG
					TAG(nanori)
					CHARACTERS
						kanji.nanori << TEXT;
					DONE
					ENDTAG
				ENDTAG
				TAG(query_code)
					TAG_PRE(q_code)
						QString qc_type(ATTR("qc_type"));
						QString skip_misclass(ATTR("skip_misclass"));
					TAG_BEGIN(q_code)
					CHARACTERS
						if (qc_type == "skip" && skip_misclass.isEmpty()) kanji.skip = TEXT;
						else if (qc_type == "four_corner") kanji.fourCorner = TEXT;
					DONE
					ENDTAG
				ENDTAG
			TAG_POST
			if (!onItemParsed(kanji)) return false;
			DONE
			TAG(header)
				TAG(date_of_creation)
					_dateOfCreation = TEXT;
				ENDTAG
			ENDTAG
		ENDTAG
	DOCUMENT_END
}
