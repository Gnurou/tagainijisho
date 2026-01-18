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

#include "tagaini_config.h"

#ifdef DEBUG_LISTS
#define CHECK_VALID checkValid();
#else
#define CHECK_VALID
#endif

/**
 * Post-order traversal that deletes all nodes in a tree.
 * Requires Node to have _left, _right, _parent members accessible from caller.
 * Must be called from a context that has friend access to Node's private members.
 */
template <typename Node> inline void deleteAllTreeNodes(Node *&root) {
    Node *current = root;
    while (current) {
        if (current->_left)
            current = current->_left;
        else if (current->_right)
            current = current->_right;
        else {
            Node *parent = current->_parent;
            if (current == root) {
                delete root;
                root = nullptr;
            } else if (current == parent->_left) {
                delete parent->_left;
                parent->_left = nullptr;
            } else {
                delete parent->_right;
                parent->_right = nullptr;
            }
            current = parent;
        }
    }
}

/**
 * Base node type lacking the logic to access parent or child nodes.
 */
template <class T> class OrderedRBNodeBase {
  public:
    enum class Color { Black, Red };

  protected:
    quint32 _leftSize;
    Color _color;

  public:
    using ValueType = T;
    OrderedRBNodeBase() : _leftSize(0), _color(Color::Red) {}

    Color color() const { return _color; }
    void setColor(Color col) { _color = col; }
    quint32 leftSize() const { return _leftSize; }
    void setLeftSize(quint32 lSize) { _leftSize = lSize; }
};

template <class T> class OrderedRBMemTree;

/**
 * A node type that implements hierarchy through simple pointers.
 */
template <class T> class OrderedRBNode : public OrderedRBNodeBase<T> {
    friend class OrderedRBTreeTests;

  private:
    OrderedRBNode<T> *_left = nullptr;
    OrderedRBNode<T> *_right = nullptr;
    OrderedRBNode<T> *_parent = nullptr;
    T _value;

  public:
    OrderedRBNode(OrderedRBMemTree<T> *, const T &va) : OrderedRBNodeBase<T>(), _value(va) {}

    const T &value() const { return _value; }
    void setValue(const T &nv) { _value = nv; }

    OrderedRBNode<T> *left() const { return _left; }
    OrderedRBNode<T> *right() const { return _right; }
    OrderedRBNode<T> *parent() const { return _parent; }
    void setLeft(OrderedRBNode<T> *nl) {
        _left = nl;
        if (nl)
            nl->setParent(this);
    }
    void setRight(OrderedRBNode<T> *nr) {
        _right = nr;
        if (nr)
            nr->setParent(this);
    }
    void setParent(OrderedRBNode<T> *np) { _parent = np; }
    void attachToTree(OrderedRBMemTree<T> *) {}

    friend class OrderedRBMemTree<T>;
};

template <class T> class OrderedRBMemTree {
  public:
    typedef OrderedRBNode<T> Node;

  private:
    Node *_root;

  public:
    OrderedRBMemTree() : _root(nullptr) {}

    /// Remove all the in-memory structures, without side-effect on the storage
    ~OrderedRBMemTree() { deleteAllTreeNodes(_root); }

    Node *root() const { return _root; }
    void setRoot(Node *node) { _root = node; }

    bool aboutToChange() { return true; }
    bool commitChanges() { return true; }
    void removeNode(Node *node) { delete node; }
    void abortChanges() {}
};

