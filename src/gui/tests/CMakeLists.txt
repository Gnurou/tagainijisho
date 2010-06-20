set(QT_USE_QTTEST TRUE)
include(${QT_USE_FILE})

set(gui_tests_SRCS
TemplateFillerTests.cc
)

qt4_wrap_cpp(gui_tests_MOC_SRCS
TemplateFillerTests.h
)

include_directories(${QT_INCLUDE_DIR})
add_executable(guitests ${gui_tests_SRCS} ${gui_tests_MOC_SRCS})
target_link_libraries(guitests tagaini_gui ${QT_LIBRARIES})
