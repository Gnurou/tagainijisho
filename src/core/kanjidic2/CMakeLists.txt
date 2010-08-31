# kanji core
set(QT_USE_QTSQL TRUE)
set(QT_DONT_USE_QTGUI TRUE)
set(tagainijisho_core_kanjidic2_SRCS
Kanjidic2Entry.cc
Kanjidic2EntrySearcher.cc
KanjiRadicals.cc
Kanjidic2Plugin.cc
)

set(tagainijisho_core_kanjidic2_MOCS
Kanjidic2Entry.h
Kanjidic2EntrySearcher.h
)

qt4_wrap_cpp(tagainijisho_core_kanjidic2_MOC_SRCS ${tagainijisho_core_kanjidic2_MOCS})
add_library(tagaini_core_kanjidic2 STATIC ${tagainijisho_core_kanjidic2_MOC_SRCS} ${tagainijisho_core_kanjidic2_SRCS})
add_dependencies(tagaini_core_kanjidic2 tagaini_core)

# Database builder
set(QT_USE_QTXML TRUE)
set(build_kanji_db_SRCS
Kanjidic2Parser.cc
KanjiVGParser.cc
BuildKanjiDB.cc
)

include(${QT_USE_FILE})
add_executable(build_kanji_db EXCLUDE_FROM_ALL ${build_kanji_db_SRCS})
target_link_libraries(build_kanji_db tagaini_sqlite tagaini_core ${QT_LIBRARIES})
if (APPLE)
	set(extra_link_flags "-bind_at_load")
endif()

# Database targets for selected languages
foreach(LANG ${DICT_LANG})
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/kanjidic2-${LANG}.db
	COMMAND build_kanji_db -l${LANG} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/kanjidic2-${LANG}.db
	DEPENDS build_kanji_db ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml)
	add_custom_target(kanjidic2-${LANG} DEPENDS ${CMAKE_BINARY_DIR}/kanjidic2-${LANG}.db)
	add_dependencies(databases-${LANG} kanjidic2-${LANG})
endforeach(LANG ${LANG})
