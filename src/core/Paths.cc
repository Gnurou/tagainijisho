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

#include "core/Paths.h"
#include "tagaini_config.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>

#include <QtDebug>

QString __userProfile;

QString lookForFile(const QString &name) {
    // First look in the current directory
    QFile resFile(name);
#ifdef DEBUG_PATHS
    qDebug("Looking file %s: %s", resFile.fileName().toLatin1().constData(),
           resFile.exists() ? "found" : "not found");
#endif // DEBUG_PATHS
       // Then look in the user personal directory
    if (!resFile.exists()) {
        resFile.setFileName(QDir(userProfile()).filePath(name));
#ifdef DEBUG_PATHS
        qDebug("Looking file %s: %s", resFile.fileName().toLatin1().constData(),
               resFile.exists() ? "found" : "not found");
#endif // DEBUG_PATHS
    }
#ifdef DATA_DIR
    // Otherwise, check for the default installation prefix, if set
    if (!resFile.exists()) {
        resFile.setFileName(QDir(DATA_DIR).filePath(name));
#ifdef DEBUG_PATHS
        qDebug("Looking file %s: %s", resFile.fileName().toLatin1().constData(),
               resFile.exists() ? "found" : "not found");
#endif // DEBUG_PATHS
    }
#endif
    // Still no clue, then look in the binary directory
    if (!resFile.exists()) {
        resFile.setFileName(QDir(QCoreApplication::applicationDirPath()).filePath(name));
#ifdef DEBUG_PATHS
        qDebug("Looking file %s: %s", resFile.fileName().toLatin1().constData(),
               resFile.exists() ? "found" : "not found");
#endif // DEBUG_PATHS
    }
    // Resource is our last chance
    if (!resFile.exists()) {
        resFile.setFileName(QDir(":/").filePath(name));
#ifdef DEBUG_PATHS
        qDebug("Looking file %s: %s", resFile.fileName().toLatin1().constData(),
               resFile.exists() ? "found" : "not found");
#endif // DEBUG_PATHS
    }
    if (!resFile.exists())
        return "";
    return resFile.fileName();
}
