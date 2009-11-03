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

import re
import pickle

class EntitiesCoding:
	entities = {}          # Associates shortcuts to long description
	reversedEntities = {}  # Associates long description to shortcut
	entitiesCoding = {}    # 2 dimentional array [tag][entity] that gives the numeric value of a given entity for that tag
	tagsCodingSize = {}    # The size in bits necessary to code a given tag

	def save(self, entitiesFile):
		pickle.dump((self.entities, self.reversedEntities, self.entitiesCoding, self.tagsCodingSize), open(entitiesFile, "w"))

	def load(self, entitiesFile):
		(self.entities, self.reversedEntities, self.entitiesCoding, self.tagsCodingSize) = pickle.load(open(entitiesFile, "r"))

def getEntities(file):
	#entitiesFile = "src/jmdictentities.dat"
	#if os.path.exists(entitiesFile):
	#	entities, reversedEntities = pickle.load(open(entitiesFile, "r"))
	#else:
	ret = EntitiesCoding()

	# First read all declared entities in the XML file
	entityRegExp = re.compile('''^<!ENTITY (.+) "(.+)">''')
	for line in file.xreadlines():
		match = entityRegExp.match(line)
		if match:
			ret.entities[match.group(1)] = match.group(2)
			ret.reversedEntities[match.group(2)] = match.group(1)
	#pickle.dump((entities, reversedEntities), open(entitiesFile, "w"))

	# Then reparse the file and associate tags to found entities
	file.seek(0)
	tagsAssociations = {}
	tagsRegExp = re.compile('''<(.+)>&(.+);</.+>''')
	for line in file.xreadlines():
		match = tagsRegExp.match(line)
		if match:
			if ret.entities.has_key(match.group(2)):
				if not tagsAssociations.has_key(match.group(1)): tagsAssociations[match.group(1)] = set()
				tagsAssociations[match.group(1)].add(match.group(2))

	# Finally, assign a coding to all the tags we found.
	for tag in tagsAssociations:
		ret.entitiesCoding[tag] = {}
		code = 1
		cpt = 0
		for entity in tagsAssociations[tag]:
			ret.entitiesCoding[tag][entity] = code
			code = code << 1
			cpt += 1
		ret.tagsCodingSize[tag] = cpt
		if cpt > 64: print "ERROR: tag %s needs more than 64 bits to be encoded!" % (tag)

	return ret

def CTypeForCodingSize(size):
	if size <= 8: return "quint8"
	elif size <= 16: return "quint16"
	elif size <= 32: return "quint32"
	elif size <= 64: return "quint64"

def SqlTypeForCodingSize(size):
	if size <= 8: return "TINYINT"
	elif size <= 16: return "SMALLINT"
	elif size <= 32: return "INT"
	elif size <= 64: return "BIGINT"

def produceEntitiesCFile(entitiesCoding, hFile, cFile):
	hFile.write("/**\n * Automatically generated - do not edit!\n */\n\n#ifndef __CORE_JMDICT_DEFS_H_\n#define __CORE_JMDICT_DEFS_H_\n\n#include <QString>\n\n")
	cFile.write("/**\n * Automatically generated - do not edit!\n */\n\n#include <QString>\n#include \"core/jmdict/JMdictDefs.h\"\n\n")

	# Filter entities coding to only keep entities we need
	validEntities = {}
	for entity in entitiesCoding.entitiesCoding:
		# Skip entities in kanji or kanas that we don't handle
		if entity[:3] == "ke_" or entity[:3] == "re_": continue
		validEntities[entity] = entitiesCoding.entitiesCoding[entity]

	hFile.write("// Types for coding the different tag values\n")
	for tag in validEntities:
		hFile.write("#define JMdict%sTagType %s\n" % (tag.capitalize(), CTypeForCodingSize(entitiesCoding.tagsCodingSize[tag])))
	hFile.write("\n")

	hFile.write("extern const QString JMdictSenseTagsList[];\n")
	cFile.write("const QString JMdictSenseTagsList[] = {\n")
	nbTags = 0
	for tag in validEntities:
		cFile.write("\t\"%s\",\n" % tag)
		nbTags += 1
	cFile.write("};\n")
	hFile.write("static const int JMdictNbSenseTags = %d;\n\n" % (nbTags))

	for tag in validEntities:
		detailedList = []
		for n in entitiesCoding.entitiesCoding[tag]:
			hFile.write("#define JMdict_%s_%s\tQ_UINT64_C(0x%x)\n" % (tag.upper(), n.replace('-', '_'), entitiesCoding.entitiesCoding[tag][n]))
			detailedList.append((entitiesCoding.entitiesCoding[tag][n], entitiesCoding.entities[n], n))

		hFile.write("\nextern const QString JMdict%sEntitiesShortDesc[];\n" % (tag.capitalize()))
		hFile.write("extern const QString JMdict%sEntitiesLongDesc[];\n" % (tag.capitalize()))
		hFile.write("extern const quint64 JMdict%sValues[];\n\n" % (tag.capitalize()))
		detailedList.sort()
		cFile.write("\nconst QString JMdict%sEntitiesShortDesc[] = {\n" % (tag.capitalize()))
		for desc in detailedList:
			cFile.write("\t\"%s\",\n" % (desc[2]))
		cFile.write("\t\"\"\n")
		cFile.write("};\n")
		cFile.write("\nconst QString JMdict%sEntitiesLongDesc[] = {\n" % (tag.capitalize()))
		for desc in detailedList:
			cFile.write("\tQT_TRANSLATE_NOOP(\"JMdictLongDescs\", \"%s\"),\n" % (desc[1]))
		cFile.write("\t\"\"\n")
		cFile.write("};\n")
		cFile.write("\nconst quint64 JMdict%sValues[] = {\n" % (tag.capitalize()))
		for desc in detailedList:
			cFile.write("\tJMdict_%s_%s,\n" % (tag.upper(), desc[2].replace('-', '_')))
		cFile.write("\t0\n")
		cFile.write("};\n")

	hFile.write("extern const QString *JMdictSenseTags[];\n");
	hFile.write("extern const quint64 *JMdictSenseTagsValues[];\n");
	cFile.write("const QString *JMdictSenseTags[] = {\n");
	for tag in validEntities:
		cFile.write("\tJMdict%sEntitiesShortDesc,\n" % (tag.capitalize()))
	cFile.write("};\n");
	cFile.write("const quint64 *JMdictSenseTagsValues[] = {\n");
	for tag in validEntities:
		cFile.write("\tJMdict%sValues,\n" % (tag.capitalize()))
	cFile.write("};\n");

	hFile.write("#endif\n")
