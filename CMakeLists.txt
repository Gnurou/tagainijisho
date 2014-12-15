# The project name decides the naming pattern of many things - choose it according
# to the standard of the platform we run on.
if(APPLE)
	project("Tagaini Jisho")
else(APPLE)
	project("tagainijisho")
endif(APPLE)

# Set the program name to be the same as the project
set(tagaini_binary ${CMAKE_PROJECT_NAME})

set(VERSION 1.0.3)

cmake_minimum_required(VERSION 2.8.0)

find_package(Qt4 4.5 REQUIRED)
# FIXME only required when CMake downloads dictionary files. Not necessary for building from source package.
find_program(GUNZIP NAMES gunzip REQUIRED)

# Global GCC options
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wnon-virtual-dtor -Wno-unused-parameter -fno-exceptions -fno-rtti")
endif(CMAKE_COMPILER_IS_GNUCC)

# Add the default database lookup data path for Linux if not defined
if(UNIX AND NOT APPLE AND NOT DATA_DIR)
	set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/tagainijisho")
endif(UNIX AND NOT APPLE AND NOT DATA_DIR)

# 64 bits Intel binary with 10.6 compatibility 
if(APPLE)
	set(CMAKE_OSX_ARCHITECTURES "${ARCHS_STANDARD_64_BIT}")
	set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk")
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6")
	set(CMAKE_PREFIX_PATH "${CMAKE_OSX_SYSROOT}/usr")  
	set(CMAKE_FRAMEWORK_PATH "${CMAKE_OSX_SYSROOT}/Library/Frameworks:${CMAKE_OSX_SYSROOT}/System/")
	set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/pack/MacOS/")
endif(APPLE)
if(WIN32)
	set(extra_link_flags "-static-libgcc -static-libstdc++ -mwindows")
endif(WIN32)

# By default, enable all languages
if(NOT DICT_LANG)
	set(DICT_LANG "fr;de;es;ru;it;pt;th;tr")
endif()
# Set DICT_LANG to always appear in the cache
set(DICT_LANG ${DICT_LANG} CACHE STRING "Languages to use for the dictionary data (semicolon-separated 2-letter codes)")

# Debug options
option(DEBUG_ENTRIES_CACHE "Debug entries cache behavior" OFF)
option(DEBUG_PATHS "Debug files lookup" OFF)
option(DEBUG_DETAILED_VIEW "Debug detailed view output" OFF)
option(DEBUG_QUERIES "Debug SQL queries" OFF)
option(DEBUG_TRANSACTIONS "Debug database transactions" OFF)
option(DEBUG_LISTS "Debug lists (very slow)" OFF)

# Build tests suite?
option(BUILD_TESTS "Build tests suite" OFF)

# Databases helper targets
add_custom_target(databases ALL)

# i18n
add_subdirectory(i18n)

# Source code
add_subdirectory(src)

# Docs
add_subdirectory(doc)

# Packaging stuff
add_subdirectory(pack)

# External resources fetching and generation
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/)
	FILE(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/)
endif()

# Uninstall
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
