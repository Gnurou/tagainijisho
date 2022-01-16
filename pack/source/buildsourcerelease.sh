#!/bin/sh
# Build a source release tarball. This only works from a git repository.
set -e
curpath=`pwd`
BASEDIR=`readlink -f \`dirname $0\``
SRCDIR=`readlink -f $BASEDIR/../..`
VERSION=`grep "set(VERSION " $SRCDIR/CMakeLists.txt |sed -n 's/set(VERSION \(.*\))$/\1/p'`
cd $SRCDIR
git archive --format=tar --prefix=gitexport/ HEAD |tar xv -C $BASEDIR
cd $BASEDIR
mkdir -p build_src
cd build_src
cmake -G "Unix Makefiles" ../gitexport
make docs
make package_source
mv tagainijisho-${VERSION}.tar.gz $curpath
cd ..
rm -Rf gitexport build_src
echo "Source release package: $curpath/tagainijisho-${VERSION}.tar.gz"
cd $curpath
