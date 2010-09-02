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

#include <QObject>
#include <QTest>

#include "core/OrderedRBNode.h"

class OrderedRBTreeTests : public QObject
{
Q_OBJECT
private:
	// Property 1 (all nodes are either red or black) is implicitly enforced.
	template <class T>
	bool isBlack(const OrderedRBNode<T> *const node)
	{
		// Property 3: all leaves are black
		return !node || node->color == OrderedRBNode<T>::BLACK;
	}

	template <class T>
	void _treeValid(const OrderedRBNode<T> *const node, int depth, int &maxdepth)
	{
		// Property 5: every path from a node to any of its descendants contains the same number of black nodes
		if (isBlack(node)) ++depth;
		if (!node) {
			if (maxdepth == -1) {
				maxdepth = depth;
				return;
			}
			else {
				QVERIFY(depth == maxdepth);
				return;
			}
		}
		// Property 2: root is black
		QVERIFY(node->parent || node->color == OrderedRBNode<T>::BLACK);
		// Property 4: both children of every red node are black
		QVERIFY(isBlack(node) || ((isBlack(node->left) && isBlack(node->right))));

		QVERIFY(!node->left || node->left->parent == node);
		QVERIFY(!node->right || node->right->parent == node);

		_treeValid(node->left, depth, maxdepth);
		_treeValid(node->right, depth, maxdepth);
	}

	template <class T>
	void treeValid(const OrderedRBTree<T> &tree)
	{
		int maxdepth = -1;
		_treeValid(tree.root, 0, maxdepth);
	}

	OrderedRBTree<QString> tree;

private slots:
	void initTestCase();
	void cleanupTestCase();

	void massInsertEnd();
	void massInsertBegin();
	void deepCheckValidity();
};
