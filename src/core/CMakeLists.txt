include(${QT_USE_FILE})

set(tagainijisho_core_SRCS
TextTools.cc
Paths.cc
Database.cc
QueryBuilder.cc
ASyncQuery.cc
ASyncEntryFinder.cc
ASyncEntryLoader.cc
Preferences.cc
Tag.cc
Entry.cc
RelativeDate.cc
SearchCommand.cc
EntrySearcher.cc
EntryLoader.cc
EntrySearcherManager.cc
ResultsList.cc
EntryListDB.cc
EntryListCache.cc
EntryListModel.cc
EntriesCache.cc
Plugin.cc
XmlParserHelper.cc
)

set(tagainijisho_core_MOCS
ASyncQuery.h
ASyncEntryFinder.h
ASyncEntryLoader.h
EntriesCache.h
Entry.h
EntrySearcher.h
EntryLoader.h
EntrySearcherManager.h
ResultsList.h
EntryListModel.h
Preferences.h
Tag.h
)

qt4_wrap_cpp(tagainijisho_core_MOC_SRCS ${tagainijisho_core_MOCS})

include_directories(${QT_INCLUDE_DIR})
# TODO remove once the SQLite stuff is entirely moved into the sqlite module
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/sqlite)

# Add rules for jmdict and kanjidic components
add_subdirectory(jmdict)
add_subdirectory(kanjidic2)

if (BUILD_TESTS)
add_subdirectory(tests)
endif()

add_library(tagaini_core STATIC ${tagainijisho_core_MOC_SRCS} ${tagainijisho_core_SRCS})
