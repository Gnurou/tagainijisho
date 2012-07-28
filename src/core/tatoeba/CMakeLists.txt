# Tatoeba core
set(QT_DONT_USE_QTGUI TRUE)

set(TATOEBA_SENTENCES_SOURCE http://tatoeba.org/files/downloads/sentences.csv)
set(TATOEBA_LINKS_SOURCE http://tatoeba.org/files/downloads/links.csv)
set(TATOEBA_INDICES_SOURCE http://tatoeba.org/files/downloads/jpn_indices.csv)

# Tatoeba
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/tatoeba/sentences.csv)
FILE(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/tatoeba)
MESSAGE(STATUS "Downloading Tatoeba sentences from ${TATOEBA_SENTENCES_SOURCE}")
FILE(DOWNLOAD ${TATOEBA_SENTENCES_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/tatoeba/sentences.csv SHOW_PROGRESS)
endif()
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/tatoeba/links.csv)
FILE(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/tatoeba)
MESSAGE(STATUS "Downloading Tatoeba links from ${TATOEBA_LINKS_SOURCE}")
FILE(DOWNLOAD ${TATOEBA_LINKS_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/tatoeba/links.csv SHOW_PROGRESS)
endif()
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/tatoeba/jpn_indices.csv)
FILE(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/tatoeba)
MESSAGE(STATUS "Downloading Tatoeba indices from ${TATOEBA_INDICES_SOURCE}")
FILE(DOWNLOAD ${TATOEBA_INDICES_SOURCE} ${CMAKE_SOURCE_DIR}/3rdparty/tatoeba/jpn_indices.csv SHOW_PROGRESS)
endif()

set(tagainijisho_core_tatoeba_SRCS
TatoebaEntry.cc
TatoebaEntryLoader.cc
TatoebaPlugin.cc
)

set(tagainijisho_core_tatoeba_MOCS
TatoebaEntry.h
)

qt4_wrap_cpp(tagainijisho_core_tatoeba_MOC_SRCS ${tagainijisho_core_tatoeba_MOCS})
add_library(tagaini_core_tatoeba STATIC ${tagainijisho_core_tatoeba_MOC_SRCS} ${tagainijisho_core_tatoeba_SRCS})
add_dependencies(tagaini_core_tatoeba tagaini_core)

# Database builder
set(QT_USE_QTXML TRUE)
set(build_tatoeba_db_SRCS
BuildTatoebaDB.cc
)

include(${QT_USE_FILE})
add_executable(build_tatoeba_db EXCLUDE_FROM_ALL ${build_tatoeba_db_SRCS})
target_link_libraries(build_tatoeba_db tagaini_sqlite ${QT_LIBRARIES})

# Database target. Always build the english DB, other languages are optional.
set(ALL_LANGS "en")
foreach(LANG ${DICT_LANG})
	set(ALL_LANGS "${ALL_LANGS},${LANG}")
endforeach()
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/tatoeba.db
	COMMAND build_tatoeba_db -l${ALL_LANGS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
	DEPENDS build_tatoeba_db ${CMAKE_SOURCE_DIR}/3rdparty/tatoeba/sentences.csv ${CMAKE_SOURCE_DIR}/3rdparty/tatoeba/links.csv ${CMAKE_SOURCE_DIR}/3rdparty/tatoeba/jpn_indices.csv ${CMAKE_BINARY_DIR}/jmdict.db)
add_custom_target(tatoeba-db DEPENDS ${CMAKE_BINARY_DIR}/tatoeba.db)
add_dependencies(databases tatoeba-db)
