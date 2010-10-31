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

/**
 * Red-Black tree inspired by the Wikipedia example:
 * http://en.wikipedia.org/wiki/Red-black_tree
 */

#ifndef __CORE_ORDEREDRBNODE_H
#define __CORE_ORDEREDRBNODE_H

#include <QtDebug>
#include <QtGlobal>

/**
 * Base node type lacking the logic to access parent or child nodes.
 */
template <class T> class OrderedRBNodeBase
{
public:
	typedef enum { BLACK, RED } Color;
protected:
	quint32 _leftSize;
	Color _color;

public:
	typedef T ValueType;
	OrderedRBNodeBase() : _leftSize(0), _color(RED)
	{
	}

	virtual ~OrderedRBNodeBase()
	{
	}

	Color color() const { return _color; }
	void setColor(Color col) { _color = col; }
	quint32 leftSize() const { return _leftSize; }
	void setLeftSize(quint32 lSize) { _leftSize = lSize; }
};

/**
 * A node type that implements hierarchy through simple pointers. This class is final and
 * do not need any virtual functions.
 */
template <class T> class OrderedRBNode : public OrderedRBNodeBase<T>
{
friend class OrderedRBTreeTests;
private:
	OrderedRBNode<T> *_left, *_right, *_parent;
	T _value;

public:
	OrderedRBNode(const T &va) : _left(0), _right(0), _parent(0), _value(va)
	{
	}

	~OrderedRBNode()
	{
	}

	const T &value() const { return _value; }
	virtual void setValue(const T &nv)
	{
		_value = nv;
	}

	OrderedRBNode<T> *left() const
	{
		// TODO create if needed
		return _left;
	}
	OrderedRBNode<T> *right() const
	{
		// TODO create if needed
		return _right;
	}
	OrderedRBNode<T> *parent() const
	{
		// TODO create if needed
		return _parent;
	}
	void setLeft(OrderedRBNode<T> *nl)
	{
		_left = nl;
		if (nl) nl->setParent(this);
	}
	void setRight(OrderedRBNode<T> *nr)
	{
		_right = nr;
		if (nr) nr->setParent(this);
	}
	void setParent(OrderedRBNode<T> *np)
	{
		_parent = np;
	}
};

template <class T> class OrderedRBMemTree
{
public:
	typedef OrderedRBNode<T> Node;

private:
	Node *_root;

public:
	OrderedRBMemTree() : _root(0)
	{
	}

	Node *root() const { return _root; }
	void setRoot(Node *node) { _root = node; }

	bool aboutToChange() { return true; }
	void changeNode(Node *n) {}
	bool changed() { return true; }
	void abortChanges() {}
};

