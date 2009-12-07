include(../../buildconfig.pri)
TEMPLATE = app
TARGET = tagainijisho
CONFIG += qt
QT = core \
    gui \
    sql \
    network
INCLUDEPATH = .. \
    ../../3rdparty/sqlite
DESTDIR = ../../
HEADERS = ElidedPushButton.h \
    BetterSpinBox.h \
    HexSpinBox.h \
    RelativeDateEdit.h \
    ProxyPaintEngine.h \
    BookletPrintEngine.h \
    BookletPrinter.h \
    KanjiValidator.h \
    EntryFormatter.h \
    TagsDialogs.h \
    FlowLayout.h \
    MultiStackedWidget.h \
    EntryMenu.h \
    EditEntryNotesDialog.h \
    SetsOrganizer.h \
    UpdateChecker.h \
    ClickableLabel.h \
    SingleEntryView.h \
    SearchBar.h \
    SearchWidget.h \
    ResultsList.h \
    SmoothScroller.h \
    ResultsView.h \
    AbstractHistory.h \
    DetailedView.h \
    ToolBarDetailedView.h \
    TrainSettings.h \
    YesNoTrainer.h \
    ReadingTrainer.h \
    PreferencesWindow.h \
    MainWindow.h \
    jmdict/JMdictEntryFormatter.h \
    jmdict/JMdictPreferences.h \
    jmdict/JMdictGUIPlugin.h \
    kanjidic2/KanjiRenderer.h \
    kanjidic2/KanjiPopup.h \
    kanjidic2/KanjiPlayer.h \
    kanjidic2/Kanjidic2EntryFormatter.h \
    kanjidic2/Kanjidic2Preferences.h \
    kanjidic2/Kanjidic2GUIPlugin.h \
    jmdict/JMdictYesNoTrainer.h
SOURCES = EntryFormatter.cc \
    BetterSpinBox.cc \
    HexSpinBox.cc \
    RelativeDateEdit.cc \
    ProxyPaintEngine.cc \
    BookletPrintEngine.cc \
    BookletPrinter.cc \
    KanjiValidator.cc \
    TagsDialogs.cc \
    FlowLayout.cc \
    MultiStackedWidget.cc \
    EntryMenu.cc \
    EditEntryNotesDialog.cc \
    SetsOrganizer.cc \
    UpdateChecker.cc \
    ClickableLabel.cc \
    SingleEntryView.cc \
    SearchBar.cc \
    SearchWidget.cc \
    ResultsList.cc \
    SmoothScroller.cc \
    ResultsView.cc \
    DetailedView.cc \
    ToolBarDetailedView.cc \
    TrainSettings.cc \
    YesNoTrainer.cc \
    ReadingTrainer.cc \
    PreferencesWindow.cc \
    MainWindow.cc \
    jmdict/JMdictEntryFormatter.cc \
    jmdict/JMdictPreferences.cc \
    jmdict/JMdictGUIPlugin.cc \
    kanjidic2/Kanjidic2EntryFormatter.cc \
    kanjidic2/KanjiRenderer.cc \
    kanjidic2/KanjiPopup.cc \
    kanjidic2/KanjiPlayer.cc \
    kanjidic2/Kanjidic2Preferences.cc \
    kanjidic2/Kanjidic2GUIPlugin.cc \
    main.cc \
    jmdict/JMdictYesNoTrainer.cc

# LIBS += -L../core \
# -ltagaini-core
LIBS += ../core/libtagaini-core.a \
    ../sqlite/libsqlite3.a
POST_TARGETDEPS = ../core/libtagaini-core.a \
	../sqlite/libsqlite3.a
FORMS += SetsOrganizer.ui \
    TrainSettings.ui \
    ReadingTrainer.ui \
    AboutDialog.ui \
    PreferencesWindow.ui \
    GeneralPreferences.ui \
    ResultsViewPreferences.ui \
    DetailedViewPreferences.ui \
    jmdict/JMdictPreferences.ui \
    kanjidic2/Kanjidic2Preferences.ui \
    kanjidic2/KanjiPopup.ui
RESOURCES += ../../tagainijisho.qrc
RC_FILE = ../../tagainijisho.rc
mac:ICON = ../../images/appicon.icns
mac:QMAKE_INFO_PLIST = ../../Info.plist
CODECFORTR = UTF-8
TRANSLATIONS = ../../i18n/tagainijisho_fr.ts \
	../../i18n/tagainijisho_de.ts \
	../../i18n/tagainijisho_es.ts \
	../../i18n/tagainijisho_ru.ts \
	../../i18n/tagainijisho_nl.ts
unix::target.path = $$BIN_DIR
INSTALLS = target
