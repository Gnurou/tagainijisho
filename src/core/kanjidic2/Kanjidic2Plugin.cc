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

#include "core/Paths.h"
#include "core/Database.h"
#include "core/EntrySearcherManager.h"
#include "core/kanjidic2/Kanjidic2EntrySearcher.h"
#include "core/kanjidic2/Kanjidic2Plugin.h"

#include <QtDebug>
#include <QFile>
#include <QDir>

#define KANJIDIC2DB_REVISION 3

#define dictFileConfigString "kanjidic/database"
#define dictFileConfigDefault "kanjidic2.db"

Kanjidic2Plugin::Kanjidic2Plugin() : Plugin("kanjidic2")
{
}

Kanjidic2Plugin::~Kanjidic2Plugin()
{
}

bool Kanjidic2Plugin::onRegister()
{
	// First connect our table to the database
	QString dbFile = getDBFile();
	if (!Database::attachDictionaryDB(dbFile, "kanjidic2", KANJIDIC2DB_REVISION)) {
		qFatal("kanjidic2 plugin fatal error: failed to attach Kanjidic2 database!");
		return false;
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
#ifdef DATAPREFIX
	// Otherwise, check for the default installation prefix, if set
	if (!dbFile.exists()) dbFile.setFileName(QDir(QUOTEMACRO(DATAPREFIX)).filePath("kanjidic2.db"));
#endif
	// Still no clue, then look in the binary directory
	if (!dbFile.exists()) dbFile.setFileName(QDir(QCoreApplication::applicationDirPath()).filePath("kanjidic2.db"));
	if (!dbFile.exists()) {
		qFatal("kanjidic2 plugin fatal error: failed to find Kanjidic2 database!");
		return "";
	}
	return dbFile.fileName();
}
