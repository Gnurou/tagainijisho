#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
import os, os.path
import urllib
import re
from gzip import GzipFile
from StringIO import StringIO
import tarfile
import shutil

sqliteversion = "3.6.21"
sqlitedest = os.path.join("3rdparty", "sqlite")
sqlitefiles = ["sqlite3.c", "sqlite3.h"]
sqlitesource = "http://www.sqlite.org/sqlite-amalgamation-%s.tar.gz" % (sqliteversion,)

pysqliteversion = "2.5.5"
pysqlitedest = os.path.join("3rdparty", "pysqlite-%s" % (pysqliteversion))
pysqlitesource = "http://pysqlite.googlecode.com/files/pysqlite-%s.tar.gz" % (pysqliteversion,)

jmdictDB = "jmdict.db"
jmdictdata = os.path.join("3rdparty", "JMdict")
jmdictentitiesdata = os.path.join("3rdparty", "jmdictentities.dat")
jmdictdatasource = "ftp://ftp.monash.edu.au/pub/nihongo/JMdict.gz"

kanjidicDB = "kanjidic2.db"
kanjidicdata = os.path.join("3rdparty", "kanjidic2.xml")
kanjidicdatasource = "http://www.csse.monash.edu.au/~jwb/kanjidic2/kanjidic2.xml.gz"
kanjivgdata = os.path.join("3rdparty", "kanjivg.xml")
kanjivgdatasource = "http://kanjivg.tagaini.net/upload/Main/kanjivg-latest-beta.xml.gz"

sys.path.append(os.path.join("src", "python"))

forciblyBuildJMdict = False
forciblyBuildKanjidic = False
lang = "en"

def getSQLite():
	sqliteOk = True
	if not os.path.exists(sqlitedest): os.mkdir(sqlitedest)
	for f in [os.path.join(sqlitedest, f) for f in sqlitefiles]:
		if not os.path.exists(f): sqliteOk = False
	if not sqliteOk:
		print "Getting SQLite from %s..." % (sqlitesource)
		sourceFile = tarfile.open(mode = "r:gz", fileobj = StringIO(urllib.urlopen(sqlitesource).read()))
		for f in sqlitefiles:
			tarName = "sqlite-%s/%s" % (sqliteversion, f)
			tempFile = sourceFile.extractfile(tarName)
			destFile = open(os.path.join(sqlitedest, f), 'w')
			destFile.write(tempFile.read())
			destFile.close()
			tempFile.close()
		sourceFile.close()
		os.system("patch 3rdparty/sqlite/sqlite3.c < sqlite3.c.patch")

def getPySQLite():
	if not os.path.exists(pysqlitedest):
		print "Getting pySQLite from %s..." % (pysqlitesource)
		sourceFile = tarfile.open(mode = "r:gz", fileobj = StringIO(urllib.urlopen(pysqlitesource).read()))
		sourceFile.extractall(os.path.split(pysqlitedest)[0])
		sourceFile.close()
		## Why the hell do we have to patch this??
		initFile = open(os.path.join(pysqlitedest, "pysqlite2", "__init__.py"), "a")
		initFile.write("\nfrom dbapi2 import *\n")
		initFile.close()
		# Remove the setup.cfg file
		os.remove(os.path.join(pysqlitedest, "setup.cfg"))
		# Add check for OSX fucking stupid link process
		# Normally we should be able to use the -O option during build to specify additional object files, 
		# but this won't work here - so we have to patch the build.py file
		newContents = open(os.path.join(pysqlitedest, "setup.py"), "r").read()
		linkList = [ "../../src/sqlite/libsqlite3.a" ]
		newContents = re.sub("extra_objects = .*", 'extra_objects = [ "%s" ]\nif sys.platform == "darwin": extra_link_args = [ "-framework", "QtCore" ]\nelse: extra_link_args = []' % ('", "'.join(linkList),), newContents)
		newContents = re.sub("(extra_objects=extra_objects,)", "\\1 extra_link_args=extra_link_args,", newContents)
		setupFile = open(os.path.join(pysqlitedest, "setup.py"), "w")
		setupFile.write(newContents)
		setupFile.close()

def getJMdict():
	if not os.path.exists(jmdictdata):
		print "Getting JMdict from %s..." % (jmdictdatasource)
		sourceFile = GzipFile(fileobj = StringIO(urllib.urlopen(jmdictdatasource).read()))
		localFile = open(jmdictdata, 'w')
		localFile.write(sourceFile.read())
		localFile.close()
		sourceFile.close()
		import jmdictentities
		print "Generating JMdict entities..."
		entitiesCoding = jmdictentities.getEntities(open(jmdictdata, "r"))
		entitiesCoding.save(jmdictentitiesdata)
		print "Generating JMdict definition files..."
		defsHFile = os.path.join("src", "core", "jmdict", "JMdictDefs.h")
		defsCFile = os.path.join("src", "core", "jmdict", "JMdictDefs.cc")
		jmdictentities.produceEntitiesCFile(entitiesCoding, open(defsHFile, "w"), open(defsCFile, "w"))

def getKanjidic():
	if not os.path.exists(kanjidicdata):
		print "Getting Kanjidic2 from %s..." % (kanjidicdatasource)
		sourceFile = GzipFile(fileobj = StringIO(urllib.urlopen(kanjidicdatasource).read()))
		localFile = open(kanjidicdata, 'w')
		localFile.write(sourceFile.read())
		localFile.close()
		sourceFile.close()

def getKanjiVG():
	if not os.path.exists(kanjivgdata):
		print "Getting KanjiVG from %s..." % (kanjivgdatasource)
		sourceFile = GzipFile(fileobj = StringIO(urllib.urlopen(kanjivgdatasource).read()))
		localFile = open(kanjivgdata, 'w')
		localFile.write(sourceFile.read())
		localFile.close()
		sourceFile.close()

