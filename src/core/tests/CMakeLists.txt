set(QT_USE_QTSQL TRUE)
set(QT_USE_QTTEST TRUE)
include(${QT_USE_FILE})

set(lists_tests_SRCS
ListsTests.cc
)

qt4_wrap_cpp(lists_tests_MOC_SRCS
ListsTests.h
)

set(orderedtree_tests_SRCS
OrderedRBTreeTests.cc
)

qt4_wrap_cpp(orderedtree_tests_MOC_SRCS
OrderedRBTreeTests.h
)

include_directories(${QT_INCLUDE_DIR})
add_executable(liststests ${lists_tests_SRCS} ${lists_tests_MOC_SRCS})
target_link_libraries(liststests tagaini_core tagaini_newsqlite ${QT_LIBRARIES})

add_executable(orderedtreetests ${orderedtree_tests_SRCS} ${orderedtree_tests_MOC_SRCS})
target_link_libraries(orderedtreetests ${QT_LIBRARIES})
