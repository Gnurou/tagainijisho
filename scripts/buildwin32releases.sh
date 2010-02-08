#!/bin/sh
QTPATH=/home/gnurou/.wine/drive_c/Qt/4.6.1
MINGWDLLPATH=/usr/i486-mingw32/bin
VERSION=`grep "set(VERSION " CMakeLists.txt |sed "s/set(VERSION \(.*\))/\1/"`
NSIFILE=tagainijisho.nsi
DBDIR=build

mkdir win32-translations
for lang in fr de es ru;
do
	lrelease $QTPATH/translations/qt_$lang.ts -qm win32-translations/qt_$lang.qm
done

for lang in en fr de es ru;
do
	makensis -DLANG=$lang -DVERSION=$VERSION -DBUILDDIR=build-win32 -DDBDIR=$DBDIR -DQTPATH=$QTPATH -DMINGWDLLPATH=$MINGWDLLPATH $NSIFILE
	mv install.exe tagainijisho-${VERSION}-${lang}.exe
done

rm -Rf win32-translations

