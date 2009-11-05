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

import sys, re, os, codecs
import xml.sax
import pysqlite2
from xmlhandler import BasicHandler
import kanjivg
from kanjivg import realord

KANJIDIC2DB_REVISION = 3

# Database connection
db = None

kanjivgs = {}

# I compiled this list of radicals from jisho.org. Don't think it will even be useful, but I keep here because I spent some time looking up the radicals
# 一,｜,丶,ノ,乙,亅,二,亠,人,亻,^,儿,入,ハ,¨,冂,冖,冫,几,凵,刀,⺉,力,勹,匕,匚,十,卜,卩,厂,厶,又,マ,九,ユ,乃,⻌ ,口,囗,土,士,夂,夕,大,女,子,宀,寸,小,⺌,尢,尸,屮,山,川,巛,工,已,巾,干,幺,广,廴,廾,弋,弓,ヨ,彑,彡,彳,⺖,⺘,⺡,⺨,⺾,⻏,⻖,也,亡,及,久,⺹,心,戈,戸,手,支,攵,文,斗,斤,方,无,日,曰,月,木,欠,止,歹,殳,比,毛,氏,气,水,火,⺣,爪,父,爻,爿,片,牛,犬,⺭,王,元,井,勿,尤,五,屯,巴,毋,玄,瓜,瓦,甘,生,用,田,疋,疒,癶,白,皮,皿,目,矛,矢,石,示,禸,禾,穴,立,⻂,世,巨,冊,母,罒,牙,竹,米,糸,缶,羊,羽,而,耒,耳,聿,肉,自,至,臼,舌,舟,艮,色,虍,虫,血,行,衣,西,臣,見,角,言,谷,豆,豕,豸,貝,赤,走,足,身,車,辛,辰,酉,釆,里,舛,麦,金,長,門,隶,隹,雨,青,非,奄,岡,免,斉,面,革,韭,音,頁,風,飛,食,首,香,品,馬,骨,高,髟,鬥,鬯,鬲,鬼,竜,韋,魚,鳥,鹵,鹿,麻,亀,啇,黄,黒,黍,黹,無,黽,鼎,鼓,鼠,鼻,齊,歯,龠

# Languages to insert into DB
languages = ( "en" )
readingsTypes = ( "ja_kun", "ja_on" )

def makeStrokesString(kanji):
	s = u''
	if kanji.element:
		s += kanji.element
		if kanji.original: s += kanji.original
	if kanji.part:
		s += unicode(kanji.part)
		if kanji.number: s += unicode(kanji.number)
	s += "["
	for child in kanji.childs:
		if isinstance(child, kanjivg.Stroke):
			if not child.svg: print "Empty stroke"
			else: s += child.svg + u';'
		else: s += makeStrokesString(child)
	s += "]"
	return s

class Kanji:
	def __init__(self):
		self.literal = None	# Unicode of the kanji
		self.grade = None
		self.strCount = None
		self.freq = None
		self.jlpt = None
		self.jis = None
		self.euc = None
		self.skip = []
		
		self.readings = []	# Pair of (type, reading) TODO change type to generated integers?
		self.meanings = []	# Pair of (lang, meaning)
		self.nanori = []

	def outputToDB(self):
		query = db.cursor()
		# insert the main entry
		query.execute("insert into entries values(?, ?, ?, ?, ?)", (self.literal, self.grade, self.strCount, self.freq, self.jlpt))

		# Insert readings/meanings
		for reading in self.readings:
			if not reading[0] in readingsTypes: continue
			query.execute("insert into readingText values(?)", (reading[1], ))
			query.execute("insert into reading values(?, ?, ?)", (query.lastrowid, self.literal, reading[0]))

		hasMeaning = False
		for meaning in self.meanings:
			if meaning[0] in languages:
				hasMeaning = True
				break
		for meaning in self.meanings:
			if hasMeaning and not meaning[0] in languages: continue
			elif not hasMeaning and meaning[0] != "en": continue
			query.execute("insert into meaningText values(?)", (meaning[1], ))
			query.execute("insert into meaning values(?, ?, ?)", (query.lastrowid, self.literal, meaning[0]))

		for reading in self.nanori:
			query.execute("insert into nanoriText values(?)", (reading, ))
			query.execute("insert into nanori values(?, ?)", (query.lastrowid, self.literal))

		# On to KanjiVG data
		global kanjivgs
		if kanjivgs.has_key(self.literal):
			kanji = kanjivgs[self.literal]
			structuredKanji = kanjivg.StructuredKanji(kanji)
			structuredKanji.components[0].docid = None

			# Insert kanjis groups
			for group in structuredKanji.components:
				# This should never happen
				if not group.parent and structuredKanji.components.index(group) != 0:
					print "Error - child kanji group without a parent!"
					raise Exception
				# Only for root component
				if not group.parent: docid = None
				else:
					docid = group.parent.docid
					if not docid:
						print "Error - no docid for parent group!"
						raise Exception
				query.execute("insert into strokeGroups values(?, ?, ?, ?, ?)", (self.literal, docid, group.number, realord(group.element), realord(group.original)))
				group.docid = query.lastrowid

			# Insert kanjis stroke paths
			for stroke in structuredKanji.strokes:
				query.execute("insert into strokes values(?, ?, ?)", (stroke.parent.docid, realord(stroke.stype), stroke.svg))
			# Ensure the number of stroke count is consistent
			query.execute("update entries set strokeCount = ? where id = ?", (len(structuredKanji.strokes), self.literal))

		# Now insert kanji codes
		# SKIP
		for skipCode in self.skip:
			t, c1, c2 = skipCode.split('-')
			query.execute("insert into skip values(?, ?, ?, ?)", (self.literal, int(t), int(c1), int(c2)))

