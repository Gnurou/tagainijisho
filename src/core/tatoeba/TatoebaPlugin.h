/*
 *  Copyright (C) 2011  Alexandre Courbot
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

#ifndef __CORE_TATOEBA_PLUGIN_H
#define __CORE_TATOEBA_PLUGIN_H

#include "core/Plugin.h"

#include <QString>
#include <QStringList>

class TatoebaEntryLoader;

class TatoebaPlugin : public Plugin
{
private:
	static TatoebaPlugin *_instance;
	QStringList _dbLanguages;

	TatoebaEntryLoader *loader;

public:
	static TatoebaPlugin *instance() { return _instance; }

	TatoebaPlugin();
	~TatoebaPlugin();

	virtual bool onRegister();
	virtual bool onUnregister();
	virtual QString pluginInfo() const;
};

#endif
