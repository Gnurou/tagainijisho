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

import sys, os
import xml.sax
import pysqlite2
from xmlhandler import BasicHandler
from jmdictentities import *
from kanjivg import realord

JMDICTDB_REVISION = 3

# Database connection
db = None

entitiesCoding = None

# Languages to insert into DB
languages = ( "en" )

def isKanji(c):
	v = realord(c)
	return (v >= 0x4E00 and v <= 0x9FC3) or (v >= 0x3400 and v <= 0x4DBF) or (v >= 0xF900 and v <= 0xFAD9) or (v >= 0x2E80 and v <= 0x2EFF) or (v >= 0x20000 and v <= 0x2A6DF)

class KanjiReading:
	def __init__(self):
		self.reading = None 		# String (keb)
		self.info = set() 		# List of properties
		self.frequency = 0		# Frequency score

	def addInfo(self, info):
		global entitiesCoding
		self.info.add(entitiesCoding.entitiesCoding["ke_inf"][info])

class KanaReading:
	def __init__(self):
		self.reading = None 		# String (reb)
		self.noKanji = False 		# Not a regular Kanji reading (re_nokanji)
		self.restrictedTo = [] 		# Pointer to KanjiReading instance (re_restr)
		self.info = set()		# List of properties
		self.frequency = 0		# Frequency score

	def addInfo(self, info):
		global entitiesCoding
		self.info.add(entitiesCoding.entitiesCoding["re_inf"][info])
	
	def restrictedToString(self):
		return ','.join([str(restricted) for restricted in self.restrictedTo])

class Sense:
	def __init__(self):
		self.restrictedToKanji = [] # String[] (stagk)
		self.restrictedToKana = [] # String[] (stagr)
		self.xref = set() # String[] (xref)
		self.antonyms = set() # String[] (ant)
		self.partOfSpeech = 0 # 64-bits int
		self.field = 0 # 16-bits int
		self.misc = 0 # 32-bits int
		self.lsource = [] # (language, word) (lsource)
		self.dialect = 0 # 16-bits int
		self.gloss = {} # lang -> String (gloss)
		self.info = [] # (s_inf)
	
	def addPos(self, pos):
		global entitiesCoding
		self.partOfSpeech |= entitiesCoding.entitiesCoding["pos"][pos]
	
	def addField(self, pos):
		global entitiesCoding
		self.field |= entitiesCoding.entitiesCoding["field"][pos]
	
	def addMisc(self, pos):
		global entitiesCoding
		self.misc |= entitiesCoding.entitiesCoding["misc"][pos]
	
	def addLSource(self, lang, word):
		self.lsource.append((lang, word))
	
	def addDialect(self, pos):
		global entitiesCoding
		self.dialect |= entitiesCoding.entitiesCoding["dial"][pos]
	
	def addGloss(self, lang, gloss):
		if self.gloss.has_key(lang):
			self.gloss[lang] += ", " + gloss
		else: self.gloss[lang] = gloss
	
	def addInfo(self, info):
		self.info.append(info)


