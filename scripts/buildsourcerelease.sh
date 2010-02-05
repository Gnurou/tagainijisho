#!/bin/sh
VERSION=`grep "set(VERSION " CMakeLists.txt |sed -n 's/set(VERSION \(.*\))$/\1/p'`
git archive --format=tar --prefix=tagainijisho-$VERSION/ HEAD |tar xv
cd tagainijisho-${VERSION}
sh prepare-source.sh
rm prepare-source.sh
cd ..
tar cvzf tagainijisho-${VERSION}.tar.gz tagainijisho-${VERSION}
rm -Rf tagainijisho-${VERSION}
