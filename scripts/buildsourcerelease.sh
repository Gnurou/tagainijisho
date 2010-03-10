#!/bin/sh
VERSION=`grep "set(VERSION " CMakeLists.txt |sed -n 's/set(VERSION \(.*\))$/\1/p'`
# Update translation files
lupdate . -ts i18n/*.ts
git add i18n/*.ts
git commit -m "Updated translation files for release $VERSION"
git tag -f $VERSION
git archive --format=tar --prefix=tagainijisho-$VERSION/ HEAD |tar xv
cd tagainijisho-${VERSION}
sh prepare-source.sh
rm -Rf prepare-source.sh
cd ..
tar cvzf tagainijisho-${VERSION}.tar.gz tagainijisho-${VERSION}
rm -Rf tagainijisho-${VERSION}
