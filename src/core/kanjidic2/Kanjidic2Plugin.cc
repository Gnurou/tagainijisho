/*
 *  Copyright (C) 2008, 2009  Alexandre Courbot
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

#include "tagaini_config.h"

#include "core/Paths.h"
#include "core/Lang.h"
#include "core/Database.h"
#include "core/EntrySearcherManager.h"
#include "core/kanjidic2/Kanjidic2EntrySearcher.h"
#include "core/kanjidic2/Kanjidic2Plugin.h"
#include "core/kanjidic2/Kanjidic2Entry.h"

#include <QtDebug>
#include <QFile>
#include <QDir>

#define dictFileConfigString "kanjidic/database"
#define dictFileConfigDefault "kanjidic2.db"

Kanjidic2Plugin *Kanjidic2Plugin::_instance = 0;

Kanjidic2Plugin::Kanjidic2Plugin() : Plugin("kanjidic2")
{
	_instance = this;
}

Kanjidic2Plugin::~Kanjidic2Plugin()
{
	_instance = 0;
}

QString Kanjidic2Plugin::pluginInfo() const
{
	return QString::fromUtf8("<p><a href=\"http://www.csse.monash.edu.au/~jwb/kanjidic.html\">Kanjidic</a> version %1, distributed under the <a href=\"http://creativecommons.org/licenses/by-sa/3.0/\">Creative Common Attribution Share Alike License, version 3.0</a>.</p><p><a href=\"http://kanjivg.tagaini.net/\">KanjiVG</a> version %2, distributed under the <a href=\"http://creativecommons.org/licenses/by-sa/3.0/\">Creative Commons Attribution-Share Alike 3.0 license</a>.</p><p>JLPT levels courtesy to the <a href=\"http://www.jlptstudy.com/\">JLPT Study Page</a>, the <a href=\"http://www.tanos.co.uk/jlpt/\">JLPT Resources Page</a>, and lists provided by <a href=\"http://www.thbz.org/kanjimots/jlpt.php3\">Thierry Bézecourt</a> and <a href=\"http://jetsdencredujapon.blogspot.com\">Alain Côté</a>.").arg(kanjidic2Version()).arg(kanjiVGVersion());
}

bool Kanjidic2Plugin::attachAllDatabases()
{
	QString dbFile;
	
	// First attach the main db
	dbFile = lookForFile("kanjidic2.db");
	if (dbFile.isEmpty()) {
		qFatal("kanjidic2 plugin fatal error: cannot find main database file!");
		return false;
	}
	if (!Database::attachDictionaryDB(dbFile, "kanjidic2", KANJIDIC2DB_REVISION)) {
		qFatal("kanjidic2 plugin fatal error: failed to attach main database!");
		return false;
	}

	_attachedDBs[""] = dbFile;
	
	// Then look for language databases
	foreach (const QString &lang, Lang::preferredDictLanguages()) {
		dbFile = lookForFile(QString("kanjidic2-%1.db").arg(lang));
		if (dbFile.isEmpty()) continue;
		if (!Database::attachDictionaryDB(dbFile, QString("kanjidic2_%1").arg(lang), KANJIDIC2DB_REVISION)) continue;
		_attachedDBs[lang] = dbFile;
	}
	if (_attachedDBs.size() == 1) {
		qFatal("kanjidic2 plugin fatal error: no language database present!");
		return false;
	}
	
	return true;
}

void Kanjidic2Plugin::detachAllDatabases()
{
	QString dbAlias;
	foreach (const QString &lang, _attachedDBs.keys()) {
		dbAlias = lang.isEmpty() ? "kanjidic2" : "kanjidic2_" + lang;
		if (!Database::detachDictionaryDB(dbAlias))
			qWarning("kanjidic2 plugin warning: Cannot detach database %s", dbAlias.toUtf8().constData());
	}
	_attachedDBs.clear();
}

bool Kanjidic2Plugin::onRegister()
{
	if (!attachAllDatabases()) {
		return false;
	}

	SQLite::Query query(Database::connection());
	// Get the dictionaries versions
	query.exec("select kanjidic2Version, kanjiVGVersion from kanjidic2.info");
	if (query.next()) {
		_kanjidic2Version = query.valueString(0).c_str();
		_kanjiVGVersion = query.valueString(1).c_str();
	}

	// Register our entry searcher
	searcher = new Kanjidic2EntrySearcher();
	EntrySearcherManager::instance().addInstance(searcher);

	// Register our entry loader
	loader = new Kanjidic2EntryLoader();
	if (!EntriesCache::instance().addLoader(KANJIDIC2ENTRY_GLOBALID, loader)) return false;

	return true;
}

bool Kanjidic2Plugin::onUnregister()
{
	// Remove the entry loader
	EntriesCache::instance().removeLoader(KANJIDIC2ENTRY_GLOBALID);
	delete loader;

	// Unregister the entry searcher
	EntrySearcherManager::instance().removeInstance(searcher);
	delete searcher;

	// Detach our databases
	detachAllDatabases();

	return true;
}
