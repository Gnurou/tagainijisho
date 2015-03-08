#!/bin/bash
curpath=`pwd`
BASEDIR=`readlink -f \`dirname $0\``
SRCDIR=`readlink -f $BASEDIR/../..`
BUILDDIR=$BASEDIR/build-win32
VERSION=`grep "set(VERSION " $SRCDIR/CMakeLists.txt |sed "s/set(VERSION \(.*\))/\1/"`
QTPATH=/usr/i686-w64-mingw32
QTTRANS=/home/gnurou/.wine/drive_c/Qt/latest
NSIFILE=$BASEDIR/tagainijisho.nsi

mkdir -p $BUILDDIR/i18n
for lang in `ls $SRCDIR/i18n/*.ts |cut -d'_' -f2 |cut -d'.' -f1`;
do
	touch $BUILDDIR/i18n/qt_$lang.qm
	lrelease $QTTRANS/translations/qt_$lang.ts -qm $BUILDDIR/i18n/qt_$lang.qm
done

cd $BUILDDIR
cmake -DCMAKE_BUILD_TYPE=Release -DEMBED_SQLITE=1 $SRCDIR
make i18n databases docs -j5
rm -Rf CMakeCache.txt CMakeFiles
i686-w64-mingw32-cmake -DCMAKE_BUILD_TYPE=Release -DEMBED_SQLITE=1 $SRCDIR
make tagainijisho -j5
i686-w64-mingw32-strip $BUILDDIR/src/gui/tagainijisho.exe

cd $BASEDIR
makensis -DVERSION=$VERSION -DBUILDDIR=$BUILDDIR -DSRCDIR=$SRCDIR -DQTPATH=$QTPATH $NSIFILE
mv install.exe $curpath/tagainijisho-$VERSION.exe
rm -Rf $BUILDDIR
cd $curpath
