set(QT_USE_QTSQL TRUE)
set(QT_USE_QTXML TRUE)
set(QT_DONT_USE_QTGUI TRUE)

set(build_jmdict_db_SRCS
	JMdictParser.cc
	BuildJMdictDB.cc
)

include(${QT_USE_FILE})
add_executable(build_jmdict_db EXCLUDE_FROM_ALL ${build_jmdict_db_SRCS})
target_link_libraries(build_jmdict_db tagaini_sqlite tagaini_core ${QT_LIBRARIES})
