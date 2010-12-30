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

#ifndef __CORE_KANJIDIC2_PLUGIN_H
#define __CORE_KANJIDIC2_PLUGIN_H

#include "core/kanjidic2/Kanjidic2EntrySearcher.h"
#include "core/kanjidic2/Kanjidic2EntryLoader.h"
#include "core/Plugin.h"

class Kanjidic2EntrySearcher;

class Kanjidic2Plugin : public Plugin
{
private:
	static Kanjidic2Plugin *_instance;
	QString _dbFile;
	QString _kanjidic2Version;
	QString _kanjiVGVersion;
	Kanjidic2EntrySearcher *searcher;
	Kanjidic2EntryLoader *loader;

public:
	Kanjidic2Plugin();
	virtual ~Kanjidic2Plugin();
	static Kanjidic2Plugin *instance() { return _instance; }
	const QString &dbFile() const { return _dbFile; }
	virtual QString pluginInfo() const;
	const QString &kanjidic2Version() const { return _kanjidic2Version; }
	const QString &kanjiVGVersion() const { return _kanjiVGVersion; }
	
	virtual bool onRegister();
	virtual bool onUnregister();
};

#endif
