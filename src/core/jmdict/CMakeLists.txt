# JMdict core
set(QT_DONT_USE_QTGUI TRUE)

# JMdict
set(JMDICT_SOURCE ftp://ftp.monash.edu.au/pub/nihongo/JMdict.gz)
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/JMdict)
	MESSAGE(STATUS "Downloading JMdict from ${JMDICT_SOURCE}")
	FILE(DOWNLOAD ${JMDICT_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz)
	execute_process(COMMAND ${GUNZIP} ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
endif()

# JMdictDescs
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/JMdictDescs.cc)
	MESSAGE(STATUS "Generating JMdictDescs")
	FILE(STRINGS ${CMAKE_SOURCE_DIR}/3rdparty/JMdict LINES REGEX ENTITY)
	foreach(LINE ${LINES})
		STRING(REGEX REPLACE ".*\\\"(.*)\\\">" "\\1" result "${LINE}" )
		FILE(APPEND ${CMAKE_SOURCE_DIR}/3rdparty/JMdictDescs.cc "QT_TRANSLATE_NOOP(\"JMdictLongDescs\", \"${result}\");\n")
	endforeach(LINE ${LINES})
endif()

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

# Database target. Always build the english DB, other languages are optional.
set(ALL_LANGS "en")
foreach(LANG ${DICT_LANG})
	set(ALL_LANGS "${ALL_LANGS},${LANG}")
endforeach()
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/jmdict.db
	COMMAND build_jmdict_db -l${ALL_LANGS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
	DEPENDS build_jmdict_db ${CMAKE_SOURCE_DIR}/3rdparty/JMdict)
add_custom_target(jmdict-db DEPENDS ${CMAKE_BINARY_DIR}/jmdict.db)
add_dependencies(databases jmdict-db)
