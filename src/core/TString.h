/*
 *  Copyright (C) 2012  Alexandre Courbot
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

#ifndef __CORE_TSTRING_H
#define __CORE_TSTRING_H

#include <string>
#include <vector>
#include <stdint.h>

#include <QString>

/// Can represent any unicode character.
class TChar {
public:
	TChar() : _code(0) {}
	TChar(uint32_t c) : _code(c) {}

	uint32_t code() const { return _code; }

	TChar &operator =(const TChar &c) { _code = c._code; return *this; }
	bool operator ==(const TChar &c) const { return _code == c._code; }
	bool operator !=(const TChar &c) const { return _code != c._code; }
	bool operator <(const TChar &c) const { return _code < c._code; }
	bool operator <=(const TChar &c) const { return _code <= c._code; }
	bool operator >(const TChar &c) const { return _code > c._code; }
	bool operator >=(const TChar &c) const { return _code >= c._code; }

private:
	uint32_t _code;
};

class TJChar : public TChar {
public:
	TJChar() : TChar() {}
	TJChar(uint32_t c) : TChar() {}
	TJChar(const TChar &c) : TChar(c) {}
};

class TString;
typedef std::vector<TString> TStringList;

/**
 * An extension of std::string that handles UTF-8 and adds useful member functions.
 */
class TString : public std::string
{
public:
	TString() : std::string() {}
	TString(const TString &s) : std::string(s) {}
	TString(const std::string &s) : std::string(s) {}
	TString(const char *s) : std::string(s) {}
	TString(const char *s, size_t n) : std::string(s, n) {}
	template<class InputIterator> TString(InputIterator begin, InputIterator end) : std::string(begin, end) {}

	/**
	 * Contrary to size(), which returns the size in bytes, this function
	 * returns the number of unicode characters.
	 *
	 * @warning Operates in linear time.
	 */
	size_t length() const;

	/**
	 * Returns the starting offset of the n'th character in the string.
	 *
	 * @warning Operates in linear time.
	 */
	size_t offsetOfChar(size_t n) const;

	/**
	 * Returns the Unicode character starting at index idx.
	 */
	TChar operator[](size_t idx) const;

	void split(const TString &sep, TStringList &ret) const;

	TString(const QString &s) : std::string(s.toUtf8().constData()) {}
	QString asQString() const { return QString::fromUtf8(c_str()); }

	/*
	class iterator : public std::string::iterator
	{
	public:
		iterator(const std::string::iterator &it) : std::string::iterator(it) {}
		iterator(const std::string::const_iterator &it) : std::string::iterator(it) {}
		TChar operator *() { return std::string::iterator::operator *(); }
	};

	iterator begin() const { return iterator(std::string::begin()); }
	iterator end() const { return iterator(std::string::end()); }*/
};

#endif
