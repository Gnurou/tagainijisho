set(QT_DONT_USE_QTGUI TRUE)
include(${QT_USE_FILE})

set(SQLITE_MIN_VERSION "3007004")
set(SQLITE_BLACKLIST "3007007;3007008;3008000")
set(SQLITE_DOWNLOAD_VERSION "3080803")

set(SQLITE_SOURCE http://www.sqlite.org/2015/sqlite-amalgamation-${SQLITE_DOWNLOAD_VERSION}.zip)

option(SHARED_SQLITE_LIBRARY "Build the SQLite library as a shared library (loadable by SQLite's interpreter)" OFF)

# Embed SQLite even if the system version looks good?
option(EMBED_SQLITE "Embed SQLite even if a system version is present and valid" OFF)

# Force embedded version on Apple
if (APPLE)
	set (EMBED_SQLITE "ON")  
endif(APPLE)

# Look for system SQLite
find_path(SQLITE_INCLUDE_DIR sqlite3.h)
find_library(SQLITE_LIBRARY
	NAMES sqlite sqlite3
	DOC "SQLite library"
)
if(SQLITE_INCLUDE_DIR AND SQLITE_LIBRARY AND NOT EMBED_SQLITE)
	file(STRINGS ${SQLITE_INCLUDE_DIR}/sqlite3.h SQLITE_VERSION LIMIT_COUNT 1 REGEX "#define SQLITE_VERSION_NUMBER ")
	string(REGEX REPLACE "#define SQLITE_VERSION_NUMBER +(.+)" "\\1" SQLITE_VERSION ${SQLITE_VERSION})
	string(COMPARE GREATER "${SQLITE_VERSION}" "${SQLITE_MIN_VERSION}" FOUND_SQLITE)
	if(NOT FOUND_SQLITE)
		string(COMPARE EQUAL "${SQLITE_VERSION}" "${SQLITE_MIN_VERSION}" FOUND_SQLITE)
	endif()
	foreach(BANNED_VERSION ${SQLITE_BLACKLIST})
		string(COMPARE EQUAL "${SQLITE_VERSION}" "${BANNED_VERSION}" IS_BANNED)
		if (IS_BANNED)
			break()
		endif()
	endforeach()
	if(FOUND_SQLITE)
		if (IS_BANNED)
			message(STATUS "Found sqlite3 version ${SQLITE_VERSION}, but this version has been blacklisted. Embedding version ${SQLITE_DOWNLOAD_VERSION} instead")
			set(EMBED_SQLITE TRUE)
		else()
			message(STATUS "Found sqlite3 version ${SQLITE_VERSION} (using ${SQLITE_INCLUDE_DIR}/sqlite3.h)")
		endif()
	else()
		message(STATUS "Found sqlite3 version ${SQLITE_VERSION} but version ${SQLITE_MIN_VERSION} or greater required, embedding version ${SQLITE_DOWNLOAD_VERSION}")
		set(EMBED_SQLITE TRUE)
	endif()
elseif(NOT EMBED_SQLITE)
	message(STATUS "System sqlite3 not found, embedding version ${SQLITE_DOWNLOAD_VERSION}...")
	set(SQLITE_VERSION ${SQLITE_DOWNLOAD_VERSION}})
	set(EMBED_SQLITE TRUE)
endif()

# Need to download SQLite?
if(EMBED_SQLITE AND NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3.c)
	message(STATUS "Downloading SQLite ${SQLITE_DOWNLOAD_VERSION} from ${SQLITE_SOURCE}")
	file(DOWNLOAD ${SQLITE_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-amalgamation-${SQLITE_DOWNLOAD_VERSION}.zip SHOW_PROGRESS)
	execute_process(COMMAND cmake -E tar xvf ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-amalgamation-${SQLITE_DOWNLOAD_VERSION}.zip WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
	file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/sqlite)
	file(RENAME ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-amalgamation-${SQLITE_DOWNLOAD_VERSION}/sqlite3.c ${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3.c)
	file(RENAME ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-amalgamation-${SQLITE_DOWNLOAD_VERSION}/sqlite3.h ${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3.h)
	file(RENAME ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-amalgamation-${SQLITE_DOWNLOAD_VERSION}/sqlite3ext.h ${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3ext.h)
	file(REMOVE ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-amalgamation-${SQLITE_DOWNLOAD_VERSION}.zip)
	file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-amalgamation-${SQLITE_DOWNLOAD_VERSION})
elseif(EMBED_SQLITE)
	message(STATUS "Embedding SQLite ${SQLITE_DOWNLOAD_VERSION}")
endif()

set(tagainijisho_sqlite_SRCS
Error.cc
Connection.cc
Query.cc
sqlite3ext.cc
sqlite3mod.c
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
add_definitions(-DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_FTS3_PARENTHESIS -DSQLITE_ENABLE_LOCKING_STYLE=0 -DSQLITE_OMIT_DEPRECATED)

if(SHARED_SQLITE_LIBRARY)
	add_library(tagaini_sqlite SHARED ${tagainijisho_sqlite_SRCS} ${tagainijisho_sqlite_MOC_SRCS})
	target_link_libraries(tagaini_sqlite ${QT_LIBRARIES} ${extra_link_flags})
	install(TARGETS tagaini_sqlite BUNDLE DESTINATION . COMPONENT Runtime LIBRARY DESTINATION lib COMPONENT Runtime)
else()
	add_library(tagaini_sqlite STATIC ${tagainijisho_sqlite_SRCS} ${tagainijisho_sqlite_MOC_SRCS})
endif()
if(NOT EMBED_SQLITE)
	target_link_libraries(tagaini_sqlite sqlite3)
endif()

if(UNIX)
	target_link_libraries(tagaini_sqlite pthread)
	target_link_libraries(tagaini_sqlite dl)
endif(UNIX)

if(APPLE)
        set(DEST_DIR "${tagaini_binary}.app/Contents/MacOS")
else(APPLE)
        set(DEST_DIR "lib")
endif(APPLE)

if (BUILD_TESTS)
add_subdirectory(tests)
endif()
