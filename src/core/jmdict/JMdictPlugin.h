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

#ifndef __CORE_JMDICT_PLUGIN_H
#define __CORE_JMDICT_PLUGIN_H

#include "core/Plugin.h"

#include <QVector>
#include <QPair>

class JMdictEntrySearcher;

class JMdictPlugin : public Plugin
{
private:
	QString _dictVersion;
	QString getDBFile() const;
	JMdictEntrySearcher *searcher;
	// Associates entities shift with their short and long descriptions
	static QVector<QPair<QString, QString> > _posEntities;
	static QVector<QPair<QString, QString> > _miscEntities;
	static QVector<QPair<QString, QString> > _dialectEntities;
	static QVector<QPair<QString, QString> > _fieldEntities;
	// Associates entities name with their bit shift
	static QMap<QString, quint8> _posBitShift;
	static QMap<QString, quint8> _miscBitShift;
	static QMap<QString, quint8> _dialectBitShift;
	static QMap<QString, quint8> _fieldBitShift;

	/**
	 * If the version if the JMdict database has been updated, this
	 * method checks whether JMdict entries that may have moved or been
	 * deleted are referenced in user data tables. If so, these entries
	 * are updated or deleted as appropriate.
	 */
	bool checkForMovedEntries();

public:
	JMdictPlugin();
	virtual ~JMdictPlugin();
	virtual bool onRegister();
	virtual bool onUnregister();
	const QString &dictVersion() const { return _dictVersion; }
	virtual QString pluginInfo() const;
	
	static QList<const QPair<QString, QString> *> posEntitiesList(quint64 mask);
	static QList<const QPair<QString, QString> *> miscEntitiesList(quint64 mask);
	static QList<const QPair<QString, QString> *> dialectEntitiesList(quint64 mask);
	static QList<const QPair<QString, QString> *> fieldEntitiesList(quint64 mask);
	
	static const QVector<QPair<QString, QString> > &posEntities() { return _posEntities; }
	static const QVector<QPair<QString, QString> > &miscEntities() { return _miscEntities; }
	static const QVector<QPair<QString, QString> > &dialectEntities() { return _dialectEntities; }
	static const QVector<QPair<QString, QString> > &fieldEntities() { return _fieldEntities; }
	
	static const QMap<QString, quint8> &posBitShifts() { return _posBitShift; }
	static const QMap<QString, quint8> &miscBitShifts() { return _miscBitShift; }
	static const QMap<QString, quint8> &dialectBitShifts() { return _dialectBitShift; }
	static const QMap<QString, quint8> &fieldBitShifts() { return _fieldBitShift; }
};

#endif
