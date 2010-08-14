# Packaging and install. Warning : not for faint of the heart
if(NOT DICT_LANG)
	set(DICT_LANG en)
endif()

set(CPACK_PACKAGE_NAME "Tagaini Jisho")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Free Japanese dictionary and study assistant")
set(CPACK_PACKAGE_VENDOR "tagaini.net")
set(CPACK_PACKAGE_CONTACT http://www.tagaini.net)
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.txt")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VERSION}-${DICT_LANG}")
# Source package should always be named the same regardless of the platform on which we build
set(CPACK_SOURCE_PACKAGE_FILE_NAME "tagainijisho-${VERSION}")
set(CPACK_SOURCE_STRIP_FILES true)
set(CPACK_STRIP_FILES true)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.txt")

# Disable all build type for safer operation. Per doc one should not set CPACK_GENERATOR directly
set(CPACK_BINARY_BUNDLE "OFF")
set(CPACK_BINARY_CYGWIN "OFF")
set(CPACK_BINARY_DEB "OFF")
set(CPACK_BINARY_DRAGNDROP "OFF")
set(CPACK_BINARY_NSIS "OFF")
set(CPACK_BINARY_OSXX11 "OFF")
set(CPACK_BINARY_PACKAGEMAKER "OFF")
set(CPACK_BINARY_RPM "OFF")
set(CPACK_BINARY_STGZ "OFF")
set(CPACK_BINARY_TBZ2 "OFF")
set(CPACK_BINARY_TGZ "OFF")
set(CPACK_BINARY_TZ "OFF")
set(CPACK_BINARY_ZIP "OFF")

set(CPACK_SOURCE_TBZ2 "OFF")
set(CPACK_SOURCE_TGZ "OFF")
set(CPACK_SOURCE_TZ "OFF")
set(CPACK_SOURCE_ZIP "OFF")

if(UNIX)
	set(CPACK_SOURCE_TGZ ON)
	if(APPLE)
		set(CPACK_BINARY_DRAGNDROP ON)
		set(CPACK_DMG_VOLUME_NAME "Tagaini Jisho")
		set(CPACK_DMG_DS_STORE "${CMAKE_CURRENT_SOURCE_DIR}/MacOS/DS_Store")
		set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/MacOS/background.png")
		set_target_properties(${tagaini_binary} PROPERTIES
			MACOSX_BUNDLE_BUNDLE_NAME "Tagaini Jisho"
			MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION}
			MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION}
			MACOSX_BUNDLE_LONG_VERSION_STRING ${VERSION}
			MACOSX_BUNDLE_GUI_IDENTIFIER "Tagaini Jisho"
			MACOSX_BUNDLE_ICON_FILE appicon.icns
		)
		
		set(ROOT_DIR "${tagaini_binary}.app/Contents")
		set(EXE_DIR "${ROOT_DIR}/MacOS")
		set(RESOURCES_DIR "${ROOT_DIR}/Resources")
		set(APPS "\${CMAKE_INSTALL_PREFIX}/${tagaini_binary}.app")
		set(DIRS ${QT_LIBRARY_DIRS})

		foreach(LANG ${DICT_LANG})
			install(FILES "${CMAKE_BINARY_DIR}/jmdict-${LANG}.db" DESTINATION ${EXE_DIR} COMPONENT Runtime)
			install(FILES "${CMAKE_BINARY_DIR}/kanjidic2-${LANG}.db" DESTINATION ${EXE_DIR} COMPONENT Runtime)
		endforeach(LANG ${LANG})
		
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/jmdict/detailed_jmdict.html ${CMAKE_SOURCE_DIR}/src/gui/jmdict/detailed_jmdict.css DESTINATION ${RESOURCES_DIR} COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/kanjidic2/detailed_kanjidic2.html ${CMAKE_SOURCE_DIR}/src/gui/kanjidic2/detailed_kanjidic2.css DESTINATION ${RESOURCES_DIR} COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/export_template.html DESTINATION ${RESOURCES_DIR} COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/detailed_default.html ${CMAKE_SOURCE_DIR}/src/gui/detailed_default.css DESTINATION ${RESOURCES_DIR} COMPONENT Runtime)
		
		file(GLOB i18nfiles ${CMAKE_BINARY_DIR}/i18n/*.qm)
		install(FILES ${i18nfiles} DESTINATION "${EXE_DIR}/i18n" COMPONENT Runtime)
		
		if(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
			MESSAGE(STATUS "Warning, docs will not be generated")
		else(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
			install(FILES ${CMAKE_SOURCE_DIR}/doc/manual.html DESTINATION ${EXE_DIR}/doc COMPONENT Runtime)
			install(DIRECTORY ${CMAKE_SOURCE_DIR}/doc/images DESTINATION ${EXE_DIR}/doc COMPONENT Runtime)
		endif(${MULTI_MARKDOWN} MATCHES "MULTI_MARKDOWN-NOTFOUND")
		
		install(CODE "file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${RESOURCES_DIR}/qt.conf\" \"[Paths]\nPlugins = PlugIns\nTranslations = Translations\n\")" COMPONENT Runtime)
		
		install(CODE "
			file(GLOB_RECURSE QTPLUGINS \"\${CMAKE_INSTALL_PREFIX}/${EXE_DIR}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
		include(BundleUtilities)
		fixup_bundle(\"${APPS}\" \"${QTPLUGINS}\" \"${DIRS}\")" COMPONENT Runtime)
		
	else(APPLE)
		set(CPACK_BINARY_DEB ON)
		foreach(LANG ${DICT_LANG})
			install(FILES ${CMAKE_BINARY_DIR}/jmdict-${LANG}.db DESTINATION share/tagainijisho PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
			install(FILES ${CMAKE_BINARY_DIR}/kanjidic2-${LANG}.db DESTINATION share/tagainijisho PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		endforeach(LANG ${LANG})
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/jmdict/detailed_jmdict.html ${CMAKE_SOURCE_DIR}/src/gui/jmdict/detailed_jmdict.css DESTINATION share/tagainijisho PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/kanjidic2/detailed_kanjidic2.html ${CMAKE_SOURCE_DIR}/src/gui/kanjidic2/detailed_kanjidic2.css DESTINATION share/tagainijisho PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/export_template.html DESTINATION share/tagainijisho PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/src/gui/detailed_default.html ${CMAKE_SOURCE_DIR}/src/gui/detailed_default.css DESTINATION share/tagainijisho PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		
		file(GLOB i18nfiles ${CMAKE_BINARY_DIR}/i18n/*.qm)
		install(FILES ${i18nfiles} DESTINATION share/tagainijisho/i18n PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		
		if (EXISTS ${CMAKE_SOURCE_DIR}/doc/manual.html)
			install(FILES ${CMAKE_SOURCE_DIR}/doc/manual.html DESTINATION share/tagainijisho/doc COMPONENT Runtime)
			install(DIRECTORY ${CMAKE_SOURCE_DIR}/doc/images DESTINATION share/tagainijisho/doc COMPONENT Runtime)
		else(EXISTS ${CMAKE_SOURCE_DIR}/doc/manual.html)
			MESSAGE(STATUS "Warning, docs were not generated")
		endif()
		
	endif(APPLE)
endif(UNIX)

if(WIN32)
	MESSAGE(STATUS "Not implemented")
endif(WIN32)

set(CPACK_SOURCE_IGNORE_FILES "~$" "\\\\.gitignore$" "^${PROJECT_SOURCE_DIR}/.git/" "^${PROJECT_SOURCE_DIR}/*.kdev4/" )

include(CPack)

