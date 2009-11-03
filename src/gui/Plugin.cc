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

#include "Plugin.h"

PluginManager PluginManager::_instance;

// Default version that does nothing.
void Plugin::registerLinkHandlers(NavigationManager *navManager)
{
}

// Default version does nothing.
void Plugin::registerMainMenuEntries(MainWindow *mainWindow)
{
}

void Plugin::registerExtenders(SearchBar *bar)
{
}

PluginManager::PluginManager() : QMap<QString, Plugin *>()
{
}

PluginManager::~PluginManager()
{
	// Delete all plugins when we are destructed.
	QMap<QString, Plugin *>::iterator it;
	for (it = _instance.begin(); it != _instance.end(); it++)
		delete it.value();
}

bool PluginManager::registerPlugin(Plugin *plugin)
{
	Plugin *oldPlugin = _instance[plugin->name()];
	if (oldPlugin) return false;
	_instance[plugin->name()] = plugin;
	return true;
}

bool PluginManager::removePlugin(Plugin *plugin)
{
	if (!_instance.contains(plugin->name())) return false;
	_instance.remove(plugin->name());
	return true;
}
