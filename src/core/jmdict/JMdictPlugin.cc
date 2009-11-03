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
#include "core/Database.h"
#include "core/EntrySearcherManager.h"
#include "core/jmdict/JMdictPlugin.h"
#include "core/jmdict/JMdictEntrySearcher.h"

#include <QtDebug>
#include <QFile>
#include <QDir>

#define JMDICTDB_REVISION 3

#define dictFileConfigString "jmdict/database"
#define dictFileConfigDefault "jmdict.db"

JMdictPlugin::JMdictPlugin() : Plugin("JMdict")
{
}

JMdictPlugin::~JMdictPlugin()
{
}

bool JMdictPlugin::onRegister()
{
	// First attach our database
	QString dbFile = getDBFile();
	if (!Database::attachDictionaryDB(dbFile, "jmdict", JMDICTDB_REVISION)) {
		qFatal("JMdict plugin fatal error: failed to attach JMdict database!");
		return false;
	}
	// Register our entry searcher
	searcher = new JMdictEntrySearcher();
	EntrySearcherManager::instance().addInstance(searcher);
	return true;
}

bool JMdictPlugin::onUnregister()
{
	// Remove our entry searcher and delete it
	EntrySearcherManager::instance().removeInstance(searcher);
	delete searcher;

	// Detach our database
	if (!Database::detachDictionaryDB("jmdict")) return false;

	return true;
}

QString JMdictPlugin::getDBFile() const
{
	// Look in the current directory
	QFile dbFile("jmdict.db");
#ifdef DATAPREFIX
	// Otherwise, check for the default installation prefix, if set	
	if (!dbFile.exists()) dbFile.setFileName(QDir(QUOTEMACRO(DATAPREFIX)).filePath("jmdict.db"));
#endif
	// Still no clue, then look in the binary directory
	if (!dbFile.exists()) dbFile.setFileName(QDir(QCoreApplication::applicationDirPath()).filePath("jmdict.db"));
	if (!dbFile.exists()) {
		qFatal("jmdict plugin fatal error: failed to find JMdict database!");
		return "";
	}
	return dbFile.fileName();
}
