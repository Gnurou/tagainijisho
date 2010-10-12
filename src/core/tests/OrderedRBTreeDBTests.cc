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
	QVERIFY(listDB.createTables(&connection));
	QVERIFY(listDB.prepareForConnection(&connection));
}

void OrderedRBTreeDBTests::insertDataTest_data()
{
	QTest::addColumn<quint32>("rowid");
	QTest::addColumn<quint32>("leftSize");
	QTest::addColumn<bool>("red");
	QTest::addColumn<quint32>("parent");
	QTest::addColumn<quint32>("left");
	QTest::addColumn<quint32>("right");
	QTest::addColumn<quint32>("listid");
	QTest::addColumn<quint8>("type");
	QTest::addColumn<quint32>("id");

	QTest::newRow("first row") << (quint32)1 << (quint32)1 << false
		<< (quint32)0 << (quint32)2 << (quint32)3
		<< (quint32)5 << (quint8)1 << (quint32)28;
	QTest::newRow("second row") << (quint32)2 << (quint32)0 << true
		<< (quint32)1 << (quint32)0 << (quint32)0
		<< (quint32)5 << (quint8)1 << (quint32)29;
	QTest::newRow("third row") << (quint32)3 << (quint32)0 << false
		<< (quint32)1 << (quint32)0 << (quint32)0
		<< (quint32)5 << (quint8)2 << (quint32)44;
}

void OrderedRBTreeDBTests::insertDataTest()
{
	QFETCH(quint32, rowid);
	QFETCH(quint32, leftSize);
	QFETCH(bool, red);
	QFETCH(quint32, parent);
	QFETCH(quint32, left);
	QFETCH(quint32, right);
	QFETCH(quint32, listid);
	QFETCH(quint8, type);
	QFETCH(quint32, id);

	QCOMPARE(listDB.insertEntry(leftSize, red, parent, left, right, listid, type, id), rowid);
}

void OrderedRBTreeDBTests::retrieveDataTest_data()
{
	insertDataTest_data();
}

void OrderedRBTreeDBTests::retrieveDataTest()
{
	QFETCH(quint32, rowid);
	QFETCH(quint32, leftSize);
	QFETCH(bool, red);
	QFETCH(quint32, parent);
	QFETCH(quint32, left);
	QFETCH(quint32, right);
	QFETCH(quint32, listid);
	QFETCH(quint8, type);
	QFETCH(quint32, id);

	EntryList res = listDB.getEntry(rowid);
	QVERIFY(res.rowId != 0);
	QCOMPARE(res.rowId, rowid);
	QCOMPARE(res.leftSize, leftSize);
	QCOMPARE(res.red, red);
	QCOMPARE(res.parent, parent);
	QCOMPARE(res.left, left);
	QCOMPARE(res.right, right);
	QCOMPARE(res.listId, listid);
	QCOMPARE(res.type, type);
	QCOMPARE(res.id, id);
}

void OrderedRBTreeDBTests::updateDataTest()
{
}

QTEST_MAIN(OrderedRBTreeDBTests)
