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
			ret += current->leftSize + 1;
			current = current->right;
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

	int calculateLeftSize()
	{
		int size = 0;
		OrderedRBNode<T> *current = left;
		while (current) {
			size += current->leftSize + 1;
			current = current->right;
		}
		return size;
	}


friend class OrderedRBTree<T>;
friend class OrderedRBTreeTests;
};

template <class T>
class OrderedRBTree
{
private:
	OrderedRBNode<T> *root;

	void insertCase1(OrderedRBNode<T> *inserted)
	{
		// Added a root node?
		if (!inserted->parent)
			inserted->color = OrderedRBNode<T>::BLACK;
		else insertCase2(inserted);
	}

	void insertCase2(OrderedRBNode<T> *inserted)
	{
		// Parent black? Tree still valid
		if (inserted->parent->color == OrderedRBNode<T>::BLACK) return;
		else insertCase3(inserted);
	}

	void insertCase3(OrderedRBNode<T> *inserted)
	{
		OrderedRBNode<T> *uncle = inserted->uncle();
		// Parent and uncle red? Recolor them.
		if (uncle != 0 && uncle->color == OrderedRBNode<T>::RED) {
			inserted->parent->color = OrderedRBNode<T>::BLACK;
			uncle->color = OrderedRBNode<T>::BLACK;
			OrderedRBNode<T> *grandParent = inserted->grandParent();
			grandParent->color = OrderedRBNode<T>::RED;
			insertCase1(grandParent);
		} else insertCase4(inserted);
	}

	void insertCase4(OrderedRBNode<T> *inserted)
	{
		OrderedRBNode<T> *grandParent = inserted->grandParent();
		// Parent red, uncle black, inserted node and parent on
		// opposite sides from their parent
		if (inserted == inserted->parent->right && inserted->parent == grandParent->left) {
			rotateLeft(inserted->parent);
			inserted = inserted->left;
		} else if (inserted == inserted->parent->left && inserted->parent == grandParent->right) {
			rotateRight(inserted->parent);
			inserted = inserted->right;
		}
		insertCase5(inserted);
	}

	void insertCase5(OrderedRBNode<T> *inserted)
	{
		// Parent red, uncle black, inserted node and parent on
		// same side from their parent
		OrderedRBNode<T> *grandParent = inserted->grandParent();
		inserted->parent->color = OrderedRBNode<T>::BLACK;
		grandParent->color = OrderedRBNode<T>::RED;
		if (inserted == inserted->parent->left && inserted->parent == grandParent->left) {
			rotateRight(grandParent);
		} else {
			rotateLeft(grandParent);
		}
	}

	void rotateLeft(OrderedRBNode<T> *node)
	{
		OrderedRBNode<T> **parentLink = node->parent ?
			node == node->parent->left ? &node->parent->left : &node->parent->right :
			&root;
		// Move right child to node's place
		*parentLink = node->right;
		node->right->parent = node->parent;
		// Move node as the left child of its right child
		node->right = (*parentLink)->left;
		if (node->right) node->right->parent = node;
		// Move node to new parent's left
		(*parentLink)->left = node;
		node->parent = *parentLink;
		// Update left weight of rotated node
		(*parentLink)->leftSize = (*parentLink)->calculateLeftSize();
	}

	void rotateRight(OrderedRBNode<T> *node)
	{
		OrderedRBNode<T> **parentLink = node->parent ?
			node == node->parent->left ? &node->parent->left : &node->parent->right :
			&root;
		// Move left child to node's place
		*parentLink = node->left;
		node->left->parent = node->parent;
		// Move node as the right child of its left child
		node->left = (*parentLink)->right;
		if (node->left) node->left->parent = node;
		// Move node to new parent's right
		(*parentLink)->right = node;
		node->parent = *parentLink;
		// Update left weight of rotated node
		node->leftSize = node->calculateLeftSize();
	}

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

	/**
	 * Insert the value val at index.
	 */
	void insert(const T &val, int index)
	{
		OrderedRBNode<T> **current = &root;
		OrderedRBNode<T> *parent = 0;
		unsigned int baseIdx = 0;

		// First find the leaf where to add our node
		while (*current) {
			parent = *current;
			int curPos = baseIdx + (*current)->leftSize;
			// We add on the left, so leftSize must be updated
			if (index <= curPos) {
				++(*current)->leftSize;
				current = &(*current)->left;
			}
			// We add on the right, update the base position index
			else {
				baseIdx += (*current)->leftSize + 1;
				current = &(*current)->right;
			}
		}
		// Add the new leaf
		*current = new OrderedRBNode<T>(val);
		(*current)->parent = parent;

		// Perform balancing
		insertCase1(*current);
	}

	/**
	 * Returns the value at index. Will crash if access is made out of bounds.
	 */
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

friend class OrderedRBTreeTests;
};

#endif