class DictionaryEntry:
	def __init__(self):
		self.seq = None			# Unique ID of this entry
		self.kanji = []			# List of kanji readings
		self.kana = []			# List of kana readings
		self.sense = []			# List of senses
		self.frequency = 0		# Frequency score

	def addKanjiReading(self, reading):
		self.kanji.append(reading)

	def addKanaReading(self, reading):
		self.kana.append(reading)

	def addSense(self, sense):
		self.sense.append(sense)
	
	def outputToDb(self):
		query = db.cursor()
		# Count number of kanjis in first reading
		kanjiCount = 0
		if len(self.kanji):
			for c in self.kanji[0].reading:
				if isKanji(c): kanjiCount += 1

		query.execute("insert into entries values(?, ?, ?)", (self.seq, self.frequency, kanjiCount));
		
		writtenKanjis = []
		priority = 0
		for kanji in self.kanji:
			query.execute("insert into kanjiText values(?)", ( kanji.reading, ))
			insertedId = query.lastrowid

			query.execute("insert into kanji values(?, ?, ?, ?)", ( self.seq, priority, insertedId, kanji.frequency ))

			for char in kanji.reading:
				# TODO Does not handle surrogates
				if isKanji(char) and not (char, priority) in writtenKanjis:
					writtenKanjis.append((char, priority))
					query.execute("insert into kanjiChar values(?, ?, ?)", ( ord(char), self.seq, priority ))

			priority += 1

		priority = 0
		for kana in self.kana:
			query.execute("insert into kanaText values(?)", ( kana.reading, ))
			insertedId = query.lastrowid

			query.execute("insert into kana values(?, ?, ?, ?, ?, ?)", ( self.seq, priority, insertedId, kana.noKanji, kana.frequency, kana.restrictedToString()))
			priority += 1

		# Propagate sense properties to senses that have none
		prevPartOfSpeech = 0
		prevMisc = 0
		for sense in self.sense:
			if sense.partOfSpeech != 0: prevPartOfSpeech = sense.partOfSpeech
			else: sense.partOfSpeech = prevPartOfSpeech
			if sense.misc != 0: prevMisc = sense.misc
			else: sense.misc = prevMisc

		# Reorder senses, so that senses with English glosses only are put last
		nsenses = []
		nsenses += self.sense
		for sense in nsenses:
			hasGloss = False
			hasEng = False
			for lang in sense.gloss.keys():
				if lang in languages:
					hasGloss = True
				elif lang == "en":
					hasEng = True
			if not hasGloss:
				self.sense.remove(sense)
				# Re-append english-only senses at the end
				if hasEng: self.sense.append(sense)

		# Remove english glosses for senses for which another language is available
		for sense in self.sense:
			for lang in sense.gloss.keys():
				if lang in languages:
					if lang != "en" and sense.gloss.has_key("en"): del sense.gloss["en"]
					break

		priority = 0
		prevPartOfSpeech = 0
		prevMisc = 0
		for sense in self.sense:
			query.execute("insert into senses values(?, ?, ?, ?, ?, ?)", (self.seq, priority, sense.partOfSpeech, sense.misc, sense.dialect, sense.field))
			for kanji in sense.restrictedToKanji:
				query.execute("insert into stagk values(?, ?, ?)", (self.seq, priority, self.kanji.index(kanji)))
			for kana in sense.restrictedToKana:
				query.execute("insert into stagr values(?, ?, ?)", (self.seq, priority, self.kana.index(kana)))

			priority += 1
		
		sensePriority = 0
		for sense in self.sense:
			# Only output glosses for which a language is available
			for lang in sense.gloss.keys():
				if not (lang in languages or lang == "en"): continue
				query.execute("insert into glossText values(?)", (sense.gloss[lang], ))
				insertedId = query.lastrowid

				query.execute("insert into gloss values(?, ?, ?, ?)", (self.seq, sensePriority, lang, insertedId))
			sensePriority += 1

