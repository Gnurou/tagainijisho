#!/bin/sh
# Build a source release tarball. This only works from a git repository.
curpath=`pwd`
BASEDIR=`readlink -f \`dirname $0\``
SRCDIR=`readlink -f $BASEDIR/../..`
VERSION=`grep "set(VERSION " $SRCDIR/CMakeLists.txt |sed -n 's/set(VERSION \(.*\))$/\1/p'`
cd $SRCDIR
lupdate . -ts i18n/*.ts
git add i18n
git commit i18n -m "Updated translation files for version $VERSION"
git archive --format=tar --prefix=gitexport/ HEAD |tar xv -C $BASEDIR
cd $BASEDIR
mkdir -p build
cd build
cmake ../gitexport
cmake .
make docs
make package_source
mv tagainijisho-${VERSION}.tar.gz ..
cd ..
rm -Rf gitexport build
cd $curpath
