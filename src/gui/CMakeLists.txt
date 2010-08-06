# Which languages are enabled?
set(QT_USE_QTSQL TRUE)
set(QT_USE_QTNETWORK TRUE)
include(${QT_USE_FILE})

set(tagainijisho_gui_SRCS
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
ScrollBarSmoothScroller.cc
EntryDelegate.cc
EntriesViewHelper.cc
ResultsView.cc
ResultsViewWidget.cc
SearchWidget.cc
TemplateFiller.cc
DetailedView.cc
EntryListView.cc
EntryListWidget.cc
ToolBarDetailedView.cc
TrainSettings.cc
YesNoTrainer.cc
ReadingTrainer.cc
PreferencesWindow.cc
ManualBrowser.cc
MainWindow.cc
)

set(tagainijisho_gui_MOCS
BetterSpinBox.h
ClickableLabel.h
EntryFormatter.h
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
SearchWidget.h
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
ScrollBarSmoothScroller.h
TagsDialogs.h
ToolBarDetailedView.h
TrainSettings.h
UpdateChecker.h
YesNoTrainer.h
ManualBrowser.h
)

set(tagainijisho_gui_UIS
ReadingTrainer.ui
SetsOrganizer.ui
PreferencesWindow.ui
TrainSettings.ui
AboutDialog.ui
ResultsViewWidget.ui
SearchWidget.ui
EntryListWidget.ui
EntryDelegatePreferences.ui
DetailedViewPreferences.ui
ResultsViewPreferences.ui
ListsViewPreferences.ui
GeneralPreferences.ui
DataPreferences.ui
ManualBrowser.ui
MainWindow.ui
)

# Add rules for jmdict and kanjidic components
add_subdirectory(jmdict)
get_directory_property(tagainijisho_gui_SRCS DIRECTORY "jmdict" DEFINITION tagainijisho_gui_SRCS)
get_directory_property(tagainijisho_gui_MOCS DIRECTORY "jmdict" DEFINITION tagainijisho_gui_MOCS)
get_directory_property(tagainijisho_gui_UIS DIRECTORY "jmdict" DEFINITION tagainijisho_gui_UIS)
add_subdirectory(kanjidic2)
get_directory_property(tagainijisho_gui_SRCS DIRECTORY "kanjidic2" DEFINITION tagainijisho_gui_SRCS)
get_directory_property(tagainijisho_gui_MOCS DIRECTORY "kanjidic2" DEFINITION tagainijisho_gui_MOCS)
get_directory_property(tagainijisho_gui_UIS DIRECTORY "kanjidic2" DEFINITION tagainijisho_gui_UIS)

qt4_wrap_cpp(tagainijisho_gui_MOC_SRCS ${tagainijisho_gui_MOCS})
qt4_wrap_ui(tagainijisho_gui_UI_SRCS ${tagainijisho_gui_UIS})
qt4_add_resources(tagainijisho_gui_RC ${CMAKE_SOURCE_DIR}/tagainijisho.qrc)

SET(ICON "")
if(APPLE)
	set(MACOSX_BUNDLE_ICON_FILE appicon.icns) 
	SET_SOURCE_FILES_PROPERTIES(appicon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	set(ICON ${CMAKE_CURRENT_SOURCE_DIR}/appicon.icns)
endif(APPLE)

set(tagainijisho_SRCS main.cc ${tagainijisho_gui_RC} ${ICON})

if (WIN32)
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.obj COMMAND i486-mingw32-windres -I${CMAKE_SOURCE_DIR}/src/gui -i${CMAKE_SOURCE_DIR}/src/gui/tagainijisho.rc -o ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.obj)
	set(tagainijisho_SRCS ${tagainijisho_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.obj)
	set(extra_link_flags "-static-libgcc -static-libstdc++ -mwindows")
endif()

include_directories(${QT_INCLUDE_DIR})

add_library(tagaini_gui STATIC ${tagainijisho_gui_SRCS} ${tagainijisho_gui_MOC_SRCS} ${tagainijisho_gui_UI_SRCS})

if (APPLE)
	set(tagainijisho_SRCS ${tagainijisho_SRCS} ${MACOSX_BUNDLE_ICON_FILE})
	set(extra_link_flags "-bind_at_load")
endif()

add_executable(${CMAKE_PROJECT_NAME} MACOSX_BUNDLE WIN32 ${tagainijisho_SRCS})

target_link_libraries(${CMAKE_PROJECT_NAME} tagaini_gui tagaini_core tagaini_sqlite ${QT_LIBRARIES} ${extra_link_flags})

install(TARGETS ${CMAKE_PROJECT_NAME} BUNDLE DESTINATION . COMPONENT Runtime RUNTIME DESTINATION bin COMPONENT Runtime)

if (BUILD_TESTS)
	add_subdirectory(tests)
endif()
