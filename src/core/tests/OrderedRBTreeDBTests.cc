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

#include "OrderedRBTreeDBTests.h"

void OrderedRBTreeDBTests::initTestCase()
{
	QVERIFY(dbFile.open());
	QVERIFY(connection.connect(dbFile.fileName()));
}

void OrderedRBTreeDBTests::cleanupTestCase()
{
	listDB.prepareForConnection(0);
	QVERIFY(connection.close());
}

void OrderedRBTreeDBTests::createTableTest()
{
	QVERIFY(connection.exec(listDB.createTableStatement()));
	listDB.prepareForConnection(&connection);
}

void OrderedRBTreeDBTests::insertDataTest_data()
{
	QTest::addColumn<quint32>("rowid");
	QTest::addColumn<quint8>("type");
	QTest::addColumn<quint32>("id");
	QTest::addColumn<QString>("name");
	QTest::addColumn<quint32>("parent");
	QTest::addColumn<quint32>("left");
	QTest::addColumn<quint32>("right");
}

void OrderedRBTreeDBTests::insertDataTest()
{
}

void OrderedRBTreeDBTests::retrieveDataTest_data()
{
	insertDataTest_data();
}

void OrderedRBTreeDBTests::retrieveDataTest()
{
}

void OrderedRBTreeDBTests::updateDataTest()
{
}

QTEST_MAIN(OrderedRBTreeDBTests)