template <class TreeBase> class OrderedRBTree {
  private:
    using Node = typename TreeBase::Node;
    using Color = typename Node::Color;
    enum class Side { Left, Right };

    OrderedRBTree(const OrderedRBTree &) = delete;
    OrderedRBTree &operator=(const OrderedRBTree &) = delete;

    TreeBase _tree;

    void setRoot(Node *node);
    static Node *grandParent(const Node *node);
    static Node *uncle(const Node *node);
    static int size(const Node *node);
    void calculateLeftSize(Node *node);
    void detach(Node *node);
    void rotateLeft(Node *pivot);
    void rotateRight(Node *pivot);

    void insertCase1(Node *inserted);
    void insertCase2(Node *inserted);
    void insertCase3(Node *inserted);
    void insertCase4(Node *inserted);
    void insertCase5(Node *inserted);

    /**
     * Remove a node which has at most one child.
     * Precondition: node must have at most one child.
     */
    void removeOneChildNode(Node *node);
    /**
     * Exchange the position of a and b in the tree.
     */
    void exchangeNodes(Node *a, Node *b);
    // If the removed node was the root, the number of black nodes did
    // not change for every path (because it only had one child)
    void removeCase1(Node *parent, Side side);
    // If the sibling is red, set the parent (which was necessarily black)
    // to be red, let the sibling become black, and rotate before continuing
    // balancing
    void removeCase2(Node *parent, Side side);
    // Parent is black, as well as sibling and its children - just repaint
    // sibling red.
    void removeCase3(Node *parent, Side side);
    // Parent is red and sibling and its childs are black. Exchange the color of
    // parent and sibling.
    void removeCase4(Node *parent, Side side);
    void removeCase5(Node *parent, Side side);
    void removeCase6(Node *parent, Side side);

    bool isBlack(const Node *node) const {
        // Property 1 (all nodes are either red or black) is implicitly
        // enforced. Property 3: all leaves are black
        return !node || node->color() == Color::Black;
    }

    unsigned int treeValid(const Node *node, int depth, int &maxdepth) const;

  public:
    using TreeType = TreeBase;

    OrderedRBTree() = default;
    ~OrderedRBTree() = default;

    /**
     * Complexity: O(log n)
     */
    unsigned int size() const;
    /**
     * Returns the value at index. Will crash if access is made out of bounds.
     * Complexity: O(log n)
     */
    const typename Node::ValueType &operator[](int index) const;
    /**
     * Insert the value val at index.
     * Complexity: O(log n)
     * TODO No test to check whether we are inserting out of bounds! (both
     * inferior and superior)
     */
    bool insert(const typename Node::ValueType &val, int index);
    /**
     * Remove the value at position index. Returns true on success, false
     * otherwise. Complexity: O(log n)
     */
    bool remove(int index);
    bool clear();

    /**
     * Returns a pointer to the node at index, null if out-of-bounds.
     * @warning Low-level API!
     */
    const Node *getNode(int index) const;
    Node *getNode(int index) {
        return const_cast<Node *>(((const OrderedRBTree<TreeBase> *)this)->getNode(index));
    }

    /**
     * Inserts node at index, returns true on success, false in case of
     * out-of-bounds insertion attempt or storage error.
     * @warning Low-level API!
     */
    bool insertNode(Node *node, int index);

    /**
     * Remove given node from tree.
     * @warning Low-level API!
     */
    bool removeNode(Node *node);

    TreeBase *tree() { return &_tree; }
    const TreeBase *tree() const { return &_tree; }

    /**
     * Checks whether the tree is valid - causes a fatal error if it is not.
     */
    void inline checkValid() const {
#ifndef QT_NO_DEBUG
        int maxdepth = -1;
        // Property 2: root is black
        Q_ASSERT(!tree()->root() || tree()->root()->color() == Color::Black);
        Q_ASSERT(treeValid(tree()->root(), 0, maxdepth) == size());
#endif
    }
    friend class OrderedRBTreeTests;
};

template <class TreeBase> void OrderedRBTree<TreeBase>::setRoot(Node *node) {
    if (node)
        detach(node);
    _tree.setRoot(node);
}

template <class TreeBase>
typename OrderedRBTree<TreeBase>::Node *OrderedRBTree<TreeBase>::grandParent(const Node *node) {
    Node *p = node->parent();
    if (p)
        return p->parent();
    else
        return nullptr;
}

template <class TreeBase>
typename OrderedRBTree<TreeBase>::Node *OrderedRBTree<TreeBase>::uncle(const Node *node) {
    Node *gp = grandParent(node);
    if (!gp)
        return nullptr;
    if (node->parent() == gp->left())
        return gp->right();
    else
        return gp->left();
}

