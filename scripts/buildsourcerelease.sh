#!/bin/sh
VERSION=`grep "VERSION" buildconfig.pri |sed -n 's/.*=\(.*\)/\1/p'`
git archive --format=tar --prefix=tagainijisho-$VERSION/ HEAD |tar xv
cd tagainijisho-${VERSION}
lrelease src/gui/gui.pro
./builddb.py -d
cd ..
tar cvzf tagainijisho-${VERSION}.tar.gz tagainijisho-${VERSION}
rm -Rf tagainijisho-${VERSION}
