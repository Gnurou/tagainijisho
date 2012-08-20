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

#include "core/TString.h"

#include <iostream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

static ssize_t lengthOfChar(char c)
{
	ssize_t s = 1;
	unsigned char m = 0x80;

	if (c & m) {
		m >>= 1;
		while (c & m) {
			s++;
			m >>=1;
		}
	}

	return s;
}

size_t TString::length() const
{
	int ret = 0;

	for (std::string::const_iterator it = begin(); it != end(); ret++) {
		it += lengthOfChar(*it);
	}

	return ret;
}

size_t TString::offsetOfChar(size_t n) const
{
	int ret = 0;

	// TODO return -1 if out of range!

	for (std::string::const_iterator it = begin(); it != end() && n > 0; n--) {
		ssize_t s = lengthOfChar(*it);

		it += s;
		ret += s;
	}

	return ret;
}

TChar TString::operator[](size_t idx) const
{
	uint32_t ret;

	char c = at(idx);
	ssize_t l = lengthOfChar(c);

	if (l > 1) {
		char m = 0x80;
		for (ssize_t i = 1; i < l; i++) m |= (0x1 << (sizeof(char) * 8 - (i + 1)));
		c &= ~m;
		ret = c;
		for (ssize_t i = 1; i < l; i++) {
			ret <<= 6;
			ret |= at(idx + i) & 0x3f;
		}
	} else {
		ret = c;
	}

	return ret;
}

void TString::split(const TString &sep, TStringList &ret) const
{
	boost::split(ret, *this, boost::is_any_of(sep), boost::token_compress_on);
	ret.erase(std::remove(ret.begin(), ret.end(), ""), ret.end());
}
