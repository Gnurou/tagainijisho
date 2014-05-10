set(QT_USE_QTNETWORK TRUE)

set(tagainijisho_gui_SRCS
TJSpinBox.cc
TJLineEdit.cc
EntryFormatter.cc
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
SavedSearchesOrganizer.cc
UpdateChecker.cc
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
BatchHandler.cc
EntriesViewHelper.cc
ResultsView.cc
ResultsViewWidget.cc
SearchWidget.cc
TemplateFiller.cc
DetailedView.cc
DetailedView2.cc
EntryListView.cc
EntryListWidget.cc
ToolBarDetailedView.cc
TrainSettings.cc
YesNoTrainer.cc
ReadingTrainer.cc
PreferencesWindow.cc
MainWindow.cc
)

set(tagainijisho_gui_MOCS
TJSpinBox.h
TJLineEdit.h
EntryFormatter.h
DetailedView.h
DetailedView2.h
EditEntryNotesDialog.h
EntryMenu.h
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
SavedSearchesOrganizer.h
SingleEntryView.h
SmoothScroller.h
ScrollBarSmoothScroller.h
TagsDialogs.h
ToolBarDetailedView.h
TrainSettings.h
UpdateChecker.h
YesNoTrainer.h
)

set(tagainijisho_gui_UIS
ReadingTrainer.ui
SavedSearchesOrganizer.ui
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
MainWindow.ui
)

# Add rules for jmdict and kanjidic components
add_subdirectory(jmdict)
add_subdirectory(kanjidic2)

qt5_wrap_cpp(tagainijisho_gui_MOC_SRCS ${tagainijisho_gui_MOCS})
qt5_wrap_ui(tagainijisho_gui_UI_SRCS ${tagainijisho_gui_UIS})
qt5_add_resources(tagainijisho_gui_RC ${CMAKE_SOURCE_DIR}/tagainijisho.qrc)

find_package(Qt5WebKit ${QTVERSION} REQUIRED)
find_package(Qt5WebKitWidgets ${QTVERSION} REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5WebKit_INCLUDE_DIRS})
include_directories(${Qt5WebKitWidgets_INCLUDE_DIRS})

add_library(tagaini_gui STATIC ${tagainijisho_gui_UI_SRCS} ${tagainijisho_gui_MOC_SRCS} ${tagainijisho_gui_SRCS})

# Build the main program
SET(ICON "")
if(APPLE)
	SET_SOURCE_FILES_PROPERTIES(appicon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	set(ICON ${CMAKE_CURRENT_SOURCE_DIR}/appicon.icns)
endif(APPLE)

set(tagainijisho_SRCS main.cc ${tagainijisho_gui_RC} ${ICON})

if (WIN32)
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.obj COMMAND ${CMAKE_RC_COMPILER} -I${CMAKE_SOURCE_DIR}/src/gui -i${CMAKE_SOURCE_DIR}/src/gui/tagainijisho.rc -o ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.obj)
	set(tagainijisho_SRCS ${tagainijisho_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.obj)
endif()

if (APPLE)
	set(tagainijisho_SRCS ${tagainijisho_SRCS} ${MACOSX_BUNDLE_ICON_FILE})
endif()

add_executable(${tagaini_binary} MACOSX_BUNDLE WIN32 ${tagainijisho_SRCS})
qt5_use_modules(${tagaini_binary} Widgets PrintSupport Network WebKit WebKitWidgets)
target_link_libraries(${tagaini_binary} tagaini_gui_jmdict tagaini_gui_kanjidic2 tagaini_gui tagaini_core_jmdict tagaini_core_kanjidic2 tagaini_core tagaini_sqlite ${extra_link_flags})
install(TARGETS ${tagaini_binary} BUNDLE DESTINATION . COMPONENT Runtime RUNTIME DESTINATION bin COMPONENT Runtime)

# If needed, add standard dependency to have identical build on all platforms
if (NOT ${tagaini_binary} MATCHES "tagainijisho")
	add_custom_target(tagainijisho DEPENDS ${tagaini_binary})
endif()

if (BUILD_TESTS)
	add_subdirectory(tests)
endif()
