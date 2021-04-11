/*
 *  Copyright (C) 2016 Alexandre Courbot
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

#include <gui/Font.h>
#include <QFontDatabase>

QString Font::_jpFontFamily;

int Font::init()
{
	int id = QFontDatabase::addApplicationFont(":/fonts/NotoSansJP-Regular.otf");
	if (id < 0) {
		qWarning("Cannot load Noto font...");
		return id;
	}

	Font::_jpFontFamily = QFontDatabase::applicationFontFamilies(id).at(0);

	return 0;
}
