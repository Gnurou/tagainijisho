include(../../buildconfig.pri)
TEMPLATE = lib
TARGET = tagaini-core
CONFIG += qt staticlib
QT = core sql
INCLUDEPATH = .. ../../3rdparty/sqlite
HEADERS += Paths.h \
    Database.h \
    QueryBuilder.h \
    Query.h \
    ASyncQuery.h \
    ASyncEntryLoader.h \
    Preferences.h \
    Tag.h \
    Entry.h \
    RelativeDate.h \
    SearchCommand.h \
    EntrySearcher.h \
    EntrySearcherManager.h \
    EntriesCache.h \
	Plugin.h \
	jmdict/JMdictDefs.h \
    jmdict/JMdictEntry.h \
    jmdict/JMdictEntrySearcher.h \
	jmdict/JMdictPlugin.h \
	kanjidic2/Kanjidic2Entry.h \
	kanjidic2/Kanjidic2EntrySearcher.h \
	kanjidic2/Kanjidic2Plugin.h
SOURCES += Database.cc \
    QueryBuilder.cc \
    Query.cc \
    ASyncQuery.cc \
    ASyncEntryLoader.cc \
    Preferences.cc \
    Tag.cc \
    Entry.cc \
    RelativeDate.cc \
    SearchCommand.cc \
    EntrySearcher.cc \
    EntrySearcherManager.cc \
    EntriesCache.cc \
	Plugin.cc \
	jmdict/JMdictDefs.cc \
    jmdict/JMdictEntry.cc \
    jmdict/JMdictEntrySearcher.cc \
	jmdict/JMdictPlugin.cc \
	kanjidic2/Kanjidic2Entry.cc \
	kanjidic2/Kanjidic2EntrySearcher.cc \
	kanjidic2/Kanjidic2Plugin.cc
#LIBS = ../sqlite/libsqlite3.a
