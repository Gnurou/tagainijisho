#!/bin/sh
QTSPEC=win32-cross-g++
QTPATH=/usr/local/qt4-win32/
MINGWDLLPATH=.
VERSION=`grep "^ *VERSION" tagainijisho.pro |sed -n 's/.*=\(.*\)/\1/p'`
NSIFILE=tagainijisho.nsi

qmake -spec win32-cross-g++
make release

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
