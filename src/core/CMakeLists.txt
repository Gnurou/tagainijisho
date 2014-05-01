# FIXME we still have a few dependencies on QtGUI:
# QColor, QFont, QPalette, QBrush are used by
# Entry, EntrylistModel, and ResultsList.
#set(QT_DONT_USE_QTGUI TRUE)
#include(${QT_USE_FILE})

include_directories(${Qt5Core_INCLUDE_DIRS})
# TODO remove this - dependency on QColor
include_directories(${Qt5Widgets_INCLUDE_DIRS})

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
Entry.h
ResultsList.h
EntryListModel.h
Preferences.h
Tag.h
)

qt5_wrap_cpp(tagainijisho_core_MOC_SRCS ${tagainijisho_core_MOCS})

#include_directories(${QT_INCLUDE_DIR})
# TODO remove once the SQLite stuff is entirely moved into the sqlite module
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/sqlite)

add_library(tagaini_core STATIC ${tagainijisho_core_MOC_SRCS} ${tagainijisho_core_SRCS})
target_link_libraries(tagaini_core tagaini_sqlite)

if(APPLE)
	set(DEST_DIR "${tagaini_binary}.app/Contents/MacOS")
else(APPLE)
	set(DEST_DIR "lib")	
endif(APPLE)

# Add rules for sub-components
add_subdirectory(jmdict)
add_subdirectory(kanjidic2)
#add_subdirectory(tatoeba)
target_link_libraries(tagaini_core tagaini_core_jmdict)
target_link_libraries(tagaini_core tagaini_core_kanjidic2)

if (BUILD_TESTS)
add_subdirectory(tests)
endif()
