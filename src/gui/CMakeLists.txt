set(QT_USE_QTSQL TRUE)
set(QT_USE_QTNETWORK TRUE)
include(${QT_USE_FILE})
add_subdirectory(jmdict)
add_subdirectory(kanjidic2)

qt4_add_resources(tagainijisho_gui_RC ${CMAKE_SOURCE_DIR}/tagainijisho.qrc)

set(tagainijisho_gui_SRCS
${tagainijisho_gui_RC}
EntryFormatter.cc
BetterSpinBox.cc
HexSpinBox.cc
RelativeDateEdit.cc
ProxyPaintEngine.cc
BookletPrintEngine.cc
BookletPrinter.cc
EntriesPrinter.cc
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
SearchFilterWidget.cc
EntryTypeFilterWidget.cc
TextFilterWidget.cc
StudyFilterWidget.cc
JLPTFilterWidget.cc
TagsFilterWidget.cc
NotesFilterWidget.cc
SearchBuilder.cc
SmoothScroller.cc
EntryDelegate.cc
EntriesViewHelper.cc
ResultsView.cc
ResultsViewWidget.cc
DetailedView.cc
EntryListView.cc
EntryListWidget.cc
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
kanjidic2/ComponentSearchWidget.cc
kanjidic2/Kanjidic2Preferences.cc
kanjidic2/Kanjidic2GUIPlugin.cc
main.cc
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.obj COMMAND i486-mingw32-windres -I${CMAKE_SOURCE_DIR} -i${CMAKE_SOURCE_DIR}/tagainijisho.rc -o ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.obj)
	set(tagainijisho_gui_SRCS ${tagainijisho_gui_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.obj)
	set(extra_link_flags "-mwindows")
endif()

qt4_wrap_cpp(tagainijisho_gui_MOC_SRCS
BetterSpinBox.h
ClickableLabel.h
DetailedView.h
EditEntryNotesDialog.h
EntryMenu.h
HexSpinBox.h
EntriesPrinter.h
KanjiValidator.h
MainWindow.h
MultiStackedWidget.h
PreferencesWindow.h
ReadingTrainer.h
RelativeDateEdit.h
EntryDelegate.h
EntriesViewHelper.h
ResultsView.h
ResultsViewWidget.h
EntryListView.h
EntryListWidget.h
SearchFilterWidget.h
EntryTypeFilterWidget.h
TextFilterWidget.h
StudyFilterWidget.h
JLPTFilterWidget.h
TagsFilterWidget.h
NotesFilterWidget.h
SearchBuilder.h
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
kanjidic2/ComponentSearchWidget.h
kanjidic2/KanjiPopup.h
)

qt4_wrap_ui(tagainijisho_gui_UI_SRCS
ReadingTrainer.ui
SetsOrganizer.ui
PreferencesWindow.ui
TrainSettings.ui
AboutDialog.ui
ResultsViewWidget.ui
EntryListWidget.ui
EntryDelegatePreferences.ui
DetailedViewPreferences.ui
ResultsViewPreferences.ui
ListsViewPreferences.ui
GeneralPreferences.ui
MainWindow.ui
jmdict/JMdictPreferences.ui
kanjidic2/KanjiPopup.ui
kanjidic2/ComponentSearchWidget.ui
kanjidic2/Kanjidic2Preferences.ui
)

include_directories(${QT_INCLUDE_DIR})
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
add_executable(tagainijisho MACOSX_BUNDLE ${tagainijisho_gui_SRCS} ${tagainijisho_gui_MOC_SRCS} ${tagainijisho_gui_UI_SRCS})
else()
add_executable(tagainijisho ${tagainijisho_gui_SRCS} ${tagainijisho_gui_MOC_SRCS} ${tagainijisho_gui_UI_SRCS})
install(TARGETS tagainijisho RUNTIME DESTINATION bin)
endif()
target_link_libraries(tagainijisho tagaini_core tagaini_sqlite ${QT_LIBRARIES} ${extra_link_flags})

