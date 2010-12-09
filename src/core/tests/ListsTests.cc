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

#include "ListsTests.h"
#include "core/EntryListCache.h"
#include "sqlite/Query.h"

void ListsTests::initTestCase()
{
	QVERIFY(dbFile.open());
	QVERIFY(connection.connect(dbFile.fileName()));
	QVERIFY(connection.connected());
	SQLite::Query query(&connection);
	QVERIFY(query.exec("CREATE TABLE lists(parent INTEGER REFERENCES lists, next INTEGER REFERENCES lists, type INTEGER, id INTEGER)"));
	QVERIFY(query.exec("CREATE INDEX idx_lists_parent ON lists(parent)"));
	QVERIFY(query.exec("CREATE INDEX idx_lists_entry ON lists(type, id)"));
	QVERIFY(query.exec("CREATE VIRTUAL TABLE listsLabels using fts3(label)"));
}

void ListsTests::cleanupTestCase()
{
	QVERIFY(connection.close());
}

void ListsTests::entryListCachedEntry_data()
{
	QTest::addColumn<quint64>("rowid");
	QTest::addColumn<quint64>("parent");
	QTest::addColumn<quint64>("next");
	QTest::addColumn<int>("type");
	QTest::addColumn<int>("id");

	QTest::newRow("Root list 1") << (quint64)1 << (quint64)0 << (quint64)2 << 1 << 10;
	QTest::newRow("Root list 2") << (quint64)2 << (quint64)0 << (quint64)0 << 1 << 20;

}

void ListsTests::entryListCachedEntry()
{

}

QTEST_MAIN(ListsTests)
