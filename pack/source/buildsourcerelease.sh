#!/bin/sh
# Build a source release tarball. This only works from a git repository.
curpath=`pwd`
BASEDIR=`readlink -f \`dirname $0\``
SRCDIR=`readlink -f $BASEDIR/../..`
VERSION=`grep "set(VERSION " $SRCDIR/CMakeLists.txt |sed -n 's/set(VERSION \(.*\))$/\1/p'`
#cd $SRCDIR/i18n
#./update.sh
#git add .	
#git commit -m "Updated translation files for version $VERSION"
cd $SRCDIR
git archive --format=tar --prefix=gitexport/ HEAD |tar xv -C $BASEDIR
cd $BASEDIR
mkdir -p build
cd build
cmake ../gitexport
cmake -DEMBED_SQLITE=1 .
make docs
make package_source
mv tagainijisho-${VERSION}.tar.gz ..
cd ..
rm -Rf gitexport build
echo "Final source release: `pwd`/tagainijisho-${VERSION}.tar.gz"
cd $curpath
