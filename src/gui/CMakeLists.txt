set(QT_USE_QTSQL TRUE)
set(QT_USE_QTNETWORK TRUE)
include(${QT_USE_FILE})
add_subdirectory(jmdict)
add_subdirectory(kanjidic2)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)    

qt4_add_resources(tagainijisho_gui_QRC ${CMAKE_SOURCE_DIR}/tagainijisho.qrc)

set(tagainijisho_gui_SRCS
${tagainijisho_gui_QRC}
EntryFormatter.cc
BetterSpinBox.cc
HexSpinBox.cc
RelativeDateEdit.cc
ProxyPaintEngine.cc
BookletPrintEngine.cc
BookletPrinter.cc
KanjiValidator.cc
TagsDialogs.cc
FlowLayout.cc
MultiStackedWidget.cc
EntryMenu.cc
EditEntryNotesDialog.cc
SetsOrganizer.cc
UpdateChecker.cc
ClickableLabel.cc
SingleEntryView.cc
SearchBar.cc
SearchWidget.cc
ResultsList.cc
SmoothScroller.cc
ResultsView.cc
DetailedView.cc
ToolBarDetailedView.cc
TrainSettings.cc
YesNoTrainer.cc
ReadingTrainer.cc
PreferencesWindow.cc
MainWindow.cc
jmdict/JMdictEntryFormatter.cc
jmdict/JMdictPreferences.cc
jmdict/JMdictGUIPlugin.cc
jmdict/JMdictYesNoTrainer.cc
kanjidic2/Kanjidic2EntryFormatter.cc
kanjidic2/KanjiRenderer.cc
kanjidic2/KanjiPopup.cc
kanjidic2/KanjiPlayer.cc
kanjidic2/Kanjidic2Preferences.cc
kanjidic2/Kanjidic2GUIPlugin.cc
main.cc
)

qt4_wrap_cpp(tagainijisho_gui_MOC_SRCS
BetterSpinBox.h
ClickableLabel.h
DetailedView.h
EditEntryNotesDialog.h
EntryMenu.h
HexSpinBox.h
KanjiValidator.h
MainWindow.h
MultiStackedWidget.h
PreferencesWindow.h
ReadingTrainer.h
RelativeDateEdit.h
ResultsList.h
ResultsView.h
SearchBar.h
SearchWidget.h
SetsOrganizer.h
SingleEntryView.h
SmoothScroller.h
TagsDialogs.h
ToolBarDetailedView.h
TrainSettings.h
UpdateChecker.h
YesNoTrainer.h
jmdict/JMdictGUIPlugin.h
jmdict/JMdictPreferences.h
jmdict/JMdictYesNoTrainer.h
kanjidic2/Kanjidic2GUIPlugin.h
kanjidic2/Kanjidic2Preferences.h
kanjidic2/KanjiPlayer.h
kanjidic2/KanjiPopup.h
)

include_directories(${QT_INCLUDE_DIR})
add_executable(tagainijisho ${tagainijisho_gui_SRCS} ${tagainijisho_gui_MOC_SRCS})
target_link_libraries(tagainijisho tagaini_core tagaini_sqlite ${QT_LIBRARIES})
