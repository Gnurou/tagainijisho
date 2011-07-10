/*
 *  Copyright (C) 2008/2009/2010/2011  Alexandre Courbot
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

#include "core/jmdict/JMdictEntryLoader.h"
#include "core/jmdict/JMdictPlugin.h"

JMdictEntryLoader::JMdictEntryLoader() : EntryLoader(), kanjiQuery(&connection), kanaQuery(&connection), sensesQuery(&connection), jlptQuery(&connection)
{
	const QMap<QString, QString> &allDBs = JMdictPlugin::instance()->attachedDBs();
	foreach (const QString &lang, allDBs.keys()) {
		QString dbAlias(lang.isEmpty() ? "jmdict" : "jmdict_" + lang);
		if (!connection.attach(allDBs[lang], dbAlias)) {
			qFatal("JMdictEntrySearcher cannot attach JMdict databases!");
		}
	}

	// Prepare queries so that we just have to bind and execute them
	kanjiQuery.prepare("select reading, frequency from jmdict.kanji join jmdict.kanjiText on kanji.docid == kanjiText.docid where id=? order by priority");
	kanaQuery.prepare("select reading, nokanji, frequency, restrictedTo from jmdict.kana join jmdict.kanaText on kana.docid == kanaText.docid where id=? order by priority");
	sensesQuery.prepare("select priority, pos, misc, dial, field, restrictedToKanji, restrictedToKana from jmdict.senses where id=? order by priority asc");
	jlptQuery.prepare("select jlpt.level from jmdict.jlpt where jlpt.id=?");
	
	foreach (const QString &lang, allDBs.keys()) {
		if (lang.isEmpty()) continue;
		SQLite::Query &query = glossQueries[lang];
		QString sqlString(QString("select gloss.lang, glossText.reading from jmdict_%1.gloss join jmdict_%1.glossText on gloss.docid == glossText.docid where gloss.id=? and gloss.sensePriority=?").arg(lang));
		query.useWith(&connection);
		query.prepare(sqlString);
	}
}

JMdictEntryLoader::~JMdictEntryLoader()
{
}

Entry *JMdictEntryLoader::loadEntry(EntryId id)
{
	JMdictEntry *entry = new JMdictEntry(id);

	loadMiscData(entry);

	// Now load readings
	// Kanji readings
	kanjiQuery.bindValue(entry->id());
	kanjiQuery.exec();
	while(kanjiQuery.next()) {
		entry->kanjis << KanjiReading(kanjiQuery.valueString(0), 0, kanjiQuery.valueUInt(1));
	}
	kanjiQuery.reset();

	// Kana readings
	kanaQuery.bindValue(entry->id());
	kanaQuery.exec();
	while(kanaQuery.next())
	{
		KanaReading kana(kanaQuery.valueString(0), 0, kanaQuery.valueUInt(2));
		// Get kana readings
		if (kanaQuery.valueBool(1) == false) {
			QStringList restrictedTo(kanaQuery.valueString(3).split(',', QString::SkipEmptyParts));
			if (restrictedTo.isEmpty()) for (int i = 0; i < entry->getKanjiReadings().size(); i++) {
				kana.addKanjiReading(i);
			}
			else for (int i = 0; i < restrictedTo.size(); i++) {
				kana.addKanjiReading(restrictedTo[i].toInt());
			}
		}
		entry->addKanaReading(kana);
	}
	kanaQuery.reset();

	// Senses
	sensesQuery.bindValue(entry->id());
	sensesQuery.exec();
	const QMap<QString, QString> allDBs = JMdictPlugin::instance()->attachedDBs();
	while(sensesQuery.next()) {
		Sense sense(sensesQuery.valueUInt64(1), sensesQuery.valueUInt64(2), sensesQuery.valueUInt64(3), sensesQuery.valueUInt64(4));
		// Get restricted readings/writing
		QStringList restrictedTo(sensesQuery.valueString(5).split(',', QString::SkipEmptyParts));
		foreach (const QString &idx, restrictedTo) sense.addStagK(idx.toInt());
		restrictedTo = sensesQuery.valueString(6).split(',', QString::SkipEmptyParts);
		foreach (const QString &idx, restrictedTo) sense.addStagR(idx.toInt());

		foreach (const QString &lang, allDBs.keys()) {
			if (lang.isEmpty()) continue;
			SQLite::Query &glossQuery = glossQueries[lang];
			glossQuery.bindValue(entry->id());
			glossQuery.bindValue(sensesQuery.valueInt(0));
			glossQuery.exec();
			while(glossQuery.next())
				sense.addGloss(Gloss(glossQuery.valueString(0), glossQuery.valueString(1)));
			glossQuery.reset();
		}
		entry->senses << sense;
	}
	sensesQuery.reset();

	// JLPT level
	jlptQuery.bindValue(entry->id());
	jlptQuery.exec();
	if (jlptQuery.next()) {
		entry->_jlpt = jlptQuery.valueInt(0);
	}
	jlptQuery.reset();
	return entry;
}