class Kanjidic2Handler(BasicHandler):
	def startDocument(self):
		self.entryCpt = 0
		return True

	def endDocument(self):
		return True

	def __init__(self):
		BasicHandler.__init__(self)
		self.currentKanji = None

	def handle_start_character(self, attrs):
		self.currentKanji = Kanji()

	def handle_end_character(self):
		if self.currentKanji.literal:
			# Output everything to the database
			self.currentKanji.outputToDB()
		self.currentKanji = None

		self.entryCpt += 1
		if self.entryCpt % 1000 == 0: print "[kanjidic2] %d entries processed" % (self.entryCpt)

	def handle_data_literal(self, string):
		string = unicode(string)
		self.currentKanji.literal = realord(string)

	def handle_data_grade(self, string):
		self.currentKanji.grade = int(string)

	def handle_data_stroke_count(self, string):
		self.currentKanji.strCount = int(string)

	def handle_data_freq(self, string):
		self.currentKanji.freq = int(string)

	def handle_data_jlpt(self, string):
		self.currentKanji.jlpt = int(string)

	def handle_start_reading(self, attrs):
		r_type = str(attrs["r_type"])
		self.currentReading = r_type

	def handle_data_reading(self, string):
		self.currentKanji.readings.append((self.currentReading, unicode(string)))

	def handle_end_reading(self):
		del self.currentReading

	def handle_start_meaning(self, attrs):
		if attrs.has_key("m_lang"): self.m_lang = str(attrs["m_lang"])
		else: self.m_lang = "en"

	def handle_data_meaning(self, string):
		self.currentKanji.meanings.append((self.m_lang, unicode(string)))

	def handle_end_meaning(self):
		del self.m_lang

	def handle_data_nanori(self, string):
		self.currentKanji.nanori.append(unicode(string))

	def handle_start_q_code(self, attrs):
		if attrs.has_key("qc_type") and attrs["qc_type"] == "skip":
			if not attrs.has_key("skip_misclass"): self.qc_type = "skip"

	def handle_data_q_code(self, string):
		if hasattr(self, "qc_type"):
			if self.qc_type == "skip":
				self.currentKanji.skip.append(string)

	def handle_end_q_code(self):
		if hasattr(self, "qc_type"): del self.qc_type

def usage():
	print "Usage: %s <kanjidic2 DB> <kanjidic2> <kanjicomponents" % (sys.argv[0])

def insertJLPTLevels(inputFile, level):
	global db

	query = db.cursor()
	file = codecs.open(inputFile, "r", "utf-8")
	for id in file.readlines():
		query.execute("update entries set jlpt = ? where id = ?", (level, realord(id)))

