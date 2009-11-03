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

#include "core/Paths.h"
#include "core/TextTools.h"
#include "core/EntriesCache.h"
#include "core/jmdict/JMdictEntry.h"

#include <QtDebug>
#include <QSqlQuery>

#include <QSqlQuery>
#include <QSqlError>

KanjiReading::KanjiReading(const QString &reading, quint32 info, quint8 frequency) : reading(reading), info(info), _frequency(frequency)
{
}

KanaReading::KanaReading(const QString &reading, qint32 info, quint8 frequency) : reading(reading), info(info), _frequency(frequency)
{
}

void KanaReading::addKanjiReading(int readingIndex)
{
	kanjiReadings << readingIndex;
}

Gloss::Gloss(const QString &lang, const QString &gloss) : _lang(lang), _gloss(gloss)
{
}

Sense::Sense(JMdictPosTagType partOfSpeech, JMdictMiscTagType misc, JMdictDialTagType dialect, JMdictFieldTagType field) : _partOfSpeech(partOfSpeech), _misc(misc), _dialect(dialect), _field(field)
{
}

void Sense::addGloss(const Gloss &gloss)
{
	glosses[gloss.lang()] = gloss;
}

const Gloss Sense::gloss(const QString &lang) const
{
	return glosses[lang];
}

QList<int> Sense::listOfSetBits(quint64 value) {
	QList<int> ret;

	for (int i = 0; value != 0; i++, value >>= 1)
		if (value & 0x1) ret << i;

	return ret;
}

QList<int> Sense::partsOfSpeech() const
{
	return listOfSetBits(partOfSpeech());
}

QList<int> Sense::miscs() const
{
	return listOfSetBits(misc());
}

QList<int> Sense::fields() const
{
	return listOfSetBits(field());
}

QList<int> Sense::dialects() const
{
	return listOfSetBits(dialect());
}

QString Sense::senseText() const
{
	const QMap<QString, Gloss> &glosses = getGlosses();
	QStringList strList;
	foreach(const QString &key, glosses.keys())
		strList << glosses[key].gloss();
	QString res(strList.join(" - ") + ".");
	return res;
}


JMdictEntry::JMdictEntry(int id) : Entry(JMDICTENTRY_GLOBALID, id), _jlpt(-1)
{
}

JMdictEntry::~JMdictEntry()
{
}

void JMdictEntry::addKanaReading(const KanaReading &reading)
{
	kanas << reading;

	const QList<qint32> &validKanjis = reading.getKanjiReadings();
	// Add the reading to all kanjis that apply
	for (int i = 0; i < validKanjis.size(); i++) {
		kanjis[validKanjis[i]].validReadings << (kanas.size() - 1);
	}
}

QStringList JMdictEntry::writings() const
{
	QStringList res;
	if (hasKanjiReadings()) foreach (const KanjiReading &reading, getKanjiReadings()) res << reading.getReading();
	else foreach (const KanaReading &reading, getKanaReadings()) res << reading.getReading();
	return res;
}

QStringList JMdictEntry::readings() const
{
	QStringList res;
	if (hasKanjiReadings()) foreach (const KanaReading &reading, getKanaReadings()) res << reading.getReading();
	return res;
}

QStringList JMdictEntry::meanings() const
{
	QStringList res;
	foreach (const Sense &sense, getSenses()) res << sense.senseText();
	return res;
}
