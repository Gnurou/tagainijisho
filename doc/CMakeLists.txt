# Documentation generation
find_program(MULTI_MARKDOWN NAMES multimarkdown)

macro(GENERATE_DOCS _doc_files)
  foreach(_current_FILE ${ARGN})
    get_filename_component(_abs_FILE ${_current_FILE} ABSOLUTE)
    get_filename_component(doc ${_abs_FILE} NAME_WE)
    get_source_file_property(output_location ${_abs_FILE} OUTPUT_LOCATION)
    set(doc "${CMAKE_CURRENT_SOURCE_DIR}/${doc}.html")
    add_custom_command(
      OUTPUT ${doc}
      COMMAND ${MULTI_MARKDOWN} ARGS ${_abs_FILE} > ${doc}
      DEPENDS ${_abs_FILE})
    set(${_doc_files} ${${_doc_files}} ${doc})
  endforeach(_current_FILE)
endmacro(GENERATE_DOCS)

if(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
  message(
    STATUS "MultiMarkdown not found, doc generation will not be available")
  add_custom_target(docs)
else(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
  file(GLOB DOCUMENTATION_FILES manual.txt install.txt quickstart.txt)
  generate_docs(DOC_FILES ${DOCUMENTATION_FILES})
  add_custom_target(docs ALL DEPENDS ${DOC_FILES})
endif(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
