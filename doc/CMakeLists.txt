# Documentation generation
find_program(MULTI_MARKDOWN NAMES mmd2XHTML.pl)

MACRO(GENERATE_DOCS _doc_files)
  FOREACH (_current_FILE ${ARGN})
    GET_FILENAME_COMPONENT(_abs_FILE ${_current_FILE} ABSOLUTE)
    GET_FILENAME_COMPONENT(doc ${_abs_FILE} NAME_WE)
    GET_SOURCE_FILE_PROPERTY(output_location ${_abs_FILE} OUTPUT_LOCATION)
    SET(doc "${CMAKE_CURRENT_SOURCE_DIR}/${doc}.html")
    ADD_CUSTOM_COMMAND(OUTPUT ${doc}
       COMMAND ${MULTI_MARKDOWN} ARGS ${_abs_FILE} DEPENDS ${_abs_FILE}
    )
    SET(${_doc_files} ${${_doc_files}} ${doc})
  ENDFOREACH (_current_FILE)
ENDMACRO(GENERATE_DOCS)

if(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
	MESSAGE(STATUS "MultiMarkdown not found, doc generation will not be available")
else(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
	FILE (GLOB DOCUMENTATION_FILES
		manual.txt
		install.txt
		quickstart.txt
	)
	GENERATE_DOCS(DOC_FILES ${DOCUMENTATION_FILES})
	add_custom_target (docs ALL DEPENDS ${DOC_FILES})
endif(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")

# Auto-screenshots
set(QT_USE_QTSQL TRUE)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTTEST TRUE)
include(${QT_USE_FILE})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_BINARY_DIR}/src)
qt4_add_resources(make_screenshots_RC ${CMAKE_SOURCE_DIR}/tagainijisho.qrc)

set(make_screenshots_SRCS
MakeScreenshots.cc
)

qt4_wrap_cpp(make_screenshots_MOC_SRCS
MakeScreenshots.h
)

include_directories(${QT_INCLUDE_DIR})
add_executable(makescreenshots ${make_screenshots_SRCS} ${make_screenshots_MOC_SRCS} ${make_screenshots_RC})
target_link_libraries(makescreenshots tagaini_gui_jmdict tagaini_gui_kanjidic2 tagaini_gui tagaini_core_jmdict tagaini_core_kanjidic2 tagaini_core tagaini_sqlite ${QT_LIBRARIES})
