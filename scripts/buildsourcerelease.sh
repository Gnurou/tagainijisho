#!/bin/sh
VERSION=`grep "^ *VERSION" tagainijisho.pro |sed -n 's/.*=\(.*\)/\1/p'`
bzr export tagainijisho-${VERSION}
cd tagainijisho-${VERSION}
./builddb.py -d
cd ..
tar cvzf tagainijisho-${VERSION}.tar.gz tagainijisho-${VERSION}
rm -Rf tagainijisho-${VERSION}
