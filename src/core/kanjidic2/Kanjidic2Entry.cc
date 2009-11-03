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

#include "core/TextTools.h"
#include "core/EntriesCache.h"
#include "core/EntrySearcherManager.h"
#include "core/kanjidic2/Kanjidic2Entry.h"

#include <QtDebug>

#include <QSqlQuery>
#include <QSqlError>

KanjiComponent::KanjiComponent(const KanjiComponent *parent, const QString &element, const QString &original, unsigned int number) : _parent(parent), _element(element), _original(original), _number(number)
{
}

KanjiComponent::~KanjiComponent()
{
}

const QString &KanjiComponent::repr(bool simplified) const {
	if (simplified && !original().isEmpty()) return original();
	else return element();
}

unsigned int KanjiComponent::unicode(bool simplified) const {
	return TextTools::singleCharToUnicode(repr(simplified));
}

KanjiStroke::KanjiStroke(const KanjiComponent *parent, const QChar &type, const QString &path) : _parent(parent), _type(type), _path(path)
{
}

KanjiStroke::~KanjiStroke()
{
}

Kanjidic2Entry::Kanjidic2Entry(const QString &kanji, bool inDB, int grade, int strokeCount, int kanjiFrequency, int jlpt) : Entry(KANJIDIC2ENTRY_GLOBALID, TextTools::singleCharToUnicode(kanji)), _inDB(inDB), _kanji(kanji), _grade(grade), _strokeCount(strokeCount), _kanjiFrequency(kanjiFrequency), _jlpt(jlpt)
{
}

KanjiComponent *Kanjidic2Entry::addComponent(const KanjiComponent *parent, const QString &element, const QString &original, unsigned int number)
{
	_components << KanjiComponent(parent, element, original, number);
	return &_components.last();
}

KanjiStroke *Kanjidic2Entry::addStroke(const KanjiComponent *parent, const QChar &type, const QString &path)
{
	_strokes << KanjiStroke(parent, type, path);
	return &_strokes.last();
}

QStringList Kanjidic2Entry::writings() const
{
	QStringList res;
	res << kanji();
	return res;
}

QStringList Kanjidic2Entry::readings() const
{
	QStringList res;
	foreach (const KanjiReading &reading, kanjiReadings()) res << reading.reading();
	return res;
}

QStringList Kanjidic2Entry::meanings() const
{
	QStringList res;
	foreach (const KanjiMeaning &meaning, kanjiMeanings()) res << meaning.meaning();
	return res;
}

QString Kanjidic2Entry::meaningsString() const
{
	QStringList strList;
	foreach(const Kanjidic2Entry::KanjiMeaning &meaning, kanjiMeanings()) {
		strList << meaning.meaning();
	}
	QString s(strList.join(", "));
	if (!s.isEmpty() && !inDB()) s = tr("(var) ") + s;
	return s;
}

QStringList Kanjidic2Entry::onyomiReadings() const
{
	QStringList ret;
	foreach(const QString &reading, readings()) {
		if (TextTools::isKatakana(reading)) ret << reading;
	}
	return ret;
}

QStringList Kanjidic2Entry::kunyomiReadings() const
{
	QStringList ret;
	foreach(const QString &reading, readings()) {
		if (TextTools::isHiragana(reading)) ret << reading;
	}
	return ret;
}
