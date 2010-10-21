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

/**
 * Implements database storage for RB tree nodes.
 * When a node is created, all its data is loaded and cached.
 * When the left/right members are first accessed, the corresponding
 * node is created and loaded.
 */
template <class T> class OrderedRBDBNode : public OrderedRBNodeBase<T>
{
private:
	quint32 _rowid;
	union {
		quint32 _leftId;
		OrderedRBDBNode<T> * _left;
	};
	union {
		quint32 _rightId;
		OrderedRBDBNode<T> * _right;
	};
	union {
		quint32 _parentId;
		OrderedRBDBNode<T> * _parent;
	};
	T _value;

	OrderedRBDBNode(EntryListDB &ldb, quint32 rowid) : _rowid(rowid)
	{
		// The new node is expected to exist in the DB with the given ID - just load it.
		EntryList entry(ldb.getEntry(rowid));
	}

public:
	OrderedRBDBNode(const T &va) : _rowid(0), _leftId(0), _rightId(0), _parentId(0), _value(va)
	{
		// Here a new node is to be inserted in the tree - we need to insert it into the DB in order
		// to get its ID.
	}

	~OrderedRBDBNode()
	{
	}

	const T &value() const { return _value; }
	virtual void setValue(const T &nv)
	{
		_value = nv;
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
};

template <class T> class OrderedRBDBTree
{
public:
	typedef OrderedRBDBNode<T> Node;

private:
	Node *_root;

public:
	OrderedRBDBTree() : _root(0)
	{
	}

	Node *root() const { return _root; }
	void setRoot(Node *node) { _root = node; }


        bool aboutToChange() { return true; }
        void changeNode(Node *n) {}
        bool changed() { return true; }
	void abortChanges() {}
};

#endif
