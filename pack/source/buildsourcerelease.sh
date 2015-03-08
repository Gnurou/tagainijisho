#!/bin/sh
# Build a source release tarball. This only works from a git repository.
curpath=`pwd`
BASEDIR=`readlink -f \`dirname $0\``
SRCDIR=`readlink -f $BASEDIR/../..`
VERSION=`grep "set(VERSION " $SRCDIR/CMakeLists.txt |sed -n 's/set(VERSION \(.*\))$/\1/p'`
cd $SRCDIR
git archive --format=tar --prefix=gitexport/ HEAD |tar xv -C $BASEDIR
cd $BASEDIR
cp -R $SRCDIR/3rdparty gitexport/
mkdir -p build
cd build
cmake -DEMBED_SQLITE=1 ../gitexport
make docs
make package_source
mv tagainijisho-${VERSION}.tar.gz $curpath
cd ..
rm -Rf gitexport build
echo "Final source release: $curpath/tagainijisho-${VERSION}.tar.gz"
cd $curpath
