# JMdict core
set(QT_DONT_USE_QTGUI TRUE)

set(tagainijisho_core_jmdict_SRCS
JMdictEntry.cc
JMdictEntrySearcher.cc
JMdictEntryLoader.cc
JMdictPlugin.cc
)

set(tagainijisho_core_jmdict_MOCS
JMdictEntry.h
JMdictEntrySearcher.h
)

qt4_wrap_cpp(tagainijisho_core_jmdict_MOC_SRCS ${tagainijisho_core_jmdict_MOCS})
add_library(tagaini_core_jmdict STATIC ${tagainijisho_core_jmdict_MOC_SRCS} ${tagainijisho_core_jmdict_SRCS})
add_dependencies(tagaini_core_jmdict tagaini_core)

# Database builder
set(QT_USE_QTXML TRUE)
set(build_jmdict_db_SRCS
JMdictParser.cc
BuildJMdictDB.cc
../XmlParserHelper.cc
)

include(${QT_USE_FILE})
add_executable(build_jmdict_db EXCLUDE_FROM_ALL ${build_jmdict_db_SRCS})
target_link_libraries(build_jmdict_db tagaini_sqlite ${QT_LIBRARIES})
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