template <class TreeBase> int OrderedRBTree<TreeBase>::size(const Node *node) {
    int ret = 0;
    const Node *current = node;
    while (current) {
        ret += current->leftSize() + 1;
        current = current->right();
    }
    return ret;
}

template <class TreeBase> void OrderedRBTree<TreeBase>::calculateLeftSize(Node *node) {
    if (!node->left())
        node->setLeftSize(0);
    else
        node->setLeftSize(size(node->left()));
}

template <class TreeBase> void OrderedRBTree<TreeBase>::detach(Node *node) {
    if (node->parent()) {
        if (node->parent()->left() == node)
            node->parent()->setLeft(nullptr);
        else if (node->parent()->right() == node)
            node->parent()->setRight(nullptr);
        node->setParent(nullptr);
    }
}

template <class TreeBase> void OrderedRBTree<TreeBase>::rotateLeft(Node *pivot) {
    enum {
        ROOT,
        LEFT,
        RIGHT
    } parentSide = pivot->parent() ? pivot == pivot->parent()->left() ? LEFT : RIGHT : ROOT;
    Node *newParent = pivot->right();
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

template <class TreeBase> void OrderedRBTree<TreeBase>::rotateRight(Node *pivot) {
    enum {
        ROOT,
        LEFT,
        RIGHT
    } parentSide = pivot->parent() ? pivot == pivot->parent()->left() ? LEFT : RIGHT : ROOT;
    Node *newParent = pivot->left();
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

template <class TreeBase> void OrderedRBTree<TreeBase>::insertCase1(Node *inserted) {
    // Added a root node?
    if (!inserted->parent())
        inserted->setColor(Color::Black);
    else
        insertCase2(inserted);
}

template <class TreeBase> void OrderedRBTree<TreeBase>::insertCase2(Node *inserted) {
    // Parent black? Tree still valid
    if (inserted->parent()->color() == Color::Black)
        return;
    else
        insertCase3(inserted);
}

template <class TreeBase> void OrderedRBTree<TreeBase>::insertCase3(Node *inserted) {
    Node *_uncle = uncle(inserted);
    // Parent and uncle red? Recolor them.
    if (_uncle != 0 && _uncle->color() == Color::Red) {
        Node *_grandParent = grandParent(inserted);
        inserted->parent()->setColor(Color::Black);
        _uncle->setColor(Color::Black);
        _grandParent->setColor(Color::Red);
        insertCase1(_grandParent);
    } else
        insertCase4(inserted);
}

template <class TreeBase> void OrderedRBTree<TreeBase>::insertCase4(Node *inserted) {
    Node *_grandParent = grandParent(inserted);
    // Parent red, uncle black, inserted node and parent on
    // opposite sides from their parent
    if (inserted == inserted->parent()->right() && inserted->parent() == _grandParent->left()) {
        rotateLeft(inserted->parent());
        inserted = inserted->left();
    } else if (inserted == inserted->parent()->left() &&
               inserted->parent() == _grandParent->right()) {
        rotateRight(inserted->parent());
        inserted = inserted->right();
    }
    insertCase5(inserted);
}

template <class TreeBase> void OrderedRBTree<TreeBase>::insertCase5(Node *inserted) {
    // Parent red, uncle black, inserted node and parent on
    // same side from their parent
    Node *_grandParent = grandParent(inserted);
    inserted->parent()->setColor(Color::Black);
    _grandParent->setColor(Color::Red);
    if (inserted == inserted->parent()->left() && inserted->parent() == _grandParent->left()) {
        rotateRight(_grandParent);
    } else {
        rotateLeft(_grandParent);
    }
}

template <class TreeBase> void OrderedRBTree<TreeBase>::removeOneChildNode(Node *node) {
    // Update leftSizes
    for (Node *n = node;;) {
        Node *p = n->parent();
        if (!p)
            break;
        if (n == p->left())
            p->setLeftSize(p->leftSize() - 1);
        n = p;
    }
    Node *parent = node->parent();
    Node *child = node->left() ? node->left() : node->right();
    Side side = Side::Right;
    // Are we deleting the root?
    if (!parent)
        setRoot(child);
    else if (parent->left() == node) {
        parent->setLeft(child);
        side = Side::Left;
    } else {
        parent->setRight(child);
        side = Side::Right;
    }
    // child will only be null if both childs of node are leaves. In that case
    // child can have no sibling!

    // Perform balancing
    if (node->color() == Color::Black) {
        // Black node replaced with red one: recolor into black
        if (child && child->color() == Color::Red)
            child->setColor(Color::Black);
        // Black node replaced by black: black count in path changed, needs
        // rebalancing
        // This can only happen if node only had two leaf children, that is, if
        // child is null.
        else
            removeCase1(parent, side);
    }

    // The node is now out of the tree, so reset it
    detach(node);
    node->setLeft(nullptr);
    node->setRight(nullptr);
    node->setColor(Color::Red);
    node->setLeftSize(0);
}

template <class TreeBase> void OrderedRBTree<TreeBase>::exchangeNodes(Node *a, Node *b) {
    // Update the links
    Node *aleft = a->left(), *aright = a->right(), *aparent = a->parent();
    bool awasleft = aparent ? aparent->left() == a : false;
    Node *bleft = b->left(), *bright = b->right(), *bparent = b->parent();
    bool bwasleft = bparent ? bparent->left() == b : false;
    // Fix cross-references
    if (aleft == b)
        aleft = a;
    if (aright == b)
        aright = a;
    if (bleft == a)
        bleft = b;
    if (bright == a)
        bright = b;

    if (bparent) {
        if (bwasleft)
            bparent->setLeft(a);
        else
            bparent->setRight(a);
    } else
        setRoot(a);
    a->setLeft(bleft);
    a->setRight(bright);

    if (aparent) {
        if (awasleft)
            aparent->setLeft(b);
        else
            aparent->setRight(b);
    } else
        setRoot(b);
    b->setLeft(aleft);
    b->setRight(aright);
    // Update color & left count
    typename Node::Color acol = a->color();
    quint32 aleftSize = a->leftSize();
    a->setColor(b->color());
    a->setLeftSize(b->leftSize());
    b->setColor(acol);
    b->setLeftSize(aleftSize);
}

// If the removed node was the root, the number of black nodes did
// not change for every path (because it only had one child)
template <class TreeBase> void OrderedRBTree<TreeBase>::removeCase1(Node *parent, Side side) {
    if (parent)
        removeCase2(parent, side);
}

// If the sibling is red, set the parent (which was necessarily black)
// to be red, let the sibling become black, and rotate before continuing
// balancing
template <class TreeBase> void OrderedRBTree<TreeBase>::removeCase2(Node *parent, Side side) {
    Node *sibling = side == Side::Left ? parent->right() : parent->left();
    if (sibling->color() == Color::Red) {
        parent->setColor(Color::Red);
        sibling->setColor(Color::Black);
        if (sibling == parent->left())
            rotateRight(parent);
        else
            rotateLeft(parent);
    }
    removeCase3(parent, side);
}

// Parent is black, as well as sibling and its children - just repaint sibling
// red.
template <class TreeBase> void OrderedRBTree<TreeBase>::removeCase3(Node *parent, Side side) {
    Node *sibling = side == Side::Left ? parent->right() : parent->left();
    if (parent->color() == Color::Black && sibling->color() == Color::Black &&
        (!sibling->left() || sibling->left()->color() == Color::Black) &&
        (!sibling->right() || sibling->right()->color() == Color::Black)) {
        sibling->setColor(Color::Red);
        removeCase1(parent->parent(), parent->parent() && parent == parent->parent()->left()
                                          ? Side::Left
                                          : Side::Right);
    } else
        removeCase4(parent, side);
}

// Parent is red and sibling and its childs are black. Exchange the color of
// parent and sibling.
template <class TreeBase> void OrderedRBTree<TreeBase>::removeCase4(Node *parent, Side side) {
    Node *sibling = side == Side::Left ? parent->right() : parent->left();
    if (parent->color() == Color::Red && sibling->color() == Color::Black &&
        (!sibling->left() || sibling->left()->color() == Color::Black) &&
        (!sibling->right() || sibling->right()->color() == Color::Black)) {
        sibling->setColor(Color::Red);
        parent->setColor(Color::Black);
    } else
        removeCase5(parent, side);
}

template <class TreeBase> void OrderedRBTree<TreeBase>::removeCase5(Node *parent, Side side) {
    Node *sibling = side == Side::Left ? parent->right() : parent->left();
    if (sibling->color() == Color::Black) {
        if (sibling == parent->right() &&
            (!sibling->right() || sibling->right()->color() == Color::Black) &&
            (sibling->left() && sibling->left()->color() == Color::Red)) {
            sibling->setColor(Color::Red);
            sibling->left()->setColor(Color::Black);
            rotateRight(sibling);
        } else if (sibling == parent->left() &&
                   (!sibling->left() || sibling->left()->color() == Color::Black) &&
                   (sibling->right() && sibling->right()->color() == Color::Red)) {
            sibling->setColor(Color::Red);
            sibling->right()->setColor(Color::Black);
            rotateLeft(sibling);
        }
    }
    removeCase6(parent, side);
}

template <class TreeBase> void OrderedRBTree<TreeBase>::removeCase6(Node *parent, Side side) {
    Node *sibling = side == Side::Left ? parent->right() : parent->left();
    sibling->setColor(parent->color());
    parent->setColor(Color::Black);

    if (sibling == parent->right()) {
        if (sibling->right())
            sibling->right()->setColor(Color::Black);
        rotateLeft(parent);
    } else {
        if (sibling->left())
            sibling->left()->setColor(Color::Black);
        rotateRight(parent);
    }
}

template <class TreeBase>
const typename OrderedRBTree<TreeBase>::Node *OrderedRBTree<TreeBase>::getNode(int index) const {
    const Node *current = _tree.root();
    unsigned int baseIdx = 0;

    while (current) {
        int curPos = baseIdx + current->leftSize();
        if (curPos == index)
            break;
        else if (index < curPos)
            current = current->left();
        else {
            baseIdx += current->leftSize() + 1;
            current = current->right();
        }
    }
    return current;
}

template <class TreeBase> bool OrderedRBTree<TreeBase>::insertNode(Node *node, int index) {
    if (!_tree.aboutToChange())
        return false;

    Node *current = _tree.root();
    unsigned int baseIdx = 0;
    // Insert into root, only if index is zero because this means the tree is
    // empty
    if (!current) {
        if (index != 0)
            goto failure;
        setRoot(node);
    }
    // Otherwise find the leaf where to add our node
    else
        while (true) {
            int curPos = baseIdx + current->leftSize();
            // Add to the left, just follow the link
            if (index <= curPos) {
                if (!current->left()) {
                    // Overflow check
                    if (index != curPos)
                        goto failure;
                    current->setLeft(node);
                    break;
                } else
                    current = current->left();
            }
            // We add on the right, update the base position index
            else {
                baseIdx += current->leftSize() + 1;
                if (!current->right()) {
                    // Overflow check
                    if (index != curPos + 1)
                        goto failure;
                    current->setRight(node);
                    break;
                } else
                    current = current->right();
            }
        }
    // Take ownership of the node
    node->attachToTree(&_tree);
    // Update the leftSizes
    current = node;
    while (current) {
        if (current->parent() && current == current->parent()->left())
            current->parent()->setLeftSize(current->parent()->leftSize() + 1);
        current = current->parent();
    }

    // Perform balancing
    insertCase1(node);

    if (!_tree.commitChanges())
        goto failure;
    CHECK_VALID
    return true;

failure:
    _tree.abortChanges();
    return false;
}

template <class TreeBase> bool OrderedRBTree<TreeBase>::removeNode(Node *node) {
    if (!node)
        return false;

    if (!_tree.aboutToChange())
        return false;
    // Node has two childs, replace its value with the leftmost value at its
    // right side and replace that latter node with its right child before
    // deleting it.
    if (node->left() && node->right()) {
        Node *successor = node->right();
        while (successor->left())
            successor = successor->left();

        // Exchange node and successor
        exchangeNodes(node, successor);
    }
    // Node has only one child or no child, replace it by the child (or nothing)
    // before deleting
    removeOneChildNode(node);

    if (!_tree.commitChanges()) {
        _tree.abortChanges();
        return false;
    }

    CHECK_VALID
    return true;
}

template <class TreeBase> unsigned int OrderedRBTree<TreeBase>::size() const {
    if (!_tree.root())
        return 0;
    else
        return size(_tree.root());
}

template <class TreeBase>
const typename OrderedRBTree<TreeBase>::Node::ValueType &
OrderedRBTree<TreeBase>::operator[](int index) const {
    const Node *node = getNode(index);

    // Will crash if access is out of bounds because current would then be 0
    if (!node)
        qFatal("Error: accessing RBTree out of bounds");
    return node->value();
}

template <class TreeBase>
bool OrderedRBTree<TreeBase>::insert(const typename TreeBase::Node::ValueType &val, int index) {
    auto *newNode = new Node(&_tree, val);

    if (!insertNode(newNode, index)) {
        _tree.removeNode(newNode);
        return false;
    }
    return true;
}

template <class TreeBase> bool OrderedRBTree<TreeBase>::remove(int index) {
    Node *node = getNode(index);
    if (!node)
        return false;
    bool ret = removeNode(node);
    _tree.removeNode(node);
    return ret;
}

template <class TreeBase> bool OrderedRBTree<TreeBase>::clear() {
    if (!_tree.aboutToChange())
        return false;
    Node *current = _tree.root();
    while (current) {
        if (current->left())
            current = current->left();
        else if (current->right())
            current = current->right();
        else {
            Node *parent = current->parent();
            enum {
                ROOT,
                LEFT,
                RIGHT
            } sideToClear = parent ? current == parent->left() ? LEFT : RIGHT : ROOT;

            switch (sideToClear) {
            case ROOT:
                _tree.removeNode(_tree.root());
                setRoot(nullptr);
                break;
            case LEFT:
                _tree.removeNode(parent->left());
                parent->setLeft(nullptr);
                break;
            case RIGHT:
                _tree.removeNode(parent->right());
                parent->setRight(nullptr);
                break;
            }
            current = parent;
        }
    }
    if (!_tree.commitChanges())
        return false;
    CHECK_VALID
    return true;
}

template <class TreeBase>
unsigned int OrderedRBTree<TreeBase>::treeValid(const Node *node, int depth, int &maxdepth) const {
    // Property 5: every path from a node to any of its descendant contains the
    // same number of black nodes
    if (isBlack(node))
        ++depth;
    if (!node) {
        if (maxdepth == -1) {
            maxdepth = depth;
            return 0;
        } else {
            Q_ASSERT(depth == maxdepth);
            return 0;
        }
    }

    // Property 4: both children of every red node are black
    Q_ASSERT(isBlack(node) || ((isBlack(node->left()) && isBlack(node->right()))));

    // Check that the tree can be parsed two-ways
    Q_ASSERT(!node->left() || node->left()->parent() == node);
    Q_ASSERT(!node->right() || node->right()->parent() == node);

    // Check that we are not looping on ourselves
    Q_ASSERT(node->parent() != node);

    unsigned int leftSize = treeValid(node->left(), depth, maxdepth);
    unsigned int rightSize = treeValid(node->right(), depth, maxdepth);
    // Check that the leftSize of the node is valid
    Q_ASSERT(leftSize == node->leftSize());
    return leftSize + rightSize + 1;
}

#endif
