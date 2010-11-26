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

template <class T> class OrderedRBMemTree;

/**
 * A node type that implements hierarchy through simple pointers.
 */
template <class T> class OrderedRBNode : public OrderedRBNodeBase<T>
{
friend class OrderedRBTreeTests;
private:
	OrderedRBNode<T> *_left, *_right, *_parent;
	T _value;

public:
	OrderedRBNode(OrderedRBMemTree<T> *tree, const T &va) : OrderedRBNodeBase<T>(), _left(0), _right(0), _parent(0), _value(va)
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
		return _left;
	}
	OrderedRBNode<T> *right() const
	{
		return _right;
	}
	OrderedRBNode<T> *parent() const
	{
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
friend class OrderedRBMemTree<T>;
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

	/// Remove all the in-memory structures, without side-effect
	~OrderedRBMemTree()
	{
		Node *current = _root;
		while (current) {
			if (current->_left) current = current->_left;
			else if (current->_right) current = current->_right;
			else {
				Node *parent = current->_parent;
				if (current == _root) {
					delete _root;
					_root = 0;
				}
				else if (current == parent->_left) {
					delete parent->_left;
					parent->_left = 0;
				}
				else {
					delete parent->_right;
					parent->_right = 0;
				}
				current = parent;
			}
		}
	}

	Node *root() const { return _root; }
	void setRoot(Node *node) { _root = node; }

	bool aboutToChange() { return true; }
	bool commitChanges() { return true; }
	void removeNode(Node *node) { delete node; }
	void abortChanges() {}
};

template <class TreeBase> class OrderedRBTree
{
private:
	OrderedRBTree(const OrderedRBTree &);
	OrderedRBTree &operator =(const OrderedRBTree &);

	TreeBase _tree;

	void setRoot(typename TreeBase::Node *node);
	static typename TreeBase::Node *grandParent(const typename TreeBase::Node *const node);
	static typename TreeBase::Node *uncle(const typename TreeBase::Node *const node);
	static int size(const typename TreeBase::Node *node);
	void calculateLeftSize(typename TreeBase::Node *node);
	void detach(typename TreeBase::Node *node);
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



	bool _isBlack(const typename TreeBase::Node *const node) const
	{
		// Property 1 (all nodes are either red or black) is implicitly enforced.
		// Property 3: all leaves are black
		return !node || node->color() == TreeBase::Node::BLACK;
	}

	void _treeValid(const typename TreeBase::Node *const node, int depth, int &maxdepth) const;


public:
	OrderedRBTree()
	{
	}

	~OrderedRBTree()
	{
	}

	/**
	 * Complexity: O(log n)
	 */
	unsigned int size() const;
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
	bool insert(const typename TreeBase::Node::ValueType &val, int index);
	/**
	 * Remove the value at position index. Returns true on success, false otherwise.
	 * Complexity: O(log n)
	 */
	bool remove(int index);
	bool clear();

	/**
	 * Returns a pointer to the node at index, null if out-of-bounds.
	 * @warning Low-level API!
	 */
	const typename TreeBase::Node *getNode(int index) const;
	typename TreeBase::Node *getNode(int index) { return const_cast<typename TreeBase::Node *>(((const OrderedRBTree<TreeBase> *)this)->getNode(index)); }

	/**
	 * Inserts node at index, returns true on success, false in case of out-of-bounds insertion attempt or storage error.
	 * @warning Low-level API!
	 */
	bool insertNode(typename TreeBase::Node *node, int index);

	/**
	 * Remove given node from tree and delete it. Return false if out-of-bounds or storage error.
	 * @warning Low-level API!
	 */
	bool removeNode(typename TreeBase::Node *node);

	TreeBase *tree() { return &_tree; }
	const TreeBase *tree() const { return &_tree; }

	/**
	 * Checks whether the tree is valid - causes a fatal error if it is not.
	 */
	void inline checkValid() const
	{
		int maxdepth = -1;
		_treeValid(tree()->root(), 0, maxdepth);
	}
friend class OrderedRBTreeTests;
};

template <class TreeBase>
void OrderedRBTree<TreeBase>::setRoot(typename TreeBase::Node *node)
{
	if (node) detach(node);
	_tree.setRoot(node);
}

template <class TreeBase>
typename TreeBase::Node *OrderedRBTree<TreeBase>::grandParent(const typename TreeBase::Node *const node)
{
	typename TreeBase::Node *p = node->parent();
	if (p != 0) return p->parent();
	else return 0;
}
template <class TreeBase>
typename TreeBase::Node *OrderedRBTree<TreeBase>::uncle(const typename TreeBase::Node *const node)
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
	if (!inserted->parent()) inserted->setColor(TreeBase::Node::BLACK);
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
		typename TreeBase::Node *_grandParent = grandParent(inserted);
		inserted->parent()->setColor(TreeBase::Node::BLACK);
		_uncle->setColor(TreeBase::Node::BLACK);
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
	_tree.removeNode(node);
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
const typename TreeBase::Node *OrderedRBTree<TreeBase>::getNode(int index) const
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
	return current;
}

