/*
 *  Copyright (C) 2009  Alexandre Courbot
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
#include "gui/KanjiValidator.h"

KanjiValidator::KanjiValidator(QObject *parent) : QValidator(parent)
{
}

QValidator::State KanjiValidator::validate(QString &input, int &pos) const
{
	int i = 0;
	while (i < input.size()) {
		const QChar &c = input[i];
		bool isKanji;
		int isSurrogate = c.isHighSurrogate() ? 1 : 0;
		// Should never happen, but done for safety
		if (isSurrogate && i == input.size() - 1) isSurrogate = 0;
		if (!isSurrogate) isKanji = TextTools::isKanjiChar(c);
		else isKanji = TextTools::isKanjiChar(c, input[i + 1]);
		if (!isKanji) {
			input.remove(i, 1 + isSurrogate);
			if (i < pos) pos -= 1 + isSurrogate;
		}
		else i += 1 + isSurrogate;
	}

	return QValidator::Acceptable;
}
