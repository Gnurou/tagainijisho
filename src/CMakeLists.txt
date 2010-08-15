include(${QT_USE_FILE})

# Make useful CMake definitions visible to the code
configure_file(tagaini_config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/tagaini_config.h)

# For config.h
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# For our own headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(sqlite)
add_subdirectory(core)
add_subdirectory(gui)
