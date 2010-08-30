# The project name decides the naming pattern of many things - choose it according
# to the standard of the platform we run on.
if(APPLE)
	project("Tagaini Jisho")
else(APPLE)
	project("tagainijisho")
endif(APPLE)
# Set the program name to be the same as the project
set(tagaini_binary ${CMAKE_PROJECT_NAME})

set(VERSION 0.2.6.2)
set(SQLITE_VERSION 3.7.2)
set(SQLITE_SOURCE http://www.sqlite.org/sqlite-amalgamation-${SQLITE_VERSION}.tar.gz)
set(JMDICT_SOURCE ftp://ftp.monash.edu.au/pub/nihongo/JMdict.gz)
set(KANJIDIC2_SOURCE http://www.csse.monash.edu.au/~jwb/kanjidic2/kanjidic2.xml.gz)
set(KANJIVG_SOURCE http://kanjivg.tagaini.net/upload/Main/kanjivg-latest.xml.gz)

cmake_minimum_required(VERSION 2.8.0)

find_package(Qt4 4.6 REQUIRED)
find_program(GUNZIP NAMES gunzip)

# Global GCC options
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wnon-virtual-dtor -Wno-unused-parameter -fno-exceptions -fno-rtti")
endif(CMAKE_COMPILER_IS_GNUCC)

# Add the default database lookup data path for Linux if not defined
if(UNIX AND NOT APPLE AND NOT DATA_DIR)
	set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/tagainijisho")
endif(UNIX AND NOT APPLE AND NOT DATA_DIR)

# Universal binary 32bits with 10.4 compatibility 
if(APPLE)
	set(CMAKE_OSX_ARCHITECTURES "ppc;i386")
	set(CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX10.4u.sdk)
	set(CMAKE_C_COMPILER "gcc-4.0")
	set(CMAKE_CXX_COMPILER "g++-4.0")
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.4)
endif(APPLE)

# Which languages are enabled?
if(NOT DICT_LANG)
	set(DICT_LANG en)
endif()
# Set DICT_LANG to always appear in the cache
set(DICT_LANG ${DICT_LANG} CACHE STRING "Languages to use for the dictionary data (semicolon-separated 2-letter codes)")

# Debug options
option(DEBUG_ENTRIES_CACHE "Debug entries cache behavior" OFF)
option(DEBUG_PATHS "Debug files lookup" OFF)
option(DEBUG_DETAILED_VIEW "Debug detailed view output" OFF)
option(DEBUG_QUERIES "Debug SQL queries" OFF)

# Build tests suite?
option(BUILD_TESTS "Build tests suite" OFF)

# Databases helper targets
add_custom_target(databases ALL)
foreach(LANG ${DICT_LANG})
	add_custom_target(databases-${LANG} ALL)
	add_dependencies(databases databases-${LANG})
endforeach(LANG ${DICT_LANG})

# Source code
add_subdirectory(src)

# i18n
add_subdirectory(i18n)

# Docs
add_subdirectory(doc)

# Packaging stuff
add_subdirectory(pack)

# External resources fetching and generation

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

if(${GUNZIP} MATCHES "GUNZIP-NOTFOUND")
	MESSAGE("Gunzip not found, building will not be possible ")
else(${GUNZIP} MATCHES "GUNZIP-NOTFOUND")
	# JMdict
	if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/JMdict)
		MESSAGE(STATUS "Downloading JMdict from ${JMDICT_SOURCE}")
		FILE(DOWNLOAD ${JMDICT_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz)
		execute_process(COMMAND ${GUNZIP} ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
	endif()

	# Kanjidic2
	if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml)
		MESSAGE(STATUS "Downloading Kanjidic2 ${KANJIDIC2_SOURCE}")
		FILE(DOWNLOAD ${KANJIDIC2_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml.gz)
		execute_process(COMMAND ${GUNZIP} ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
	endif()

	# KanjiVG
	if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml)
		MESSAGE(STATUS "Downloading KanjiVG ${KANJIVG_SOURCE}")
		FILE(DOWNLOAD ${KANJIVG_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml.gz)
		execute_process(COMMAND ${GUNZIP} ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
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
endif(${GUNZIP} MATCHES "GUNZIP-NOTFOUND")

# Uninstall
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
