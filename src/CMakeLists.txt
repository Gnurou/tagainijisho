include(${QT_USE_FILE})
set(tagainijisho_SRCS
	core/Entry.cc
	gui/main.cc
)

qt4_wrap_cpp(tagainijisho_MOC_SRCS
	core/Entry.h
)

include_directories(${QT_INCLUDE_DIR} ${QT_QTNETWORK_INCLUDE_DIR})
#target_link_libraries(${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES})
add_executable(tagainijisho ${tagainijisho_SRCS} ${tagainijisho_MOC_SRCS})

