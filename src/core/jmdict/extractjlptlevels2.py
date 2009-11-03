#!/usr/bin/python
# -*- coding: utf-8 -*-
#
#  Copyright (C) 2008  Alexandre Courbot
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
import codecs
import sys
import re
import os
from PyQt4 import QtCore
from PyQt4.QtSql import *
from database import getDb

# Database connection name used to access our custom driver
dbConnectionName = "custom-sqlite"

def addJLPTLevels(dbFile, jlptFile, level, outFileName):
	global db, dbConnectionName

	outFile = codecs.open(outFileName, 'w', 'utf-8')

	db = getDb(dbConnectionName)
	db.setDatabaseName(dbFile)
	if not db.open():
		usage()
		sys.exit(1)
	query = QSqlQuery(db)

	re1 = QtCore.QRegExp(u'~?([\w]+)~?\s+\\[?~?([\w]+)~?\\]?.*')
	re2 = QtCore.QRegExp(u'~?([\w]+)~?.*')
	file = open(jlptFile, "r")
	for line in file.xreadlines():
		if len(line.strip()) == 0 or line[0] == '#': continue
		s = QtCore.QString.fromUtf8(line)
		if re1.exactMatch(s):
			matches = re1.capturedTexts()
			kanji = unicode(matches[1])
			kana = unicode(matches[2])
			qString = "select kana.id from kanaText join kana on kana.docid = kanaText.docid join kanji on kana.id = kanji.id join kanjiText on kanji.docid = kanjiText.docid join entries on entries.id = kana.id where kanaText.reading match ? and kanjiText.reading match ? order by kanji.priority asc, entries.frequency desc"
			query.prepare(qString)
			query.addBindValue(QtCore.QVariant(kana))
			query.addBindValue(QtCore.QVariant(kanji))
		elif re2.exactMatch(s):
			matches = re2.capturedTexts()
			kanji = None
			kana = unicode(matches[1])
			qString = "select kana.id from kanaText join kana on kana.docid = kanaText.docid join entries on entries.id = kana.id where kanaText.reading match ? order by entries.frequency desc"
			query.prepare(qString)
			query.addBindValue(QtCore.QVariant(kana))
		else:
			print "Unmatched line: " + line,
			continue

		if not query.exec_(): print query.lastError().text(); raise Exception
		if not query.next(): print "No result for %s %s" % (kanji, kana); continue
		outFile.write("%d\n" % (query.value(0).toInt()[0]))
		if query.next(): print "Multiple results for %s %s" % (kanji, kana)

if __name__ == "__main__":
	if len(sys.argv) != 5: usage()
	else: 
		addJLPTLevels(sys.argv[1], sys.argv[2], int(sys.argv[3]), sys.argv[4])
