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

#ifndef __TEXTANALYSIS_H_
#define __TEXTANALYSIS_H_

#include <QChar>
#include <QString>

namespace TextTools {
	bool isHiraganaChar(const QChar c);
	bool isKatakanaChar(const QChar c);
	bool isKanaChar(const QChar c);
	bool isPunctuationChar(const QChar c);
	bool isKanjiChar(const uint c);
	/**
	 * Returns true if c is a kanji. c2 should be given when
	 * c is the high part of a surrogate character.
	 */
	bool isKanjiChar(const QChar c, const QChar c2 = 0);
	/**
	 * Returns true if s[pos] is a kanji. Takes automatically
	 * care of the surrogate problem by using the next character
	 * as necessary.
	 */
	bool isKanjiChar(const QString &s, int pos = 0);
	/**
	 * Returns true if c is a japanese character. c2 should be
	 * given when c is the high part of a surrogate character.
	 */
	bool isJapaneseChar(const QChar c, const QChar c2 = 0);
	/**
	 * Returns true if s[pos] is a japanese character. Takes
	 * automatically care of the surrogate problem by using the
	 * next character as necessary.
	 */
	bool isJapaneseChar(const QString &s, int pos = 0);
	bool isRomajiChar(const QChar c);

	bool isHiragana(const QString & string);
	bool isKatakana(const QString & string);
	bool isKana(const QString & string);
	bool isKanji(const QString & string);
	bool isJapanese(const QString & string);
	bool isRomaji(const QString & string);

	int kanasTableRow(const QChar c);

	QChar hiraganaChar2Katakana(const QChar hira);
	QString hiragana2Katakana(const QString &hira);

	QString unicodeToSingleChar(unsigned int unicode);
	unsigned int singleCharToUnicode(const QString &chr, int pos = 0);

	class KanaInfo  {
	public:
		typedef enum { Small, Normal } Size;
		typedef enum { Common, Rare } Usage;
		QString reading;
		Size size;
		Usage usage;

		// For QMap
		KanaInfo() {}
		KanaInfo(const QString &_reading, Size _size = Normal, Usage _usage = Common)
			: reading(_reading), size(_size), usage(_usage) {}
	};

	const KanaInfo &kanaInfo(const QChar c);
}

#endif
