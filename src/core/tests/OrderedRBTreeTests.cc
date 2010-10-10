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

#define TEST_SIZE 3000

void OrderedRBTreeTests::initTestCase()
{
}

void OrderedRBTreeTests::cleanupTestCase()
{
}

void OrderedRBTreeTests::testNode()
{
	OrderedRBNode<int> node(5), left(-1), right(10), grandChild(0);
	left.setColor(OrderedRBNode<int>::BLACK);
	right.setColor(OrderedRBNode<int>::BLACK);
	QCOMPARE(node.color(), OrderedRBNode<int>::RED);
	QCOMPARE(left.color(), OrderedRBNode<int>::BLACK);
	QCOMPARE(right.color(), OrderedRBNode<int>::BLACK);
	QCOMPARE(node.value(), 5);
	QCOMPARE(left.value(), -1);
	QCOMPARE(right.value(), 10);
	QCOMPARE(grandChild.value(), 0);

	QVERIFY(!node.parent());
	QVERIFY(!node.left());
	QVERIFY(!node.right());
	node.setLeft(&left);
	left.setRight(&grandChild);
	node.setRight(&right);
	QVERIFY(node.left() == &left);
	QVERIFY(left.parent() == &node);
	QVERIFY(node.right() == &right);
	QVERIFY(right.parent() == &node);
	QVERIFY(left.right() == &grandChild);
	QVERIFY(grandChild.parent() == &left);
	QVERIFY(grandChild.grandParent() == &node);
	QVERIFY(grandChild.uncle() == &right);

	grandChild.calculateLeftSize();
	left.calculateLeftSize();
	right.calculateLeftSize();
	node.calculateLeftSize();
	QCOMPARE(right.size(), 1);
	QCOMPARE(grandChild.size(), 1);
	QCOMPARE(left.size(), 2);
	QCOMPARE(node.size(), 4);

	right.detach();
	QVERIFY(!right.parent());
}

void OrderedRBTreeTests::massInsertEnd()
{
	QCOMPARE(treeEnd.size(), 0);
	treeValid(treeEnd);

	// Mass-insert from end
	for (int i = 0; i < TEST_SIZE; i++) {
		treeEnd.insert(QString("String at position %1").arg(i), i);
		QCOMPARE(treeEnd.size(), i + 1);
	}
	treeValid(treeEnd);
	for (int i = 0; i < TEST_SIZE; i++) {
		QCOMPARE(treeEnd[i], QString("String at position %1").arg(i));
	}
}

void OrderedRBTreeTests::massInsertBegin()
{
	QCOMPARE(treeBegin.size(), 0);
	treeValid(treeBegin);

	for (int i = 0; i < TEST_SIZE; i++) {
		treeBegin.insert(QString("String at position %1").arg(TEST_SIZE - (i + 1)), 0);
		QCOMPARE(treeBegin.size(), i + 1);
	}
	treeValid(treeBegin);
	for (int i = 0; i < TEST_SIZE; i++) {
		QCOMPARE(treeBegin[i], QString("String at position %1").arg(i));
	}
}

void OrderedRBTreeTests::massInsertRandom()
{
	treeRandom.clear();
	QCOMPARE(treeRandom.size(), 0);
	treeValid(treeRandom);
	QList<int> insertPos;

	for (int i = 0; i < TEST_SIZE; i++) {
		int pos = qrand() % (treeRandom.size() + 1);
		treeRandom.insert(QString("String at position %1").arg(i), pos);
		insertPos.insert(pos, i);
		QCOMPARE(treeRandom.size(), i + 1);
	}
	treeValid(treeRandom);
	for (int i = 0; i < 3000; i++) {
		QCOMPARE(treeRandom[i], QString("String at position %1").arg(insertPos[i]));
	}
}

void OrderedRBTreeTests::deepCheckValidity()
{
	tree.clear();
	QCOMPARE(tree.size(), 0);
	treeValid(tree);

	for (int i = 0; i < 300; i++) {
		int pos = qrand() % 300;
		tree.insert(QString("String at position %1").arg(pos), pos);
		QCOMPARE(tree.size(), i + 1);
		treeValid(tree);
	}
}

void OrderedRBTreeTests::massRemoveEnd()
{
	QCOMPARE(treeEnd.size(), TEST_SIZE);
	treeValid(treeEnd);

	for (int i = TEST_SIZE - 1; i >= 0; i--) {
		treeEnd.remove(i);
		QCOMPARE(treeEnd.size(), i);
	}
	treeValid(treeEnd);
}

void OrderedRBTreeTests::massRemoveBegin()
{
	QCOMPARE(treeBegin.size(), TEST_SIZE);
	treeValid(treeBegin);

	for (int i = TEST_SIZE - 1; i >= 0; i++) {
	}
}

void OrderedRBTreeTests::massRemoveRandom()
{
	QCOMPARE(treeRandom.size(), TEST_SIZE);
	treeValid(treeRandom);

	for (int i = TEST_SIZE - 1; i >= 0; i++) {
	}
}

QTEST_MAIN(OrderedRBTreeTests)
