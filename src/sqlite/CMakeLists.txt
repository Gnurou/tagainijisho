include(${QT_USE_FILE})

set(SQLITE_MIN_VERSION 3070400)
set(SQLITE_LATEST_VERSION 3070701)
set(SQLITE_SOURCE http://www.sqlite.org/sqlite-amalgamation-${SQLITE_LATEST_VERSION}.zip)

# Embed SQLite even if the system version looks good?
option(EMBED_SQLITE "Embed SQLite even if a system version is present" OFF)

# Look for system SQLite
find_path(SQLITE_INCLUDE_DIR sqlite3.h)
find_library(SQLITE_LIBRARY
	NAMES sqlite sqlite3
	DOC "SQLite library"
)
if(SQLITE_INCLUDE_DIR AND SQLITE_LIBRARY AND NOT EMBED_SQLITE)
	message(STATUS "Found sqlite3: ${SQLITE_LIBRARY}")
elseif(NOT EMBED_SQLITE)
	message(STATUS "System sqlite3 not found, embedding it...")
	set(EMBED_SQLITE TRUE)
endif()

# Need to download SQLite?
if(EMBED_SQLITE AND NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3.c)
	message(STATUS "Downloading SQLite ${SQLITE_LATEST_VERSION} from ${SQLITE_SOURCE}")
	file(DOWNLOAD ${SQLITE_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-amalgamation-${SQLITE_LATEST_VERSION}.zip)
	execute_process(COMMAND cmake -E tar xvf ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-amalgamation-${SQLITE_LATEST_VERSION}.zip WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
	file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/sqlite)
	file(RENAME ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-amalgamation-${SQLITE_LATEST_VERSION}/sqlite3.c ${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3.c)
	file(RENAME ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-amalgamation-${SQLITE_LATEST_VERSION}/sqlite3.h ${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3.h)
	file(REMOVE ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-amalgamation-${SQLITE_LATEST_VERSION}.zip)
	file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-amalgamation-${SQLITE_LATEST_VERSION})
endif()

set(tagainijisho_sqlite_SRCS
SQLite.cc
Error.cc
Connection.cc
Query.cc
sqlite3ext.cc
# TODO Lame!
${CMAKE_SOURCE_DIR}/src/core/TextTools.cc
)

if(EMBED_SQLITE)
	include_directories(${CMAKE_SOURCE_DIR}/3rdparty/sqlite)
	set(tagainijisho_sqlite_SRCS
		${tagainijisho_sqlite_SRCS}
		${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3.c
	)
else()
	include_directories(${SQLITE_INCLUDE_DIR})
endif()

include_directories(${QT_INCLUDE_DIR})
add_definitions(-DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_LOCKING_STYLE=0 -DSQLITE_OMIT_DEPRECATED)
add_library(tagaini_sqlite STATIC ${tagainijisho_sqlite_SRCS} ${tagainijisho_sqlite_MOC_SRCS})
if(NOT EMBED_SQLITE)
	target_link_libraries(tagaini_sqlite sqlite3)
endif()

if (BUILD_TESTS)
add_subdirectory(tests)
endif()
