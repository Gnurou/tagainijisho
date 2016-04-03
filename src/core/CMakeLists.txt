set(QT_DONT_USE_QTGUI TRUE)
#include(${QT_USE_FILE})

set(tagainijisho_core_SRCS
TextTools.cc
Paths.cc
Lang.cc
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
EntryListDB.cc
EntryListCache.cc
EntriesCache.cc
Plugin.cc
XmlParserHelper.cc
)

#include_directories(${QT_INCLUDE_DIR})
# TODO remove once the SQLite stuff is entirely moved into the sqlite module
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/sqlite)

add_library(tagaini_core STATIC ${tagainijisho_core_SRCS})
target_link_libraries(tagaini_core tagaini_sqlite Qt5::Core)

if(APPLE)
	set(DEST_DIR "${tagaini_binary}.app/Contents/MacOS")
else(APPLE)
	set(DEST_DIR "lib")	
endif(APPLE)

# Add rules for sub-components
add_subdirectory(jmdict)
add_subdirectory(kanjidic2)
#add_subdirectory(tatoeba)
