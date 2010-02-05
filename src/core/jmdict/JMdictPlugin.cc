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

#include "config.h"

#include "core/Paths.h"
#include "core/Database.h"
#include "core/EntrySearcherManager.h"
#include "core/jmdict/JMdictPlugin.h"
#include "core/jmdict/JMdictEntrySearcher.h"

#include <QtDebug>
#include <QFile>
#include <QDir>

#define dictFileConfigString "jmdict/database"
#define dictFileConfigDefault "jmdict.db"

QVector<QPair<QString, QString> > JMdictPlugin::_posEntities;
QVector<QPair<QString, QString> > JMdictPlugin::_miscEntities;
QVector<QPair<QString, QString> > JMdictPlugin::_dialectEntities;
QVector<QPair<QString, QString> > JMdictPlugin::_fieldEntities;

QMap<QString, quint8> JMdictPlugin::_posBitShift;
QMap<QString, quint8> JMdictPlugin::_miscBitShift;
QMap<QString, quint8> JMdictPlugin::_dialectBitShift;
QMap<QString, quint8> JMdictPlugin::_fieldBitShift;

QList<const QPair<QString, QString> *> JMdictPlugin::posEntitiesList(quint64 mask)
{
	QList<const QPair<QString, QString> *> res;
	int cpt(0);
	while (mask != 0 && cpt < _posEntities.size()) {
		if (mask & 1) res << &_posEntities[cpt];
		++cpt; mask >>= 1;
	}
	return res;
}

QList<const QPair<QString, QString> *> JMdictPlugin::miscEntitiesList(quint64 mask)
{
	QList<const QPair<QString, QString> *> res;
	int cpt(0);
	while (mask != 0 && cpt < _miscEntities.size()) {
		if (mask & 1) res << &_miscEntities[cpt];
		++cpt; mask >>= 1;
	}
	return res;
}

QList<const QPair<QString, QString> *> JMdictPlugin::dialectEntitiesList(quint64 mask)
{
	QList<const QPair<QString, QString> *> res;
	int cpt(0);
	while (mask != 0 && cpt < _dialectEntities.size()) {
		if (mask & 1) res << &_dialectEntities[cpt];
		++cpt; mask >>= 1;
	}
	return res;
}

QList<const QPair<QString, QString> *> JMdictPlugin::fieldEntitiesList(quint64 mask)
{
	QList<const QPair<QString, QString> *> res;
	int cpt(0);
	while (mask != 0 && cpt < _fieldEntities.size()) {
		if (mask & 1) res << &_fieldEntities[cpt];
		++cpt; mask >>= 1;
	}
	return res;
}

JMdictPlugin::JMdictPlugin() : Plugin("JMdict")
{
}

JMdictPlugin::~JMdictPlugin()
{
}

QString JMdictPlugin::pluginInfo() const
{
	return QString("<p><a href=\"http://www.csse.monash.edu.au/~jwb/jmdict.html\">JMDict</a> version %1, distributed under the <a href=\"http://creativecommons.org/licenses/by-sa/3.0/\">Creative Common Attribution Share Alike Licence, version 3.0</a>.</p>").arg(dictVersion());
}

bool JMdictPlugin::onRegister()
{
	// First attach our database
	QString dbFile = getDBFile();
	if (!Database::attachDictionaryDB(dbFile, "jmdict", JMDICTDB_REVISION)) {
		qFatal("JMdict plugin fatal error: failed to attach JMdict database!");
		return false;
	}
	
	QSqlQuery query;
	// Get the dictionary version
	query.exec("select JMdictVersion from jmdict.info");
	if (query.next()) _dictVersion = query.value(0).toString();
	// Populate the entities tables
	query.exec("select bitShift, name, description from jmdict.posEntities order by bitShift");
	while (query.next()) {
		QString name(query.value(1).toString());
		_posEntities << QPair<QString, QString>(name, query.value(2).toString());
		_posBitShift[name] = query.value(0).toInt();
	}
	query.exec("select bitShift, name, description from jmdict.miscEntities order by bitShift");
	while (query.next()) {
		QString name(query.value(1).toString());
		_miscEntities << QPair<QString, QString>(name, query.value(2).toString());
		_miscBitShift[name] = query.value(0).toInt();
	}
	query.exec("select bitShift, name, description from jmdict.dialectEntities order by bitShift");
	while (query.next()) {
		QString name(query.value(1).toString());
		_dialectEntities << QPair<QString, QString>(name, query.value(2).toString());
		_dialectBitShift[name] = query.value(0).toInt();
	}
	query.exec("select bitShift, name, description from jmdict.fieldEntities order by bitShift");
	while (query.next()) {
		QString name(query.value(1).toString());
		_fieldEntities << QPair<QString, QString>(name, query.value(2).toString());
		_fieldBitShift[name] = query.value(0).toInt();
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

	// Clear all entities tables
	_posEntities.clear();
	_miscEntities.clear();
	_dialectEntities.clear();
	_fieldEntities.clear();
	
	// Detach our database
	if (!Database::detachDictionaryDB("jmdict")) return false;

	return true;
}

QString JMdictPlugin::getDBFile() const
{
	// Look in the current directory
	QFile dbFile("jmdict.db");
#ifdef DATA_DIR
	// Otherwise, check for the default installation prefix, if set	
	if (!dbFile.exists()) dbFile.setFileName(QDir(QUOTEMACRO(DATA_DIR)).filePath("jmdict.db"));
#endif
	// Still no clue, then look in the binary directory
	if (!dbFile.exists()) dbFile.setFileName(QDir(QCoreApplication::applicationDirPath()).filePath("jmdict.db"));
	if (!dbFile.exists()) {
		qFatal("jmdict plugin fatal error: failed to find JMdict database!");
		return "";
	}
	return dbFile.fileName();
}
