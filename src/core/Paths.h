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

#ifndef __CORE_PATHS_H
#define __CORE_PATHS_H

#include <QString>

#define _QUOTEMACRO(x) #x
#define QUOTEMACRO(x) _QUOTEMACRO(x)

extern QString __userProfile;
inline const QString &userProfile() { return __userProfile; }

/**
 * Searches in the standard paths for a file that matches, and
 * return its absolute path if found, or an empty string if no
 * matching file exist. Paths are scanned in this order:
 * - The current directory,
 * - The data prefix, if set,
 * - The binary path,
 * - The ressources of the binary,
 *
 * This routine should be used whenever external data files are needed.
 */
QString lookForFile(const QString &name);

#endif
