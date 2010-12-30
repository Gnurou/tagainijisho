# Tatoeba core
set(QT_DONT_USE_QTGUI TRUE)

set(tagainijisho_core_tatoeba_SRCS
TatoebaEntry.cc
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
if (APPLE)
	set(extra_link_flags "-bind_at_load")
endif()

# Database targets for selected languages
#foreach(LANG ${DICT_LANG})
#add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/tatoeba-${LANG}.db
#COMMAND build_tatoeba_db -l${LANG} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}/tatoeba-${LANG}.db
#DEPENDS build_tatoeba_db ${CMAKE_SOURCE_DIR}/3rdparty/tatoeba)
#add_custom_target(tatoeba-${LANG} DEPENDS ${CMAKE_BINARY_DIR}/tatoeba-${LANG}.db)
#add_dependencies(databases-${LANG} tatoeba-${LANG})
#endforeach(LANG ${LANG})
