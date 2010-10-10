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
#define VSTRING "String at position %1"

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
		treeEnd.insert(QString(VSTRING).arg(i), i);
		QCOMPARE(treeEnd.size(), i + 1);
	}
	treeValid(treeEnd);
	for (int i = 0; i < TEST_SIZE; i++) {
		QCOMPARE(treeEnd[i], QString(VSTRING).arg(i));
	}
}

void OrderedRBTreeTests::massInsertBegin()
{
	QCOMPARE(treeBegin.size(), 0);
	treeValid(treeBegin);

	for (int i = 0; i < TEST_SIZE; i++) {
		treeBegin.insert(QString(VSTRING).arg(TEST_SIZE - (i + 1)), 0);
		QCOMPARE(treeBegin.size(), i + 1);
	}
	treeValid(treeBegin);
	for (int i = 0; i < TEST_SIZE; i++) {
		QCOMPARE(treeBegin[i], QString(VSTRING).arg(i));
	}
}

void OrderedRBTreeTests::massInsertRandom()
{
	treeRandom.clear();
	QCOMPARE(treeRandom.size(), 0);
	treeValid(treeRandom);

	for (int i = 0; i < TEST_SIZE; i++) {
		int pos = qrand() % (treeRandom.size() + 1);
		treeRandom.insert(QString(VSTRING).arg(i), pos);
		treeRandomPos.insert(pos, i);
		QCOMPARE(treeRandom.size(), i + 1);
	}
	treeValid(treeRandom);
	for (int i = 0; i < TEST_SIZE; i++) {
		QCOMPARE(treeRandom[i], QString(VSTRING).arg(treeRandomPos[i]));
	}
}

void OrderedRBTreeTests::massRemoveEnd()
{
	QCOMPARE(treeEnd.size(), TEST_SIZE);
	treeValid(treeEnd);

	for (int i = TEST_SIZE - 1; i >= 0; i--) {
		treeEnd.remove(i);
		QCOMPARE(treeEnd.size(), i);
		// Every 16 times, check that the tree's order is respected and that it is valid
		if ((i & 0xf) == 0) {
			treeValid(treeEnd);
			for (int j = 0; j < treeEnd.size(); j++)
			     QCOMPARE(treeEnd[j], QString(VSTRING).arg(j));
		}
	}
	treeValid(treeEnd);
}

void OrderedRBTreeTests::massRemoveBegin()
{
	QCOMPARE(treeBegin.size(), TEST_SIZE);
	treeValid(treeBegin);

	for (int i = 0; i < TEST_SIZE; ) {
		treeBegin.remove(0);
		i++;
		QCOMPARE(treeBegin.size(), TEST_SIZE - i);
		// Every 16 times, check that the tree's order is respected and that it is valid
		if ((i & 0xf) == 0) {
			treeValid(treeBegin);
			for (int j = 0; j < TEST_SIZE - i; j++)
				QCOMPARE(treeBegin[j], QString(VSTRING).arg(j + i));
		}
	}
	treeValid(treeBegin);
}

void OrderedRBTreeTests::massRemoveRandom()
{
	QCOMPARE(treeRandom.size(), TEST_SIZE);
	treeValid(treeRandom);

	for (int i = 0; i < TEST_SIZE; ) {
		int pos = qrand() % (treeRandom.size() - 1);
		treeRandom.remove(pos);
		treeRandomPos.removeAt(pos);
		i++;
		QCOMPARE(treeRandom.size(), TEST_SIZE - i);
		treeValid(treeRandom);
		// Every 16 times, check that the tree's order is respected and that it is valid
		if ((i & 0xf) == 0) {
			treeValid(treeRandom);
			for (int j = 0; j < TEST_SIZE - i; j++)
				QCOMPARE(treeRandom[j], QString(VSTRING).arg(treeRandomPos[j]));
		}
	}
	treeValid(treeRandom);
}

#define SHORT_TEST_SIZE (TEST_SIZE / 10)
void OrderedRBTreeTests::deepCheckValidity()
{
	tree.clear();
	QCOMPARE(tree.size(), 0);
	treeValid(tree);

	for (int i = 0; i < SHORT_TEST_SIZE; i++) {
		int pos = qrand() % SHORT_TEST_SIZE;
		tree.insert(QString(VSTRING).arg(pos), pos);
		QCOMPARE(tree.size(), i + 1);
		treeValid(tree);
	}
/*
	for (int i = 0; i < SHORT_TEST_SIZE; ) {
		int pos = qrand() % tree.size();
		tree.remove(pos);
		i++;
		QCOMPARE(tree.size(), SHORT_TEST_SIZE - i);
		treeValid(tree);
	}*/
}

QTEST_MAIN(OrderedRBTreeTests)
