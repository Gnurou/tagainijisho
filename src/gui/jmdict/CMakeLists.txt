# JMdict GUI
set(tagainijisho_gui_jmdict_SRCS
    JMdictEntryFormatter.cc JMdictPreferences.cc JMdictFilterWidget.cc
    JMdictGUIPlugin.cc JMdictYesNoTrainer.cc)

set(tagainijisho_gui_jmdict_UIS JMdictPreferences.ui)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})

qt5_wrap_ui(tagainijisho_gui_jmdict_UI_SRCS ${tagainijisho_gui_jmdict_UIS})
add_library(
  tagaini_gui_jmdict STATIC
  ${tagainijisho_gui_jmdict_UI_SRCS} ${tagainijisho_gui_jmdict_MOC_SRCS}
  ${tagainijisho_gui_jmdict_SRCS})
target_link_libraries(tagaini_gui_jmdict tagaini_gui)
