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
KanaSelector.cc
)

set(tagainijisho_gui_kanjidic2_MOCS
Kanjidic2EntryFormatter.h
Kanjidic2GUIPlugin.h
Kanjidic2Preferences.h
KanjiPlayer.h
KanjiPopup.h
KanjiResultsView.h
Kanjidic2FilterWidget.h
KanjiSelector.h
KanaView.h
KanaSelector.h
)

set(tagainijisho_gui_kanjidic2_UIS
KanjiPopup.ui
KanjiSelector.ui
KanaSelector.ui
#ComponentSearchWidget.ui
#RadicalSearchWidget.ui
Kanjidic2Preferences.ui
)

qt4_wrap_cpp(tagainijisho_gui_kanjidic2_MOC_SRCS ${tagainijisho_gui_kanjidic2_MOCS})
qt4_wrap_ui(tagainijisho_gui_kanjidic2_UI_SRCS ${tagainijisho_gui_kanjidic2_UIS})
add_library(tagaini_gui_kanjidic2 STATIC ${tagainijisho_gui_kanjidic2_UI_SRCS} ${tagainijisho_gui_kanjidic2_MOC_SRCS} ${tagainijisho_gui_kanjidic2_SRCS})