template <class TreeBase> class OrderedRBTree
{
private:
	TreeBase _tree;

	void setRoot(typename TreeBase::Node *node);
	static typename TreeBase::Node *grandParent(const typename TreeBase::Node *node);
	static typename TreeBase::Node *uncle(const typename TreeBase::Node *node);
	static int size(const typename TreeBase::Node *node);
	static void calculateLeftSize(typename TreeBase::Node *node);
	static void detach(typename TreeBase::Node *node);
	void rotateLeft(typename TreeBase::Node *pivot);
	void rotateRight(typename TreeBase::Node *pivot);

	void insertCase1(typename TreeBase::Node *inserted);
	void insertCase2(typename TreeBase::Node *inserted);
	void insertCase3(typename TreeBase::Node *inserted);
	void insertCase4(typename TreeBase::Node *inserted);
	void insertCase5(typename TreeBase::Node *inserted);

	typedef enum { Left, Right } Side;

	/**
	 * Delete a node which has at most one child.
	 * Precondition: node must have at most one child.
	 */
	void deleteOneChildNode(typename TreeBase::Node *node);
	// If the removed node was the root, the number of black nodes did
	// not change for every path (because it only had one child)
	void removeCase1(typename TreeBase::Node *parent, Side side);
	// If the sibling is red, set the parent (which was necessarily black)
	// to be red, let the sibling become black, and rotate before continuing
	// balancing
	void removeCase2(typename TreeBase::Node *parent, Side side);
	// Parent is black, as well as sibling and its children - just repaint sibling
	// red.
	void removeCase3(typename TreeBase::Node *parent, Side side);
	// Parent is red and sibling and its childs are black. Exchange the color of parent
	// and sibling.
	void removeCase4(typename TreeBase::Node *parent, Side side);
	void removeCase5(typename TreeBase::Node *parent, Side side);
	void removeCase6(typename TreeBase::Node *parent, Side side);

public:
	OrderedRBTree()
	{
	}

	~OrderedRBTree()
	{
		clear();
	}

	/**
	 * Complexity: O(log n)
	 */
	int size() const;
	/**
	 * Returns the value at index. Will crash if access is made out of bounds.
	 * Complexity: O(log n)
	 */
	const typename TreeBase::Node::ValueType & operator[](int index) const;
	/**
	 * Insert the value val at index.
	 * Complexity: O(log n)
	 * TODO No test to check whether we are inserting out of bounds! (both inferior and superior)
	 */
	void insert(const typename TreeBase::Node::ValueType &val, int index);
	/**
	 * Remove the value at position index. Returns true on success, false otherwise.
	 * Complexity: O(log n)
	 */
	bool remove(int index);
	void clear();

	const TreeBase *tree() const { return &_tree; }
	TreeBase *tree() { return &_tree; }

friend class OrderedRBTreeTests;
};

template <class TreeBase>
void OrderedRBTree<TreeBase>::setRoot(typename TreeBase::Node *node)
{
	if (node) detach(node);
	_tree.setRoot(node);
}

template <class TreeBase>
typename TreeBase::Node *OrderedRBTree<TreeBase>::grandParent(const typename TreeBase::Node *node)
{
	typename TreeBase::Node *p = node->parent();
	if (p != 0) return p->parent();
	else return 0;
}
template <class TreeBase>
typename TreeBase::Node *OrderedRBTree<TreeBase>::uncle(const typename TreeBase::Node *node)
{
	typename TreeBase::Node *gp = grandParent(node);
	if (!gp) return 0;
	if (node->parent() == gp->left()) return gp->right();
	else return gp->left();
}