template <class TreeBase>
bool OrderedRBTree<TreeBase>::insertNode(typename TreeBase::Node *node, int index)
{
	if (!_tree.aboutToChange()) return false;

	typename TreeBase::Node *current = _tree.root();
	unsigned int baseIdx = 0;
	// Insert into root, only if index is zero because this means the tree is empty
	if (!current) {
		if (index != 0) goto failure;
		setRoot(node);
	}
	// Otherwise find the leaf where to add our node
	else while (true) {
		int curPos = baseIdx + current->leftSize();
		// Add to the left, just follow the link
		if (index <= curPos) {
			if (!current->left()) {
				// Overflow check
				if (index != curPos) goto failure;
				current->setLeft(node);
				break;
			}
			else current = current->left();
			}
		// We add on the right, update the base position index
		else {
			baseIdx += current->leftSize() + 1;
			if (!current->right()) {
				// Overflow check
				if (index != curPos + 1) goto failure;
				current->setRight(node);
				break;
			}
			else current = current->right();
		}
	}
	// Update the leftSizes
	current = node;
	while (current) {
		if (current->parent() && current == current->parent()->left())
			current->parent()->setLeftSize(current->parent()->leftSize() + 1);
		current = current->parent();
	}

	// Perform balancing
	insertCase1(node);

	if (!_tree.commitChanges()) goto failure;
	return true;

failure:
	_tree.abortChanges();
	return false;
}

template <class TreeBase>
bool OrderedRBTree<TreeBase>::removeNode(typename TreeBase::Node *node)
{
	if (!node) return false;

	if (!_tree.aboutToChange()) return false;
	// Node has two childs, replace its value with the leftmost value at its
	// right side and replace that latter node with its right child before deleting it.
	if (node->left() && node->right()) {
		typename TreeBase::Node *successor = node->right();
		while (successor->left()) successor = successor->left();
		node->setValue(successor->value());
		deleteOneChildNode(successor);
	}
	// Node has only one child or no child, replace it by the child (or nothing) before deleting
	else deleteOneChildNode(node);

	if (!_tree.commitChanges()) {
		_tree.abortChanges();
		return false;
	}
	return true;
}

template <class TreeBase>
unsigned int OrderedRBTree<TreeBase>::size() const
{
	if (!_tree.root()) return 0;
	else return size(_tree.root());
}

template <class TreeBase>
const typename TreeBase::Node::ValueType & OrderedRBTree<TreeBase>::operator[](int index) const
{
	const typename TreeBase::Node *node = getNode(index);

	// Will crash if access is out of bounds because current would then be 0
	if (!node) qFatal("Error: accessing RBTree out of bounds");
	return node->value();
}

template <class TreeBase>
bool OrderedRBTree<TreeBase>::insert(const typename TreeBase::Node::ValueType &val, int index)
{
	typename TreeBase::Node *newNode = new typename TreeBase::Node(&_tree, val);

	if (!insertNode(newNode, index)) {
		delete newNode;
		return false;
	}
	return true;
}

template <class TreeBase>
bool OrderedRBTree<TreeBase>::remove(int index)
{
	return removeNode(getNode(index));
}

template <class TreeBase>
bool OrderedRBTree<TreeBase>::clear()
{
	if (!_tree.aboutToChange()) return false;
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
				_tree.removeNode(_tree.root());
				setRoot(0);
				break;
			case LEFT:
				_tree.removeNode(parent->left());
				parent->setLeft(0);
				break;
			case RIGHT:
				_tree.removeNode(parent->right());
				parent->setRight(0);
				break;
			}
			current = parent;
		}
	}
	if (!_tree.commitChanges()) return false;
	return true;
}

template <class TreeBase>
void OrderedRBTree<TreeBase>::_treeValid(const typename TreeBase::Node *const node, int depth, int &maxdepth) const
{
	// Property 5: every path from a node to any of its descendant contains the same number of black nodes
	if (_isBlack(node)) ++depth;
	if (!node) {
		if (maxdepth == -1) {
			maxdepth = depth;
			return;
		}
		else {
			Q_ASSERT(depth == maxdepth);
			return;
		}
	}
	// Property 2: root is black
	Q_ASSERT(node->parent() || node->color() == TreeBase::Node::BLACK);
	// Property 4: both children of every red node are black
	Q_ASSERT(_isBlack(node) || ((_isBlack(node->left()) && _isBlack(node->right()))));

	// Check that the tree can be parsed two-ways
	Q_ASSERT(!node->left() || node->left()->parent() == node);
	Q_ASSERT(!node->right() || node->right()->parent() == node);

	_treeValid(node->left(), depth, maxdepth);
	_treeValid(node->right(), depth, maxdepth);
}

#endif

