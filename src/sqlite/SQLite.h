/*
 *  Copyright (C) 2010  Alexandre Courbot
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

#ifndef __SQLITE_SQLITE_H
#define __SQLITE_SQLITE_H

#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Register the callback that will automatically call register_functions
 * for every new SQLite connection */
void sqlite3ext_init();
/* Register all our custom functions */
int sqlite3ext_register_functions(sqlite3 *handler);
/* Register all our custom tokenizers */
int sqlite3ext_register_tokenizers(sqlite3 *db);

#ifdef __cplusplus
}
#endif

#endif
