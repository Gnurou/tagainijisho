/*
 *  Copyright (C) 2010 Alexandre Courbot
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

#include "core/kanjidic2/KanjiRadicals.h"

#include "sqlite/Query.h"
#include "core/Database.h"
#include <QVariant>

KanjiRadicals::KanjiRadicals()
{
	// Get all the radical information!
	SQLite::Query query(Database::connection());
	query.exec("select kanji, number from kanjidic2.radicalsList order by rowid");
	while (query.next()) {
		uint kanji = query.valueUInt(0);
		quint8 rad = query.valueUInt(1);
		kanji2rad[kanji] = rad;
		QList<uint> &l = rad2kanji[rad];
		l << kanji;
	}
}

const KanjiRadicals &KanjiRadicals::instance()
{
	static KanjiRadicals _instance;
	return _instance;
}
