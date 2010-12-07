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
#ifndef __CORE_ORDEREDRBDBNODE_H
#define __CORE_ORDEREDRBDBNODE_H

#include "core/OrderedRBNode.h"
#include "core/EntryListDB.h"

#include <QSet>

template <class T> class OrderedRBDBTree;

/**
 * Implements database storage for RB tree nodes.
 * When a node is created, all its data is loaded and cached.
 * When the left/right members are first accessed, the corresponding
 * node is created and loaded.
 */
template <class T> class OrderedRBDBNode : public OrderedRBNodeBase<T>
{
private:
	OrderedRBDBTree<T> * _tree;
	mutable OrderedRBDBNode<T> * _left;
	mutable OrderedRBDBNode<T> * _right;
	mutable OrderedRBDBNode<T> * _parent;
	DBListEntry<T> e;

public:
	OrderedRBDBNode(OrderedRBDBTree<T> *tree, const T &va) : OrderedRBNodeBase<T>(), _tree(tree), _left(0), _right(0), _parent(0), e()
	{
		// Here a new node is to be inserted in the tree - we need to insert it right now into
		// the DB in order to get its ID.
		setValue(va);
		// Propage the color information up to the DB layer (e.red is false at construction time) 
		setColor(OrderedRBDBNode<T>::RED);
		updateDB();
	}

	OrderedRBDBNode(OrderedRBDBTree<T> *tree, quint32 rowid) : OrderedRBNodeBase<T>(), _tree(tree), _left(0), _right(0), _parent(0), e()
	{
		// The new node is expected to exist in the DB with the given ID - just load it.
		e = _tree->dbAccess()->getEntry(rowid);
		setColor(e.red ?  OrderedRBNodeBase<T>::RED : OrderedRBNodeBase<T>::BLACK);
		setLeftSize(e.leftSize);
	}

	~OrderedRBDBNode()
	{
		// This destructor should not remove the node from the database as it would break data persistency.
		// In order to permanently remove nodes, removeNode() should be used instead.
	}

	void setColor(typename OrderedRBNodeBase<T>::Color col)
	{
		OrderedRBNodeBase<T>::setColor(col);
		e.red = (col == OrderedRBNodeBase<T>::RED);
		_tree->nodeChanged(this);
	}

	void setLeftSize(quint32 lSize)
	{
		OrderedRBNodeBase<T>::setLeftSize(lSize);
		e.leftSize = lSize;
		_tree->nodeChanged(this);
	}

	const T &value() const { return e.data; }
	void setValue(const T &nv)
	{
		e.data = nv;
		_tree->nodeChanged(this);
	}

	OrderedRBDBNode<T> *left() const
	{
		// Load the node if not already done and return it
		if (!_left && e.left) {
			_left = new OrderedRBDBNode<T>(_tree, e.left);
			_left->_parent = const_cast<OrderedRBDBNode<T> *>(this);
		}
		return _left;
	}
	OrderedRBDBNode<T> *right() const
	{
		// Load the node if not already done and return it
		if (!_right && e.right) {
			_right = new OrderedRBDBNode<T>(_tree, e.right);
			_right->_parent = const_cast<OrderedRBDBNode<T> *>(this);
		}
		return _right;
	}
	OrderedRBDBNode<T> *parent() const
	{
		// Parent is always available
		return _parent;
	}
	void setLeft(OrderedRBDBNode<T> *nl)
	{
		_left = nl;
		e.left = nl ? nl->e.rowId : 0;
		_tree->nodeChanged(this);
		if (nl) nl->setParent(this);
	}
	void setRight(OrderedRBDBNode<T> *nr)
	{
		_right = nr;
		e.right = nr ? nr->e.rowId : 0;
		_tree->nodeChanged(this);
		if (nr) nr->setParent(this);
	}
	void setParent(OrderedRBDBNode<T> *np)
	{
		_parent = np;
		e.parent = np ? np->e.rowId : 0;
		_tree->nodeChanged(this);
	}
	void attachToTree(OrderedRBDBTree<T> *tree)
	{
		if (_tree == tree) return;
		if (_tree) _tree->forgetNode(this);
		_tree = tree;
		_tree->nodeChanged(this);
	}
	OrderedRBDBTree<T> *tree() { return _tree; }

	/**
	 * Called by OrderedRBDBTree for every node that has been marked as being changed.
	 */
	bool updateDB()
	{
		e.rowId = _tree->dbAccess()->insertEntry(e);
		return e.rowId != 0;
	}

	quint32 rowId() const
	{
		return e.rowId;
	}
friend class OrderedRBDBTree<T>;
};

