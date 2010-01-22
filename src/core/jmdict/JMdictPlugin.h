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
	QString getDBFile() const;
	JMdictEntrySearcher *searcher;
	// Associates entities shift with their short and long descriptions
	static QVector<QPair<QString, QString> > _posEntities;
	static QVector<QPair<QString, QString> > _miscEntities;
	static QVector<QPair<QString, QString> > _dialectEntities;
	static QVector<QPair<QString, QString> > _fieldEntities;

public:
	JMdictPlugin();
	virtual ~JMdictPlugin();
	virtual bool onRegister();
	virtual bool onUnregister();
	
	static QList<const QPair<QString, QString> *> posEntities(quint64 mask);
	static QList<const QPair<QString, QString> *> miscEntities(quint64 mask);
	static QList<const QPair<QString, QString> *> dialectEntities(quint64 mask);
	static QList<const QPair<QString, QString> *> fieldEntities(quint64 mask);
};

#endif
