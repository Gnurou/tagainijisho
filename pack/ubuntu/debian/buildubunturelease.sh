#!/bin/sh
curpath=`pwd`
BASEDIR=`readlink -f \`dirname $0\``
SRCDIR=`readlink -f $BASEDIR/../..`
VERSION=`grep "set(VERSION " $SRCDIR/CMakeLists.txt |sed -n 's/set(VERSION \(.*\))$/\1/p'`
cd $SRCDIR
cp $SRCDIR/tagainijisho-$VERSION.tar.gz $SRCDIR/tagainijisho_$VERSION.orig.tar.gz
cp -R $SRCDIR/pack/ubuntu/debian $SRCDIR
dch -e .
debuild -S -sa
rm $SRCDIR/tagainijisho_$VERSION.orig.tar.gz
