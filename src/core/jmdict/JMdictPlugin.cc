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

/* For now, JMdict do not have moved entries information. We can only delete entries that are not present anymore */
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
		QList<int> rowIds;
		// Check the study table
		CHECK(query.exec(QString("select training.id, score, dateAdded, dateLastTrain, nbTrained, nbSuccess, dateLastMistake from training left join jmdict.entries on training.type = %1 and training.id = entries.id where training.type = %1 and entries.id is null").arg(JMDICTENTRY_GLOBALID)));
		while (query.next())
			rowIds << query.valueInt(0);
		query.clear();
		foreach (int entryId, rowIds) {
			//int entryId = query.valueInt(0);
			SQLite::Query query2(Database::connection());
			/*query2.prepare("select movedTo from jmdict.deletedEntries where id = ?");
			query2.bindValue(entryId);
			CHECK(query2.exec());
			int movedTo = 0;
			if (query2.next()) movedTo = query2.valueInt(0);
			if (!movedTo) {*/
				// The entry has been removed and not replaced, there is nothing to do but delete it...
				query2.prepare("delete from training where type = ? and id = ?");
				query2.bindValue(JMDICTENTRY_GLOBALID);
				query2.bindValue(entryId);
				CHECK(query2.exec());
			/*} else {
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
			}*/
		}
		rowIds.clear();
		// Check the tags table - move existing tags to merged entry
		CHECK(query.exec(QString("select taggedEntries.id, tagId, taggedEntries.rowid from taggedEntries left join jmdict.entries on taggedEntries.type = %1 and taggedEntries.id = entries.id where taggedEntries.type = %1 and entries.id is null").arg(JMDICTENTRY_GLOBALID)));
		while (query.next())
			rowIds << query.valueInt(2);
		query.clear();
		foreach (int rowId, rowIds) {
			//int entryId = query.valueInt(0);
			//int tagId = query.valueInt(1);
			//int rowId = query.valueInt(2);
			SQLite::Query query2(Database::connection());
			/*query2.prepare("select movedTo from jmdict.deletedEntries where id = ?");
			query2.bindValue(entryId);
			CHECK(query2.exec());
			int movedTo = 0;
			if (query2.next()) movedTo = query2.valueInt(0);
			if (!movedTo) {*/
				// Just remove the tag on the deleted entry
				query2.prepare("delete from taggedEntries where rowid = ?");
				query2.bindValue(rowId);
				CHECK(query2.exec());
			/*} else {
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
					query2.prepare("delete from taggedEntries where rowid = ?");
					query2.bindValue(rowId);
					CHECK(query2.exec());
				}
			}*/
		}
		rowIds.clear();
		// Check the notes table
		CHECK(query.exec(QString("select notes.id, noteId from notes left join jmdict.entries on notes.type = %1 and notes.id = entries.id where notes.type = %1 and entries.id is null").arg(JMDICTENTRY_GLOBALID)));
		while (query.next())
			rowIds << query.valueInt(1);
		query.clear();
		foreach (int noteId, rowIds) {
			//int entryId = query.valueInt(0);
			//int noteId = query.valueInt(1);
			SQLite::Query query2(Database::connection());
			/*query2.prepare("select movedTo from jmdict.deletedEntries where id = ?");
			query2.bindValue(entryId);
			CHECK(query2.exec());
			int movedTo = 0;
			if (query2.next()) movedTo = query2.valueInt(0);
			if (!movedTo) {*/
				// No destination, drop the note :(
				query2.prepare("delete from notes where noteId = ?");
				query2.bindValue(noteId);
				CHECK(query2.exec());
			/*} else {
				// Move the note to its destination
				query2.prepare("update notes set id = ? where noteId = ?");
				query2.bindValue(movedTo);
				query2.bindValue(noteId);
				CHECK(query2.exec());
			}*/
		}
		rowIds.clear();
		// Check the lists table
		CHECK(query.exec(QString("select lists.rowid, lists.id from lists left join jmdict.entries on lists.type = %1 and lists.id = entries.id where lists.type = %1 and entries.id is null").arg(JMDICTENTRY_GLOBALID)));
		while (query.next())
			rowIds << query.valueInt(0);
		query.clear();
		//foreach (int rowId, rowIds) {
			/*int entryId = query.valueInt(1);
			SQLite::Query query2(Database::connection());
			query2.prepare("select movedTo from jmdict.deletedEntries where id = ?");
			query2.bindValue(entryId);
			CHECK(query2.exec());
			int movedTo = 0;
			if (query2.next()) movedTo = query2.valueInt(0);
			if (!movedTo) {*/
				// No destination, remove from list


#warning TODO reenable this block using lower-level list DB functions, and surrounding foreach!
				/*
				EntryListModel listModel;
				QModelIndex toRemoveIdx(listModel.index(rowId));
				CHECK(listModel.removeRow(toRemoveIdx.row(), listModel.parent(toRemoveIdx)));
				*/


			/*} else {
				// Otherwise set the entry id to the new one
				query2.prepare("update lists set id = ? where rowid = ?");
				query2.bindValue(movedTo);
				query2.bindValue(rowId);
				CHECK(query2.exec());
			}*/
		//}
		rowIds.clear();
		// Finally set our new version number
		CHECK(query.exec(QString("insert or replace into versions values(\"JMdictDB\", %1)").arg(curVersion)));
	}
	return true;

#undef CHECK
errorOccured:
	qCritical("%s", Database::connection()->lastError().message().toUtf8().constData());
	return false;
}

bool JMdictPlugin::attachAllDatabases()
{
	QString dbFile;

	// First attach the main db
	dbFile = lookForFile("jmdict.db");
	if (dbFile.isEmpty()) {
		qFatal("JMdict plugin fatal error: cannot find main database file!");
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

	if (!checkForMovedEntries()) {
		qCritical("%s", QCoreApplication::translate("JMdictPlugin", "An error seems to have occured while updating the JMdict database records - the program might crash during usage. Please report this bug.").toUtf8().constData());
	}

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
