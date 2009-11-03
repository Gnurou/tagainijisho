/*
 *  Copyright (C) 2008  Alexandre Courbot
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

#ifndef __GUI_ABSTRACT_HISTORY_H
#define __GUI_ABSTRACT_HISTORY_H

#include <QList>

/**
 * An abstract implementation of the history pattern. The
 * data to be tracked and the way to retrieve it are free.
 */
template <class T, class S>
class AbstractHistory
{
private:
	S _storage;
	int _pos;
	int _maxSize;

	/**
	 * Ensures that the history size does not exceeds _maxSize
	 * and resizes the history (erasing oldest elements) if needed.
	 */
	void checkCapacity();

public:
	AbstractHistory(int maxSize) : _storage(), _pos(0), _maxSize(maxSize) {}
	void add(const T &item);
	bool next(T &item);
	bool previous(T &item);
	bool first(T &item);
	bool last(T &item);
	bool goTo(unsigned int pos, T &item);

	void clear();

	int pos() const { return _pos; }
	int size() const { return _storage.size(); }
	int capacity() const { return _maxSize; }
	void setCapacity(int capacity) { _maxSize = capacity; checkCapacity(); }
	bool hasPrevious() const { return pos() > 1; }
	bool hasNext() const { return pos() < size(); }
};

template <class T, class S>
void AbstractHistory<T, S>::checkCapacity()
{
	if (size() > capacity()) {
		int diff = size() - capacity();
		_pos -= diff;
		if (_pos <= 0) {
			if (size()) _pos = 1;
			else _pos = 0;
		}
		while (diff--) _storage.removeFirst();
	}
}

template <class T, class S>
void AbstractHistory<T, S>::add(const T &item)
{
	// Clear items after the current one
	while (pos() < size()) _storage.removeLast();
	// Do not add the same item twice
	if (pos() > 0 && _storage[pos() - 1] == item) return;
	_storage.append(item);
	++_pos;
	
	checkCapacity();
}

template <class T, class S>
bool AbstractHistory<T, S>::next(T &item)
{
	if (!hasNext()) return false;
	item = _storage[_pos++];
	return true;
}

template <class T, class S>
bool AbstractHistory<T, S>::previous(T &item)
{
	if (!hasPrevious()) return false;
	item = _storage[--_pos - 1];
	return true;
}

template <class T, class S>
bool AbstractHistory<T, S>::first(T &item)
{
	return goTo(0, item);
}

template <class T, class S>
bool AbstractHistory<T, S>::last(T &item)
{
	return goTo(_storage.size(), item);
}

template <class T, class S>
bool AbstractHistory<T, S>::goTo(unsigned int pos, T &item)
{
	if (_storage.size() == 0) return false;
	_pos = pos;
	item = _storage[_pos - 1];
	return true;
}

template <class T, class S>
void AbstractHistory<T, S>::clear()
{
	_pos = 0;
	_storage.clear();
}

#endif
