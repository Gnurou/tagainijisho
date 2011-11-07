FILE (GLOB TRANSLATIONS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.ts)
QT4_ADD_TRANSLATION(QM_FILES ${TRANSLATIONS_FILES})
add_custom_target (i18n ALL DEPENDS ${QM_FILES})

set(UI_LANG "")
foreach(tsfile ${TRANSLATIONS_FILES})
	string(REGEX REPLACE ".*_(..)\\.ts" "\\1" rmatch ${tsfile})
	set(UI_LANG "${UI_LANG};${rmatch}")
endforeach(tsfile ${TRANSLATIONS_FILES})

set(UI_LANG ${UI_LANG} CACHE STRING "Languages to be used for the UI")

