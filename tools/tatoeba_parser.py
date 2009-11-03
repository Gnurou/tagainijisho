# -*- coding: utf-8 -*-
#!/usr/bin/python

import MeCab
import re

tagger = MeCab.Tagger()

f = open("sentences_ja_20090326.csv", "r")
lines = f.readlines()
for line in lines:
	sentence = re.sub(r"\{([^\}]*)\}\{[^\}]*\}", r"\1", re.sub(r"\"(.*)\".*", r"\1", line.split(";")[2]))
	node = tagger.parseToNode(sentence)
	prevNType = u''
	while node:
		features = node.feature.split(",")
		if len(features) > 7:
			nSurface = node.surface
			nType = unicode(features[0], "utf-8")
			nDictForm = unicode(features[6], "utf-8")
			nReading = unicode(features[7], "utf-8")
			if nType == u'名詞': print nSurface, nReading
			elif nType == u'動詞': print nSurface, nDictForm
			elif nType == u'助動詞' and prevNType == u'動詞': print "Conjugated:", nReading
			# Special case of 'て', recognized as a particle after a verb in 連用形 form
			elif nType == u'助詞': pass
			prevNType = nType
		else:
			prevNType = u''
		node = node.next
