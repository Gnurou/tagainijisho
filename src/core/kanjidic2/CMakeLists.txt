# kanjidic2 and KanjiVG core
set(QT_DONT_USE_QTGUI TRUE)

set(KANJIDIC2_SOURCE http://www.edrdg.org/kanjidic/kanjidic2.xml.gz)
set(KANJIVG_SOURCE https://github.com/KanjiVG/kanjivg/releases/download/r20160426/kanjivg-20160426.xml.gz)

# Kanjidic2
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml)
	MESSAGE(STATUS "Downloading Kanjidic2 from ${KANJIDIC2_SOURCE}")
	FILE(DOWNLOAD ${KANJIDIC2_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml.gz SHOW_PROGRESS)
	execute_process(COMMAND ${GUNZIP} ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty RESULT_VARIABLE result)
	if(NOT "${result}" STREQUAL 0)
		MESSAGE(FATAL_ERROR "Unable to run [${GUNZIP} ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml.gz]. Reported error was [${result}]")
	endif(NOT "${result}" STREQUAL 0)
endif()

# Kanjidic2Descs
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/Kanjidic2Descs.cc)
	MESSAGE(STATUS "Generating Kanjidic2Descs")
	#	FILE(STRINGS ${CMAKE_SOURCE_DIR}/3rdparty/JMdict LINES REGEX ENTITY)
	#	foreach(LINE ${LINES})
	#		STRING(REGEX REPLACE ".*\\\"(.*)\\\">" "\\1" result "${LINE}" )
	#		FILE(APPEND ${CMAKE_SOURCE_DIR}/3rdparty/Kanjidic2Descs.cc "QT_TRANSLATE_NOOP(\"JMdictLongDescs\", \"${result}\");\n")
	#	endforeach(LINE ${LINES})
endif()

# KanjiVG
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml)
	MESSAGE(STATUS "Downloading KanjiVG from ${KANJIVG_SOURCE}")
	FILE(DOWNLOAD ${KANJIVG_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml.gz SHOW_PROGRESS)
	execute_process(COMMAND ${GUNZIP} ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty RESULT_VARIABLE result)
	if(NOT "${result}" STREQUAL 0)
		MESSAGE(FATAL_ERROR "Unable to run [${GUNZIP} ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml.gz]. Reported error was [${result}]")
	endif(NOT "${result}" STREQUAL 0)
	FILE(RENAME ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml)
endif()

set(tagainijisho_core_kanjidic2_SRCS
Kanjidic2Entry.cc
Kanjidic2EntrySearcher.cc
Kanjidic2EntryLoader.cc
KanjiRadicals.cc
Kanjidic2Plugin.cc
)

add_library(tagaini_core_kanjidic2 STATIC ${tagainijisho_core_kanjidic2_SRCS})
target_link_libraries(tagaini_core_kanjidic2 tagaini_sqlite Qt5::Core)

# Database builder
set(build_kanji_db_SRCS
Kanjidic2Parser.cc
KanjiVGParser.cc
BuildKanjiDB.cc
../XmlParserHelper.cc
)

if(NOT CMAKE_CROSSCOMPILING)
add_executable(build_kanji_db EXCLUDE_FROM_ALL ${build_kanji_db_SRCS})
target_link_libraries(build_kanji_db tagaini_sqlite Qt5::Core)

# Database target. Always build the english DB, other languages are optional.
set(ALL_LANGS "en")
foreach(LANG ${DICT_LANG})
	set(ALL_LANGS "${ALL_LANGS},${LANG}")
endforeach()
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/kanjidic2.db
	COMMAND build_kanji_db -l${ALL_LANGS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
	WORKING_DIRECTORY ${CMAKE_PREFIX_PATH}/bin
	DEPENDS build_kanji_db ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml)
add_custom_target(kanjidic2-db DEPENDS ${CMAKE_BINARY_DIR}/kanjidic2.db)
add_dependencies(databases kanjidic2-db)
endif(NOT CMAKE_CROSSCOMPILING)
