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

#include "OrderedRBTreeTests.h"

#include <QtDebug>

#define TEST_SIZE 10000

void OrderedRBTreeTests::initTestCase()
{
}

void OrderedRBTreeTests::cleanupTestCase()
{
}

void OrderedRBTreeTests::massInsertEnd()
{
	QCOMPARE(tree.size(), 0);
	treeValid(tree);

	// Mass-insert from end
	for (int i = 0; i < TEST_SIZE; i++) {
		tree.insert(QString("String at position %1").arg(i), i);
		QCOMPARE(tree.size(), i + 1);
	}
	treeValid(tree);
	for (int i = 0; i < TEST_SIZE; i++) {
		QCOMPARE(tree[i], QString("String at position %1").arg(i));
	}

	// Clear
	tree.clear();
	QCOMPARE(tree.size(), 0);

}

void OrderedRBTreeTests::massInsertBegin()
{
	QCOMPARE(tree.size(), 0);
	treeValid(tree);

	for (int i = 0; i < TEST_SIZE; i++) {
		tree.insert(QString("String at position %1").arg(TEST_SIZE - (i + 1)), 0);
		QCOMPARE(tree.size(), i + 1);
	}
	treeValid(tree);
	for (int i = 0; i < TEST_SIZE; i++) {
		QCOMPARE(tree[i], QString("String at position %1").arg(i));
	}

	// Clear
	tree.clear();
	QCOMPARE(tree.size(), 0);
	treeValid(tree);
}

void OrderedRBTreeTests::massInsertRandom()
{
	QCOMPARE(tree.size(), 0);
	treeValid(tree);
	QList<int> insertPos;

	for (int i = 0; i < TEST_SIZE; i++) {
		int pos = qrand() % (tree.size() + 1);
		tree.insert(QString("String at position %1").arg(i), pos);
		insertPos.insert(pos, i);
		QCOMPARE(tree.size(), i + 1);
	}
	treeValid(tree);
	for (int i = 0; i < 3000; i++) {
		QCOMPARE(tree[i], QString("String at position %1").arg(insertPos[i]));
	}

	// Clear
	tree.clear();
	QCOMPARE(tree.size(), 0);
	treeValid(tree);
}

void OrderedRBTreeTests::deepCheckValidity()
{
	QCOMPARE(tree.size(), 0);
	treeValid(tree);

	for (int i = 0; i < 300; i++) {
		int pos = qrand() % 300;
		tree.insert(QString("String at position %1").arg(pos), pos);
		QCOMPARE(tree.size(), i + 1);
		treeValid(tree);
	}
	// Clear
	tree.clear();
	QCOMPARE(tree.size(), 0);
	treeValid(tree);
}

QTEST_MAIN(OrderedRBTreeTests)
