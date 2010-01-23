# TODO Download & extract SQLite, JMdict, kanjidic2 and kanjivg

project(tagainijisho)
set(VERSION 0.2.5)
cmake_minimum_required(VERSION 2.6.0)
find_package(Qt4 REQUIRED)

# Global GCC options
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-unused-parameter -fno-exceptions -fno-rtti")
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

configure_file(${CMAKE_SOURCE_DIR}/src/config.h.cmake ${CMAKE_BINARY_DIR}/src/config.h)
# For config.h
include_directories(${CMAKE_BINARY_DIR}/src)

# For our own headers
include_directories(${CMAKE_SOURCE_DIR}/src)

#MESSAGE(STATUS "Downloading JMdict...")
#FILE(DOWNLOAD ftp://ftp.monash.edu.au/pub/nihongo/JMdict.gz ./boo.gz)
#add_custom_command(TARGET boo PRE_BUILD COMMAND gunzip ./boo.gz DEPENDS ./boo.gz)

add_custom_command(OUTPUT kanjidic2.db
	COMMAND build_kanji_db ${CMAKE_SOURCE_DIR} kanjidic2.db
	DEPENDS build_kanji_db ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml)

add_custom_command(OUTPUT jmdict.db
	COMMAND build_jmdict_db ${CMAKE_SOURCE_DIR} jmdict.db
	DEPENDS build_jmdict_db ${CMAKE_SOURCE_DIR}/3rdparty/JMdict)

add_custom_target(databases DEPENDS jmdict.db kanjidic2.db)

add_subdirectory(src)