class JMDictHandler(BasicHandler):
	def __init__(self):
		BasicHandler.__init__(self)
		self.glossLanguage = None
		# Track the number of entries processed so far
		self.entryCount = 0

	#
	# Document handling
	#
	def startDocument(self):
		self.entryCpt = 0
		return True

	def endDocument(self):
		return True

	#
	# Entry handling
	#
	def handle_start_entry(self, attrs):
		self.currentEntry = DictionaryEntry()
	
	def handle_data_ent_seq(self, string):
		self.currentEntry.seq = int(string)
	
	def handle_end_entry(self):
		# Only add an entry if we have senses in the accepted languages
		# for it
		if len(self.currentEntry.sense):
			self.currentEntry.outputToDb()
		self.currentEntry = None
		self.entryCpt += 1
		if self.entryCpt % 10000 == 0: print "[jmdict] %d entries processed" % (self.entryCpt)

	#
	# Kanji reading handling
	#
	def handle_start_k_ele(self, attrs):
		self.currentKanji = KanjiReading()
	
	def handle_data_keb(self, string):
		self.currentKanji.reading = unicode(string)
	
	def handle_data_ke_inf(self, string):
		global reversedEntities
		self.currentKanji.addInfo(entitiesCoding.reversedEntities[str(string)])
	
	def handle_data_ke_pri(self, string):
		code = str(string)
		if code == "news1": addScore = 50
		elif code == "news2": addScore = 10
		elif code == "ichi1": addScore = 50
		elif code == "ichi2": addScore = 10
		elif code == "spec1": addScore = 50
		elif code == "spec2": addScore = 10
		elif code == "gai1": addScore = 50
		elif code == "gai2": addScore = 10
		elif code[0:2] == "nf": addScore = (51 - int(code[2:4]))
		else: print "Error - unknown score code", code
		self.currentKanji.frequency += addScore

	def handle_end_k_ele(self):
		self.currentEntry.addKanjiReading(self.currentKanji)
		if self.currentEntry.frequency < self.currentKanji.frequency:
			self.currentEntry.frequency = self.currentKanji.frequency
		self.currentKanji = None
	
	#
	# Kana reading handling
	#
	def handle_start_r_ele(self, attrs):
		self.currentKana = KanaReading()
	
	def handle_data_reb(self, string):
		self.currentKana.reading = unicode(string)
	
	def handle_start_re_nokanji(self, attrs):
		self.currentKana.noKanji = True
	
	def handle_data_re_pri(self, string):
		code = str(string)
		if code == "news1": addScore = 50
		elif code == "news2": addScore = 10
		elif code == "ichi1": addScore = 50
		elif code == "ichi2": addScore = 10
		elif code == "spec1": addScore = 50
		elif code == "spec2": addScore = 10
		elif code == "gai1": addScore = 50
		elif code == "gai2": addScore = 10
		elif code[0:2] == "nf": addScore = (51 - int(code[2:4]))
		self.currentKana.frequency += addScore

	def handle_data_re_restr(self, string):
		string = unicode(string)
		results = filter(lambda reading: reading.reading == string, self.currentEntry.kanji)
		if len(results) != 1: print "Entry %d: re_restr found %d potential results!" % (self.currentEntry.seq, len(results))
		if len(results) == 0: return
		self.currentKana.restrictedTo.append(self.currentEntry.kanji.index(results[0]))
	
	def handle_data_re_inf(self, string):
		global reversedEntities
		self.currentKana.addInfo(entitiesCoding.reversedEntities[str(string)])
	
	def handle_end_r_ele(self):
		self.currentEntry.addKanaReading(self.currentKana)
		if self.currentEntry.frequency < self.currentKana.frequency:
			self.currentEntry.frequency = self.currentKana.frequency
		self.currentKana = None

	#
	# Sense reading handling
	#
	def handle_start_sense(self, attrs):
		self.currentSense = Sense()
	
	def handle_data_stagk(self, string):
		string = unicode(string)
		results = filter(lambda reading: reading.reading == string, self.currentEntry.kanji)
		if len(results) != 1: print "Entry %d: stagk found %d potential results!" % (self.currentEntry.seq, len(results))
		if len(results) == 0: return
		self.currentSense.restrictedToKanji.append(results[0])
	
	def handle_data_stagr(self, string):
		string = unicode(string)
		results = filter(lambda reading: reading.reading == string, self.currentEntry.kana)
		if len(results) != 1: print "Entry %d: stagr found %d potential results!" % (self.currentEntry.seq, len(results))
		if len(results) == 0: return
		self.currentSense.restrictedToKana.append(results[0])
	
	def handle_data_xref(self, string):
		self.currentSense.xref.add(unicode(string))
	
	def handle_data_ant(self, string):
		self.currentSense.antonyms.add(unicode(string))

	def handle_data_pos(self, string):
		global reversedEntities
		self.currentSense.addPos(entitiesCoding.reversedEntities[str(string)])

	def handle_data_field(self, string):
		global reversedEntities
		self.currentSense.addField(entitiesCoding.reversedEntities[str(string)])

	def handle_data_misc(self, string):
		global reversedEntities
		self.currentSense.addMisc(entitiesCoding.reversedEntities[str(string)])
	
	def handle_start_lsource(self, attrs):
		if attrs.has_key("xml_lang"): lang = str(attrs["xml:lang"])
		else: lang = "eng"
		# JMdict is not consistent with kanjidic language naming scheme - fix that
		if lang == "spa": lang = "es"
		elif lang == "ger": lang = "de"
		self.currentLSource = lang[0:2]
	
	def handle_data_lsource(self, string):
		self.currentSense.addLSource(self.currentLSource, unicode(string))
		del self.currentLSource
	
	def handle_data_dial(self, string):
		global reversedEntities
		self.currentSense.addDialect(entitiesCoding.reversedEntities[str(string)])
	
	def handle_start_gloss(self, attrs):
		if attrs.has_key("xml:lang"): lang = str(attrs["xml:lang"])
		else: lang = "eng"
		# JMdict is not consistent with kanjidic language naming scheme - fix that
		if lang == "spa": lang = "es"
		elif lang == "ger": lang = "de"
		self.currentGlossLang = lang[0:2]
	
	def handle_data_gloss(self, string):
		self.currentSense.addGloss(self.currentGlossLang, unicode(string))
		del self.currentGlossLang
	
	def handle_data_s_inf(self, string):
		self.currentSense.addInfo(unicode(string))
	
	def handle_end_sense(self):
		# Only add a sense if we have gloss entries for it! (i.e. it has
		# definitions in the accepted languages)
		if len(self.currentSense.gloss):
			self.currentEntry.addSense(self.currentSense)
		self.currentSense = None

