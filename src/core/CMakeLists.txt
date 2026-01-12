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
    XmlParserHelper.cc)

add_library(tagaini_core STATIC ${tagainijisho_core_SRCS})
# Include directories inherited from tagaini_sqlite (sqlite headers + common)
target_link_libraries(tagaini_core PUBLIC tagaini_sqlite Qt::Core)
# Add local headers for this module
target_include_directories(tagaini_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(APPLE)
  set(DEST_DIR "${tagaini_binary}.app/Contents/MacOS")
else(APPLE)
  set(DEST_DIR "lib")
endif(APPLE)

# Add rules for sub-components
add_subdirectory(jmdict)
add_subdirectory(kanjidic2)
# add_subdirectory(tatoeba)
