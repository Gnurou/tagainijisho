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

#include "TStringTests.h"

#include "core/TString.h"
#include "core/Compress.h"

#include <QtDebug>

#include <iostream>
#include <vector>
#include <string>

void TStringTests::initTestCase()
{
}

void TStringTests::cleanupTestCase()
{
}

void TStringTests::testLength()
{
	TString t("abcあいう互");
	QCOMPARE(t.size(), (size_t)15);
	QCOMPARE(t.length(), (size_t)7);

	QCOMPARE(t.offsetOfChar(0), (size_t)0);
	QCOMPARE(t.offsetOfChar(1), (size_t)1);
	QCOMPARE(t.offsetOfChar(2), (size_t)2);
	QCOMPARE(t.offsetOfChar(3), (size_t)3);
	QCOMPARE(t.offsetOfChar(4), (size_t)6);
	QCOMPARE(t.offsetOfChar(5), (size_t)9);
	QCOMPARE(t.offsetOfChar(6), (size_t)12);

	QCOMPARE(t[0], (TChar)'a');
	QCOMPARE(t[1], (TChar)'b');
	QCOMPARE(t[2], (TChar)'c');
	QCOMPARE(t[3], (TChar)0x3042);
	QCOMPARE(t[6], (TChar)0x3044);
	QCOMPARE(t[9], (TChar)0x3046);
	QCOMPARE(t[12], (TChar)0x4e92);
}

void TStringTests::testSplit()
{
	TStringList res;
	TString("word1 ,boo2,|test3   ").split(", |", res);

	QCOMPARE(res[0], TString("word1"));
	QCOMPARE(res[1], TString("boo2"));
	QCOMPARE(res[2], TString("test3"));
	QCOMPARE(res.size(), (size_t)3);
}

void TStringTests::testIterators()
{
	// TODO
	TString t("abcあいう互");
	TString::iterator it = t.begin();

	while (it != t.end()) {
		//std::cout << *it << std::endl;
		it++;
	}
}

void TStringTests::testCompress()
{
	std::string s("hgfdg ghfdjgdjhdfhjbvfdh jvgdfghjdfgjhdfhjdfhjg udgfuhdguhdfghudfhgdf hdfghjdfjhdfhg dfdd jgdfhjdf jhdfj dfjgdfhjdf hjdfjgdhjdfdfdfhdf dgjhdfjgdfgdfg");
	std::vector<uint8_t> src((uint8_t *)s.c_str(), (uint8_t *)s.c_str() + s.size()), dst, src2;

	QVERIFY(!Tagaini::Compress(src, dst));
	QVERIFY(!Tagaini::Decompress(dst, src2));
	QCOMPARE(src.size(), src2.size());
	QCOMPARE(src, src2);
}


QTEST_MAIN(TStringTests)
