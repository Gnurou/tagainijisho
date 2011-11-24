# Documentation generation
find_program(MULTI_MARKDOWN NAMES multimarkdown)

MACRO(GENERATE_DOCS _doc_files)
  FOREACH (_current_FILE ${ARGN})
    GET_FILENAME_COMPONENT(_abs_FILE ${_current_FILE} ABSOLUTE)
    GET_FILENAME_COMPONENT(doc ${_abs_FILE} NAME_WE)
    GET_SOURCE_FILE_PROPERTY(output_location ${_abs_FILE} OUTPUT_LOCATION)
    SET(doc "${CMAKE_CURRENT_SOURCE_DIR}/${doc}.html")
    ADD_CUSTOM_COMMAND(OUTPUT ${doc}
       COMMAND ${MULTI_MARKDOWN} ARGS ${_abs_FILE} -o ${doc} DEPENDS ${_abs_FILE}
    )
    SET(${_doc_files} ${${_doc_files}} ${doc})
  ENDFOREACH (_current_FILE)
ENDMACRO(GENERATE_DOCS)

if(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
	MESSAGE(STATUS "MultiMarkdown not found, doc generation will not be available")
	add_custom_target(docs)
else(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
	FILE (GLOB DOCUMENTATION_FILES
		manual.txt
		install.txt
		quickstart.txt
	)
	GENERATE_DOCS(DOC_FILES ${DOCUMENTATION_FILES})
	add_custom_target(docs ALL DEPENDS ${DOC_FILES})
endif(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