def createDB(dbFile, kanjidic2File, kanjivgdata, jlptFiles = None):
	global db

	db = pysqlite2.connect(dbFile)
	db.text_factory = str
	query = db.cursor()
	query.execute('pragma encoding = "UTF-16le"')

	print "[kanjidic2] Creating tables..."
	sys.stdout.flush()

	global DICTDB_REVISION
	query.execute("create table info(version INT)")
	query.execute("insert into info values(%d)" % (KANJIDIC2DB_REVISION))
	query.execute("create table entries(id INTEGER PRIMARY KEY, grade TINYINT, strokeCount TINYINT, frequency SMALLINT, jlpt TINYINT)")
	query.execute("create table reading(docid INTEGER PRIMARY KEY, entry INTEGER SECONDARY KEY REFERENCES entries, type TEXT)")
	query.execute("create virtual table readingText using fts3(reading, TOKENIZE katakana)")
	query.execute("create table meaning(docid INTEGER PRIMARY KEY, entry INTEGER SECONDARY KEY REFERENCES entries, lang TEXT)")
	query.execute("create virtual table meaningText using fts3(meaning)")
	query.execute("create table nanori(docid INTEGER PRIMARY KEY, entry INTEGER SECONDARY KEY REFERENCES entries)")
	query.execute("create virtual table nanoriText using fts3(reading, TOKENIZE katakana)")

	query.execute("create table strokeGroups(kanji INTEGER, parentGroup INTEGER SECONDARY KEY REFERENCES strokeGroups, number TINYINT, element INTEGER, original INTEGER)")
	query.execute("create table strokes(parentGroup INTEGER SECONDARY KEY REFERENCES strokeGroups, strokeType INTEGER, path TEXT)")
	query.execute("create table skip(entry INTEGER, type TINYINT, c1 TINYINT, c2 TINYINT)")

	print "[kanjidic2] Parsing KanjiVG data..."
	handler = kanjivg.KanjisHandler()
	xml.sax.parse(kanjivgdata, handler)
	_kanjivgs = handler.kanjis.values()
	global kanjivgs
	for kanji in _kanjivgs:
		kanjivgs[realord(kanji.midashi)] = kanji
	del _kanjivgs

	# Insert entries in the database using an XML parser
	print "[kanjidic2] Inserting entries..."
	handler = Kanjidic2Handler()
	file = open(kanjidic2File, "r")
	if not file:
		usage()
		sys.exit(1)
	xml.sax.parse(file, handler)

	# Update JLPT levels
	print "[kanjidic2] Inserting JLPT levels..."
	if not jlptFiles:
		insertJLPTLevels("src/core/kanjidic2/jlpt-level2.txt", 2)
		insertJLPTLevels("src/core/kanjidic2/jlpt-level3.txt", 3)
		insertJLPTLevels("src/core/kanjidic2/jlpt-level4.txt", 4)
	else:
		insertJLPTLevels(jlptFiles[0].srcpath(), 2)
		insertJLPTLevels(jlptFiles[1].srcpath(), 3)
		insertJLPTLevels(jlptFiles[2].srcpath(), 4)

	# Create tables indexes for faster search
	print "[kanjidic2] Creating tables indexes..."
	sys.stdout.flush()
	# Dramatically speeds up sorting by frequency
	query.execute("create index idx_entries_frequency on entries(frequency)")
	query.execute("create index idx_jlpt on entries(jlpt)")
	query.execute("create index idx_reading_entry on reading(entry)")
	query.execute("create index idx_meaning_entry on meaning(entry)")
	query.execute("create index idx_nanori_entry on nanori(entry)")
	query.execute("create index idx_strokeGroups_kanji on strokeGroups(kanji)")
	query.execute("create index idx_strokeGroups_parentGroup on strokeGroups(parentGroup)")
	query.execute("create index idx_strokeGroups_element on strokeGroups(element)")
	query.execute("create index idx_strokeGroups_original on strokeGroups(original)")
	query.execute("create index idx_strokes_parentGroup on strokes(parentGroup)")
	query.execute("create index idx_strokes_strokeType on strokes(strokeType)")
	query.execute("create index idx_skip on skip(entry)")
	query.execute("create index idx_skip_type on skip(type, c1, c2)")

	# And vacuum everything!
	print "[kanjidic2] Optimizing database organization..."
	sys.stdout.flush()
	query.execute("analyze")
	query.execute("vacuum")

	db.commit()
	db.close()
	del db
	os.chmod(dbFile, 0x124)

# target: [ db file ]
# source: [ kanjidic2, kradfile, strokesfile ]
#def sconsCreateKanjidic2DB(target, source, env):
	#global languages
	#languages = env['lang']
	#createDB(target[0].path, source[0].path, source[1].path, source[2].path)
