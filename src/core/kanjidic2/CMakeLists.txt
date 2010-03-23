set(QT_USE_QTSQL TRUE)
set(QT_USE_QTXML TRUE)
set(QT_DONT_USE_QTGUI TRUE)

set(build_kanji_db_SRCS
	Kanjidic2Parser.cc
	KanjiVGParser.cc
	BuildKanjiDB.cc
)

include(${QT_USE_FILE})
add_executable(build_kanji_db EXCLUDE_FROM_ALL ${build_kanji_db_SRCS})
target_link_libraries(build_kanji_db tagaini_sqlite tagaini_core ${QT_LIBRARIES})
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(extra_link_flags "-bind_at_load")
endif()

