# JMdict
set(JMDICT_MIRRORS http://ftp.edrdg.org/pub/Nihongo/JMdict.gz
		   http://www.bcit-broadcast.com/monash/JMdict.gz
		   ftp://ftp.net.usf.edu/pub/ftp.monash.edu.au/pub/nihongo/JMdict.gz)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/JMdict)
	foreach(JMDICT_SOURCE ${JMDICT_MIRRORS})
		MESSAGE(STATUS "Downloading JMdict from ${JMDICT_SOURCE}")
		FILE(DOWNLOAD ${JMDICT_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz STATUS DLSTATUS SHOW_PROGRESS)
		LIST(GET DLSTATUS 0 ERRCODE)
		LIST(GET DLSTATUS 1 ERRMSG)
		if(NOT ERRCODE STREQUAL 0)
			MESSAGE(WARNING "Error downloading. " ${ERRMSG} " (Error Code " ${ERRCODE} ")")
		else()
			break()
		endif()
	endforeach()
	execute_process(COMMAND ${GZIP} -d ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty RESULT_VARIABLE result)
	if(NOT "${result}" STREQUAL 0)
		MESSAGE(FATAL_ERROR "Unable to run [${GZIP} -d ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz]. Reported error was [${result}]")
	endif(NOT "${result}" STREQUAL 0)
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

add_library(tagaini_core_jmdict STATIC ${tagainijisho_core_jmdict_SRCS})
target_link_libraries(tagaini_core_jmdict tagaini_sqlite Qt5::Core)

# Database builder
set(build_jmdict_db_SRCS
JMdictParser.cc
BuildJMdictDB.cc
../XmlParserHelper.cc
)

if(NOT CMAKE_CROSSCOMPILING)
add_executable(build_jmdict_db EXCLUDE_FROM_ALL ${build_jmdict_db_SRCS})
target_link_libraries(build_jmdict_db tagaini_sqlite Qt5::Core)

# Database target. Always build the english DB, other languages are optional.
set(ALL_LANGS "en")
foreach(LANG ${DICT_LANG})
	set(ALL_LANGS "${ALL_LANGS},${LANG}")
endforeach()
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/jmdict.db
	COMMAND build_jmdict_db -l${ALL_LANGS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS build_jmdict_db ${CMAKE_SOURCE_DIR}/3rdparty/JMdict)
add_custom_target(jmdict-db DEPENDS ${CMAKE_BINARY_DIR}/jmdict.db)
add_dependencies(databases jmdict-db)
endif(NOT CMAKE_CROSSCOMPILING)
