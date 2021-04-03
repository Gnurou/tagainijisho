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
#include "core/Lang.h"
#include "core/TextTools.h"
#include "core/EntriesCache.h"
#include "core/jmdict/JMdictEntry.h"
#include "core/jmdict/JMdictEntrySearcher.h"
#include "core/jmdict/JMdictPlugin.h"
#include "sqlite/Query.h"

#include <QtDebug>

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

Sense::Sense(const QSet<QString> &partOfSpeech, const QSet<QString> &misc, const QSet<QString> &dialect, const QSet<QString> &field) : _partOfSpeech(partOfSpeech), _misc(misc), _dialect(dialect), _field(field)
{
}

void Sense::addGloss(const Gloss &gloss)
{
	glosses << gloss;
}

QString Sense::senseText() const
{
	const QList<Gloss> &glosses = getGlosses();
	QStringList strList;
	foreach(const Gloss &gloss, glosses) {
		strList << QString(gloss.gloss()).replace("\n", ", ");
	}
	QString res(strList.join(" - "));
	if (res.isEmpty()) return res;
	else return res + ".";
}


JMdictEntry::JMdictEntry(EntryId id) : Entry(JMDICTENTRY_GLOBALID, id), _jlpt(-1)
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

QString JMdictEntry::mainRepr() const
{
	// For a JMdict entry, the main representation is the first writing, unless
	// the first sense has the 'uk' tag, in which case it becomes the first reading
	if (senses.isEmpty() || kanas.isEmpty() || !writtenInKana()) return Entry::mainRepr();
	return kanas[0].getReading();
}

QStringList JMdictEntry::writings() const
{
	QStringList res;
	if (hasKanjiReadings()) foreach (const KanjiReading &reading, getKanjiReadings()) res << reading.getReading();
	else foreach (const KanaReading &reading, getKanaReadings()) res << reading.getReading();

	if (res.isEmpty()) {
		res.append(tr("INVALID ENTRY"));
	}

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
	foreach (const Sense *sense, getSenses()) {
		if (sense->senseText().isEmpty()) continue;
		res << sense->senseText();
	}

	if (res.isEmpty()) {
		res.append(tr("This entry has been removed from the JMdict and we have no information about its original content or potential replacement. Sorry about this. Please remove it."));
	}
	return res;
}

QList<const Sense *> JMdictEntry::getSenses() const
{
	QList<const Sense *> res;
	QSet<QString> filter(JMdictEntrySearcher::miscFilterEntities() - JMdictEntrySearcher::explicitlyRequestedMiscs());
	foreach (const Sense &sense, getAllSenses()) {
		if ((sense.misc() & filter).isEmpty())
			res << &sense;
	}
	return res;
}

bool JMdictEntry::writtenInKana() const
{
	return (!senses.isEmpty() && senses[0].misc().contains("uk"));
}
