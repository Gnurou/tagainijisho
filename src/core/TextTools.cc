/*
 *  Copyright (C) 2008  Alexandre Courbot
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

#include "TextTools.h"

#define UNICODE_HIRAGANA_BEGIN 0x3040
#define UNICODE_HIRAGANA_END 0x309F

#define UNICODE_KATAKANA_BEGIN 0x30A0
#define UNICODE_KATAKANA_END 0x30FF

#define UNICODE_KATAKANA_EXT_BEGIN 0x31F0
#define UNICODE_KATAKANA_EXT_END 0x31FF

#define UNICODE_CJK_EXTA_BEGIN 0x3400
#define UNICODE_CJK_EXTA_END 0x4DBF
#define UNICODE_CJK_BEGIN 0x4E00
#define UNICODE_CJK_END 0x9FC3
#define UNICODE_CJK_COMPAT_BEGIN 0xF900
#define UNICODE_CJK_COMPAT_END 0xFAD9
#define UNICODE_CJK_EXTB_BEGIN 0x20000
#define UNICODE_CJK_EXTB_END 0x2A6DF
#define UNICODE_CJK_RADICALS_BEGIN 0x2E80
#define UNICODE_CJK_RADICALS_END 0x2EFF

#define UNICODE_PUNCTUATION_BEGIN 0x3000
#define UNICODE_PUNCTUATION_END 0x303f

namespace TextTools {

bool isHiraganaChar(const QChar c) {
	return c >= UNICODE_HIRAGANA_BEGIN && c <= UNICODE_HIRAGANA_END;
}

bool isKatakanaChar(const QChar c) {
	return ( (c >= UNICODE_KATAKANA_BEGIN && c <= UNICODE_KATAKANA_END) ||
			 (c >= UNICODE_KATAKANA_EXT_BEGIN && c <= UNICODE_KATAKANA_EXT_END));
}

bool isPunctuationChar(const QChar c) {
	return c >= UNICODE_PUNCTUATION_BEGIN && c <= UNICODE_PUNCTUATION_END;
}

bool isKanaChar(const QChar c) {
	return (isHiraganaChar(c) || isKatakanaChar(c));
}

bool isKanjiChar(const QChar c, const QChar c2) {
	// Do we have a surrogate character here?
	if (!c.isHighSurrogate()) {
		return ( (c >= UNICODE_CJK_BEGIN && c <= UNICODE_CJK_END) ||
				 (c >= UNICODE_CJK_COMPAT_BEGIN && c <= UNICODE_CJK_COMPAT_END) ||
				 (c >= UNICODE_CJK_EXTA_BEGIN && c <= UNICODE_CJK_EXTA_END) ||
				 (c >= UNICODE_CJK_RADICALS_BEGIN && c <= UNICODE_CJK_RADICALS_END));
	}
	// If we do, the second argument must be filled!
	else {
		uint ucs4 = QChar::surrogateToUcs4(c, c2);
		return ucs4 >= UNICODE_CJK_EXTB_BEGIN && ucs4 <= UNICODE_CJK_EXTB_END;
	}
}

bool isKanjiChar(const QString &s, int pos)
{
	QChar c(s[pos]);
	QChar c2;
	if (c.isHighSurrogate() && pos < s.size() - 1) c2 = s[pos + 1];
	return isKanjiChar(c, c2);
}

bool isJapaneseChar(const QChar c, const QChar c2) {
	return isKanaChar(c) || isKanjiChar(c, c2) || isPunctuationChar(c);
}

bool isJapaneseChar(const QString &s, int pos) {
	return isKanaChar(s[pos]) || isKanjiChar(s, pos) || isPunctuationChar(s[pos]);
}

bool isRomajiChar(const QChar c) {
	return c <= 255;
}

bool isHiragana(const QString & string)
{
	for (int i = 0; i < string.size(); i++)
		if (!(isHiraganaChar(string[i]) || string[i].isPunct())) return false;
	return true;
}

bool isKatakana(const QString & string)
{
	for (int i = 0; i < string.size(); i++)
		if (!isKatakanaChar(string[i]) || string[i].isPunct()) return false;
	return true;
}

bool isKana(const QString & string)
{
	for (int i = 0; i < string.size(); i++)
		if (!isKanaChar(string[i]) || string[i].isPunct()) return false;
	return true;
}

bool isKanji(const QString & string)
{
	for (int i = 0; i < string.size(); i++) {
		const QChar &c = string[i];
		if (!isKanjiChar(string, i) || c.isPunct()) return false;
		if (c.isHighSurrogate()) i++;
	}
	return true;
}

bool isJapanese(const QString & string)
{
	for (int i = 0; i < string.size(); i++) {
		const QChar &c = string[i];
		if (!isJapaneseChar(string, i) || string[i].isPunct()) return false;
		if (c.isHighSurrogate()) i++;
	}
	return true;
}

bool isRomaji(const QString & string)
{
	for (int i = 0; i < string.size(); i++) {
		if (!isRomajiChar(string[i]) || string[i].isPunct()) return false;
	}
	return true;
}

#define KANASTABLE_NBROWS 14
static QChar kanasTable[KANASTABLE_NBROWS][5] = {
	{ QChar(0x3042), QChar(0x3044), QChar(0x3046), QChar(0x3048), QChar(0x304a) }, // あ
	{ QChar(0x304b), QChar(0x304d), QChar(0x304f), QChar(0x3051), QChar(0x3053) }, // か
	{ QChar(0x304c), QChar(0x304e), QChar(0x3050), QChar(0x3052), QChar(0x3054) }, // が
	{ QChar(0x3055), QChar(0x3057), QChar(0x3059), QChar(0x305b), QChar(0x305d) }, // さ
	{ QChar(0x3056), QChar(0x3058), QChar(0x305a), QChar(0x305c), QChar(0x305e) }, // ざ
	{ QChar(0x305f), QChar(0x3061), QChar(0x3064), QChar(0x3066), QChar(0x3068) }, // た
	{ QChar(0x3060), QChar(0x3062), QChar(0x3065), QChar(0x3067), QChar(0x3069) }, // だ
	{ QChar(0x306a), QChar(0x306b), QChar(0x306c), QChar(0x306d), QChar(0x306e) }, // な
	{ QChar(0x306f), QChar(0x3072), QChar(0x3075), QChar(0x3078), QChar(0x307b) }, // は
	{ QChar(0x3070), QChar(0x3073), QChar(0x3076), QChar(0x3079), QChar(0x307c) }, // ば
	{ QChar(0x3071), QChar(0x3074), QChar(0x3077), QChar(0x307a), QChar(0x307d) }, // ぱ
	{ QChar(0x307e), QChar(0x307f), QChar(0x3080), QChar(0x3081), QChar(0x3082) }, // ま
	{ QChar(0x3084), QChar(0x3086), QChar(0x3088), QChar(0x3088), QChar(0x3088) }, // や
	{ QChar(0x3089), QChar(0x308a), QChar(0x308b), QChar(0x308c), QChar(0x308d) }, // ら
};

int kanasTableRow(const QChar c)
{
	for (int i = 0; i < KANASTABLE_NBROWS; i++) {
		if (c < kanasTable[i][0] || c > kanasTable[i][4]) continue;
		for (int j = 0; j < 5; j++) {
			if (c == kanasTable[i][j]) return i;
		}
	}
	return -1;
}

QChar hiraganaChar2Katakana(const QChar hira)
{
	if (!isHiraganaChar(hira)) return hira;
	ushort code(hira.unicode());
	// These special characters should not be converted
	if (code >= 0x3099 && code <= 0x309c) return hira;
	return QChar(code + 0x60);
}

QString hiragana2Katakana(const QString &hira)
{
	QString kata(hira.size());
//	qDebug() << hira.size();
	for (int i = 0; i < hira.size(); i++) {
		QChar c(hira[i]);
//		qDebug() << c << isHiraganaChar(c);
		if (isHiraganaChar(hira[i])) kata[i] = hiraganaChar2Katakana(hira[i]);
		else kata[i] = hira[i];
	}
	return kata;
}

QString unicodeToSingleChar(unsigned int unicode)
{
	QString ret;
	if (unicode) {
		if (unicode > 0x10000) {
			ret = QChar(QChar::highSurrogate(unicode));
			ret += QChar(QChar::lowSurrogate(unicode));
		}
		else ret = QChar(unicode);
	}
	return ret;
}

unsigned int singleCharToUnicode(const QString &chr)
{
	if (chr.isEmpty()) return 0;
	if (chr.size() == 1) return chr[0].unicode();
	else if (chr.size() == 2 && chr[0].isHighSurrogate() && chr[1].isLowSurrogate()) return QChar::surrogateToUcs4(chr[0], chr[1]);
	else return 0;
}

}
