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

static const unsigned int nbEntries = 3;
static EntryListEntry entries[nbEntries] =
{
	{ 0, 1, false, 0, 2, 3, { 5, 1, 28 } },
	{ 0, 0, true,  1, 0, 0, { 5, 1, 29 } },
	{ 0, 0, false, 1, 0, 0, { 5, 2, 44 } }
};

void OrderedRBTreeDBTests::initTestCase()
{
	QVERIFY(dbFile.open());
	QVERIFY(connection.connect(dbFile.fileName()));
}

void OrderedRBTreeDBTests::cleanupTestCase()
{
	stringListDB.prepareForConnection(0);
	listDB.prepareForConnection(0);
	QVERIFY(connection.close());
}

void OrderedRBTreeDBTests::createTableTest()
{
	QVERIFY(listDB.createTables(&connection));
	QVERIFY(listDB.prepareForConnection(&connection));

	QVERIFY(stringListDB.createTables(&connection));
	QVERIFY(stringListDB.prepareForConnection(&connection));
	tree.tree()->setDBAccess(&stringListDB);
}

void OrderedRBTreeDBTests::insertDataTest()
{
	SQLite::Query q;
	q.useWith(&connection);
	QVERIFY(q.prepare(QString("select count(*) from %1").arg(listDB.tableName())));
	for (unsigned int i = 0; i < nbEntries; i++) {
		QCOMPARE(listDB.insertEntry(entries[i]), i + 1);
		entries[i].rowId = i + 1;
	
		// Check the number of DB rows is consistent
		QVERIFY(q.exec());
		QVERIFY(q.next());
		QCOMPARE(q.valueUInt(0), i + 1);
		q.reset();
	}

}

void OrderedRBTreeDBTests::retrieveDataTest()
{
	for (unsigned int i = 0; i < nbEntries; i++) {
		EntryListEntry res = listDB.getEntry(i + 1);
		QCOMPARE(res.rowId, entries[i].rowId);
		QCOMPARE(res.leftSize, entries[i].leftSize);
		QCOMPARE(res.red, entries[i].red);
		QCOMPARE(res.parent, entries[i].parent);
		QCOMPARE(res.left, entries[i].left);
		QCOMPARE(res.right, entries[i].right);

		QCOMPARE(res.data.listId, entries[i].data.listId);
		QCOMPARE(res.data.type, entries[i].data.type);
		QCOMPARE(res.data.id, entries[i].data.id);
	}
	EntryListEntry root = listDB.getRoot();
	QCOMPARE(root.rowId, (unsigned int)1);
}

void OrderedRBTreeDBTests::updateDataTest()
{
	// Change values in DB
	for (unsigned int i = 0; i < nbEntries; i++) {
		entries[i].rowId = nbEntries - i;
		QVERIFY(listDB.insertEntry(entries[i]));
	}

	// Ensure the number of rows did not change
	SQLite::Query q;
	q.useWith(&connection);
	QVERIFY(q.exec(QString("select count(*) from %1").arg(listDB.tableName())));
	QVERIFY(q.next());
	QCOMPARE(q.valueUInt(0), nbEntries);
	
	// Ensure the data is consistent
	for (unsigned int i = 0; i < nbEntries; i++) {
		EntryListEntry res = listDB.getEntry(i + 1);
		unsigned int idx = nbEntries - (i + 1);
		QCOMPARE(res.rowId, entries[idx].rowId);
		QCOMPARE(res.leftSize, entries[idx].leftSize);
		QCOMPARE(res.red, entries[idx].red);
		QCOMPARE(res.parent, entries[idx].parent);
		QCOMPARE(res.left, entries[idx].left);
		QCOMPARE(res.right, entries[idx].right);

		QCOMPARE(res.data.listId, entries[idx].data.listId);
		QCOMPARE(res.data.type, entries[idx].data.type);
		QCOMPARE(res.data.id, entries[idx].data.id);
	}
}

void OrderedRBTreeDBTests::removeDataTest()
{
	SQLite::Query q;
	q.useWith(&connection);
	QVERIFY(q.prepare(QString("select count(*), min(rowid), max(rowid) from %1").arg(listDB.tableName())));
	for (unsigned int i = 0; i < nbEntries; i++) {
		listDB.removeEntry(i + 1);
		// Check the number of DB rows is consistent
		QVERIFY(q.exec());
		QVERIFY(q.next());
		QCOMPARE(q.valueUInt(0), nbEntries - (i + 1));
		q.reset();
	}
}

template <> QString DBListEntry<QString>::tableDataMembers()
{
	return "str VARCHAR";
}

template <> void DBListEntry<QString>::bindDataValues(SQLite::Query &query) const
{
	query.bindValue(data);
}

template <> void DBListEntry<QString>::readDataValues(SQLite::Query &query, int start)
{
	data = query.valueString(start);
}

void OrderedRBTreeDBTests::createTreeTest()
{
	QCOMPARE(tree.size(), 0);

	tree.insert(QString("Test"), 0);
	QCOMPARE(tree.size(), 1);
	tree.insert(QString("Test2"), 1);
	QCOMPARE(tree.size(), 2);
	QCOMPARE(tree[0], QString("Test"));
	QCOMPARE(tree[1], QString("Test2"));
}

void OrderedRBTreeDBTests::massInsertTest()
{
}

void OrderedRBTreeDBTests::massRemoveTest()
{
}

QTEST_MAIN(OrderedRBTreeDBTests)

