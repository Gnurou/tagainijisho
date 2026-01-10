# JMdict GUI
set(tagainijisho_gui_jmdict_SRCS
    JMdictEntryFormatter.cc JMdictPreferences.cc JMdictFilterWidget.cc
    JMdictGUIPlugin.cc JMdictYesNoTrainer.cc)

set(tagainijisho_gui_jmdict_UIS JMdictPreferences.ui)

qt6_wrap_ui(tagainijisho_gui_jmdict_UI_SRCS ${tagainijisho_gui_jmdict_UIS})

add_library(
  tagaini_gui_jmdict STATIC
  ${tagainijisho_gui_jmdict_UI_SRCS} ${tagainijisho_gui_jmdict_MOC_SRCS}
  ${tagainijisho_gui_jmdict_SRCS})
target_link_libraries(tagaini_gui_jmdict tagaini_gui)
