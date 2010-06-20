set(QT_USE_QTSQL TRUE)
include(${QT_USE_FILE})

set(tagainijisho_sqlite_SRCS
sqlite3all.c
sqlite3ext.cc
qsql_sqlite.cc
_qsqlcachedresult.cc
Error.cc
Connection.cc
Query.cc
)

qt4_wrap_cpp(tagainijisho_sqlite_MOC_SRCS
qsql_sqlite.h
Connection.h
Query.h
)

include_directories(${QT_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/sqlite)
add_definitions(-DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_LOCKING_STYLE=0)
add_library(tagaini_sqlite STATIC ${tagainijisho_sqlite_SRCS} ${tagainijisho_sqlite_MOC_SRCS})

add_subdirectory(tests)
