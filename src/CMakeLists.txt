# Make useful CMake definitions visible to the code
configure_file(tagaini_config.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/tagaini_config.h)

# Interface library to provide common include paths to all targets
add_library(tagaini_common INTERFACE)
target_include_directories(
  tagaini_common
  INTERFACE ${CMAKE_CURRENT_BINARY_DIR} # For tagaini_config.h
            ${CMAKE_CURRENT_SOURCE_DIR} # For source headers
)

add_subdirectory(sqlite)
add_subdirectory(core)
add_subdirectory(gui)
