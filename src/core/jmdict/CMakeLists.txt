set(tagainijisho_core_SRCS ${tagainijisho_core_SRCS}
jmdict/JMdictEntry.cc
jmdict/JMdictEntrySearcher.cc
jmdict/JMdictPlugin.cc
)

set(tagainijisho_core_MOCS ${tagainijisho_core_MOCS}
jmdict/JMdictEntry.h
jmdict/JMdictEntrySearcher.h
)

set(QT_USE_QTSQL TRUE)
set(QT_USE_QTXML TRUE)
set(QT_DONT_USE_QTGUI TRUE)

set(build_jmdict_db_SRCS
JMdictParser.cc
BuildJMdictDB.cc
)

# Database builder
include(${QT_USE_FILE})
add_executable(build_jmdict_db EXCLUDE_FROM_ALL ${build_jmdict_db_SRCS})
target_link_libraries(build_jmdict_db tagaini_sqlite tagaini_core ${QT_LIBRARIES})
if (APPLE)
	set(extra_link_flags "-bind_at_load")
endif()

# Database targets for selected languages
foreach(LANG ${DICT_LANG})
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/jmdict-${LANG}.db
	COMMAND build_jmdict_db -l${LANG} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/jmdict-${LANG}.db
	DEPENDS build_jmdict_db ${CMAKE_SOURCE_DIR}/3rdparty/JMdict)
	add_custom_target(jmdict-${LANG} DEPENDS ${CMAKE_BINARY_DIR}/jmdict-${LANG}.db)
	add_dependencies(databases-${LANG} jmdict-${LANG})
endforeach(LANG ${LANG})
