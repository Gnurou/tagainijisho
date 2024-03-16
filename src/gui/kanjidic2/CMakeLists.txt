# kanjidic2 GUI
set(tagainijisho_gui_kanjidic2_SRCS
    Kanjidic2EntryFormatter.cc
    KanjiRenderer.cc
    KanjiPopup.cc
    KanjiPlayer.cc
    KanjiResultsView.cc
    KanjiSelector.cc
    Kanjidic2Preferences.cc
    Kanjidic2FilterWidget.cc
    Kanjidic2GUIPlugin.cc
    KanaView.cc
    KanaSelector.cc)

set(tagainijisho_gui_kanjidic2_UIS
    KanjiPopup.ui KanjiSelector.ui KanaSelector.ui
    # ComponentSearchWidget.ui RadicalSearchWidget.ui
    Kanjidic2Preferences.ui)

qt5_wrap_ui(tagainijisho_gui_kanjidic2_UI_SRCS
            ${tagainijisho_gui_kanjidic2_UIS})
add_library(tagaini_gui_kanjidic2 STATIC ${tagainijisho_gui_kanjidic2_UI_SRCS}
                                         ${tagainijisho_gui_kanjidic2_SRCS})
target_link_libraries(tagaini_gui_kanjidic2 tagaini_gui)
