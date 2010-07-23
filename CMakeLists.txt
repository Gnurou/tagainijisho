project(tagainijisho)
set(VERSION 0.2.5.90)
set(PACKAGE tagainijisho)
set(SQLITE_VERSION 3.6.23.1)
set(SQLITE_SOURCE http://www.sqlite.org/sqlite-amalgamation-${SQLITE_VERSION}.tar.gz)
set(JMDICT_SOURCE ftp://ftp.monash.edu.au/pub/nihongo/JMdict.gz)
set(KANJIDIC2_SOURCE http://www.csse.monash.edu.au/~jwb/kanjidic2/kanjidic2.xml.gz)
set(KANJIVG_SOURCE http://kanjivg.tagaini.net/upload/Main/kanjivg-latest.xml.gz)

cmake_minimum_required(VERSION 2.8)

find_program(MULTI_MARKDOWN NAMES mmd2XHTML.pl)
find_program(GUNZIP NAMES gunzip)
find_package(Qt4 4.5 REQUIRED)

MACRO(GENERATE_DOCS _doc_files)
  FOREACH (_current_FILE ${ARGN})
    GET_FILENAME_COMPONENT(_abs_FILE ${_current_FILE} ABSOLUTE)
    GET_FILENAME_COMPONENT(doc ${_abs_FILE} NAME_WE)
    GET_SOURCE_FILE_PROPERTY(output_location ${_abs_FILE} OUTPUT_LOCATION)
    SET(doc "${CMAKE_CURRENT_SOURCE_DIR}/doc/${doc}.html")
    ADD_CUSTOM_COMMAND(OUTPUT ${doc}
       COMMAND ${MULTI_MARKDOWN} ARGS ${_abs_FILE} DEPENDS ${_abs_FILE}
    )
    SET(${_doc_files} ${${_doc_files}} ${doc})
  ENDFOREACH (_current_FILE)
ENDMACRO(GENERATE_DOCS)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/src/tagaini_config.h.cmake ${CMAKE_BINARY_DIR}/src/tagaini_config.h)

# Global GCC options
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wnon-virtual-dtor -Wno-unused-parameter -fno-exceptions -fno-rtti")
endif(CMAKE_COMPILER_IS_GNUCC)

# Debug options
option(DEBUG_ENTRIES_CACHE "Debug entries cache behavior" OFF)
option(DEBUG_PATHS "Debug files lookup" OFF)
option(DEBUG_DETAILED_VIEW "Debug detailed view output" OFF)

# Build tests suite?
option(BUILD_TESTS "Build tests suite" OFF)

# For config.h
include_directories(${CMAKE_BINARY_DIR}/src)

# For our own headers
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source code location
add_subdirectory(src)

# External resources fetching and generation

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/)
	FILE(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/)
endif()

# SQLITE
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3.c) 
	MESSAGE(STATUS "Downloading SQLite ${SQLITE_VERSION} from ${SQLITE_SOURCE}")
	FILE(DOWNLOAD ${SQLITE_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-${SQLITE_VERSION}.tar.gz)
	execute_process(COMMAND cmake -E tar zxf ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-${SQLITE_VERSION}.tar.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
	FILE(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/sqlite)
	FILE(RENAME ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-${SQLITE_VERSION}/sqlite3.c ${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3.c)
	FILE(RENAME ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-${SQLITE_VERSION}/sqlite3.h ${CMAKE_SOURCE_DIR}/3rdparty/sqlite/sqlite3.h)
	FILE(REMOVE ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-${SQLITE_VERSION}.tar.gz)
	FILE(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/3rdparty/sqlite-${SQLITE_VERSION})
endif()

if(${GUNZIP} MATCHES "GUNZIP-NOTFOUND")
	MESSAGE("Gunzip not found, building will not be possible ")
else(${GUNZIP} MATCHES "GUNZIP-NOTFOUND")
	# JMdict
	if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/JMdict)
		MESSAGE(STATUS "Downloading JMdict from ${JMDICT_SOURCE}")
		FILE(DOWNLOAD ${JMDICT_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz)
		execute_process(COMMAND gunzip ${CMAKE_SOURCE_DIR}/3rdparty/JMdict.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
	endif()

	# KanjiDict2
	if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml)
		MESSAGE(STATUS "Downloading KanjiDict2 ${KANJIDIC2_SOURCE}")
		FILE(DOWNLOAD ${KANJIDIC2_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml.gz)
		execute_process(COMMAND gunzip ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
	endif()

	# KanjiVG
	if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml)
		MESSAGE(STATUS "Downloading KanjiVG ${KANJIVG_SOURCE}")
		FILE(DOWNLOAD ${KANJIVG_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml.gz)
		execute_process(COMMAND gunzip ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml.gz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty)
		FILE(RENAME ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg-latest.xml ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml)
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
endif(${GUNZIP} MATCHES "GUNZIP-NOTFOUND")

# Which languages are enabled?
if(NOT DICT_LANG)
	set(DICT_LANG de en es fr ru)
endif()

# This will create databases for each choosen languages
add_custom_target(databases ALL)

# Create database targets for each language
foreach(LANG ${DICT_LANG})
	add_custom_target(database-${LANG} ALL DEPENDS ${CMAKE_BINARY_DIR}/jmdict-${LANG}.db ${CMAKE_BINARY_DIR}/kanjidic2-${LANG}.db)		
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/jmdict-${LANG}.db
	COMMAND build_jmdict_db -l${LANG} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/jmdict-${LANG}.db
	DEPENDS build_jmdict_db ${CMAKE_SOURCE_DIR}/3rdparty/JMdict)	
	add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/kanjidic2-${LANG}.db
	COMMAND build_kanji_db -l${LANG} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/kanjidic2-${LANG}.db
	DEPENDS build_kanji_db ${CMAKE_SOURCE_DIR}/3rdparty/kanjidic2.xml ${CMAKE_SOURCE_DIR}/3rdparty/kanjivg.xml)	
	add_dependencies(databases database-${LANG})	
endforeach(LANG ${DICT_LANG})

# i18n
FILE (GLOB TRANSLATIONS_FILES ${CMAKE_SOURCE_DIR}/i18n/*.ts)
QT4_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS_FILES})
add_custom_target (i18n ALL DEPENDS ${QM_FILES})

# docs
if(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
	MESSAGE("MultiMarkdown not found, doc generation will not be available")
else(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
	FILE (GLOB DOCUMENTATION_FILES ${CMAKE_SOURCE_DIR}/doc/*.txt)
	GENERATE_DOCS(DOC_FILES ${DOCUMENTATION_FILES})
	add_custom_target (docs ALL DEPENDS ${DOC_FILES})
endif(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")

# Uninstall
ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# Packaging
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Free Japanese dictionary and study assistant")
set(CPACK_PACKAGE_VENDOR "tagaini.net")
set(CPACK_PACKAGE_CONTACT http://www.tagaini.net)
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VERSION}")
set(CPACK_SOURCE_STRIP_FILES true)
set(CPACK_STRIP_FILES true)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")

# Disable all build type for safer operation. Per doc one should not set CPACK_GENERATOR directly
set(CPACK_BINARY_STGZ OFF)
set(CPACK_BINARY_TGZ OFF)
set(CPACK_BINARY_TZ OFF)
set(CPACK_BINARY_DEB OFF)
set(CPACK_BINARY_DRAGNDROP OFF)		
set(CPACK_BINARY_BUNDLE OFF)
set(CPACK_BINARY_NSIS OFF)
set(CPACK_BINARY_OSXX11 OFF)
set(CPACK_BINARY_PACKAGEMAKER OFF)
set(CPACK_BINARY_RPM OFF)
set(CPACK_SOURCE_TBZ2 OFF)
set(CPACK_SOURCE_TGZ OFF)
set(CPACK_SOURCE_TZ OFF)
set(CPACK_SOURCE_ZIP OFF)

if(UNIX)
	set(CPACK_SOURCE_TGZ ON)
	if(APPLE)
		set(CMAKE_OSX_ARCHITECTURES "ppc;i386") 		
		set(CPACK_BINARY_DRAGNDROP ON)
		set(MACOSX_BUNDLE_ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/images/appicon.icns) 
		set(plugin_dest_dir ${CMAKE_PROJECT_NAME}.app/Contents/MacOS)
  		set(qtconf_dest_dir ${CMAKE_PROJECT_NAME}.app/Contents/Resources)
		set(APPS "\${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}.app")
		SET_SOURCE_FILES_PROPERTIES(${MACOSX_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
		install(CODE "file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")" COMPONENT Runtime)
		install(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/PlugIns COMPONENT Runtime)
		install(DIRECTORY "${QT_PLUGINS_DIR}/imageformats" DESTINATION ${plugin_dest_dir}/PlugIns COMPONENT Runtime)
		install(FILES "${CMAKE_CURRENT_BINARY_DIR}/jmdict-fr.db" DESTINATION ../MacOS/ RENAME "jmdict.db" COMPONENT Runtime)
		install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kanjidic2-fr.db" DESTINATION ../MacOS/ RENAME "kanjidic2.db" COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/jmdict/detailed_jmdict.html ${CMAKE_SOURCE_DIR}/src/gui/jmdict/detailed_jmdict.css DESTINATION share/tagainijisho COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/kanjidic2/detailed_kanjidic2.html ${CMAKE_SOURCE_DIR}/src/gui/kanjidic2/detailed_kanjidic2.css DESTINATION share/tagainijisho COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/export_template.html DESTINATION share/tagainijisho COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/detailed_default.html ${CMAKE_SOURCE_DIR}/src/gui/detailed_default.css DESTINATION share/tagainijisho COMPONENT Runtime)
		if (EXISTS ${CMAKE_SOURCE_DIR}/doc/manual.html)
			install(FILES ${CMAKE_SOURCE_DIR}/doc/manual.html DESTINATION share/tagainijisho/doc COMPONENT Runtime)
			install(DIRECTORY ${CMAKE_SOURCE_DIR}/doc/images DESTINATION share/tagainijisho/doc COMPONENT Runtime)
		endif()
		file(GLOB i18nfiles ${CMAKE_CURRENT_BINARY_DIR}/*.qm)
		install(FILES ${i18nfiles} DESTINATION share/tagainijisho/i18n COMPONENT Runtime)
		INSTALL(CODE "
		    file(GLOB_RECURSE QTPLUGINS
		    \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/PlugIns/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
		    include(BundleUtilities)
		    fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
		    " COMPONENT Runtime)
	else(APPLE)
		if(NOT DATA_DIR)
			set(DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/tagainijisho")
		endif(NOT DATA_DIR)		
		set(CPACK_BINARY_DEB ON)
		set(CPACK_BINARY_NSIS ON)
		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/jmdict-fr.db DESTINATION share/tagainijisho RENAME "jmdict.db" PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kanjidic2-fr.db DESTINATION share/tagainijisho RENAME "kanjidic2.db" PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/jmdict/detailed_jmdict.html ${CMAKE_SOURCE_DIR}/src/gui/jmdict/detailed_jmdict.css DESTINATION share/tagainijisho PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/kanjidic2/detailed_kanjidic2.html ${CMAKE_SOURCE_DIR}/src/gui/kanjidic2/detailed_kanjidic2.css DESTINATION share/tagainijisho PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/export_template.html DESTINATION share/tagainijisho PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/detailed_default.html ${CMAKE_SOURCE_DIR}/src/gui/detailed_default.css DESTINATION share/tagainijisho PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		if (EXISTS ${CMAKE_SOURCE_DIR}/doc/manual.html)
			install(FILES ${CMAKE_SOURCE_DIR}/doc/manual.html DESTINATION share/tagainijisho/doc COMPONENT Runtime)
			install(DIRECTORY ${CMAKE_SOURCE_DIR}/doc/images DESTINATION share/tagainijisho/doc COMPONENT Runtime)
		endif()		
		file(GLOB i18nfiles ${CMAKE_CURRENT_BINARY_DIR}/*.qm)
		install(FILES ${i18nfiles} DESTINATION share/tagainijisho/i18n PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
	endif(APPLE)
endif(UNIX)

if(WIN32)
	MESSAGE(STATUS "Not implemented")
endif(WIN32)

set(CPACK_SOURCE_IGNORE_FILES "~$" "\\\\.gitignore$" "^${PROJECT_SOURCE_DIR}/.git/" "^${PROJECT_SOURCE_DIR}/*.kdev4/" )

include(CPack)
