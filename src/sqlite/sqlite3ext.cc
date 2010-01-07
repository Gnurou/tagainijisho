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
#include <QSet>
#include <QtDebug>

extern "C" {
	int isToIgnore(const char *token);
	const char *hiraganasToKatakanas(const char *src);
}

static QSet<QString> ignoredWords;
static QByteArray kanasConverted;

int isToIgnore(const char *token)
{
	if (!strcmp(token, "a")) return true;
	if (!strcmp(token, "an")) return true;
	if (!strcmp(token, "to")) return true;
	if (!strcmp(token, "of")) return true;
	return false;
	//return ignoredWords.contains(token);
}

const char *hiraganasToKatakanas(const char *src)
{
	QString s(QString::fromUtf8(src));
//	qDebug("%x", s[0].unicode());
//	return src;
//	printf("%x %x %x\n", (unsigned char)src[0], (unsigned char)src[1], (unsigned char)src[2]);
	kanasConverted = TextTools::hiragana2Katakana(s).toUtf8();
//	qDebug() << src << kanasConverted;
	return kanasConverted.constData();
}