def usage():
	print "Usage: %s <output directory> <JMdict XML file>" % (sys.argv[0])
	print "Will create the files jmdict.db, JMdictDefs.h and JMdictDefs.cpp in the output directory"

def insertJLPTLevels(inputFile, level):
	global db

	query = db.cursor()
	file = open(inputFile, "r")
	for id in file.readlines():
		if id[0] == "#": continue
		query.execute("insert or ignore into jlpt values(?, ?)", (int(id), level))

def createDB(dbFile, JMdictFile, jlptFiles = None):
	global db
	db = pysqlite2.connect(dbFile)
	db.text_factory = str
	
	query = db.cursor()
	query.execute('pragma encoding = "UTF-16le"')
	
	print "[jmdict] Creating tables..."
	sys.stdout.flush()

	global JMDICTDB_REVISION
	query.execute("create table info(version INT)")
	query.execute("insert into info values(%d)" % (JMDICTDB_REVISION))
	query.execute("create table entries(id INTEGER PRIMARY KEY, frequency TINYINT, kanjiCount TINYINT)")
	query.execute("create table kanji(id INTEGER SECONDARY KEY REFERENCES entries, priority INT, docid INT, frequency TINYINT)")
	query.execute("create virtual table kanjiText using fts3(reading)")
	query.execute("create table kana(id INTEGER SECONDARY KEY REFERENCES entries, priority INT, docid INT, nokanji BOOLEAN, frequency TINYINT, restrictedTo TEXT)")
	query.execute("create virtual table kanaText using fts3(reading, TOKENIZE katakana)")
	query.execute("create table senses(id INTEGER SECONDARY KEY REFERENCES entries, priority INT, pos %s, misc %s, dial %s, field %s)" % (SqlTypeForCodingSize(entitiesCoding.tagsCodingSize["pos"]), SqlTypeForCodingSize(entitiesCoding.tagsCodingSize["misc"]), SqlTypeForCodingSize(entitiesCoding.tagsCodingSize["dial"]), SqlTypeForCodingSize(entitiesCoding.tagsCodingSize["field"])))
	query.execute("create table stagk(id INTEGER SECONDARY KEY REFERENCES entries, sensePriority INT, kanjiPriority INT)")
	query.execute("create table stagr(id INTEGER SECONDARY KEY REFERENCES entries, sensePriority INT, kanaPriority INT)")
	query.execute("create table gloss(id INTEGER SECONDARY KEY REFERENCES entries, sensePriority INT, lang CHAR(3), docid INTEGER SECONDARY KEY NOT NULL)")
	query.execute("create virtual table glossText using fts3(reading)")
	query.execute("create table kanjiChar(kanji INTEGER, id INTEGER SECONDARY KEY REFERENCES entries, priority INT)")

	# Insert entries in the database using an XML parser
	print "[jmdict] Inserting entries..."
	handler = JMDictHandler()
	file = open(JMdictFile, "r")
	if not file:
		usage()
		sys.exit(1)		
	xml.sax.parse(file, handler)

	# Insert JLPT levels
	print "[jmdict] Inserting JLPT levels..."
	query.execute("create table jlpt(id INTEGER PRIMARY KEY, level TINYINT)")
	if not jlptFiles:
		insertJLPTLevels("src/core/jmdict/jlpt-level4.txt", 4)
		insertJLPTLevels("src/core/jmdict/jlpt-level3.txt", 3)
		insertJLPTLevels("src/core/jmdict/jlpt-level2.txt", 2)
		insertJLPTLevels("src/core/jmdict/jlpt-level1.txt", 1)
	else:
		insertJLPTLevels(jlptFiles[0].srcpath(), 4)
		insertJLPTLevels(jlptFiles[1].srcpath(), 3)
		insertJLPTLevels(jlptFiles[2].srcpath(), 2)
		insertJLPTLevels(jlptFiles[3].srcpath(), 1)
	
	# Create tables indexes for faster search
	print "[jmdict] Creating tables indexes..."
	sys.stdout.flush()
	# Dramatically speeds up sorting by frequency
	query.execute("create index idx_entries_frequency on entries(frequency)")
	query.execute("create index idx_kanji on kanji(id)")
	query.execute("create index idx_kanji_docid on kanji(docid)")
	query.execute("create index idx_kana on kana(id)")
	query.execute("create index idx_kana_docid on kana(docid)")
	query.execute("create index idx_senses on senses(id)")
	query.execute("create index idx_stagk on stagk(id, sensePriority)")
	query.execute("create index idx_stagr on stagr(id, sensePriority)")
	query.execute("create index idx_gloss on gloss(id)")
	query.execute("create index idx_gloss_docid on gloss(docid)")
	query.execute("create index idx_kanjichar on kanjiChar(kanji)")
	query.execute("create index idx_kanjichar_id on kanjiChar(id)")
	query.execute("create index idx_jlpt on jlpt(level)")

	# And vacuum everything!
	print "[jmdict] Optimizing database organization..."
	sys.stdout.flush()
	query.execute("analyze")
	query.execute("vacuum")

	db.commit()
	db.close()
	del db
	os.chmod(dbFile, 0x124)

# target: [ db file, defshFile, defscFile ]
# source: [ jmdict, ... ]
#def sconsCreateJMdictDB(target, source, env):
	#global languages
#
	#languages = env['lang']
	#if len(languages) == 0: languages = [ "en" ]
#
	#global entitiesCoding
	#print "[jmdict] Getting dictionary entities..."
	#sys.stdout.flush()
	#entitiesCoding = getEntities(open(source[0].path, 'r'))
	#print "[jmdict] %d entities found" % (len(entitiesCoding.entities))
	#produceEntitiesCFile(entitiesCoding, open(target[1].path, "w"), open(target[2].path, "w"))
#
	#createDB(target[0].path, source[0].path)
