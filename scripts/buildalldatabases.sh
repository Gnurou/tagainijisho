#!/bin/sh

make build_jmdict_db build_kanji_db
for lang in en fr de es ru;
do
	echo "Building \"$lang\" databases..."
	./src/core/jmdict/build_jmdict_db -l$lang .. jmdict-$lang.db
	./src/core/kanjidic2/build_kanji_db -l$lang .. kanjidic2-$lang.db
done
