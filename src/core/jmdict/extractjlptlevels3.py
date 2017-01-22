#!/usr/bin/python2
# -*- coding: utf-8 -*-

import csv, sys, codecs
from pysqlite2 import dbapi2 as sqlite3
import cStringIO

class UTF8Recoder:
	"""
	Iterator that reads an encoded stream and reencodes the input to UTF-8
	"""
	def __init__(self, f, encoding):
		self.reader = codecs.getreader(encoding)(f)
		
	def __iter__(self):
		return self
		
	def next(self):
		return self.reader.next().encode("utf-8")
		
class UnicodeReader:
	"""
	A CSV reader which will iterate over lines in the CSV file "f",
	which is encoded in the given encoding.
	"""

	def __init__(self, f, dialect=csv.excel, encoding="utf-8", **kwds):
		f = UTF8Recoder(f, encoding)
		self.reader = csv.reader(f, dialect=dialect, **kwds)

	def next(self):
		row = self.reader.next()
		return [unicode(s, "utf-8") for s in row]

	def __iter__(self):
		return self

def findMatches(kanji, kana):
	# Look for kanji/kana combinations
	r = set()
	for kj in kanji:
		for kn in kana:
			qString = "select kana.id from kanaText join kana on kana.docid = kanaText.docid join kanji on kana.id = kanji.id join kanjiText on kanji.docid = kanjiText.docid join entries on entries.id = kana.id where kanaText.reading match ? and kanjiText.reading match ? order by kanji.priority asc, entries.frequency desc"
			r = r.union(set([e[0] for e in cursor.execute(qString, (kn, kj)).fetchall()]))
	# Look for kana only combinations
	if len(r) == 0 and len(kanji) == 0:
		# Find word that can only be written in kanji
		for kn in kana:
			qString = "select kana.id from kanaText join kana on kana.docid = kanaText.docid left join kanji on kana.id = kanji.id join entries on entries.id = kana.id join senses on senses.id == kana.id where kanaText.reading match ? and (kanji.docid is null or (senses.misc0 & 4 != 0)) order by entries.frequency desc"
			r = r.union(set([e[0] for e in cursor.execute(qString, (kn,)).fetchall()]))
	# Find anything matching the kana in despair
	if (len(r) == 0):
		for kn in kana:
			qString = "select kana.id from kanaText join kana on kana.docid = kanaText.docid join entries on entries.id = kana.id where kanaText.reading match ? order by entries.frequency desc"
			r = r.union(set([e[0] for e in cursor.execute(qString, (kn,)).fetchall()]))
	return r

if __name__ == "__main__":
	csvreader = UnicodeReader(codecs.open(sys.argv[1], 'r'), delimiter=',', quotechar='"')
	out = open(sys.argv[1][:-3] + "out", "w")
	connection = sqlite3.connect(sys.argv[2])
	connection.enable_load_extension(True)
	cursor = connection.cursor()
	# Load tagaini extensions
	cursor.execute("select load_extension('../../../build/src/sqlite/libtagaini_sqlite.so')")
	for row in csvreader:
		# Splits
		kanji = row[0]
		kana = row[1]
		kanji = kanji.replace(u"・", "/")
		kana = kana.replace(u"・", "/")
		kanji = kanji.replace(u"、", "/")
		kana = kana.replace(u"、", "/")
		kanji = kanji.replace(u" ", "/")
		kana = kana.replace(u" ", "/")
		kanji = [r.strip() for r in kanji.split("/")]
		kana = [r.strip() for r in kana.split("/")]
		if len(kanji) == 1 and len(kanji[0]) == 0: kanji = []

		r = findMatches(kanji, kana)

		# Remove ending "suru"
		for i in range(len(kanji)):
			k = kanji[i]
			if k.endswith(u"する"): kanji[i] = k[:-2]
		for i in range(len(kana)):
			k = kana[i]
			if k.endswith(u"する"): kana[i] = k[:-2]
		r = r.union(findMatches(kanji, kana))

		# Look by removing final "to"
		for i in range(len(kana)):
			k = kana[i]
			if k.endswith(u"と"): kana[i] = k[:-1]
		for i in range(len(kanji)):
			k = kanji[i]
			if k.endswith(u"と"): kanji[i] = k[:-1]
		r = r.union(findMatches(kanji, kana))

		for i in r:
			out.write("%d\n" % (i,))

		if len(r) == 0:
			if len(kanji) > 0: print "Nothing found for", kana[0], kanji[0]
			else: print "Nothing found for", kana[0]
