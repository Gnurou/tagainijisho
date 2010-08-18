set(QT_USE_QTSQL TRUE)
include(${QT_USE_FILE})

set(tagainijisho_sqlite_SRCS
sqlite3all.c
sqlite3ext.cc
qsql_sqlite.cc
_qsqlcachedresult.cc
)

qt4_wrap_cpp(tagainijisho_sqlite_MOC_SRCS
qsql_sqlite.h
)

include_directories(${QT_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/sqlite)
add_definitions(-DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_LOCKING_STYLE=0)
add_library(tagaini_sqlite STATIC ${tagainijisho_sqlite_SRCS} ${tagainijisho_sqlite_MOC_SRCS})

set(tagainijisho_newsqlite_SRCS
Error.cc
Connection.cc
Query.cc
sqlite3all.c
sqlite3ext.cc
# TODO Lame!
${CMAKE_SOURCE_DIR}/src/core/TextTools.cc
)

qt4_wrap_cpp(tagainijisho_newsqlite_MOC_SRCS
)

add_library(tagaini_newsqlite STATIC ${tagainijisho_newsqlite_SRCS} ${tagainijisho_newsqlite_MOC_SRCS})

if (BUILD_TESTS)
add_subdirectory(tests)
endif()

