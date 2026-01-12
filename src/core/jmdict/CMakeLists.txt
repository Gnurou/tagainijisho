set(JMDICT_FILE
    ""
    CACHE FILEPATH "Path to JMdict file")

# JMdict
set(JMDICT_MIRRORS https://www.edrdg.org/pub/Nihongo/JMdict.gz)

if(JMDICT_FILE)
  if(NOT EXISTS ${JMDICT_FILE})
    message(FATAL_ERROR "Specified JMdict file does not exist: ${JMDICT_FILE}")
  endif()
else()
  set(JMDICT_FILE "${CMAKE_SOURCE_DIR}/3rdparty/JMdict")

  if(NOT EXISTS ${JMDICT_FILE})
    foreach(JMDICT_SOURCE ${JMDICT_MIRRORS})
      message(STATUS "Downloading JMdict from ${JMDICT_SOURCE}")
      file(
        DOWNLOAD ${JMDICT_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz
        STATUS DLSTATUS
        SHOW_PROGRESS)
      list(GET DLSTATUS 0 ERRCODE)
      list(GET DLSTATUS 1 ERRMSG)
      if(NOT ERRCODE STREQUAL 0)
        message(WARNING "Error downloading. " ${ERRMSG} " (Error Code "
                        ${ERRCODE} ")")
      else()
        break()
      endif()
    endforeach()
    execute_process(
      COMMAND ${GZIP} -d ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty
      RESULT_VARIABLE result)
    if(NOT "${result}" STREQUAL 0)
      message(
        FATAL_ERROR
          "Unable to run [${GZIP} -d ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz]. Reported error was [${result}]"
      )
    endif(NOT "${result}" STREQUAL 0)
  endif()
endif()

# JMdictDescs
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/JMdictDescs.cc)
  message(STATUS "Generating JMdictDescs")
  file(STRINGS ${JMDICT_FILE} LINES REGEX ENTITY)
  foreach(LINE ${LINES})
    string(REGEX REPLACE ".*\\\"(.*)\\\">" "\\1" result "${LINE}")
    file(APPEND ${CMAKE_SOURCE_DIR}/3rdparty/JMdictDescs.cc
         "QT_TRANSLATE_NOOP(\"JMdictLongDescs\", \"${result}\");\n")
  endforeach(LINE ${LINES})
endif()

set(tagainijisho_core_jmdict_SRCS JMdictEntry.cc JMdictEntrySearcher.cc
                                  JMdictEntryLoader.cc JMdictPlugin.cc)

add_library(tagaini_core_jmdict STATIC ${tagainijisho_core_jmdict_SRCS})
target_link_libraries(tagaini_core_jmdict PUBLIC tagaini_core)
target_include_directories(tagaini_core_jmdict
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Database builder
set(build_jmdict_db_SRCS JMdictParser.cc BuildJMdictDB.cc ../XmlParserHelper.cc)

if(NOT CMAKE_CROSSCOMPILING)
  add_executable(build_jmdict_db EXCLUDE_FROM_ALL ${build_jmdict_db_SRCS})
  target_link_libraries(build_jmdict_db tagaini_core)

  # Database target. Always build the english DB, other languages are optional.
  set(ALL_LANGS "en")
  foreach(LANG ${DICT_LANG})
    set(ALL_LANGS "${ALL_LANGS},${LANG}")
  endforeach()
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/jmdict.db
    COMMAND build_jmdict_db -l${ALL_LANGS} ${JMDICT_FILE} ${CMAKE_SOURCE_DIR}
            ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS build_jmdict_db ${JMDICT_FILE})
  add_custom_target(jmdict-db DEPENDS ${CMAKE_BINARY_DIR}/jmdict.db)
  add_dependencies(databases jmdict-db)
endif(NOT CMAKE_CROSSCOMPILING)
