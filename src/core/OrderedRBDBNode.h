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

/**
 * Implements database storage for RB tree nodes.
 * When a node is created, all its data is loaded and cached.
 * When the left/right members are first accessed, the corresponding
 * node is created and loaded.
 */
template <class T> class OrderedRBDBNode : public OrderedRBNodeBase<T>
{
private:
	DBListEntry<T> e;
	OrderedRBDBNode<T> * _left;
	OrderedRBDBNode<T> * _right;
	OrderedRBDBNode<T> * _parent;

	OrderedRBDBNode(DBList<T> &ldb, quint32 rowid) : _left(0), _right(0), _parent(0)
	{
		// The new node is expected to exist in the DB with the given ID - just load it.
		e = ldb.getEntry(rowid);
	}

public:
	OrderedRBDBNode(const T &va) : _left(0), _right(0), _parent(0)
	{
		// Here a new node is to be inserted in the tree - we need to insert it into
		// the DB in order to get its ID.
		setValue(va);
	}

	~OrderedRBDBNode()
	{
	}

	const T &value() const { return e.data; }
	void setValue(const T &nv)
	{
		e.data = nv;
	}

	OrderedRBDBNode<T> *left() const
	{
		// If the node is already loaded, return it - otherwise load it.
		return _left;
	}
	OrderedRBDBNode<T> *right() const
	{
		// If the node is already loaded, return it - otherwise load it.
		return _right;
	}
	OrderedRBDBNode<T> *parent() const
	{
		// If the node is already loaded, return it - otherwise load it.
		return _parent;
	}
	void setLeft(OrderedRBDBNode<T> *nl)
	{
		_left = nl;
		if (nl) nl->_parent = this;
	}
	void setRight(OrderedRBDBNode<T> *nr)
	{
		_right = nr;
		if (nr) nr->_parent = this;
	}
	void setParent(OrderedRBDBNode<T> *np)
	{
		_parent = np;
	}

	void updateDB(DBList<T> &ldb)
	{
		ldb.insertEntry(e);
	}
};

template <class T> class OrderedRBDBTree
{
public:
	typedef OrderedRBDBNode<T> Node;

private:
	Node *_root;
	QSet<Node *> _changedNodes;

public:
	OrderedRBDBTree() : _root(0)
	{
	}

	Node *root() const { return _root; }
	void setRoot(Node *node) { _root = node; }


        bool aboutToChange()
	{
		_changedNodes.clear();
		// TODO start transaction
		return true;
	}
        void changeNode(Node *n)
	{
		_changedNodes << n;
	}
        bool changed()
	{
		foreach (Node *n, _changedNodes) {
			n->updateDB();
		}
		_changedNodes.clear();
		// TODO Commit transaction
		return true;
	}
	void abortChanges()
	{
		_changedNodes.clear();
		// TODO Abort transaction
	}
};

#endif
