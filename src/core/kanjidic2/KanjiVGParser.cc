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

#include "core/TextTools.h"
#include "core/kanjidic2/KanjiVGParser.h"

// Used to support the recursivity of the strokegr tag
bool KanjiVGParser::parse_strokegr(QXmlStreamReader &reader, KanjiVGItem &kanji, QStack<KanjiVGGroupItem *> gStack, quint8 &strokeCounter)
{
	TAG_BEGIN(strokegr)
		TAG_PRE(strokegr)
			// For now we do not consider groups without any element
			bool hasElement(HAS_ATTR("element"));
			if (hasElement) {
				kanji.groups << KanjiVGGroupItem();
				KanjiVGGroupItem &group = kanji.groups.last();
				if (HAS_ATTR("element")) group.element = TextTools::singleCharToUnicode(ATTR("element"));
				if (HAS_ATTR("original")) group.original = TextTools::singleCharToUnicode(ATTR("original"));
				if (gStack.isEmpty()) group.isRoot = true;
				gStack << &group;
			}
			if (!parse_strokegr(reader, kanji, gStack, strokeCounter)) return false;
			if (hasElement) gStack.pop();
		DONE
		TAG_PRE(stroke)
			QString path(ATTR("path"));
			kanji.strokes << KanjiVGStrokeItem();
			KanjiVGStrokeItem &stroke = kanji.strokes.last();
			stroke.path = path;
			foreach (KanjiVGGroupItem *group, gStack) group->pathsIndexes << strokeCounter;
			++strokeCounter;
		TAG_BEGIN(stroke)
		ENDTAG
	TAG_POST
	return true;
}

bool KanjiVGParser::parse(QXmlStreamReader &reader)
{
	DOCUMENT_BEGIN(reader)
		TAG(kanjis)
			TAG_PRE(kanji)
				int midashi(TextTools::singleCharToUnicode(ATTR("midashi")));
				bool shallInsert(TextTools::isJapaneseChar(TextTools::unicodeToSingleChar(midashi)));
				KanjiVGItem kanji;
				kanji.id = midashi;
				quint8 strokeCounter(0);
			TAG_BEGIN(kanji)
				if (shallInsert) {
					TAG_PRE(strokegr)
						QStack<KanjiVGGroupItem *> gStack;
						KanjiVGParser::parse_strokegr(reader, kanji, gStack, strokeCounter);
					DONE
					TAG_PRE(stroke)
						QString path(ATTR("path"));
						kanji.strokes << KanjiVGStrokeItem();
						KanjiVGStrokeItem &stroke = kanji.strokes.last();
						stroke.path = path;
						++strokeCounter;
					TAG_BEGIN(stroke)
					ENDTAG
				}
			TAG_POST
				onItemParsed(kanji);
			DONE
		ENDTAG
	DOCUMENT_END
}
