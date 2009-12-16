Name "Tagaini Jisho ${VERSION}"
OutFile "install.exe"

InstallDir "$PROGRAMFILES\Tagaini Jisho"
RequestExecutionLevel admin
LicenseData "COPYING"
Page license
Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

Section "Tagaini Jisho"
IfFileExists "$LOCALAPPDATA\VirtualStore\Program Files\Tagaini Jisho\kanjidic2.db" VSTOREKANJIDICEXISTS VSTOREKANJIDICEND
VSTOREKANJIDICEXISTS:
Delete "$LOCALAPPDATA\VirtualStore\Program Files\Tagaini Jisho\jmdict.db"
Delete "$LOCALAPPDATA\VirtualStore\Program Files\Tagaini Jisho\kanjidic2.db"
RMDir "$LOCALAPPDATA\VirtualStore\Program Files\Tagaini Jisho"
VSTOREKANJIDICEND:

SetOutPath "$INSTDIR"
File "tagainijisho.exe"
File "jmdict.db"
SetFileAttributes "jmdict.db" READONLY
File "kanjidic2.db"
SetFileAttributes "kanjidic2.db" READONLY
File "${QTPATH}/bin/QtCore4.dll"
File "${QTPATH}/bin/QtGui4.dll"
File "${QTPATH}/bin/QtSql4.dll"
File "${QTPATH}/bin/QtNetWork4.dll"
File "${MINGWDLLPATH}/mingwm10.dll"
File "${MINGWDLLPATH}/libgcc_s_dw2-1.dll"
File /oname=qt.conf "qt.conf.windows"
SetOutPath "$INSTDIR\translations"
File "win32-translations/qt_fr.qm"
File "win32-translations/qt_de.qm"
File "win32-translations/qt_es.qm"
File "win32-translations/qt_ru.qm"
SetOutPath "$INSTDIR\imageformats"
File "${QTPATH}/plugins/imageformats/qgif4.dll"
WriteUninstaller "$INSTDIR\uninstall.exe"
CreateDirectory "$SMPROGRAMS\Tagaini Jisho"
CreateShortCut "$SMPROGRAMS\Tagaini Jisho\Tagaini Jisho.lnk" "$INSTDIR\tagainijisho.exe"
CreateShortCut "$SMPROGRAMS\Tagaini Jisho\Uninstall.lnk" "$INSTDIR\uninstall.exe"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tagaini Jisho" "DisplayName" "Tagaini Jisho"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tagaini Jisho" "UninstallString" "$INSTDIR/uninstall.exe"
SectionEnd

Section "uninstall"
Delete "$SMPROGRAMS\Tagaini Jisho\Tagaini Jisho.lnk"
Delete "$SMPROGRAMS\Tagaini Jisho\Uninstall.lnk"
RMDir "$SMPROGRAMS\Tagaini Jisho"
Delete "$INSTDIR\translations\qt_ru.qm"
Delete "$INSTDIR\translations\qt_es.qm"
Delete "$INSTDIR\translations\qt_de.qm"
Delete "$INSTDIR\translations\qt_fr.qm"
RMDir "$INSTDIR\translations"
Delete "$INSTDIR\imageformats\qgif4.dll"
RMDir "$INSTDIR\imageformats"
Delete "$INSTDIR\QtCore4.dll"
Delete "$INSTDIR\QtGui4.dll"
Delete "$INSTDIR\QtSql4.dll"
Delete "$INSTDIR\QtNetwork4.dll"
Delete "$INSTDIR\qt.conf"
Delete "$INSTDIR\libgcc_s_dw2-1.dll"
Delete "$INSTDIR\mingwm10.dll"
Delete "$INSTDIR\tagainijisho.exe"
Delete "$INSTDIR\jmdict.db"
Delete "$INSTDIR\kanjidic2.db"
Delete "$INSTDIR\uninstall.exe"
RMDir "$INSTDIR"
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tagaini Jisho"
SectionEnd

