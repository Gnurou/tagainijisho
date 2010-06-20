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

#include "gui/TemplateFiller.h"
#include "gui/tests/TemplateFillerTests.h"

void TemplateFillerTests::initTestCase()
{
	tmpl = 
		"First text\n"
		"<!--PART: front-->\n"
		"Front text\n"
		"<!--/PART-->\n"
		"Intermedium\n"
		"<!--PART: back-->\n"
		"Back text\n"
		"<!--/PART-->\n"
		"Conclusion";
}

void TemplateFillerTests::cleanupTestCase()
{
}

void TemplateFillerTests::extract_1()
{
	TemplateFiller filler;
	QStringList parts;
	QString expected;
	
	parts << "front";
	expected =
		"<!--PART: front-->\n"
		"Front text\n"
		"<!--/PART-->\n";
	QString ret = filler.extract(tmpl, parts);
	QCOMPARE(ret, expected);
}

void TemplateFillerTests::extract_2()
{
	TemplateFiller filler;
	QStringList parts;
	QString expected;
	
	parts << "back";
	expected =
		"First text\n"
		"Intermedium\n"
		"<!--PART: back-->\n"
		"Back text\n"
		"<!--/PART-->\n"
		"Conclusion";
	QString ret = filler.extract(tmpl, parts, true);
	QCOMPARE(ret, expected);
}

void TemplateFillerTests::extract_3()
{
	TemplateFiller filler;
	QStringList parts;
	QString expected;
	
	expected =
		"First text\n"
		"Intermedium\n"
		"Conclusion";
	QString ret = filler.extract(tmpl, parts, true);
	QCOMPARE(ret, expected);
}

QTEST_MAIN(TemplateFillerTests)