#!/bin/sh
VERSION=`grep "set(VERSION " CMakeLists.txt |sed "s/set(VERSION \(.*\))/\1/"`
QTPATH=`grep "^set(QT_ROOT" scripts/Toolchain-win32.cmake |sed "s/set(QT_ROOT \(.*\))/\1/"`
MINGWDLLPATH=${MINGWDLLPATH-/usr/i486-mingw32/lib}
DBDIR=${DBDIR-.}
NSIFILE=${NSIFILE-tagainijisho.nsi}
BUILDDIR=build-win32

mkdir win32-translations
for lang in fr de es ru nl;
do
	touch win32-translations/qt_$lang.qm
	lrelease $QTPATH/translations/qt_$lang.ts -qm win32-translations/qt_$lang.qm
done

mkdir $BUILDDIR
cd $BUILDDIR
cmake -DCMAKE_TOOLCHAIN_FILE=../scripts/Toolchain-win32.cmake -DCMAKE_BUILD_TYPE=Release ..
make tagainijisho
cd ..

for lang in en fr de es ru;
do
	makensis -DLANG=$lang -DVERSION=$VERSION -DBUILDDIR=$BUILDDIR -DDBDIR=$DBDIR -DQTPATH=$QTPATH -DMINGWDLLPATH=$MINGWDLLPATH $NSIFILE
	mv install.exe tagainijisho-${VERSION}-${lang}.exe
done

rm -Rf win32-translations

