set(KANJIDIC2_FILE
    ""
    CACHE FILEPATH "Path to kanjidic2.xml file")
set(KANJIVG_FILE
    ""
    CACHE FILEPATH "Path to KanjiVG XML file")

set(KANJIDIC2_SOURCE https://www.edrdg.org/kanjidic/kanjidic2.xml.gz)
set(KANJIVG_SOURCE
    https://github.com/KanjiVG/kanjivg/releases/download/r20220427/kanjivg-20220427.xml.gz
)

# Kanjidic2
if(KANJIDIC2_FILE)
  if(NOT EXISTS ${KANJIDIC2_FILE})
    message(
      FATAL_ERROR
        "Specified kanjidic2.xml file does not exist: ${KANJIDIC2_FILE}")
  endif()
else()
  set(KANJIDIC2_FILE "${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml")

  if(NOT EXISTS ${KANJIDIC2_FILE})
    message(STATUS "Downloading Kanjidic2 from ${KANJIDIC2_SOURCE}")
    file(DOWNLOAD ${KANJIDIC2_SOURCE}
         ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml.gz SHOW_PROGRESS)
    execute_process(
      COMMAND ${GZIP} -d ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml.gz
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty
      RESULT_VARIABLE result)
    if(NOT "${result}" STREQUAL 0)
      message(
        FATAL_ERROR
          "Unable to run [${GZIP} -d ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml.gz]. Reported error was [${result}]"
      )
    endif(NOT "${result}" STREQUAL 0)
  endif()
endif()

# KanjiVG
if(KANJIVG_FILE)
  if(NOT EXISTS ${KANJIVG_FILE})
    message(
      FATAL_ERROR "Specified KanjiVG XML file does not exist: ${KANJIVG_FILE}")
  endif()
else()
  set(KANJIVG_FILE "${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml")

  if(NOT EXISTS ${KANJIVG_FILE})
    message(STATUS "Downloading KanjiVG from ${KANJIVG_SOURCE}")
    file(DOWNLOAD ${KANJIVG_SOURCE}
         ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml.gz SHOW_PROGRESS)
    execute_process(
      COMMAND ${GZIP} -d ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml.gz
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty
      RESULT_VARIABLE result)
    if(NOT "${result}" STREQUAL 0)
      message(
        FATAL_ERROR
          "Unable to run [${GZIP} -d ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml.gz]. Reported error was [${result}]"
      )
    endif(NOT "${result}" STREQUAL 0)
    file(RENAME ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml ${KANJIVG_FILE})
  endif()
endif()

set(tagainijisho_core_kanjidic2_SRCS
    Kanjidic2Entry.cc Kanjidic2EntrySearcher.cc Kanjidic2EntryLoader.cc
    KanjiRadicals.cc Kanjidic2Plugin.cc)

add_library(tagaini_core_kanjidic2 STATIC ${tagainijisho_core_kanjidic2_SRCS})
target_link_libraries(tagaini_core_kanjidic2 tagaini_sqlite Qt5::Core)

# Database builder
set(build_kanji_db_SRCS Kanjidic2Parser.cc KanjiVGParser.cc BuildKanjiDB.cc
                        ../XmlParserHelper.cc)

if(NOT CMAKE_CROSSCOMPILING)
  add_executable(build_kanji_db EXCLUDE_FROM_ALL ${build_kanji_db_SRCS})
  target_link_libraries(build_kanji_db tagaini_sqlite Qt5::Core)

  # Database target. Always build the english DB, other languages are optional.
  set(ALL_LANGS "en")
  foreach(LANG ${DICT_LANG})
    set(ALL_LANGS "${ALL_LANGS},${LANG}")
  endforeach()
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/kanjidic2.db
    COMMAND build_kanji_db -l${ALL_LANGS} ${KANJIDIC2_FILE} ${KANJIVG_FILE}
            ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS build_kanji_db ${KANJIDIC2_FILE} ${KANJIVG_FILE})
  add_custom_target(kanjidic2-db DEPENDS ${CMAKE_BINARY_DIR}/kanjidic2.db)
  add_dependencies(databases kanjidic2-db)
endif(NOT CMAKE_CROSSCOMPILING)
