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
#include "tagaini_config.h"
#include <QLocale>

static const QStringList _dictLangs(QStringList((QString("en;") + QString(DICT_LANG)).split(';')));
static const QStringList _guiLangs(QStringList(QString(UI_LANG).split(';')));

PreferenceItem<QString> Lang::preferredDictLanguage("", "preferredDictLanguage", "");
PreferenceItem<QString> Lang::preferredGUILanguage("", "preferredGUILanguage", "");
PreferenceItem<bool> Lang::alwaysShowEnglish("", "alwaysShowEnglish", false);

const QStringList &Lang::supportedDictLanguages()
{
	return _dictLangs;
}

const QStringList &Lang::supportedGUILanguages()
{
	return _guiLangs;
}

QStringList Lang::preferredDictLanguages()
{
	QStringList ret;
	QString userLang;

	// If no preferred content language is set, but a GUI language is, use that as the content language.
	if (preferredDictLanguage.isDefault() && _dictLangs.contains(preferredGUILanguage.value())) userLang = preferredGUILanguage.value();
	// Otherwise, use the explicitly set preferred content language.
	else if (_dictLangs.contains(preferredDictLanguage.value())) userLang = preferredDictLanguage.value();
	// Check if the user explicitely set a preferred language
	if (!userLang.isEmpty()) ret << userLang;
	// Otherwise check the locale
	else {
		QString locale(QLocale::system().name().left(2));
		if (locale != "en" && _dictLangs.contains(locale)) ret << locale;
	}

	// English should always be here as last ressort
	if (!ret.contains("en"))
		ret << "en";

	return ret;
}
