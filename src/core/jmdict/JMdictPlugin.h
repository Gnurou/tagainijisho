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
#include <QMap>

class JMdictEntrySearcher;
class JMdictEntryLoader;

class JMdictPlugin : public Plugin
{
private:
	static JMdictPlugin *_instance;
	QString _dictVersion;
	QMap<QString, QString> _attachedDBs;

	JMdictEntrySearcher *searcher;
	JMdictEntryLoader *loader;

	static QMap<QString, QPair<QString, quint16>> _posMap;
	static QVector<QString> _posShift;
	static QMap<QString, QPair<QString, quint16>> _miscMap;
	static QVector<QString> _miscShift;
	static QMap<QString, QPair<QString, quint16>> _dialMap;
	static QVector<QString> _dialShift;
	static QMap<QString, QPair<QString, quint16>> _fieldMap;
	static QVector<QString> _fieldShift;

	static void queryEntities(SQLite::Query *query, const QString &entity, QMap<QString, QPair<QString, quint16>> *map, QVector<QString> *shift);

	bool attachAllDatabases();
	void detachAllDatabases();

public:
	JMdictPlugin();
	virtual ~JMdictPlugin();
	static JMdictPlugin *instance() { return _instance; }
	virtual bool onRegister();
	virtual bool onUnregister();
	const QString &dictVersion() const { return _dictVersion; }
	virtual QString pluginInfo() const;
	const QMap<QString, QString> &attachedDBs() const { return _attachedDBs; }

	// Maps the short string to long description and bitshift
	static const QMap<QString, QPair<QString, quint16>> &posMap() { return _posMap; }
	static const QVector<QString> &posShift() { return _posShift; }
	static const QMap<QString, QPair<QString, quint16>> &miscMap() { return _miscMap; }
	static const QVector<QString> &miscShift() { return _miscShift; }
	static const QMap<QString, QPair<QString, quint16>> &dialMap() { return _dialMap; }
	static const QVector<QString> &dialShift() { return _dialShift; }
	static const QMap<QString, QPair<QString, quint16>> &fieldMap() { return _fieldMap; }
	static const QVector<QString> &fieldShift() { return _fieldShift; }

	static QSet<QString> shiftsToSet(const QVector<QString>& shift, const QVector<quint64>& bits);

	// Helpers for building queries
	static std::size_t numColumns(const QMap<QString, QPair<QString, quint16>> &map);
	static QString dbColumns(const QMap<QString, QPair<QString, quint16>> &map, const QString &column_name);
};

#endif
