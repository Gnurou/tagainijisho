project(tagainijisho)
set(VERSION 0.2.5.90)
set(SQLITE_VERSION 3.6.23.1)
set(SQLITE_SOURCE http://www.sqlite.org/sqlite-amalgamation-${SQLITE_VERSION}.tar.gz)
set(JMDICT_SOURCE ftp://ftp.monash.edu.au/pub/nihongo/JMdict.gz)
set(KANJIDIC2_SOURCE http://www.csse.monash.edu.au/~jwb/kanjidic2/kanjidic2.xml.gz)
set(KANJIVG_SOURCE http://kanjivg.tagaini.net/upload/Main/kanjivg-latest.xml.gz)
find_program(MULTI_MARKDOWN NAMES mmd2XHTML.pl)

cmake_minimum_required(VERSION 2.8.0)
find_package(Qt4 4.5 REQUIRED)

MACRO(GENERATE_DOCS _doc_files)
  FOREACH (_current_FILE ${ARGN})
    GET_FILENAME_COMPONENT(_abs_FILE ${_current_FILE} ABSOLUTE)
    GET_FILENAME_COMPONENT(doc ${_abs_FILE} NAME_WE)
    GET_SOURCE_FILE_PROPERTY(output_location ${_abs_FILE} OUTPUT_LOCATION)
    SET(doc "${CMAKE_CURRENT_SOURCE_DIR}/doc/${doc}.html")
    ADD_CUSTOM_COMMAND(OUTPUT ${doc}
       COMMAND ${MULTI_MARKDOWN} ARGS ${_abs_FILE} DEPENDS ${_abs_FILE}
    )
    SET(${_doc_files} ${${_doc_files}} ${doc})
  ENDFOREACH (_current_FILE)
ENDMACRO(GENERATE_DOCS)

# Global GCC options
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wnon-virtual-dtor -Wno-unused-parameter -fno-exceptions -fno-rtti")
endif(CMAKE_COMPILER_IS_GNUCC)

# Universal binaries for OSX
IF(APPLE)
	set(CMAKE_OSX_ARCHITECTURES "ppc;i386") 
endif(APPLE)

# Add the default database lookup data path for Linux if not defined
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	if(NOT DATA_DIR)
		set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/tagainijisho")
	endif(NOT DATA_DIR)
endif()

# Debug options
option(DEBUG_ENTRIES_CACHE "Debug entries cache behavior" OFF)
option(DEBUG_PATHS "Debug files lookup" OFF)
option(DEBUG_DETAILED_VIEW "Debug detailed view output" OFF)

# Build tests suite?
option(BUILD_TESTS "Build tests suite" OFF)

configure_file(${CMAKE_SOURCE_DIR}/src/tagaini_config.h.cmake ${CMAKE_BINARY_DIR}/src/tagaini_config.h)
# For config.h
include_directories(${CMAKE_BINARY_DIR}/src)

# For our own headers
include_directories(${CMAKE_SOURCE_DIR}/src)

# External resources fecthing and generation

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/)
	FILE(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/)
endif()

# SQLITE
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3.c) 
	MESSAGE(STATUS "Downloading SQLite ${SQLITE_VERSION} from ${SQLITE_SOURCE}")
	FILE(DOWNLOAD ${SQLITE_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-${SQLITE_VERSION}.tar.gz)
	execute_process(COMMAND cmake -E tar zxf ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-${SQLITE_VERSION}.tar.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
	FILE(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/sqlite)
	FILE(RENAME ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-${SQLITE_VERSION}/sqlite3.c ${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3.c)
	FILE(RENAME ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-${SQLITE_VERSION}/sqlite3.h ${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3.h)
	FILE(REMOVE ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-${SQLITE_VERSION}.tar.gz)
	FILE(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-${SQLITE_VERSION})
endif()

# JMdict
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/JMdict)
	MESSAGE(STATUS "Downloading JMdict from ${JMDICT_SOURCE}")
	FILE(DOWNLOAD ${JMDICT_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz)
	execute_process(COMMAND gunzip ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
endif()

# KanjiDict2
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml)
	MESSAGE(STATUS "Downloading KanjiDict2 ${KANJIDIC2_SOURCE}")
	FILE(DOWNLOAD ${KANJIDIC2_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml.gz)
	execute_process(COMMAND gunzip ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
endif()

# KanjiVG
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml)
	MESSAGE(STATUS "Downloading KanjiVG ${KANJIVG_SOURCE}")
	FILE(DOWNLOAD ${KANJIVG_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml.gz)
	execute_process(COMMAND gunzip ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
	FILE(RENAME ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml)
endif()

# JMdictDescs
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/JMdictDescs.cc)
	MESSAGE(STATUS "Generating JMdictDescs")	
	FILE(STRINGS ${CMAKE_SOURCE_DIR}/3rdparty/JMdict LINES REGEX ENTITY)
	foreach(LINE ${LINES})
		STRING(REGEX REPLACE ".*\\\"(.*)\\\">" "\\1" result "${LINE}" )
		FILE(APPEND ${CMAKE_SOURCE_DIR}/3rdparty/JMdictDescs.cc "QT_TRANSLATE_NOOP(\"JMdictLongDescs\", \"${result}\");\n")
	endforeach(LINE ${LINES})
endif()

# Which languages are enabled?
if(NOT DICT_LANG)
	set(DICT_LANG de en es fr ru)
endif()

# This will create databases for each choosen languages
add_custom_target(databases ALL)

# Create database targets for each language
foreach(LANG ${DICT_LANG})
	add_custom_target(database-${LANG} ALL DEPENDS ${CMAKE_BINARY_DIR}/jmdict-${LANG}.db ${CMAKE_BINARY_DIR}/kanjidic2-${LANG}.db)		
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/jmdict-${LANG}.db
	COMMAND build_jmdict_db -l${LANG} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/jmdict-${LANG}.db
	DEPENDS build_jmdict_db ${CMAKE_SOURCE_DIR}/3rdparty/JMdict)	
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/kanjidic2-${LANG}.db
	COMMAND build_kanji_db -l${LANG} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/kanjidic2-${LANG}.db
	DEPENDS build_kanji_db ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml)	
	add_dependencies(databases database-${LANG})	
endforeach(LANG ${DICT_LANG})

# i18n
FILE (GLOB TRANSLATIONS_FILES ${CMAKE_SOURCE_DIR}/i18n/*.ts)
QT4_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS_FILES})
add_custom_target (i18n ALL DEPENDS ${QM_FILES})

# docs
if(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
	MESSAGE("MultiMarkdown not found, doc generation will not be available")
else(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
	FILE (GLOB DOCUMENTATION_FILES ${CMAKE_SOURCE_DIR}/doc/*.txt)
	GENERATE_DOCS(DOC_FILES ${DOCUMENTATION_FILES})
	add_custom_target (docs ALL DEPENDS ${DOC_FILES})
endif(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")

# TODO does not triggers building of db files if they do not exist
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/jmdict.db ${CMAKE_CURRENT_BINARY_DIR}/kanjidic2.db DESTINATION share/tagainijisho PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

# Documentation installation
if (EXISTS ${CMAKE_SOURCE_DIR}/doc/manual.html)
	install(FILES ${CMAKE_SOURCE_DIR}/doc/manual.html DESTINATION share/tagainijisho/doc)
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/doc/images DESTINATION share/tagainijisho/doc)
endif()

add_subdirectory(src)

# Packaging
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Free Japanese dictionary and study assistant")
set(CPACK_PACKAGE_VENDOR "tagaini.net")
set(CPACK_PACKAGE_CONTACT http://www.tagaini.net)
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VERSION}")
set(CPACK_SOURCE_STRIP_FILES true)
set(CPACK_STRIP_FILES true)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README")

if(APPLE)
    set(CPACK_SYSTEM_NAME "MacOSX-Universal")
    set(CPACK_PACKAGE_FILE_NAME "Tagaini Jisho ${VERSION}")
    set(CPACK_PACKAGE_ICON "")
    set(CPACK_BUNDLE_NAME "Tagaini Jisho ${VERSION}")
    set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/images/appicon.icns")
    set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/scripts/Info.plist")
    set(CPACK_BUNDLE_STARTUP_COMMAND "")
endif(APPLE)

if(WIN32)
	
endif(WIN32)

# For source code, use .tar.gz on Unix, and .zip on Windows
if(UNIX)
	SET(CPACK_SOURCE_GENERATOR TGZ)
else(UNIX)
	SET(CPACK_SOURCE_GENERATOR ZIP)
endif(UNIX)

set(CPACK_SOURCE_IGNORE_FILES "~$" "\\\\.gitignore$" "^${PROJECT_SOURCE_DIR}/.git/" "^${PROJECT_SOURCE_DIR}/*.kdev4/" )

include(CPack)