/**
 * Persistent Ordered RB tree backed up by a database.
 *
 * Instances of this class need to be connected to a suitable DBList instance in order to fetch and store their data.
 * They also need to be affected a list identifier, from which the root of the tree can be fetched. The documentation
 * of DBList contains details about the DB implementation.
 */
template <class T> class OrderedRBDBTree
{
public:
	typedef OrderedRBDBNode<T> Node;

private:
	DBListInfo _listInfo;
	mutable Node *_root;
	bool mustUpdateRootTable;
	QSet<Node *> _changedNodes;

protected:
	DBList<T> *_ldb;

public:
	OrderedRBDBTree() : _listInfo(), _root(0), mustUpdateRootTable(false), _ldb(0) 
	{
	}

	/// Remove all the in-memory structures, leaving the database unchanged
	~OrderedRBDBTree()
	{
		clearMemCache();
	}

	Node *root() const { return _root; }

	void setRoot(Node *node)
	{
		_root = node;
		if (node) _listInfo.rootId = node->e.rowId;
		else _listInfo.rootId = 0;
		mustUpdateRootTable = true;
	}

        bool aboutToChange()
	{
		_changedNodes.clear();
		return _ldb->connection()->transaction();
	}

        void nodeChanged(Node *n)
	{
		_changedNodes << n;
	}

	void forgetNode(Node *node)
	{
		_changedNodes.remove(node);
	}

	void removeNode(Node *node)
	{
		// Make sure deleted nodes do not get updated
		forgetNode(node);
		if (!_ldb->removeEntry(node->e.rowId)) {
			qDebug("OrderedRBDBTree: cannot remove entry from database!\n");
		}
		delete node;
	}

        bool commitChanges()
	{
		foreach (Node *n, _changedNodes) {
			if (!n->updateDB()) {
				_ldb->connection()->rollback();
				return false;
			}
		}
		_changedNodes.clear();
		if (!_ldb->connection()->commit()) {
			_ldb->connection()->rollback();
			return false;
		}
		if (mustUpdateRootTable) {
			_ldb->insertList(_listInfo);
			mustUpdateRootTable = false;
		}
		return true;
	}

	void abortChanges()
	{
		_changedNodes.clear();
		_ldb->connection()->rollback();
	}

	quint32 listId() const
	{
		return _listInfo.listId;
	}

	void setListId(quint32 id)
	{
		_listInfo =_ldb->getList(id);
		// Overwrite the id in case the list was not recorded yet
		_listInfo.listId = id;
		clearMemCache();
	}

	const QString &label() const { return _listInfo.label; }

	bool setLabel(const QString &l)
	{
		QString oldLabel = _listInfo.label;
		_listInfo.label = l;
		bool ret = _ldb->insertList(_listInfo);
		if (!ret) _listInfo.label = oldLabel;
		return ret;
	}

	void setDBAccess(DBList<T> *ldb)
	{
		_ldb = ldb;
	}

	DBList<T> *dbAccess() { return _ldb; }

	/**
	 * Remove the list record for this list from the DB. This does
	 * not remove the list nodes, so the list MUST be empty before this
	 * method is called!
	 */
	bool removeList()
	{
		if (!_ldb->removeList(_listInfo.listId)) return false;
		_listInfo = DBListInfo();
		return true;
	}

	/**
	 * Create a new empty list in the DB layer and associate this tree
	 * to it.
	 */
	bool newList()
	{
		clearMemCache();
		_listInfo = _ldb->newList();
		if (!_listInfo.listId) return false;
		else return true;
	}

	void clearMemCache();
};

template <class T>
void OrderedRBDBTree<T>::clearMemCache()
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
	// Restore the root node, otherwise the tree will not work anymore
	if (_listInfo.rootId != 0) _root = new Node(const_cast<OrderedRBDBTree<T> *>(this), _listInfo.rootId);
}

#endif