def buildPySQLite():
	wd = os.getcwd()
	os.chdir(pysqlitedest)
	buildString = "python setup.py -q build_ext -I../sqlite"
	if sys.platform != "darwin":
		buildString += " -lQtCore"
		if os.environ.has_key("QT4DIR"): buildString += " -L" + os.path.join(os.environ["QT4DIR"], "lib")
	os.system(buildString)
	os.system("python setup.py -q install --prefix=install --install-purelib=install --install-platlib=install")
	os.chdir(wd)
	del wd
	sys.path.insert(0, os.path.join(pysqlitedest, "install"))

def buildJMdict():
	if os.path.exists(jmdictDB) and (forciblyBuildJMdict or os.path.getmtime(jmdictDB) <= os.path.getmtime(jmdictdata)):
		os.remove(jmdictDB)
	if not os.path.exists(jmdictDB):
		import jmdict
		# TODO read entities from a file!
		print "Building JMdict database"
		jmdict.languages = [ lang ]
		jmdict.entitiesCoding = jmdict.EntitiesCoding()
		jmdict.entitiesCoding.load(jmdictentitiesdata)
		print "Creating JMdict DB..."
		jmdict.createDB(jmdictDB, jmdictdata)

def buildKanjidic():
	if os.path.exists(kanjidicDB) and (forciblyBuildKanjidic or os.path.getmtime(kanjidicDB) <= os.path.getmtime(kanjidicdata)):
		os.remove(kanjidicDB)
	if not os.path.exists(kanjidicDB):
		import kanjidic2
		print "Building Kanjidic database"
		kanjidic2.languages = [ lang ]
		print "Creating Kanjidic2 DB..."
		kanjidic2.createDB(kanjidicDB, kanjidicdata, kanjivgdata)


def getAllDependencies():
	getSQLite()
	getPySQLite()
	getJMdict()
	getKanjidic()
	getKanjiVG()

def buildDatabases():
	buildPySQLite()
	buildJMdict()
	buildKanjidic()

def cleanSQLite(purge = False):
	if purge:
		for f in filter(os.path.exists, [os.path.join(sqlitedest, f) for f in sqlitefiles]): os.remove(f)

def cleanPySQLite(purge = False):
	if os.path.exists(pysqlitedest):
		# TODO clean in PySQLite dir
		if purge:
			shutil.rmtree(pysqlitedest)

def cleanJMdict(purge = False):
	if os.path.exists(jmdictDB): os.remove(jmdictDB)
	if purge and os.path.exists(jmdictdata): os.remove(jmdictdata)

def cleanKanjidic(purge = False):
	if os.path.exists(kanjidicDB): os.remove(kanjidicDB)
	if purge and os.path.exists(kanjidicdata): os.remove(kanjidicdata)

def usage():
       print """Usage: builddb.py [options]
Options:
-h         --help              Print this help message.
-c<o>      --clean <o>         Clean all generated data for:
                               s: SQLite
                               p: PySQLite
                               j: JMdict
                               k: Kanjidic2
                               a: All of the above
-p<o>      --purge <o>         Purge all generated and downloaded data for:
                               s: SQLite
                               p: PySQLite
                               j: JMdict
                               k: Kanjidic2
                               a: All of the above
-d         --download          Just download all dependencies, do not build.
-j         --force-jmdict      Forcibly build JMdict database.
-k         --force-kanjidic    Forcibly build kanjidic database.
-l<la>     --lang <la>         Generate the dictionary for language <la>
                               instead of English.
                               Valid values: en,de,fr,es,ru"""

if __name__ == '__main__':
	import getopt
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hc:p:djkl:", ["help", "clear", "purge", "download", "force-jmdict", "force-kanjidic", "lang"])
	except getopt.GetoptError:
		usage()
		sys.exit(1)
	doNotBuild = False
	if not os.path.exists("3rdparty"): os.mkdir("3rdparty")
	for opt, arg in opts:
		if opt in ("-h", "--help"):
			usage()
			sys.exit(0)
		elif opt in ("-j", "--force-jmdict"):
			forciblyBuildJMdict = True
		elif opt in ("-k", "--force-kanjidic"):
			forciblyBuildKanjidic = True
		elif opt in ("-l", "--lang"):
			if arg not in ("en", "de", "fr", "es", "ru"):
				usage()
				sys.exit(1)
			lang = arg
		elif opt in ("-c", "--clean"):
			doNotBuild = True
			for c in arg:
				passed = False
				if c == 's' or c == 'a':
					cleanSQLite(); passed = True
				if c == 'p' or c == 'a':
					cleanPySQLite(); passed = True
				if c == 'j' or c == 'a':
					cleanJMdict(); passed = True
				if c == 'k' or c == 'a':
					cleanKanjidic(); passed = True
				if not passed:
					usage()
					sys.exit(1)
		elif opt in ("-p", "--purge"):
			doNotBuild = True
			for c in arg:
				passed = False
				if c == 's' or c == 'a':
					cleanSQLite(True); passed = True
				if c == 'p' or c == 'a':
					cleanPySQLite(True); passed = True
				if c == 'j' or c == 'a':
					cleanJMdict(True); passed = True
				if c == 'k' or c == 'a':
					cleanKanjidic(True); passed = True
				if not passed:
					usage()
					sys.exit(1)
		elif opt in ("-d", "--download"):
			getAllDependencies()
			sys.exit(0)
	if doNotBuild: sys.exit(0)
	getAllDependencies()
	buildDatabases()
