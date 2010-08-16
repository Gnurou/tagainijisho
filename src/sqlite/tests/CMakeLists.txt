set(QT_USE_QTGUI FALSE)
set(QT_USE_QTSQL FALSE)
set(QT_USE_QTTEST TRUE)
include(${QT_USE_FILE})

set(sqlite_tests_SRCS
SQLiteTests.cc
)

qt4_wrap_cpp(sqlite_tests_MOC_SRCS
SQLiteTests.h
)

include_directories(${QT_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/sqlite)
add_executable(sqlitetests ${sqlite_tests_SRCS} ${sqlite_tests_MOC_SRCS})
target_link_libraries(sqlitetests tagaini_newsqlite ${QT_LIBRARIES})
