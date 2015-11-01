/*
 *  Copyright (C) 2011  Alexandre Courbot
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

#include "core/tatoeba/TatoebaPlugin.h"
#include "core/tatoeba/TatoebaEntryLoader.h"

#include "core/Lang.h"
#include "core/Paths.h"
#include "core/Database.h"
#include "core/EntriesCache.h"

TatoebaPlugin *TatoebaPlugin::_instance = 0;

TatoebaPlugin::TatoebaPlugin() : Plugin("Tatoeba")
{
	_instance = this;
}

TatoebaPlugin::~TatoebaPlugin()
{
	_instance = 0;
}

bool TatoebaPlugin::onRegister()
{
	// Look for and attach our database files
	QString dbFile(lookForFile("tatoeba.db"));
	if (dbFile.isEmpty()) {
		qFatal("Tatoeba plugin fatal error: cannot find main database file!");
		return false;
	}
	if (!Database::attachDictionaryDB(dbFile, "tatoeba", TATOEBADB_REVISION)) {
		qFatal("Tatoeba plugin fatal error: failed to attach main database %s!", dbFile.toLatin1().constData());
		return false;
	}
	foreach (const QString &lang, supportedLanguages()) {
		dbFile = lookForFile(QString("tatoeba-%1.db").arg(lang));
		if (!dbFile.isEmpty()) {
			if (!Database::attachDictionaryDB(dbFile, "tatoeba_" + lang, TATOEBADB_REVISION)) {
				qWarning("Tatoeba plugin fatal error: failed to attach database %s!", dbFile.toLatin1().constData());
				continue;
			}
			_dbLanguages << lang;
		}
	}

	// Instanciate and register entry loader
	loader = new TatoebaEntryLoader();
	if (!EntriesCache::instance().addLoader(TATOEBAENTRY_GLOBALID, loader)) return false;

	return true;
}

bool TatoebaPlugin::onUnregister()
{
	// Remove entry loader
	EntriesCache::instance().removeLoader(TATOEBAENTRY_GLOBALID);

	// Remove database files
	foreach (const QString &lang, _dbLanguages) {
		Database::detachDictionaryDB(QString("tatoeba_%1").arg(lang));
	}
	Database::detachDictionaryDB("tatoeba");

	return true;
}

QString TatoebaPlugin::pluginInfo() const
{
	return QString("<p><a href=\"http://www.tatoeba.org\">Tatoeba</a>, distributed under the <a href=\"http://www.creativecommons.org/licenses/by/2.0/\">Creative Commons Attribution License, version 2.0</a>.</p>");
}

