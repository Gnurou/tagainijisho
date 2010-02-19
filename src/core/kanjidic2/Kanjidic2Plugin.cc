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
#include "core/Database.h"
#include "core/EntrySearcherManager.h"
#include "core/kanjidic2/Kanjidic2EntrySearcher.h"
#include "core/kanjidic2/Kanjidic2Plugin.h"

#include <QtDebug>
#include <QFile>
#include <QDir>

#define dictFileConfigString "kanjidic/database"
#define dictFileConfigDefault "kanjidic2.db"

Kanjidic2Plugin::Kanjidic2Plugin() : Plugin("kanjidic2")
{
}

Kanjidic2Plugin::~Kanjidic2Plugin()
{
}

QString Kanjidic2Plugin::pluginInfo() const
{
	return QString::fromUtf8("<p><a href=\"http://www.csse.monash.edu.au/~jwb/kanjidic.html\">Kanjidic</a> version %1, distributed under the <a href=\"http://creativecommons.org/licenses/by-sa/3.0/\">Creative Common Attribution Share Alike Licence, version 3.0</a>.</p><p><a href=\"http://kanjivg.tagaini.net/\">KanjiVG</a> version %2, distributed under the <a href=\"http://creativecommons.org/licenses/by-sa/3.0/\">Creative Commons Attribution-Share Alike 3.0 licence</a>.</p><p>JLPT levels courtesy to the <a href=\"http://www.jlptstudy.com/\">JLPT Study Page</a> and lists provided by <a href=\"http://www.thbz.org/kanjimots/jlpt.php3\">Thierry Bézecourt</a> and <a href=\"http://jetsdencredujapon.blogspot.com\">Alain Côté</a>, used with kind permission.").arg(kanjidic2Version()).arg(kanjiVGVersion());
}

bool Kanjidic2Plugin::onRegister()
{
	// First connect our table to the database
	QString dbFile = getDBFile();
	if (!Database::attachDictionaryDB(dbFile, "kanjidic2", KANJIDIC2DB_REVISION)) {
		qFatal("kanjidic2 plugin fatal error: failed to attach Kanjidic2 database!");
		return false;
	}

	// Get the versions used
	QSqlQuery query;
	query.exec("select kanjidic2Version, kanjiVGVersion from kanjidic2.info");
	if (query.next()) {
		_kanjidic2Version = query.value(0).toString();
		_kanjiVGVersion = query.value(1).toString();
	}

	// Register our entry searcher
	searcher = new Kanjidic2EntrySearcher();
	EntrySearcherManager::instance().addInstance(searcher);
	return true;
}

bool Kanjidic2Plugin::onUnregister()
{
	// Unregister the entry searcher
	EntrySearcherManager::instance().removeInstance(searcher);
	delete searcher;

	// Detach the database
	if (!Database::detachDictionaryDB("kanjidic2")) return false;

	return true;
}

QString Kanjidic2Plugin::getDBFile() const
{
	// Look in the current directory
	QFile dbFile("kanjidic2.db");
#ifdef DATA_DIR
	// Otherwise, check for the default installation prefix, if set
	if (!dbFile.exists()) dbFile.setFileName(QDir(QUOTEMACRO(DATA_DIR)).filePath("kanjidic2.db"));
#endif
	// Still no clue, then look in the binary directory
	if (!dbFile.exists()) dbFile.setFileName(QDir(QCoreApplication::applicationDirPath()).filePath("kanjidic2.db"));
	if (!dbFile.exists()) {
		qFatal("kanjidic2 plugin fatal error: failed to find Kanjidic2 database!");
		return "";
	}
	return dbFile.fileName();
}
