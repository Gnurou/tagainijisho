#!/bin/sh
curpath=`pwd`
BASEDIR=`readlink -f \`dirname $0\``
cd $BASEDIR
git archive --format=tar --prefix=gitexport HEAD |tar xv
cd gitexport
cmake .
cmake .
make package_source
cd ..
rm -Rf gitexport
cd $curpath

