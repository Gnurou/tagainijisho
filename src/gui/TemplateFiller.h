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

#ifndef GUI_TEMPLATEFILLER_H
#define GUI_TEMPLATEFILLER_H

#include "gui/EntryFormatter.h"

#include <QString>

class TemplateFiller
{
public:
	TemplateFiller() {}
	
	/**
	 * Fills in the template using the given formatter with the given entry.
	 */
	QString fill(const QString& tmpl, const EntryFormatter* formatter, const EntryPointer& entry);
	/**
	 * Extracts the requested parts from the template. Parts are delimited using the PART tag
	 * enclosed into HTML comments. The last argument specifies whether text that is not in any
	 * part should be included or not.
	 */
	QString extract(const QString &tmpl, const QStringList &parts, bool includeRootText = false);
};

#endif // GUI_TEMPLATEFILLER_H
