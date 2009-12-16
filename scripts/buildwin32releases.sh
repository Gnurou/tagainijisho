#!/bin/sh
QTSPEC=win32-cross-g++
QTPATH=/usr/local/qt4-win32/
MINGWDLLPATH=.
VERSION=`grep "VERSION" buildconfig.pri |sed -n 's/.*=\(.*\)/\1/p'`
NSIFILE=tagainijisho.nsi

qmake -spec win32-cross-g++
make
i486-mingw32-ranlib src/core/libtagaini-core.a
make
mv tagainijisho.exe tagainijisho.exe.save
make distclean
mv tagainijisho.exe.save tagainijisho.exe
qmake
make
mkdir win32-translations
lrelease $QTPATH/translations/qt_fr.ts -qm win32-translations/qt_fr.qm
lrelease $QTPATH/translations/qt_de.ts -qm win32-translations/qt_de.qm
lrelease $QTPATH/translations/qt_es.ts -qm win32-translations/qt_es.qm
lrelease $QTPATH/translations/qt_ru.ts -qm win32-translations/qt_ru.qm
lrelease $QTPATH/translations/qt_ru.ts -qm win32-translations/qt_nl.qm

rm -f jmdict.db kanjidic2.db
./builddb.py -len
makensis -DVERSION=$VERSION -DQTPATH=$QTPATH -DMINGWDLLPATH=$MINGWDLLPATH $NSIFILE
mv install.exe tagainijisho-${VERSION}-en.exe

rm -f jmdict.db kanjidic2.db
./builddb.py -lfr
makensis -DVERSION=$VERSION -DQTPATH=$QTPATH -DMINGWDLLPATH=$MINGWDLLPATH $NSIFILE
mv install.exe tagainijisho-${VERSION}-fr.exe

rm -f jmdict.db kanjidic2.db
./builddb.py -lde
makensis -DVERSION=$VERSION -DQTPATH=$QTPATH -DMINGWDLLPATH=$MINGWDLLPATH $NSIFILE
mv install.exe tagainijisho-${VERSION}-de.exe

rm -f jmdict.db kanjidic2.db
./builddb.py -les
makensis -DVERSION=$VERSION -DQTPATH=$QTPATH -DMINGWDLLPATH=$MINGWDLLPATH $NSIFILE
mv install.exe tagainijisho-${VERSION}-es.exe

rm -f jmdict.db kanjidic2.db
./builddb.py -lru
makensis -DVERSION=$VERSION -DQTPATH=$QTPATH -DMINGWDLLPATH=$MINGWDLLPATH $NSIFILE
mv install.exe tagainijisho-${VERSION}-ru.exe

rm -Rf win32-translations

