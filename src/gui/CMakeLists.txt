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
EntryListModel.cc
ResultsList.cc
BatchHandler.cc
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
MainWindow.cc
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

qt5_wrap_ui(tagainijisho_gui_UI_SRCS ${tagainijisho_gui_UIS})
qt5_add_resources(tagainijisho_gui_RC ${CMAKE_SOURCE_DIR}/tagainijisho.qrc)

add_library(tagaini_gui STATIC ${tagainijisho_gui_UI_SRCS} ${tagainijisho_gui_SRCS})
target_link_libraries(tagaini_gui tagaini_core tagaini_sqlite Qt5::Core Qt5::Widgets Qt5::PrintSupport Qt5::Network ${extra_link_flags})

# Build the main program
SET(ICON "")
if(APPLE)
	SET_SOURCE_FILES_PROPERTIES(appicon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	set(ICON ${CMAKE_CURRENT_SOURCE_DIR}/appicon.icns)
endif(APPLE)

set(tagainijisho_SRCS main.cc ${tagainijisho_gui_RC} ${ICON})

if (WIN32)
	if (MSVC)
		add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.obj COMMAND ${CMAKE_RC_COMPILER} -I${CMAKE_SOURCE_DIR}/src/gui -i${CMAKE_SOURCE_DIR}/src/gui/tagainijisho.rc  ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.res)
	else()
		add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.obj COMMAND ${CMAKE_RC_COMPILER} -I${CMAKE_SOURCE_DIR}/src/gui -i${CMAKE_SOURCE_DIR}/src/gui/tagainijisho.rc -o ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.obj)
		set(tagainijisho_SRCS ${tagainijisho_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/tagainijisho_rc.obj)
	endif()
endif()

if (APPLE)
	set(tagainijisho_SRCS ${tagainijisho_SRCS} ${MACOSX_BUNDLE_ICON_FILE})
endif()

add_executable(${tagaini_binary} MACOSX_BUNDLE WIN32 ${tagainijisho_SRCS})
target_link_libraries(${tagaini_binary} tagaini_gui_jmdict tagaini_gui_kanjidic2 tagaini_gui tagaini_core_jmdict tagaini_core_kanjidic2 tagaini_core tagaini_sqlite ${extra_link_flags})

# TODO not clean!
if(NOT WIN32)
	install(TARGETS ${tagaini_binary} BUNDLE DESTINATION . COMPONENT Runtime RUNTIME DESTINATION bin COMPONENT Runtime)
else(NOT WIN32)
	install(TARGETS ${tagaini_binary} BUNDLE DESTINATION . COMPONENT Runtime RUNTIME DESTINATION "." COMPONENT Runtime)
endif(NOT WIN32)

# This macro can copy system installed QT plugin to custom location in bundle.
macro(install_qt5_plugin _qt_plugin_name _qt_plugins_var _dest_dir)
	get_target_property(_qt_plugin_path "${_qt_plugin_name}" LOCATION)
	if(EXISTS "${_qt_plugin_path}")
		get_filename_component(_qt_plugin_file "${_qt_plugin_path}" NAME)
		get_filename_component(_qt_plugin_type "${_qt_plugin_path}" PATH)
		get_filename_component(_qt_plugin_type "${_qt_plugin_type}" NAME)
		set(_qt_plugin_dest "${_dest_dir}/${_qt_plugin_type}")
		install(FILES "${_qt_plugin_path}" DESTINATION "${_qt_plugin_dest}" COMPONENT Runtime)
		set(${_qt_plugins_var} "${${_qt_plugins_var}};\${CMAKE_INSTALL_PREFIX}/${_qt_plugin_dest}/${_qt_plugin_file}")
	else()
		message(FATAL_ERROR "QT plugin ${_qt_plugin_name} not found")
	endif()
endmacro()

# Determine location of QT libraries and frameworks
get_property(_Qt5_Core_LOCATION TARGET Qt5::Core PROPERTY LOCATION)
get_filename_component(Qt_LIB_DIR "${_Qt5_Core_LOCATION}" PATH)
if(APPLE)
	get_filename_component(Qt_LIB_DIR "${Qt_LIB_DIR}" PATH)
endif(APPLE)

if(APPLE)
	set(BUNDLE_DIR "${tagaini_binary}.app")
	install_qt5_plugin("Qt5::QCocoaIntegrationPlugin" plugins_to_fixup "${BUNDLE_DIR}/Contents/PlugIns")

	# Qt config
	install(CODE "file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${BUNDLE_DIR}/Contents/Resources/qt.conf\" \"[Paths]\nPlugins = PlugIns\nTranslations = i18n\n\")" COMPONENT Runtime)

	# Qt plugins & bundle
	install(CODE "
		include(BundleUtilities)
		set(BU_CHMOD_BUNDLE_ITEMS ON)
		fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/${BUNDLE_DIR}\" \"${plugins_to_fixup}\" \"${Qt_LIB_DIR}\")" COMPONENT Runtime)
endif(APPLE)

# If needed, add standard dependency to have identical build on all platforms
if (NOT ${tagaini_binary} MATCHES "tagainijisho")
	add_custom_target(tagainijisho DEPENDS ${tagaini_binary})
endif()
