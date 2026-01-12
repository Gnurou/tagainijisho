cmake_minimum_required(VERSION 3.16)
cmake_policy(VERSION 3.16)
if(POLICY CMP0177)
  cmake_policy(SET CMP0177 NEW)
endif()

# macOS deployment target (these variables should be set before any project command)
# Qt6 requires 10.15+ due to std::filesystem usage in headers
if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
endif(APPLE)

project("tagainijisho")

# Set the program name to be the same as the project
set(tagaini_binary ${CMAKE_PROJECT_NAME})

set(VERSION 1.2.2)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Widgets PrintSupport Network LinguistTools)

# FIXME only required when CMake downloads dictionary files. Not necessary for
# building from source package.
find_program(GZIP NAMES gzip REQUIRED)

# Global compiler options - applied to all targets via add_compile_options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wnon-virtual-dtor -Wno-unused-parameter)
endif()

# Add the default database lookup data path for Linux if not defined
if(UNIX
   AND NOT APPLE
   AND NOT DATA_DIR)
  set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/tagainijisho")
endif(
  UNIX
  AND NOT APPLE
  AND NOT DATA_DIR)

if(WIN32)
  if(!MSVC)
    set(extra_link_flags "-static-libgcc -static-libstdc++ -mwindows")
  endif()
endif(WIN32)

# By default, enable all languages
if(NOT DICT_LANG)
  set(DICT_LANG "fr;de;es;ru;it;pt;th;tr")
endif()
# Set DICT_LANG to always appear in the cache
set(DICT_LANG
    ${DICT_LANG}
    CACHE
      STRING
      "Languages to use for the dictionary data (semicolon-separated 2-letter codes)"
)

# Debug options
option(DEBUG_ENTRIES_CACHE "Debug entries cache behavior" OFF)
option(DEBUG_PATHS "Debug files lookup" OFF)
option(DEBUG_DETAILED_VIEW "Debug detailed view output" OFF)
option(DEBUG_QUERIES "Debug SQL queries" OFF)
option(DEBUG_TRANSACTIONS "Debug database transactions" OFF)
option(DEBUG_LISTS "Debug lists (very slow)" OFF)

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
  file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/)
endif()

# Uninstall
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P
                            "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# Code formatting
add_custom_target(
  format
  find
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    -name "*.cc"
    -o
    -name "*.h"
  | grep -v DictionaryDescriptions
  | xargs clang-format -i
)
