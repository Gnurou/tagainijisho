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

#include "core/Lang.h"
#include <QLocale>

static const QStringList _langs(QStringList() << "en" << "fr" << "de" << "es" << "ru" << "cz");

PreferenceItem<QString> Lang::preferredLanguage("", "preferredLanguages", "");

const QStringList &Lang::supportedLanguages()
{
	return _langs;
}

QStringList Lang::preferredLanguages()
{
	QStringList ret;
	// Check if the user explicitely set a preferred language
	if (preferredLanguage.value() != "en" && _langs.contains(preferredLanguage.value())) ret << preferredLanguage.value();
	// Otherwise check the locale
	else {
		QString locale(QLocale::system().name().left(2));
		if (locale != "en" && _langs.contains(locale)) ret << locale;
	}
	// English should always be here as last ressort
	ret << "en";
	return ret;
}
