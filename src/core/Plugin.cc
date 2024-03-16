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

#include "Plugin.h"

QMap<QString, Plugin *> Plugin::_plugins;

bool Plugin::registerPlugin(Plugin *plugin) {
    if (_plugins.contains(plugin->name()))
        return false;
    _plugins[plugin->name()] = plugin;
    if (!plugin->onRegister()) {
        _plugins.remove(plugin->name());
        return false;
    }
    return true;
}

bool Plugin::removePlugin(const QString &name) {
    if (!_plugins.contains(name))
        return false;
    if (!_plugins[name]->onUnregister())
        return false;
    _plugins.remove(name);
    return true;
}

Plugin::Plugin(const QString &name) : _name(name) {}

Plugin::~Plugin() {}

bool Plugin::onRegister() { return true; }

bool Plugin::onUnregister() { return true; }
