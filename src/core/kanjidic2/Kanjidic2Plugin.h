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

#include "core/Plugin.h"

class Kanjidic2EntrySearcher;

class Kanjidic2Plugin : public Plugin
{
private:
	Kanjidic2EntrySearcher *searcher;
	QString getDBFile() const;

public:
	Kanjidic2Plugin();
	virtual ~Kanjidic2Plugin();
	virtual bool onRegister();
	virtual bool onUnregister();
};

#endif
