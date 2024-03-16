/*
 *  Copyright (C) 2009  Alexandre Courbot
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

#ifndef __CORE_PLUGIN_H
#define __CORE_PLUGIN_H

#include "core/ASyncQuery.h"

#include <QMap>

class Plugin {
  private:
    static QMap<QString, Plugin *> _plugins;
    const QString _name;

  public:
    static bool registerPlugin(Plugin *plugin);
    static Plugin *getPlugin(const QString &name) { return _plugins[name]; }
    static bool pluginExists(const QString &name) { return _plugins.contains(name); }
    static bool removePlugin(const QString &name);
    static const QMap<QString, Plugin *> &plugins() { return _plugins; }

    Plugin(const QString &name);
    virtual ~Plugin();

    const QString &name() const { return _name; }

    /**
     * Returns a string giving version and license information about
     * this plugin and the data it uses.
     */
    virtual QString pluginInfo() const { return ""; }

    /**
     * Called when the plugin is registered.
     *
     * @return true if the plugin is successfully registered, false otherwise.
     */
    virtual bool onRegister();
    /**
     * Called when the plugin is unregistered.
     *
     * @return true if the plugin is successfully unregistered, false otherwise.
     */
    virtual bool onUnregister();
};

#endif
