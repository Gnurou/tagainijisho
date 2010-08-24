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
#include "core/EntryListModel.h"
#include "core/jmdict/JMdictPlugin.h"
#include "core/jmdict/JMdictEntrySearcher.h"

#include <QtDebug>
#include <QFile>
#include <QDir>
#include <QLocale>

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

bool JMdictPlugin::checkForMovedEntries()
{
#define CHECK(x) if (!(x)) goto errorOccured
	// Turn the version into an integer and check whether we should look for deleted/moved entries in the user data
	unsigned int curVersion =  QString(_dictVersion.mid(0, 4) + _dictVersion.mid(5, 2) + _dictVersion.mid(8, 2)).toInt();
	unsigned int lastVersion = 0;
	SQLite::Query query(Database::connection());
	CHECK(query.exec("select version from versions where id=\"JMdictDB\""));
	if (query.next()) lastVersion = query.valueInt(0);
	// Our version is more recent - make sure that there is no orphan entries!
	if (curVersion > lastVersion) {
		// Check the study table
		CHECK(query.exec(QString("select training.id, score, dateAdded, dateLastTrain, nbTrained, nbSuccess, dateLastMistake from training left join jmdict.entries on training.type = %1 and training.id = entries.id where training.type = %1 and entries.id is null").arg(JMDICTENTRY_GLOBALID)));
		while (query.next()) {
			int entryId = query.valueInt(0);
			SQLite::Query query2(Database::connection());
			query2.prepare("select movedTo from jmdict.deletedEntries where id = ?");
			query2.bindValue(entryId);
			CHECK(query2.exec());
			int movedTo = 0;
			if (query2.next()) movedTo = query2.valueInt(0);
			if (!movedTo) {
				// The entry has been removed and not replaced, there is nothing to do but delete it...
				query2.prepare("delete from training where type = ? and id = ?");
				query2.bindValue(JMDICTENTRY_GLOBALID);
				query2.bindValue(entryId);
				CHECK(query2.exec());
			} else {
				// We can update the entry - but being careful to treat training data accordingly
				query2.prepare("select id, score, dateAdded, dateLastTrain, nbTrained, nbSuccess, dateLastMistake from training where type = ? and id = ?");
				query2.bindValue(JMDICTENTRY_GLOBALID);
				query2.bindValue(movedTo);
				CHECK(query2.exec());
				if (!query2.next()) {
					// Destination does not exist, we just have to change the id of the row
					query2.prepare("update training set id = ? where type = ? and id = ?");
					query2.bindValue(movedTo);
					query2.bindValue(JMDICTENTRY_GLOBALID);
					query2.bindValue(entryId);
					CHECK(query2.exec());
				} else {
					// Destination exists, have to merge the training data
					int nScore = (query.valueInt(1) + query2.valueInt(1)) / 2;
					int nDateAdded = qMin(query.valueInt(2), query.valueInt(2));
					int nDateLastTrain = qMax(query.valueInt(3), query.valueInt(3));
					int nNbTrained = query.valueInt(4) + query2.valueInt(4);
					int nNbSuccess = query.valueInt(5) + query2.valueInt(5);
					int nDateLastMistake = qMax(query.valueInt(6), query.valueInt(6));
					// Delete the moved entry
					query2.prepare("delete from training where type = ? and id = ?");
					query2.bindValue(JMDICTENTRY_GLOBALID);
					query2.bindValue(entryId);
					CHECK(query2.exec());
					// Update the remaining entry with its new values
					query2.prepare("update training set score = ?, dateAdded = ?, dateLastTrain = ?, nbTrained = ?, nbSuccess = ?, dateLastMistake = ? where type = ? and id = ?");
					query2.bindValue(nScore);
					query2.bindValue(nDateAdded);
					query2.bindValue(nDateLastTrain);
					query2.bindValue(nNbTrained);
					query2.bindValue(nNbSuccess);
					query2.bindValue(nDateLastMistake);
					query2.bindValue(JMDICTENTRY_GLOBALID);
					query2.bindValue(movedTo);
					CHECK(query2.exec());
				}
			}
		}
		// Check the tags table - move existing tags to merged entry
		CHECK(query.exec(QString("select taggedEntries.id, tagId, taggedEntries.rowid from taggedEntries left join jmdict.entries on taggedEntries.type = %1 and taggedEntries.id = entries.id where taggedEntries.type = %1 and entries.id is null").arg(JMDICTENTRY_GLOBALID)));
		while (query.next()) {
			int entryId = query.valueInt(0);
			int tagId = query.valueInt(1);
			int rowId = query.valueInt(2);
			SQLite::Query query2(Database::connection());
			query2.prepare("select movedTo from jmdict.deletedEntries where id = ?");
			query2.bindValue(entryId);
			CHECK(query2.exec());
			int movedTo = 0;
			if (query2.next()) movedTo = query2.valueInt(0);
			if (!movedTo) {
				// Just remove the tag on the deleted entry
				query2.prepare("delete from taggedEntry where rowid = ?");
				query2.bindValue(rowId);
				CHECK(query2.exec());
			} else {
				// Move the tag to the destination entry, if not already tagged
				query2.prepare("select rowid from taggedEntries where type = ? and id = ? and tagId = ?");
				query2.bindValue(JMDICTENTRY_GLOBALID);
				query2.bindValue(movedTo);
				query2.bindValue(tagId);
				CHECK(query2.exec());
				// Not tagged, just change the id!
				if (!query2.next()) {
					query2.prepare("update taggedEntries set id = ? where rowid = ?");
					query2.bindValue(movedTo);
					query2.bindValue(rowId);
					CHECK(query2.exec());
				} else {
					// Already tagged, just drop the tag on the deleted entry
					query2.prepare("delete from taggedEntry where rowid = ?");
					query2.bindValue(rowId);
					CHECK(query2.exec());
				}
			}
		}
		// Check the notes table
		CHECK(query.exec(QString("select notes.id, noteId from notes left join jmdict.entries on notes.type = %1 and notes.id = entries.id where notes.type = %1 and entries.id is null").arg(JMDICTENTRY_GLOBALID)));
		while (query.next()) {
			int entryId = query.valueInt(0);
			int noteId = query.valueInt(1);
			SQLite::Query query2(Database::connection());
			query2.prepare("select movedTo from jmdict.deletedEntries where id = ?");
			query2.bindValue(entryId);
			CHECK(query2.exec());
			int movedTo = 0;
			if (query2.next()) movedTo = query2.valueInt(0);
			if (!movedTo) {
				// No destination, drop the note :(
				query2.prepare("delete from notes where noteId = ?");
				query2.bindValue(noteId);
				CHECK(query2.exec());
			} else {
				// Move the note to its destination
				query2.prepare("update notes set id = ? where noteId = ?");
				query2.bindValue(movedTo);
				query2.bindValue(noteId);
				CHECK(query2.exec());
			}
		}
		// Check the lists table
		CHECK(query.exec(QString("select lists.id, lists.position, lists.parent, lists.rowid from lists left join jmdict.entries on lists.type = %1 and lists.id = entries.id where lists.type = %1 and entries.id is null").arg(JMDICTENTRY_GLOBALID)));
		while (query.next()) {
			int entryId = query.valueInt(0);
			int position = query.valueInt(1);
			int parent = query.valueInt(2);
			int rowId = query.valueInt(3);
			SQLite::Query query2(Database::connection());
			query2.prepare("select movedTo from jmdict.deletedEntries where id = ?");
			query2.bindValue(entryId);
			CHECK(query2.exec());
			int movedTo = 0;
			if (query2.next()) movedTo = query2.valueInt(0);
			if (!movedTo) {
				// No destination, remove from list
				EntryListModel listModel;
				QModelIndex parentIdx(listModel.index(parent));
				CHECK(listModel.removeRow(position, parentIdx));
			} else {
				// Otherwise set the entry id to the new one
				query2.prepare("update lists set id = ? where rowid = ?");
				query2.bindValue(movedTo);
				query2.bindValue(rowId);
				CHECK(query2.exec());
			}
		}
		// Finally set out new version number 
		CHECK(query.exec(QString("insert or replace into versions values(\"JMdictDB\", %1)").arg(curVersion)));
	}
	return true;

#undef CHECK
errorOccured:
	return false;
}

