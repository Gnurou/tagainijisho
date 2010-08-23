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

#ifndef __CORE_ORDEREDRBNODE_H
#define __CORE_ORDEREDRBNODE_H

#include <QtGlobal>

template <class T>
class OrderedRBTree;

template <class T>
class OrderedRBNode {
private:
	OrderedRBNode<T> *left, *right;
	OrderedRBNode<T> *parent;

	quint32 leftSize;
	enum { BLACK, RED } color;
	T value;

	OrderedRBNode(const T &va) : left(0), right(0), parent(0), leftSize(0), color(RED), value(va)
	{
	}

	int size() const
	{
		int ret = 0;
		const OrderedRBNode<T> *current = this;
		while (current) {
			ret += leftSize + 1;
			current = right;
		}
		return ret;
	}

	int position() const
	{
		const OrderedRBNode<T> *current = this;
		const OrderedRBNode<T> *curParent = this->parent;
		int ret = this->leftSize;
		while (curParent) {
			if (current = curParent->right) ret += curParent->leftSize + 1;
			current = curParent;
			curParent = curParent->parent;
		}
		return ret;
	}

	OrderedRBNode<T> *grandParent()
	{
		if (parent != 0) return parent->parent;
		else return 0;
	}

	OrderedRBNode<T> *uncle()
	{
		OrderedRBNode<T> *gp = grandParent();
		if (!gp) return 0;
		if (parent == gp->left) return gp->right;
		else return gp->left;
	}


friend class OrderedRBTree<T>;
};

template <class T>
class OrderedRBTree
{
private:
	OrderedRBNode<T> *root;

public:
	OrderedRBTree() : root(0)
	{
	}

	~OrderedRBTree()
	{
		OrderedRBNode<T> *current = root;
		while (current) {
			if (current->left) current = current->left;
			else if (current->right) current = current->right;
			else {
				OrderedRBNode<T> *parent = current->parent;
				OrderedRBNode<T> *&toClear = parent ? (current == parent->left) ? parent->left : parent->right : current;
				delete toClear;
				toClear = 0;
				current = parent;
			}
		}
	}

	int size() const
	{
		if (!root) return 0;
		else return root->size();
	}

	void insert(const T &val, int index)
	{
		OrderedRBNode<T> *&current = root;
		OrderedRBNode<T> *parent = 0;
		unsigned int baseIdx = 0;

		// First find the leaf where to add our node
		while (current) {
			int curPos = baseIdx + current->leftSize;
			// We add on the left, so leftSize must be updated
			if (index <= curPos) {
				++current->leftSize;
				current = current->left;
			}
			// We add on the right, update the base position index
			else {
				baseIdx += current->leftSize + 1;
				current = current->right;
			}
			parent = root->parent;
		}
		// Add the new leaf
		current = new OrderedRBNode<T>(val);
		current->parent = parent;

		// Now perform balancing
		// Added a root node?
		if (!current->parent) {
			current->color = OrderedRBNode<T>::BLACK;
		}
		// Ensure that all red nodes have black children
		else if (current->parent->color == OrderedRBNode<T>::RED) {

		}
	}

	const T& operator[](int index) const
	{
		const OrderedRBNode<T> *current = root;
		unsigned int baseIdx = 0;

		while (current) {
			int curPos = baseIdx + current->leftSize;
			if (curPos == index) break;
			else if (curPos < index) current = current->left;
			else {
				baseIdx += current->leftSize + 1;
				current = current->right;
			}
		}
		// Will crash if access is out of bounds because current would then be 0
		return current->value;
	}
};

#endif
