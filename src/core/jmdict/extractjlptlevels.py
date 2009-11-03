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

# Tries to automatically associate a JLPT level to JMdict entries, according to the information available on
# http://www.jlptstudy.com. Ambigous or unknown entries are left for manual inclusion.
def addJLPTLevels(dbFile, jlptFile, level, outFile):
	global db, dbConnectionName

	outFile = codecs.open(outFile, "w", "utf-8")

	db = getDb(dbConnectionName)
	db.setDatabaseName(dbFile)
	if not db.open():
		usage()
		sys.exit(1)
	query = QSqlQuery(db)
	
	nbpos = 0
	nben = 0

	levelRegExp = re.compile('''.*<td class="(.+)"(?:>| />)([^<$\n]*).*''')
	file = open(jlptFile, "r")
	for line in file.xreadlines():
		match = levelRegExp.match(line)
		if match:
			if match.group(1) == "pos":
				kana = None
				kanji = None
				en = None
				nbpos += 1
			elif match.group(1) == "kanji":
				if kana == None:
					kana = QtCore.QString.fromUtf8(match.group(2))
					kana.remove(u"～")
					kana.remove(u"。")
					kana.remove(u"・")
					kana.remove(u"、")
					idx = kana.indexOf("/")
					if idx >= 0: kana.truncate(idx)
#					idx = kana.indexOf(u"・")
#					if idx >= 0: kana.truncate(idx)
					kana.remove(QtCore.QRegExp(u"（.*）"))
					kana.remove(QtCore.QRegExp(u"\(.*\)"))
					kana.remove(QtCore.QRegExp(u"する$"))
				else:
					kanji = QtCore.QString.fromUtf8(match.group(2))
					kanji.remove(u"～")
					idx = kanji.indexOf("/")
					if idx >= 0: kanji.truncate(idx)
			elif match.group(1) == "en":
				nben += 1
				en = QtCore.QString.fromUtf8(match.group(2))
				if kana != None:
					qString = "select kana.id from kanaText join kana on kana.docid = kanaText.docid join kanji on kana.id = kanji.id join kanjiText on kanji.docid = kanjiText.docid join entries on entries.id = kana.id where kanaText.reading match ? and kanjiText.reading match ? order by entries.frequency desc"
					query.prepare(qString)
					query.addBindValue(QtCore.QVariant(kana))
					query.addBindValue(QtCore.QVariant(kanji))
					if not query.exec_(): print query.lastError().text(); raise Exception
					# No result? Let's try by looking at the kana and definition then.
					if not query.next():
						query.prepare("select kana.id from kanaText join kana on kana.docid = kanaText.docid join gloss on gloss.id = kana.id join glossText on gloss.docid = glossText.docid join entries on entries.id = kana.id where kanaText.reading match ? and glossText.reading match ? order by entries.frequency desc")
						query.addBindValue(QtCore.QVariant(kana))
						query.addBindValue(QtCore.QVariant(en))
						if not query.exec_(): print query.lastError().text(); raise Exception
						if not query.next():
							# Still not? Then try kana and accept if there is only one entry corresponding
							query.prepare("select kana.id from kanaText join kana on kana.docid = kanaText.docid join entries on entries.id = kana.id where kanaText.reading match ? order by entries.frequency desc")
							query.addBindValue(QtCore.QVariant(kana))
							if not query.exec_(): print query.lastError().text(); raise Exception
							if not query.next():
								outFile.write("############%s %s\n" % (unicode(kanji), unicode(kana)))
								continue
							if query.next():
								outFile.write("############%s %s\n" % (unicode(kanji), unicode(kana)))
								continue
							query.first() 

					# Make sure not to violate primary key constraint
					#query2.prepare("insert or ignore into jlpt values (?, %d)" % (4))
					#query2.addBindValue(QtCore.QVariant(query.value(0)))
					#if not query2.exec_(): print query2.lastError().text(); raise Exception
					outFile.write("%d\n" % (query.value(0).toInt()[0]))
			else: print "Bad entry: %s" % (match.group(1))
	print nbpos, nben

if __name__ == "__main__":
	if len(sys.argv) != 5: usage()
	else: 
		addJLPTLevels(sys.argv[1], sys.argv[2], int(sys.argv[3]), sys.argv[4])
