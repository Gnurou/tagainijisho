project(tagainijisho)
set(VERSION 0.2.5)
cmake_minimum_required(VERSION 2.4.0)
find_package(Qt4 REQUIRED)

# Global GCC options
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-unused-parameter")
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

add_subdirectory(src)
