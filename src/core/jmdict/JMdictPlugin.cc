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
	
	// Turn the version into an integer and check whether we should look for deleted/moved entries in the user data
	unsigned int curVersion =  QString(_dictVersion.mid(0, 4) + _dictVersion.mid(5, 2) + _dictVersion.mid(8, 2)).toInt();
	unsigned int lastVersion = 0;
	query.exec("select version from versions where id=\"JMdictDB\"");
	if (query.next()) lastVersion = query.value(0).toInt();
	// Our version is more recent - make sure that there is no orphan entries!
	if (curVersion > lastVersion) {
		// Check the study table
		query.exec(QString("select training.id, score, dateAdded, dateLastTrain, nbTrained, nbSuccess, dateLastMistake from training left join jmdict.entries on training.type = %1 and training.id = entries.id where training.type = %1 and entries.id is null").arg(JMDICTENTRY_GLOBALID));
		while (query.next()) {
			int entryId = query.value(0).toInt();
			QSqlQuery query2;
			query2.prepare("select movedTo from jmdict.deletedEntries where id = ?");
			query2.addBindValue(entryId);
			query2.exec();
			int movedTo = 0;
			if (query2.next()) movedTo = query2.value(0).toInt();
			if (!movedTo) {
				// The entry has been removed and not replaced, there is nothing to do but delete it...
				query2.prepare("delete from training where id = ?");
				query2.addBindValue(entryId);
				query2.exec();
			} else {
				// We can update the entry - but being careful to treat training data accordingly
				query2.prepare("select score, dateAdded, dateLastTrain, nbTrained, nbSuccess, dateLastMistake from training where id = ?");
				query2.addBindValue(movedTo);
				query2.exec();
				if (!query2.next()) {
					// Destination does not exist, we just have to change the id of the row
					// TODO
					qDebug() << "Simple move" << movedTo;
				} else {
					int nScore;
					int nDateAdded;
					int nDateLastTrain;
					int nNbTrained;
					int nNbSuccess;
					int nDateLastMistake;
					// Destination exists, have to merge the training data
					// TODO
					qDebug() << "Merge needed" << entryId << movedTo;
				}
			}
		}
		// Finally set out new version number 
		//query.exec("insert or replace into versions values(\"JMdictDB\", " + curVersion + ")");
	}
	
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
