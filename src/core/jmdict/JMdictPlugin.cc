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

#include "tagaini_config.h"

#include "core/Paths.h"
#include "core/Lang.h"
#include "core/Database.h"
#include "core/EntrySearcherManager.h"
#include "core/jmdict/JMdictPlugin.h"
#include "core/jmdict/JMdictEntry.h"
#include "core/jmdict/JMdictEntrySearcher.h"
#include "core/jmdict/JMdictEntryLoader.h"

#include <QtDebug>
#include <QFile>
#include <QDir>

#define dictFileConfigString "jmdict/database"
#define dictFileConfigDefault "jmdict.db"

JMdictPlugin *JMdictPlugin::_instance = 0;

QMap<QString, QPair<QString, quint16>> JMdictPlugin::_posMap;
QVector<QString> JMdictPlugin::_posShift;
QMap<QString, QPair<QString, quint16>> JMdictPlugin::_miscMap;
QVector<QString> JMdictPlugin::_miscShift;
QMap<QString, QPair<QString, quint16>> JMdictPlugin::_dialMap;
QVector<QString> JMdictPlugin::_dialShift;
QMap<QString, QPair<QString, quint16>> JMdictPlugin::_fieldMap;
QVector<QString> JMdictPlugin::_fieldShift;

JMdictPlugin::JMdictPlugin() : Plugin("JMdict")
{
	_instance = this;
}

JMdictPlugin::~JMdictPlugin()
{
	_instance = 0;
}

QString JMdictPlugin::pluginInfo() const
{
	return QString("<p><a href=\"http://www.csse.monash.edu.au/~jwb/jmdict.html\">JMDict</a> version %1, distributed under the <a href=\"http://creativecommons.org/licenses/by-sa/3.0/\">Creative Common Attribution Share Alike License, version 3.0</a>.</p><p><a href=\"http://www.kanji.org/\">SKIP codes</a> are developed by Jack Halpern and distributed under the <a href=\"http://creativecommons.org/licenses/by-sa/4.0/\">Creative Commons Attribution-ShareAlike 4.0 International</a> licence.</p>").arg(dictVersion());
}

bool JMdictPlugin::attachAllDatabases()
{
	QString dbFile;

	// First attach the main db
	dbFile = lookForFile("jmdict.db");
	if (dbFile.isEmpty()) {
		#ifdef DATA_DIR
		const char *system_path = DATA_DIR;
		#else
		const char *system_path = "";
		#endif

		qFatal("JMdict plugin fatal error: cannot find main database file!\n\nSystem path: %s", QDir(system_path).filePath("jmdict.db").toStdString().c_str());
		return false;
	}
	if (!Database::attachDictionaryDB(dbFile, "jmdict", JMDICTDB_REVISION)) {
		qFatal("JMdict plugin fatal error: failed to attach main database!");
		return false;
	}

	_attachedDBs[""] = dbFile;

	// Then look for language databases
	foreach (const QString &lang, Lang::preferredDictLanguages()) {
		dbFile = lookForFile(QString("jmdict-%1.db").arg(lang));
		if (dbFile.isEmpty()) continue;
		if (!Database::attachDictionaryDB(dbFile, QString("jmdict_%1").arg(lang), JMDICTDB_REVISION)) continue;
		_attachedDBs[lang] = dbFile;
	}
	if (_attachedDBs.size() == 1) {
		qFatal("JMdict plugin fatal error: no language database present!");
		return false;
	}

	return true;
}

void JMdictPlugin::detachAllDatabases()
{
	QString dbAlias;
	foreach (const QString &lang, _attachedDBs.keys()) {
		dbAlias = lang.isEmpty() ? "jmdict" : "jmdict_" + lang;
		if (!Database::detachDictionaryDB(dbAlias))
			qWarning("JMdict plugin warning: Cannot detach database %s", dbAlias.toUtf8().constData());
	}
	_attachedDBs.clear();
}

void JMdictPlugin::queryEntities(SQLite::Query *query, const QString &entity, QMap<QString, QPair<QString, quint16>> *map, QVector<QString> *shift)
{
	QString queryString = QString("select bitShift, name, description from jmdict.%1Entities order by bitShift").arg(entity);
	query->exec(queryString);
	while (query->next()) {
		quint8 bitShift(query->valueUInt(0));
		QString name(query->valueString(1));
		QString desc(query->valueString(2));
		(*map)[name] = QPair<QString, quint8>(desc, bitShift);
		Q_ASSERT(shift->size() == bitShift);
		shift->append(name);
	}
}

bool JMdictPlugin::onRegister()
{
	if (!attachAllDatabases()) {
		return false;
	}

	SQLite::Query query(Database::connection());
	// Get the dictionary version
	query.exec("select JMdictVersion from jmdict.info");
	if (query.next()) _dictVersion = query.valueString(0);
	query.clear();

	// Populate the entities tables
	queryEntities(&query, "pos", &_posMap, &_posShift);
	queryEntities(&query, "misc", &_miscMap, &_miscShift);
	queryEntities(&query, "dialect", &_dialMap, &_dialShift );
	queryEntities(&query, "field", &_fieldMap, &_fieldShift);

	// Register our entry searcher
	searcher = new JMdictEntrySearcher();
	EntrySearcherManager::instance().addInstance(searcher);

	// Register our entry loader
	loader = new JMdictEntryLoader();
	if (!EntriesCache::instance().addLoader(JMDICTENTRY_GLOBALID, loader)) return false;

	return true;
}

bool JMdictPlugin::onUnregister()
{
	// Remove the entry loader
	EntriesCache::instance().removeLoader(JMDICTENTRY_GLOBALID);
	delete loader;

	// Remove our entry searcher and delete it
	EntrySearcherManager::instance().removeInstance(searcher);
	delete searcher;

	// Clear all entities tables
	_posMap.clear();
	_posShift.clear();
	_miscMap.clear();
	_miscShift.clear();
	_dialMap.clear();
	_dialShift.clear();
	_fieldMap.clear();
	_fieldShift.clear();

	// Detach our databases
	detachAllDatabases();

	return true;
}

QSet<QString> JMdictPlugin::shiftsToSet(const QVector<QString>& shift, const QVector<quint64>& bits) {
	QSet<QString> ret;

	for (int i = 0; i < bits.size(); i++) {
		int cpt = 0;
		quint64 bits_set = bits[i];
		while (bits_set != 0) {
			if (bits_set & 1)
				ret << shift[cpt + (i * 64)];
			bits_set >>= 1;
			cpt++;
		}
	}
	return ret;
}

std::size_t JMdictPlugin::numColumns(const QMap<QString, QPair<QString, quint16>> &map) {
	return (map.size() + 63) / 64;
}

QString JMdictPlugin::dbColumns(const QMap<QString, QPair<QString, quint16>> &map, const QString &column_name) {
	QStringList columns;
	for (std::size_t i = 0; i < numColumns(map); i++)
		columns << QString("%1%2").arg(column_name).arg(i);

	return columns.join(", ");
}