template <class TreeBase>
int OrderedRBTree<TreeBase>::size(const typename TreeBase::Node *node)
{
	int ret = 0;
	const typename TreeBase::Node *current = node;
	while (current) {
		ret += current->leftSize() + 1;
		current = current->right();
	}
	return ret;
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::calculateLeftSize(typename TreeBase::Node *node)
{
	if (!node->left()) node->setLeftSize(0);
	else node->setLeftSize(size(node->left()));
}
	
template <class TreeBase>
void OrderedRBTree<TreeBase>::detach(typename TreeBase::Node *node)
{
	if (node->parent()) {
		if (node->parent()->left() == node) node->parent()->setLeft(0);
		else if (node->parent()->right() == node) node->parent()->setRight(0);
		node->setParent(0);
	}
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::rotateLeft(typename TreeBase::Node *pivot)
{
	enum { ROOT, LEFT, RIGHT } parentSide = pivot->parent() ?
		pivot == pivot->parent()->left() ? LEFT : RIGHT : ROOT;
	typename TreeBase::Node *newParent = pivot->right();
	// Move right child to node's place
	switch (parentSide) {
	case ROOT:
		setRoot(newParent);
		break;
	case LEFT:
		pivot->parent()->setLeft(newParent);
		break;
	case RIGHT:
		pivot->parent()->setRight(newParent);
		break;
	}
	// Move node as the left child of its right child
	pivot->setRight(newParent->left());
	// Move node to new parent's left
	newParent->setLeft(pivot);
	// Update left weight of rotated node
	calculateLeftSize(newParent);
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::rotateRight(typename TreeBase::Node *pivot)
{
	enum { ROOT, LEFT, RIGHT } parentSide = pivot->parent() ?
		pivot == pivot->parent()->left() ? LEFT : RIGHT : ROOT;
	typename TreeBase::Node *newParent = pivot->left();
	// Move left child to node's place
	switch (parentSide) {
	case ROOT:
		setRoot(newParent);
		break;
	case LEFT:
		pivot->parent()->setLeft(newParent);
		break;
	case RIGHT:
		pivot->parent()->setRight(newParent);
		break;
	}
	// Move node as the right child of its left child
	pivot->setLeft(newParent->right());
	// Move node to new parent's right
	newParent->setRight(pivot);
	// Update left weight of rotated node
	calculateLeftSize(pivot);
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::insertCase1(typename TreeBase::Node *inserted)
{
	// Added a root Node<T>?
	if (!inserted->parent())
		inserted->setColor(TreeBase::Node::BLACK);
	else insertCase2(inserted);
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::insertCase2(typename TreeBase::Node *inserted)
{
	// Parent black? Tree still valid
	if (inserted->parent()->color() == TreeBase::Node::BLACK) return;
	else insertCase3(inserted);
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::insertCase3(typename TreeBase::Node *inserted)
{
	typename TreeBase::Node *_uncle = uncle(inserted);
	// Parent and uncle red? Recolor them.
	if (_uncle != 0 && _uncle->color() == TreeBase::Node::RED) {
		inserted->parent()->setColor(TreeBase::Node::BLACK);
		_uncle->setColor(TreeBase::Node::BLACK);
		typename TreeBase::Node *_grandParent = grandParent(inserted);
		_grandParent->setColor(TreeBase::Node::RED);
		insertCase1(_grandParent);
	} else insertCase4(inserted);
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::insertCase4(typename TreeBase::Node *inserted)
{
	typename TreeBase::Node *_grandParent = grandParent(inserted);
	// Parent red, uncle black, inserted node and parent on
	// opposite sides from their parent
	if (inserted == inserted->parent()->right() && inserted->parent() == _grandParent->left()) {
		rotateLeft(inserted->parent());
		inserted = inserted->left();
	} else if (inserted == inserted->parent()->left() && inserted->parent() == _grandParent->right()) {
		rotateRight(inserted->parent());
		inserted = inserted->right();
	}
	insertCase5(inserted);
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::insertCase5(typename TreeBase::Node *inserted)
{
	// Parent red, uncle black, inserted Node<T> and parent on
	// same side from their parent
	typename TreeBase::Node *_grandParent = grandParent(inserted);
	inserted->parent()->setColor(TreeBase::Node::BLACK);
	_grandParent->setColor(TreeBase::Node::RED);
	if (inserted == inserted->parent()->left() && inserted->parent() == _grandParent->left()) {
		rotateRight(_grandParent);
	} else {
		rotateLeft(_grandParent);
	}
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::deleteOneChildNode(typename TreeBase::Node *node)
{
	// Update leftSizes
	for (typename TreeBase::Node *n = node; ; ) {
		typename TreeBase::Node *p = n->parent();
		if (p == 0) break;
		if (n == p->left()) p->setLeftSize(p->leftSize() - 1);
		n = p;
	}
	typename TreeBase::Node *parent = node->parent();
	typename TreeBase::Node *child = node->left() ? node->left() : node->right();
	Side side = Right;
	// Are we deleting the root?
	if (!parent) setRoot(child);
	else if (parent->left() == node) {
		parent->setLeft(child);
		side = Left;
	}
	else {
		parent->setRight(child);
		side = Right;
	}

	// child will only be null if both childs of node are leaves. In that case
	// child can have no sibling!

	// Perform balancing
	if (node->color() == TreeBase::Node::BLACK) {
		// Black node replaced with red one: recolor into black
		if (child && child->color() == TreeBase::Node::RED) child->setColor(TreeBase::Node::BLACK);
		// Black node replaced by black: black count in path changed, needs
		// rebalancing
		// This can only happen if node only had two leaf children, that is, if
		// child is null.
		else removeCase1(parent, side);
	}
	delete node;
}

// If the removed node was the root, the number of black nodes did
// not change for every path (because it only had one child)
template <class TreeBase>
void OrderedRBTree<TreeBase>::removeCase1(typename TreeBase::Node *parent, Side side)
{
	if (parent) removeCase2(parent, side);
}

// If the sibling is red, set the parent (which was necessarily black)
// to be red, let the sibling become black, and rotate before continuing
// balancing
template <class TreeBase>
void OrderedRBTree<TreeBase>::removeCase2(typename TreeBase::Node *parent, Side side)
{
	typename TreeBase::Node *sibling = side == Left ? parent->right() : parent->left();
	if (sibling->color() == TreeBase::Node::RED) {
		parent->setColor(TreeBase::Node::RED);
		sibling->setColor(TreeBase::Node::BLACK);
		if (sibling == parent->left()) rotateRight(parent);
		else rotateLeft(parent);
	}
	removeCase3(parent, side);
}

// Parent is black, as well as sibling and its children - just repaint sibling
// red.
template <class TreeBase>
void OrderedRBTree<TreeBase>::removeCase3(typename TreeBase::Node *parent, Side side)
{
	typename TreeBase::Node *sibling = side == Left ? parent->right() : parent->left();
	if (parent->color() == TreeBase::Node::BLACK &&
	    sibling->color() == TreeBase::Node::BLACK &&
	    (!sibling->left() || sibling->left()->color() == TreeBase::Node::BLACK) &&
	    (!sibling->right() || sibling->right()->color() == TreeBase::Node::BLACK)) {
		sibling->setColor(TreeBase::Node::RED);
		removeCase1(parent->parent(), parent->parent() && parent == parent->parent()->left() ? Left : Right);
	}
	else removeCase4(parent, side);
}

// Parent is red and sibling and its childs are black. Exchange the color of parent
// and sibling.
template <class TreeBase>
void OrderedRBTree<TreeBase>::removeCase4(typename TreeBase::Node *parent, Side side)
{
	typename TreeBase::Node *sibling = side == Left ? parent->right() : parent->left();
	if (parent->color() == TreeBase::Node::RED &&
	    sibling->color() == TreeBase::Node::BLACK &&
	    (!sibling->left() || sibling->left()->color() == TreeBase::Node::BLACK) &&
	    (!sibling->right() || sibling->right()->color() == TreeBase::Node::BLACK)) {
		sibling->setColor(TreeBase::Node::RED);
		parent->setColor(TreeBase::Node::BLACK);
	}
	else removeCase5(parent, side);
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::removeCase5(typename TreeBase::Node *parent, Side side)
{
	typename TreeBase::Node *sibling = side == Left ? parent->right() : parent->left();
	if (sibling->color() == TreeBase::Node::BLACK) {
		if (sibling == parent->right() &&
		    (!sibling->right() || sibling->right()->color() == TreeBase::Node::BLACK) &&
		    (sibling->left() && sibling->left()->color() == TreeBase::Node::RED)) {
			sibling->setColor(TreeBase::Node::RED);
			sibling->left()->setColor(TreeBase::Node::BLACK);
			rotateRight(sibling);
		} else if (sibling == parent->left() &&
		    (!sibling->left() || sibling->left()->color() == TreeBase::Node::BLACK) &&
		    (sibling->right() && sibling->right()->color() == TreeBase::Node::RED)) {
			sibling->setColor(TreeBase::Node::RED);
			sibling->right()->setColor(TreeBase::Node::BLACK);
			rotateLeft(sibling);
		}
	}
	removeCase6(parent, side);
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::removeCase6(typename TreeBase::Node *parent, Side side)
{
	typename TreeBase::Node *sibling = side == Left ? parent->right() : parent->left();
	sibling->setColor(parent->color());
	parent->setColor(TreeBase::Node::BLACK);

	if (sibling == parent->right()) {
		if (sibling->right()) sibling->right()->setColor(TreeBase::Node::BLACK);
		rotateLeft(parent);
	} else {
		if (sibling->left()) sibling->left()->setColor(TreeBase::Node::BLACK);
		rotateRight(parent);
	}
}

template <class TreeBase>
int OrderedRBTree<TreeBase>::size() const
{
	if (!_tree.root()) return 0;
	else return size(_tree.root());
}

template <class TreeBase>
const typename TreeBase::Node::ValueType & OrderedRBTree<TreeBase>::operator[](int index) const
{
	const typename TreeBase::Node *current = _tree.root();
	unsigned int baseIdx = 0;

	while (current) {
		int curPos = baseIdx + current->leftSize();
		if (curPos == index) break;
		else if (index < curPos) current = current->left();
		else {
			baseIdx += current->leftSize() + 1;
			current = current->right();
		}
	}
	// Will crash if access is out of bounds because current would then be 0
	if (!current) qFatal("Error: accessing RBTree out of bounds");
	return current->value();
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::insert(const typename TreeBase::Node::ValueType &val, int index)
{
	typename TreeBase::Node *current = _tree.root();
	unsigned int baseIdx = 0;
	typename TreeBase::Node *newNode = new typename TreeBase::Node(val);

	// Insert into root
	if (!current) setRoot(newNode);
	// Otherwise find the leaf where to add our node
	else while (true) {
		int curPos = baseIdx + current->leftSize();
		// We add on the left, so leftSize must be updated
		if (index <= curPos) {
			current->setLeftSize(current->leftSize() + 1);
			if (!current->left()) {
				current->setLeft(newNode);
				break;
			}
			else current = current->left();
			}
		// We add on the right, update the base position index
		else {
			baseIdx += current->leftSize() + 1;
			if (!current->right()) {
				current->setRight(newNode);
				break;
			}
			else current = current->right();
		}
	}
	// Perform balancing
	insertCase1(newNode);
}

template <class TreeBase>
bool OrderedRBTree<TreeBase>::remove(int index)
{
	if (index < 0) return false;

	typename TreeBase::Node *current = _tree.root();
	unsigned int baseIdx = 0;

	// First find the node to remove
	while (current) {
		int curPos = baseIdx + current->leftSize();
		if (curPos == index) break;
		else if (index < curPos) current = current->left();
		else {
			baseIdx += current->leftSize() + 1;
			current = current->right();
		}
	}

	// Index to remove not found
	if (!current) return false;

	// Node has two childs, replace its value with the leftmost value at its
	// right side and replace that latter node with its right child before deleting it.
	if (current->left() && current->right()) {
		typename TreeBase::Node *successor = current->right();
		while (successor->left()) successor = successor->left();
		current->setValue(successor->value());
		deleteOneChildNode(successor);
		return true;
	}
	// Node has only one child, replace it by the child before deleting
	else if (current->left() || current->right()) {
		deleteOneChildNode(current);
		return true;
	}
	// We are removing a leaf
	else {
		deleteOneChildNode(current);
		return true;
	}
	return false;
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::clear()
{
	typename TreeBase::Node *current = _tree.root();
	while (current) {
		if (current->left()) current = current->left();
		else if (current->right()) current = current->right();
		else {
			typename TreeBase::Node *parent = current->parent();
			enum { ROOT, LEFT, RIGHT } sideToClear = parent ?
				current == parent->left() ? LEFT : RIGHT : ROOT;

			switch (sideToClear) {
			case ROOT:
				delete _tree.root();
				setRoot(0);
				break;
			case LEFT:
				delete parent->left();
				parent->setLeft(0);
				break;
			case RIGHT:
				delete parent->right();
				parent->setRight(0);
				break;
			}
			current = parent;
		}
	}
}

#endif

