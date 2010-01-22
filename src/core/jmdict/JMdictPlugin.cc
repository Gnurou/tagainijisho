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

QList<const QPair<QString, QString> *> JMdictPlugin::posEntities(quint64 mask)
{
	QList<const QPair<QString, QString> *> res;
	int cpt;
	while (mask != 0 && cpt < _posEntities.size()) {
		if (mask & 1) res << &_posEntities[cpt];
		++cpt; mask >>= 1;
	}
	return res;
}

QList<const QPair<QString, QString> *> JMdictPlugin::miscEntities(quint64 mask)
{
	QList<const QPair<QString, QString> *> res;
	int cpt;
	while (mask != 0 && cpt < _miscEntities.size()) {
		if (mask & 1) res << &_miscEntities[cpt];
		++cpt; mask >>= 1;
	}
	return res;
}

QList<const QPair<QString, QString> *> JMdictPlugin::dialectEntities(quint64 mask)
{
	QList<const QPair<QString, QString> *> res;
	int cpt;
	while (mask != 0 && cpt < _dialectEntities.size()) {
		if (mask & 1) res << &_dialectEntities[cpt];
		++cpt; mask >>= 1;
	}
	return res;
}

QList<const QPair<QString, QString> *> JMdictPlugin::fieldEntities(quint64 mask)
{
	QList<const QPair<QString, QString> *> res;
	int cpt;
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

bool JMdictPlugin::onRegister()
{
	// First attach our database
	QString dbFile = getDBFile();
	if (!Database::attachDictionaryDB(dbFile, "jmdict", JMDICTDB_REVISION)) {
		qFatal("JMdict plugin fatal error: failed to attach JMdict database!");
		return false;
	}
	
	// Populate the entities tables
	QSqlQuery query;
	query.exec("select name, description from jmdict.posEntities order by bitShift");
	while (query.next()) _posEntities << QPair<QString, QString>(query.value(0).toString(), query.value(1).toString());
	query.exec("select name, description from jmdict.miscEntities order by bitShift");
	while (query.next()) _miscEntities << QPair<QString, QString>(query.value(0).toString(), query.value(1).toString());
	query.exec("select name, description from jmdict.dialectEntities order by bitShift");
	while (query.next()) _dialectEntities << QPair<QString, QString>(query.value(0).toString(), query.value(1).toString());
	query.exec("select name, description from jmdict.fieldEntities order by bitShift");
	while (query.next()) _fieldEntities << QPair<QString, QString>(query.value(0).toString(), query.value(1).toString());
	
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
