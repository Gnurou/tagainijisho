# Packaging and install. Warning : not for faint of the heart
set(CPACK_PACKAGE_NAME "Tagaini Jisho")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Free Japanese dictionary and study assistant")
set(CPACK_PACKAGE_VENDOR "tagaini.net")
set(CPACK_PACKAGE_CONTACT http://www.tagaini.net)
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VERSION}")
# Source package should always be named the same regardless of the platform on which we build
set(CPACK_SOURCE_PACKAGE_FILE_NAME "tagainijisho-${VERSION}")
set(CPACK_SOURCE_STRIP_FILES true)
set(CPACK_STRIP_FILES true)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

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
			MACOSX_BUNDLE_GUI_IDENTIFIER "net.tagaini.TagainiJisho"
			MACOSX_BUNDLE_ICON_FILE appicon.icns
			MACOSX_BUNDLE_COPYRIGHT "© 2008 - 2013, Alexandre Courbot."
		)
		
		set(ROOT_DIR "${tagaini_binary}.app/Contents")
		set(EXE_DIR "${ROOT_DIR}/MacOS")
		set(DB_DIR ${EXE_DIR})
		set(I18N_DIR "${EXE_DIR}/i18n")
		set(DOC_DIR "${EXE_DIR}/doc")
		set(RESOURCES_DIR "${ROOT_DIR}/Resources")
		set(APPS "\${CMAKE_INSTALL_PREFIX}/${tagaini_binary}.app")

		# Extra Qt translations
		foreach(LANG ${UI_LANG})
			set(QMFILE "/Developer/Applications/Qt/translations/qt_${LANG}.qm")
			if(EXISTS ${QMFILE})
				install(FILES ${QMFILE} DESTINATION "${ROOT_DIR}/Translations" COMPONENT Runtime)
			endif(EXISTS ${QMFILE})
		endforeach(LANG ${UI_LANG})
	
		# Qt config
		install(CODE "file(WRITE \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${RESOURCES_DIR}/qt.conf\" \"[Paths]\nPlugins = PlugIns\nTranslations = Translations\n\")" COMPONENT Runtime)
		
		# Qt plugins
		install(CODE "
			file(GLOB_RECURSE QTPLUGINS \"\${CMAKE_INSTALL_PREFIX}/${EXE_DIR}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
		include(BundleUtilities)
		fixup_bundle(\"${APPS}\" \"${QTPLUGINS}\" \"${QT_LIBRARY_DIRS}\")" COMPONENT Runtime)
	else(APPLE)
		set(CPACK_PACKAGE_NAME "tagainijisho")	
		set(ROOT_DIR share/tagainijisho)
		set(DB_DIR ${ROOT_DIR})
		set(I18N_DIR ${ROOT_DIR}/i18n)
		set(DOC_DIR ${ROOT_DIR}/doc)
		set(RESOURCES_DIR ${ROOT_DIR})

		# Desktop and icon files
		install(FILES ${CMAKE_SOURCE_DIR}/tagainijisho.desktop DESTINATION share/applications PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/tagainijisho.appdata.xml DESTINATION share/applications PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Runtime)
		install(FILES ${CMAKE_SOURCE_DIR}/images/tagainijisho-16x16.png DESTINATION share/icons/hicolor/16x16/apps RENAME tagainijisho.png PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Icons)
		install(FILES ${CMAKE_SOURCE_DIR}/images/tagainijisho-22x22.png DESTINATION share/icons/hicolor/22x22/apps RENAME tagainijisho.png PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Icons)
		install(FILES ${CMAKE_SOURCE_DIR}/images/tagainijisho-32x32.png DESTINATION share/icons/hicolor/32x32/apps RENAME tagainijisho.png PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Icons)
		install(FILES ${CMAKE_SOURCE_DIR}/images/tagainijisho-48x48.png DESTINATION share/icons/hicolor/48x48/apps RENAME tagainijisho.png PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Icons)
		install(FILES ${CMAKE_SOURCE_DIR}/images/tagainijisho-64x64.png DESTINATION share/icons/hicolor/64x64/apps RENAME tagainijisho.png PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Icons)
		install(FILES ${CMAKE_SOURCE_DIR}/images/tagainijisho-128x128.png DESTINATION share/icons/hicolor/128x128/apps RENAME tagainijisho.png PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Icons)
	endif(APPLE)

	# Databases
	install(FILES ${CMAKE_BINARY_DIR}/jmdict.db DESTINATION ${DB_DIR} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Databases)
	install(FILES ${CMAKE_BINARY_DIR}/kanjidic2.db DESTINATION ${DB_DIR} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Databases)
	foreach(LANG en;${DICT_LANG})
		install(FILES ${CMAKE_BINARY_DIR}/jmdict-${LANG}.db DESTINATION ${DB_DIR} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Databases)
		install(FILES ${CMAKE_BINARY_DIR}/kanjidic2-${LANG}.db DESTINATION ${DB_DIR} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Databases)
	endforeach(LANG en;${DICT_LANG})

	# i18n files
	foreach(LANG ${UI_LANG})
		install(FILES ${CMAKE_BINARY_DIR}/i18n/tagainijisho_${LANG}.qm DESTINATION ${I18N_DIR} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT I18n)
	endforeach(LANG ${UI_LANG})

	# Template files
	install(FILES ${CMAKE_SOURCE_DIR}/src/gui/jmdict/detailed_jmdict.html ${CMAKE_SOURCE_DIR}/src/gui/jmdict/detailed_jmdict.css DESTINATION ${RESOURCES_DIR} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Templates)
	install(FILES ${CMAKE_SOURCE_DIR}/src/gui/kanjidic2/detailed_kanjidic2.html ${CMAKE_SOURCE_DIR}/src/gui/kanjidic2/detailed_kanjidic2.css DESTINATION ${RESOURCES_DIR} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Templates)
	install(FILES ${CMAKE_SOURCE_DIR}/src/gui/export_template.html DESTINATION ${RESOURCES_DIR} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Templates)
	install(FILES ${CMAKE_SOURCE_DIR}/src/gui/detailed_default.html ${CMAKE_SOURCE_DIR}/src/gui/detailed_default.css DESTINATION ${RESOURCES_DIR} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ COMPONENT Templates)

	# Documentation
	if (EXISTS ${CMAKE_SOURCE_DIR}/doc/manual.html)
		install(FILES ${CMAKE_SOURCE_DIR}/doc/manual.html DESTINATION ${DOC_DIR} COMPONENT Docs)
		install(DIRECTORY ${CMAKE_SOURCE_DIR}/doc/images DESTINATION ${DOC_DIR} COMPONENT Docs)
	else(EXISTS ${CMAKE_SOURCE_DIR}/doc/manual.html)
		MESSAGE(STATUS "Warning, docs were not generated")
	endif()

	# Custom install targets
	add_custom_target(install_runtime
		${CMAKE_COMMAND}
		-D "COMPONENT=Runtime"
		-P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
	)
	add_dependencies(install_runtime tagainijisho)
	add_custom_target(install_databases
		${CMAKE_COMMAND}
		-D "COMPONENT=Databases"
		-P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
	)
	add_dependencies(install_databases databases)
	add_custom_target(install_i18n
		${CMAKE_COMMAND}
		-D "COMPONENT=I18n"
		-P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
	)
	add_dependencies(install_i18n i18n)
	add_custom_target(install_templates
		${CMAKE_COMMAND}
		-D "COMPONENT=Templates"
		-P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
	)
	#add_dependencies(install_templates templates)
	add_custom_target(install_icons
		${CMAKE_COMMAND}
		-D "COMPONENT=Icons"
		-P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
	)
	#add_dependencies(install_icons icons)
	add_custom_target(install_docs
		${CMAKE_COMMAND}
		-D "COMPONENT=Docs"
		-P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
	)
	add_dependencies(install_docs docs)
	add_custom_target(install_nonarch)
	add_dependencies(install_nonarch install_databases install_i18n install_templates install_icons install_docs)
endif(UNIX)

if(WIN32)
	MESSAGE(STATUS "Not implemented")
endif(WIN32)

set(CPACK_SOURCE_IGNORE_FILES "~$" "\\\\.gitignore$" "^${PROJECT_SOURCE_DIR}/.git/" "^${PROJECT_SOURCE_DIR}/*.kdev4/" )

include(CPack)

