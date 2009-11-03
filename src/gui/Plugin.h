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

#ifndef __PLUGIN_H
#define __PLUGIN_H

#include <QMap>
#include <QString>
#include <QMenuBar>
#include <QObject>

#include "NavigationManager.h"
#include "MainWindow.h"
#include "SearchBar.h"

class ThreadedDatabaseConnection;

/**
 * Provides different functions related to the management
 * of entries for a given dictionary.
 */
class Plugin : public QObject
{
private:
	QString _name;

public:
	/**
	 * @param name Unique identifier for this plugin.
	 */
	Plugin(const QString &name) : _name(name) {}
	virtual ~Plugin() {}

	const QString &name() const { return _name; }

	/**
	 * Register navigation URL handlers for that plugin to the
	 * NavigationManager given in parameter.
	 */
	virtual void registerLinkHandlers(NavigationManager *navManager);

	/**
	 * Register main window menu entries for this plugin.
	 */
	virtual void registerMainMenuEntries(MainWindow *mainWindow);

	virtual void registerExtenders(SearchBar *bar);

	/**
	 * Returns a string giving version and licence information about 
	 * this plugin and the data it uses.
	 */
	virtual QString pluginInfo() { return ""; }
};

/**
 * Provides the necessary static functions to register plugins,
 * as well as a const reference to the manager singleton that
 * can be manipulated as a mapping.
 */
class PluginManager : public QMap<QString, Plugin *>
{
private:
	static PluginManager _instance;
	PluginManager();
public:
	~PluginManager();

	/**
	 * Returns the list of all plugins for manipulation by clients.
	 */
	static const PluginManager &instance() { return _instance; }

	/**
	 * Registers a new plugin, returns true on success. If a plugin with the
	 * same identifier already exists, nothing is done and false is returned.
	 */
	static bool registerPlugin(Plugin *plugin);
	static bool removePlugin(Plugin *plugin);
};

#endif
