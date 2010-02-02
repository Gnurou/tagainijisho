set(QT_USE_QTSQL TRUE)
include(${QT_USE_FILE})
add_subdirectory(jmdict)
add_subdirectory(kanjidic2)

set(tagainijisho_core_SRCS
TextTools.cc
Database.cc
QueryBuilder.cc
Query.cc
ASyncQuery.cc
ASyncEntryLoader.cc
Preferences.cc
Tag.cc
Entry.cc
RelativeDate.cc
SearchCommand.cc
EntrySearcher.cc
EntrySearcherManager.cc
EntriesCache.cc
Plugin.cc
XmlParserHelper.cc
jmdict/JMdictEntry.cc
jmdict/JMdictEntrySearcher.cc
jmdict/JMdictPlugin.cc
kanjidic2/Kanjidic2Entry.cc
kanjidic2/Kanjidic2EntrySearcher.cc
kanjidic2/Kanjidic2Plugin.cc
)

qt4_wrap_cpp(tagainijisho_core_MOC_SRCS
ASyncEntryLoader.h
ASyncQuery.h
Database.h
EntriesCache.h
Entry.h
EntrySearcher.h
EntrySearcherManager.h
Preferences.h
Query.h
Tag.h
jmdict/JMdictEntry.h
jmdict/JMdictEntrySearcher.h
kanjidic2/Kanjidic2Entry.h
kanjidic2/Kanjidic2EntrySearcher.h
)

include_directories(${QT_INCLUDE_DIR})
# TODO remove once the SQLite stuff is entirely moved into the sqlite module
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/sqlite)
add_library(tagaini_core STATIC ${tagainijisho_core_SRCS} ${tagainijisho_core_MOC_SRCS})
