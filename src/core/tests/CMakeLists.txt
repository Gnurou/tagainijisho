set(QT_USE_QTSQL TRUE)
set(QT_USE_QTTEST TRUE)
include(${QT_USE_FILE})

set(tstring_tests_SRCS
TStringTests.cc
)
qt4_wrap_cpp(tstring_tests_MOC_SRCS
TStringTests.h
)

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

set(orderedtreedb_tests_SRCS
OrderedRBTreeDBTests.cc
)

qt4_wrap_cpp(orderedtreedb_tests_MOC_SRCS
OrderedRBTreeDBTests.h
)

include_directories(${QT_INCLUDE_DIR})
add_executable(tstringtests ${tstring_tests_SRCS} ${tstring_tests_MOC_SRCS})
target_link_libraries(tstringtests ${QT_LIBRARIES} tagaini_core tagaini_sqlite)

add_executable(liststests ${lists_tests_SRCS} ${lists_tests_MOC_SRCS})
target_link_libraries(liststests ${QT_LIBRARIES} tagaini_core tagaini_sqlite)

add_executable(orderedtreetests ${orderedtree_tests_SRCS} ${orderedtree_tests_MOC_SRCS})
target_link_libraries(orderedtreetests ${QT_LIBRARIES})

add_executable(orderedtreedbtests ${orderedtreedb_tests_SRCS} ${orderedtreedb_tests_MOC_SRCS})
target_link_libraries(orderedtreedbtests ${QT_LIBRARIES} tagaini_sqlite tagaini_core)
