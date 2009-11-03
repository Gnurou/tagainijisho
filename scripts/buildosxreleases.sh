#!/bin/sh
VERSION=`grep "^ *VERSION" tagainijisho.pro |sed -n 's/.*=\(.*\)/\1/p'`

rm -Rf tagainijisho.app
qmake -spec macx-g++
make
macdeployqt tagainijisho.app
echo "Translations = Translations" >> tagainijisho.app/Contents/Resources/qt.conf
mkdir tagainijisho.app/Contents/Translations
cp /Developer/Applications/Qt/translations/qt_fr.qm tagainijisho.app/Contents/Translations
cp /Developer/Applications/Qt/translations/qt_de.qm tagainijisho.app/Contents/Translations
cp /Developer/Applications/Qt/translations/qt_es.qm tagainijisho.app/Contents/Translations
cp /Developer/Applications/Qt/translations/qt_ru.qm tagainijisho.app/Contents/Translations
rm -Rf tagainijisho.app/Contents/PlugIns/accessible
rm -Rf tagainijisho.app/Contents/PlugIns/codecs
rm -Rf tagainijisho.app/Contents/PlugIns/sqldrivers
rm -f tagainijisho.app/Contents/PlugIns/imageformats/libqico.dylib tagainijisho.app/Contents/PlugIns/imageformats/libqjpeg.dylib tagainijisho.app/Contents/PlugIns/imageformats/libqmng.dylib tagainijisho.app/Contents/PlugIns/imageformats/libqtiff.dylib

./builddb.py -len
mv -f jmdict.db kanjidic2.db tagainijisho.app/Contents/MacOS
hdiutil convert images/macinstaller/Tagaini\ Jisho.dmg -format UDSP -o Tagaini\ Jisho
hdiutil mount Tagaini\ Jisho.sparseimage
cp -R tagainijisho.app/Contents /Volumes/Tagaini\ Jisho/Tagaini\ Jisho.app
hdiutil unmount /Volumes/Tagaini\ Jisho
hdiutil convert Tagaini\ Jisho.sparseimage -format UDBZ -o Tagaini\ Jisho.dmg
rm Tagaini\ Jisho.sparseimage
mv Tagaini\ Jisho.dmg Tagaini\ Jisho-$VERSION-en.dmg

./builddb.py -lfr
mv -f jmdict.db kanjidic2.db tagainijisho.app/Contents/MacOS
hdiutil convert images/macinstaller/Tagaini\ Jisho.dmg -format UDSP -o Tagaini\ Jisho
hdiutil mount Tagaini\ Jisho.sparseimage
cp -R tagainijisho.app/Contents /Volumes/Tagaini\ Jisho/Tagaini\ Jisho.app
hdiutil unmount /Volumes/Tagaini\ Jisho
hdiutil convert Tagaini\ Jisho.sparseimage -format UDBZ -o Tagaini\ Jisho.dmg
rm Tagaini\ Jisho.sparseimage
mv Tagaini\ Jisho.dmg Tagaini\ Jisho-$VERSION-fr.dmg

./builddb.py -lde
mv -f jmdict.db kanjidic2.db tagainijisho.app/Contents/MacOS
hdiutil convert images/macinstaller/Tagaini\ Jisho.dmg -format UDSP -o Tagaini\ Jisho
hdiutil mount Tagaini\ Jisho.sparseimage
cp -R tagainijisho.app/Contents /Volumes/Tagaini\ Jisho/Tagaini\ Jisho.app
hdiutil unmount /Volumes/Tagaini\ Jisho
hdiutil convert Tagaini\ Jisho.sparseimage -format UDBZ -o Tagaini\ Jisho.dmg
rm Tagaini\ Jisho.sparseimage
mv Tagaini\ Jisho.dmg Tagaini\ Jisho-$VERSION-de.dmg

./builddb.py -les
mv -f jmdict.db kanjidic2.db tagainijisho.app/Contents/MacOS
hdiutil convert images/macinstaller/Tagaini\ Jisho.dmg -format UDSP -o Tagaini\ Jisho
hdiutil mount Tagaini\ Jisho.sparseimage
cp -R tagainijisho.app/Contents /Volumes/Tagaini\ Jisho/Tagaini\ Jisho.app
hdiutil unmount /Volumes/Tagaini\ Jisho
hdiutil convert Tagaini\ Jisho.sparseimage -format UDBZ -o Tagaini\ Jisho.dmg
rm Tagaini\ Jisho.sparseimage
mv Tagaini\ Jisho.dmg Tagaini\ Jisho-$VERSION-es.dmg

./builddb.py -lru
mv -f jmdict.db kanjidic2.db tagainijisho.app/Contents/MacOS
hdiutil convert images/macinstaller/Tagaini\ Jisho.dmg -format UDSP -o Tagaini\ Jisho
hdiutil mount Tagaini\ Jisho.sparseimage
cp -R tagainijisho.app/Contents /Volumes/Tagaini\ Jisho/Tagaini\ Jisho.app
hdiutil unmount /Volumes/Tagaini\ Jisho
hdiutil convert Tagaini\ Jisho.sparseimage -format UDBZ -o Tagaini\ Jisho.dmg
rm Tagaini\ Jisho.sparseimage
mv Tagaini\ Jisho.dmg Tagaini\ Jisho-$VERSION-ru.dmg
