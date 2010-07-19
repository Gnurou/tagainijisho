project(tagainijisho)
set(VERSION 0.2.5.90)
cmake_minimum_required(VERSION 2.6.0)
find_package(Qt4 4.5 REQUIRED)

# Global GCC options
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wnon-virtual-dtor -Wno-unused-parameter -fno-exceptions -fno-rtti")
endif(CMAKE_COMPILER_IS_GNUCC)

# Universal binaries for OSX
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	set(CMAKE_OSX_ARCHITECTURES "ppc;i386") 
endif()

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

#MESSAGE(STATUS "Downloading JMdict...")
#FILE(DOWNLOAD ftp://ftp.monash.edu.au/pub/nihongo/JMdict.gz ./boo.gz)
#add_custom_command(TARGET boo PRE_BUILD COMMAND gunzip ./boo.gz DEPENDS ./boo.gz)

FILE(STRINGS ${CMAKE_SOURCE_DIR}/3rdparty/JMdict LINES REGEX ENTITY)
foreach(LINE ${LINES})
	STRING(REGEX REPLACE ".*\\\"(.*)\\\">" "\\1" result "${LINE}" )
	FILE(APPEND ${CMAKE_SOURCE_DIR}/3rdparty/JMdictDescs.cc "QT_TRANSLATE_NOOP(\"JMdictLongDescs\", \"${result}\");\n")
endforeach(LINE ${LINES})

# Which languages are enabled?
if(NOT DICT_LANG)
	set(DICT_LANG de en es fr ru)
endif()

add_custom_target(databases ALL)

foreach(LANG ${DICT_LANG})
	add_custom_target(database-${LANG} ALL DEPENDS jmdict-${LANG}.db kanjidic2-${LANG}.db)		
	add_custom_command(OUTPUT jmdict-${LANG}.db
	COMMAND build_jmdict_db -l${LANG} ${CMAKE_SOURCE_DIR} jmdict-${LANG}.db
	DEPENDS build_jmdict_db ${CMAKE_SOURCE_DIR}/3rdparty/JMdict)	
	add_custom_command(OUTPUT kanjidic2-${LANG}.db
	COMMAND build_kanji_db -l${LANG} ${CMAKE_SOURCE_DIR} kanjidic2-${LANG}.db
	DEPENDS build_kanji_db ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml)	
	add_dependencies(databases database-${LANG})	
endforeach(LANG ${DICT_LANG})

# TODO does not triggers building of db files if they do not exist
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/jmdict.db ${CMAKE_CURRENT_BINARY_DIR}/kanjidic2.db DESTINATION share/tagainijisho PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

# Documentation installation
if (EXISTS ${CMAKE_SOURCE_DIR}/doc/manual.html)
	install(FILES ${CMAKE_SOURCE_DIR}/doc/manual.html DESTINATION share/tagainijisho/doc)
	install(DIRECTORY ${CMAKE_SOURCE_DIR}/doc/images DESTINATION share/tagainijisho/doc)
endif()

add_subdirectory(src)
