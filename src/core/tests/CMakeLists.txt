set(QT_USE_QTSQL TRUE)
set(QT_USE_QTTEST TRUE)
include(${QT_USE_FILE})

set(lists_tests_SRCS
ListsTests.cc
)

qt4_wrap_cpp(lists_tests_MOC_SRCS
ListsTests.h
)

include_directories(${QT_INCLUDE_DIR})
add_executable(liststests ${lists_tests_SRCS} ${lists_tests_MOC_SRCS})
target_link_libraries(liststests tagaini_core ${QT_LIBRARIES})
