#!/bin/sh
VERSION=`grep "set(VERSION " CMakeLists.txt |sed "s/set(VERSION \(.*\))/\1/"`
BUNDLEPATH=build/src/gui

# Include and re-link the Qt libraries in the bundle
macdeployqt $BUNDLEPATH/tagainijisho.app
# Include HTML export template
cp src/gui/export_template.html $BUNDLEPATH/tagainijisho.app/Contents/
# Include Tagaini translations
mkdir $BUNDLEPATH/tagainijisho.app/Contents/i18n
cp i18n/tagainijisho_fr.qm $BUNDLEPATH/tagainijisho.app/Contents/i18n
cp i18n/tagainijisho_de.qm $BUNDLEPATH/tagainijisho.app/Contents/i18n
cp i18n/tagainijisho_en.qm $BUNDLEPATH/tagainijisho.app/Contents/i18n
cp i18n/tagainijisho_ru.qm $BUNDLEPATH/tagainijisho.app/Contents/i18n
cp i18n/tagainijisho_nl.qm $BUNDLEPATH/tagainijisho.app/Contents/i18n
# Include Qt translations
echo "Translations = Translations" >> $BUNDLEPATH/tagainijisho.app/Contents/Resources/qt.conf
mkdir $BUNDLEPATH/tagainijisho.app/Contents/Translations
cp /Developer/Applications/Qt/translations/qt_fr.qm $BUNDLEPATH/tagainijisho.app/Contents/Translations
cp /Developer/Applications/Qt/translations/qt_de.qm $BUNDLEPATH/tagainijisho.app/Contents/Translations
cp /Developer/Applications/Qt/translations/qt_es.qm $BUNDLEPATH/tagainijisho.app/Contents/Translations
cp /Developer/Applications/Qt/translations/qt_ru.qm $BUNDLEPATH/tagainijisho.app/Contents/Translations
cp /Developer/Applications/Qt/translations/qt_nl.qm $BUNDLEPATH/tagainijisho.app/Contents/Translations
# The .plist file correctly sets the application name in the mac menu
sed "s/\\\$VERSION/$VERSION/g" scripts/Info.plist >$BUNDLEPATH/tagainijisho.app/Contents/Info.plist
# Remove unneeded Qt stuff
rm -Rf $BUNDLEPATH/tagainijisho.app/Contents/PlugIns/accessible
rm -Rf $BUNDLEPATH/tagainijisho.app/Contents/PlugIns/codecs
rm -Rf $BUNDLEPATH/tagainijisho.app/Contents/PlugIns/sqldrivers
rm -f $BUNDLEPATH/tagainijisho.app/Contents/PlugIns/imageformats/libqico.dylib tagainijisho.app/Contents/PlugIns/imageformats/libqjpeg.dylib tagainijisho.app/Contents/PlugIns/imageformats/libqmng.dylib tagainijisho.app/Contents/PlugIns/imageformats/libqtiff.dylib
mkdir $BUNDLEPATH/tagainijisho.app/Contents/MacOS/doc
cp -R doc/images $BUNDLEPATH/tagainijisho.app/Contents/MacOS/doc/
cp -R doc/manual.html $BUNDLEPATH/tagainijisho.app/Contents/MacOS/doc/

# Create the disk images from our master
for lang in en fr de es ru;
do
	mv -f jmdict-$lang.db $BUNDLEPATH/tagainijisho.app/Contents/MacOS/jmdict.db
	mv -f kanjidic2-$lang.db $BUNDLEPATH/tagainijisho.app/Contents/MacOS/kanjidic2.db
	hdiutil convert images/macinstaller/Tagaini\ Jisho.dmg -format UDSP -o Tagaini\ Jisho
	hdiutil mount Tagaini\ Jisho.sparseimage
	cp -R $BUNDLEPATH/tagainijisho.app/Contents /Volumes/Tagaini\ Jisho/Tagaini\ Jisho.app
	hdiutil unmount /Volumes/Tagaini\ Jisho
	hdiutil convert Tagaini\ Jisho.sparseimage -format UDBZ -o Tagaini\ Jisho.dmg
	rm Tagaini\ Jisho.sparseimage
	mv Tagaini\ Jisho.dmg Tagaini\ Jisho-$VERSION-$lang.dmg
done

