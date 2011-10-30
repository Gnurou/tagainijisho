/*
 *  Copyright (C) 2008, 2009, 2010  Alexandre Courbot
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

#include "core/Lang.h"
#include "core/kanjidic2/Kanjidic2EntryLoader.h"
#include "core/kanjidic2/Kanjidic2Entry.h"
#include "core/kanjidic2/Kanjidic2Plugin.h"

Kanjidic2EntryLoader::Kanjidic2EntryLoader() : EntryLoader(), kanjiQuery(&connection), variationsQuery(&connection), readingsQuery(&connection), nanoriQuery(&connection), componentsQuery(&connection), radicalsQuery(&connection), skipQuery(&connection), fourCornerQuery(&connection)
{
	const QMap<QString, QString> &allDBs = Kanjidic2Plugin::instance()->attachedDBs();
	foreach (const QString &lang, allDBs.keys()) {
		QString dbAlias(lang.isEmpty() ? "kanjidic2" : "kanjidic2_" + lang);
		if (!connection.attach(allDBs[lang], dbAlias)) {
			qFatal("Kanjidic2EntrySearcher cannot attach Kanjidic2 databases!");
		}
	}

	// Prepare loading queries for faster execution
	kanjiQuery.prepare("select grade, strokeCount, frequency, jlpt, paths from kanjidic2.entries where id = ?");
	variationsQuery.prepare("select distinct original from strokeGroups where element = ? and original not null");
	readingsQuery.prepare("select type, reading from kanjidic2.reading join kanjidic2.readingText on kanjidic2.reading.docid = kanjidic2.readingText.docid where entry = ? order by type");
	nanoriQuery.prepare("select reading from kanjidic2.nanori join kanjidic2.nanoriText on kanjidic2.nanori.docid = kanjidic2.nanoriText.docid where entry = ?");
	componentsQuery.prepare("select element, original, isRoot, pathsRefs from strokeGroups where kanji = ? order by rowid");
	radicalsQuery.prepare("select rl.number, rl.kanji from kanjidic2.radicals as r join kanjidic2.radicalsList as rl on r.number = rl.number where r.kanji = ? and r.type is not null order by rl.number, rl.rowid");
	skipQuery.prepare("select type, c1, c2 from skip where entry = ? limit 1");
	fourCornerQuery.prepare("select topLeft, topRight, botLeft, botRight, extra from fourCorner where entry = ? limit 1");

	foreach (const QString &lang, allDBs.keys()) {
		if (lang.isEmpty()) continue;
		SQLite::Query &query = meaningsQueries[lang];
		QString sqlString(QString("select reading from kanjidic2_%1.meaning join kanjidic2_%1.meaningText on kanjidic2_%1.meaning.docid = kanjidic2_%1.meaningText.docid where entry = ?").arg(lang));
		query.useWith(&connection);
		query.prepare(sqlString);
	}
}

QList<Kanjidic2Entry::KanjiMeaning> Kanjidic2EntryLoader::getMeanings(int id)
{
	QList<Kanjidic2Entry::KanjiMeaning> ret;
	const QMap<QString, QString> &allDBs = Kanjidic2Plugin::instance()->attachedDBs();
	foreach (const QString &lang, Lang::preferredLanguages()) {
		if (!allDBs.contains(lang)) continue;
		SQLite::Query &meaningsQuery = meaningsQueries[lang];
		meaningsQuery.bindValue(id);
		meaningsQuery.exec();
		while(meaningsQuery.next()) {
			ret << Kanjidic2Entry::KanjiMeaning(lang, meaningsQuery.valueString(0));
		}
		meaningsQuery.reset();
	}
	// Here we probably have a kana or roman character that we can build up
	if (ret.isEmpty()) {
		QString character(TextTools::unicodeToSingleChar(id));
		TextTools::KanaInfo kInfo(TextTools::kanaInfo(QChar(id)));
		QString reading(kInfo.reading);
		QString info(reading.isEmpty() || kInfo.size == TextTools::KanaInfo::Normal ? "" : " (small)");
		if (reading.isEmpty()) reading = character;

		if (TextTools::isRomaji(character)) ret << Kanjidic2Entry::KanjiMeaning("en", QString("roman character \"%1\"").arg(reading));
		else if (TextTools::isHiragana(character)) ret << Kanjidic2Entry::KanjiMeaning("en", QString("hiragana \"%1\"%2").arg(reading).arg(info));
		else if (TextTools::isKatakana(character)) ret << Kanjidic2Entry::KanjiMeaning("en", QString("katakana \"%1\"%2").arg(reading).arg(info));
	}
	return ret;
}

Entry *Kanjidic2EntryLoader::loadEntry(EntryId id)
{
	QString character = TextTools::unicodeToSingleChar(id);

	kanjiQuery.bindValue(id);
	kanjiQuery.exec();
	Kanjidic2Entry *entry;
	QStringList paths;
	// We have no information about this kanji! This is probably an unknown radical
	if (!kanjiQuery.next()) {
		entry = new Kanjidic2Entry(character, false);
	} else {
		// Else load the kanji
		int grade = kanjiQuery.valueIsNull(0) ? -1 : kanjiQuery.valueInt(0);
		int strokeCount = kanjiQuery.valueIsNull(1) ? -1 :kanjiQuery.valueInt(1);
		qint32 frequency = kanjiQuery.valueIsNull(2) ? -1 : kanjiQuery.valueInt(2);
		int jlpt = kanjiQuery.valueIsNull(3) ? -1 : kanjiQuery.valueInt(3);
		// Get the strokes paths for later processing
		QByteArray pathsBA(kanjiQuery.valueBlob(4));
		if (!pathsBA.isEmpty()) paths = QString(qUncompress(pathsBA)).split('|');

		entry = new Kanjidic2Entry(character, true, grade, strokeCount, frequency, jlpt);
	}
	kanjiQuery.reset();
	
	loadMiscData(entry);
	
	// Find the kanjis this one is a variation of
	variationsQuery.bindValue(id);
	variationsQuery.exec();
	while (variationsQuery.next()) {
		entry->_variationOf << variationsQuery.valueInt(0);
	}
	variationsQuery.reset();

	// Now load readings, meanings and nanoris
	// Readings
	readingsQuery.bindValue(id);
	readingsQuery.exec();
	while (readingsQuery.next()) {
		entry->_readings << Kanjidic2Entry::KanjiReading(readingsQuery.valueString(0), readingsQuery.valueString(1));
	}
	readingsQuery.reset();

	// Meanings
	entry->_meanings = getMeanings(id);
	// If this kanji has no meaning, but is derived from a kanji that does, then the meanings are inherited
	if (entry->_meanings.isEmpty()) {
		// Look for every variation until we find one that has meanings
		foreach (int vid, entry->_variationOf) {
			QList<Kanjidic2Entry::KanjiMeaning> meanings(getMeanings(vid));
			if (!meanings.isEmpty()) {
				entry->_meanings = meanings;
			}
		}
	}

	// Nanori
	nanoriQuery.bindValue(id);
	nanoriQuery.exec();
	while(nanoriQuery.next()) {
		entry->_nanoris << nanoriQuery.valueString(0);
	}
	nanoriQuery.reset();

	// Insert the strokes
	foreach (const QString &path, paths) entry->addStroke(0, path);

	// Load components
	componentsQuery.bindValue(id);
	componentsQuery.exec();
	while(componentsQuery.next()) {
		QString element(TextTools::unicodeToSingleChar(componentsQuery.valueUInt(0)));
		QString original(TextTools::unicodeToSingleChar(componentsQuery.valueUInt(1)));;
		
		KanjiComponent *comp(entry->addComponent(element, original, componentsQuery.valueBool(2)));
		// Add references to the strokes belonging to this component
		QByteArray pathsRefs(componentsQuery.valueBlob(3));
		for (int i = 0; i < pathsRefs.size(); i++) {
			quint8 idx(static_cast<quint8>(pathsRefs[i]));
			comp->addStroke(&entry->_strokes[idx]);
		}
	}
	componentsQuery.reset();
	
	// Load radicals
	radicalsQuery.bindValue(id);
	radicalsQuery.exec();
	// We take the first radical character corresponding to the number,
	// as it is the one that represents the radical the best
	int curNbr = -1;
	while (radicalsQuery.next()) {
		quint8 nbr = radicalsQuery.valueUInt(0);
		uint kanji = radicalsQuery.valueUInt(1);
		if (curNbr == nbr) continue;
		curNbr = nbr;
		entry->_radicals << QPair<uint, quint8>(kanji, nbr);
	}
	radicalsQuery.reset();

	// Load skip code
	skipQuery.bindValue(id);
	skipQuery.exec();
	if (skipQuery.next()) {
		entry->_skip = QString("%1-%2-%3").arg(skipQuery.valueInt(0)).arg(skipQuery.valueInt(1)).arg(skipQuery.valueInt(2));
	}
	skipQuery.reset();

	// Load four corner code
	fourCornerQuery.bindValue(id);
	fourCornerQuery.exec();
	if (fourCornerQuery.next()) {
		entry->_fourCorner = QString("%1%2%3%4.%5").arg(fourCornerQuery.valueInt(0)).arg(fourCornerQuery.valueInt(1)).arg(fourCornerQuery.valueInt(2)).arg(fourCornerQuery.valueInt(3)).arg(fourCornerQuery.valueInt(4));
	}
	fourCornerQuery.reset();
	
	return entry;
}
