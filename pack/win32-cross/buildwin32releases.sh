#!/bin/bash
set -e

curpath=`pwd`
BASEDIR=`readlink -f \`dirname $0\``
SRCDIR=`readlink -f $BASEDIR/../..`
BUILDDIR=$BASEDIR/build-win32

# Build databases and docs using the native toolchain
mkdir -p $BUILDDIR
cd $BUILDDIR
cmake -DCMAKE_BUILD_TYPE=Release $SRCDIR
make i18n databases docs -j5

# Build everything else using the cross toolchain
rm -Rf CMakeCache.txt CMakeFiles
x86_64-w64-mingw32-cmake -DCMAKE_BUILD_TYPE=Release $SRCDIR
make -j5

cpack -G NSIS64

mv tagainijisho-$VERSION.exe $curpath
rm -Rf $BUILDDIR
cd $curpath
