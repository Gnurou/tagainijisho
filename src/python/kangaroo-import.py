#!/usr/bin/python
# -*- coding: utf8 -*-
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

import sys
import re
import time
from PyQt4 import QtSql
from PyQt4 import QtCore

kangFile = open(sys.argv[1], "r")
kRegExp = re.compile('''^([^ /]+)(?: \[(.*)\])?/ (\*?) / .*$''')

db = QtSql.QSqlDatabase.addDatabase('QSQLITE')
db.setDatabaseName(sys.argv[2])
dbOk = db.open()
if not dbOk:
	sys.exit(1)
query = QtSql.QSqlQuery()

query.exec_("attach \"%s\" as jmdict" % (sys.argv[3]))

unknownEntries = []

QtSql.QSqlDatabase.database().transaction()

for line in kangFile.xreadlines():
	match = kRegExp.match(line)
	if match:
		if match.group(2):
			kana = match.group(2).decode('utf-8')
			kanji = match.group(1).decode('utf-8')
		else:
			kana = match.group(1).decode('utf-8')
			kanji = None
		star = match.group(3).decode('utf-8')
		
		if not kanji: kanjiString = 'is null'
		else: kanjiString = u'like "%s"' % (kanji)
		queryString = u'select distinct jmdict.kana.id from jmdict.kanaText join jmdict.kana on jmdict.kanaText.docid = jmdict.kana.docid left join jmdict.kanji on jmdict.kanji.id =jmdict.kana.id left join jmdict.kanjiText on jmdict.kanjiText.docid = jmdict.kanji.docid where jmdict.kanaText.reading match "%s" and jmdict.kanjiText.reading %s' % (kana, kanjiString)
		if not query.exec_(queryString): print query.lastError().text()
		ids = []
		while query.next(): ids.append(query.value(0).toInt())
		# Ok, here we may have a kana representation - the kanji is actually the kana,
		# and the unique reading of the entry.
		if len(ids) == 0:
			queryString = u'select distinct jmdict.kana.id from jmdict.kanaText join jmdict.kana on jmdict.kanaText.docid = jmdict.kana.docid left join jmdict.kanji on jmdict.kanji.id =jmdict.kana.id left join jmdict.kanjiText on jmdict.kanjiText.docid = jmdict.kanji.docid where jmdict.kanaText.reading match "%s"' % (kanji)
			if not query.exec_(queryString): print query.lastError().text()
			while query.next(): ids.append(query.value(0).toInt())

		# One more chance: unique kana reading
		if len(ids) == 0:
			queryString = u'select distinct jmdict.kana.id from jmdict.kanaText join jmdict.kana on jmdict.kanaText.docid = jmdict.kana.docid left join jmdict.kanji on jmdict.kanji.id =jmdict.kana.id left join jmdict.kanjiText on jmdict.kanjiText.docid = jmdict.kanji.docid where jmdict.kanaText.reading match "%s"' % (kana)
			if not query.exec_(queryString): print query.lastError().text()
			while query.next(): ids.append(query.value(0).toInt())

		if len(ids) != 1:
			unknownEntries.append((kana, kanji))
			continue

		# Check that the entry is not imported
		queryString = 'select id from user_entries where type = 0 and id = %d' % (ids[0][0])
		query.exec_(queryString)
		if query.next(): continue
		
		if star == u'*': initialSuccess = 20
		else: initialSuccess = 0
		queryString = 'insert into user_entries values(0, %d, 1, 0, %d, 0, %d, 0)' % (ids[0][0], int(time.time()), initialSuccess)
		if not query.exec_(queryString): print query.lastError().text()

		# Update nbSuccess to run the score calculation trigger
		queryString = 'update user_entries set nbSuccess = %d where type = 0 and id = %d' % (initialSuccess, ids[0][0])
		if not query.exec_(queryString): print query.lastError().text()

QtSql.QSqlDatabase.database().commit()

if len(unknownEntries) != 0:
	print "Unprocessed entries (either unknown or ambigous) - please process them manually:"
	for entry in unknownEntries:
		print entry[0],
		if entry[1]: print u'(%s)' % (entry[1]),
		print