bool JMdictPlugin::onRegister()
{
	// First attach our database
	QLocale locale;
	QStringList supportedLanguages;
	supportedLanguages << "en" << "fr" << "de" << "es" << "ru";
	QString localeCode(locale.name().left(2));
	supportedLanguages.removeAll(localeCode);
	QString dbFile;

	// First look for the dictionary corresponding to the current locale
	dbFile = lookForFile(QString("jmdict-%1.db").arg(localeCode));
	// Cannot be found? Look for the other available version
	if (dbFile.isEmpty()) foreach (const QString &lang, supportedLanguages) {
		dbFile = lookForFile(QString("jmdict-%1.db").arg(lang));
		if (!dbFile.isEmpty()) break;
	}
	// No DB file, we have a big trouble here.
	if (dbFile.isEmpty()) {
		qFatal("JMdict plugin fatal error: cannot find any dictionary file!");
	}
	if (!Database::attachDictionaryDB(dbFile, "jmdict", JMDICTDB_REVISION)) {
		qFatal("JMdict plugin fatal error: failed to attach JMdict database!");
		return false;
	}
	
	SQLite::Query query(Database::connection());
	// Get the dictionary version
	query.exec("select JMdictVersion from jmdict.info");
	if (query.next()) _dictVersion = query.valueString(0);
	
	if (!checkForMovedEntries()) {
		qCritical("%s", QCoreApplication::translate("JMdictPlugin", "An error seems to have occured while updating the JMdict database records - the program might crash during usage. Please report this bug.").toUtf8().constData());
	}

	// Populate the entities tables
	query.exec("select bitShift, name, description from jmdict.posEntities order by bitShift");
	while (query.next()) {
		QString name(query.valueString(1));
		_posEntities << QPair<QString, QString>(name, query.valueString(2));
		_posBitShift[name] = query.valueInt(0);
	}
	query.exec("select bitShift, name, description from jmdict.miscEntities order by bitShift");
	while (query.next()) {
		QString name(query.valueString(1));
		_miscEntities << QPair<QString, QString>(name, query.valueString(2));
		_miscBitShift[name] = query.valueInt(0);
	}
	query.exec("select bitShift, name, description from jmdict.dialectEntities order by bitShift");
	while (query.next()) {
		QString name(query.valueString(1));
		_dialectEntities << QPair<QString, QString>(name, query.valueString(2));
		_dialectBitShift[name] = query.valueInt(0);
	}
	query.exec("select bitShift, name, description from jmdict.fieldEntities order by bitShift");
	while (query.next()) {
		QString name(query.valueString(1));
		_fieldEntities << QPair<QString, QString>(name, query.valueString(2));
		_fieldBitShift[name] = query.valueInt(0);
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
