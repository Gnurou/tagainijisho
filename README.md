Tagaini Jisho
=============

This is Tagaini Jisho, a free-as-in-speech Japanese dictionary and learning assistant since 2008.

About
-----

Tagaini Jisho is a Japanese learning assistant built around a vocabulary and kanji dictionary. Its
goal is to make it easy to:

* Lookup and find words or kanji (later referred to as "entries") that you want to study,
* Mark and organize the entries you want to study or remember, and
* Consolidate your global knowledge by connecting related studied entries together to facilitate
  memorization.

Using Tagaini, you can add entries to your study list, tag them, add notes, practice them as
flashcards, and easily navigate to related entries. A powerful search engine lets you look words and
kanji up from fragments of information, like character components or number of strokes. Finally,
export options allow you to print booklets for study or export entries to CSV for e.g. using them
with [Anki](https://apps.ankiweb.net/).

Tagaini Jisho runs on Linux/Unix, MacOS X, and Microsoft Windows.

License
-------

Copyright (C) 2008-2022 Alexandre Courbot.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the COPYING
file for more details.

Documentation is Copyright (C) 2010 Neil Caldwell & Alexandre Courbot, distributed under the
Creative Commons Attribution-Share Alike 3.0 license.

Features
--------

- Japanese dictionary based on [JMdict](https://www.edrdg.org/jmdict/j_jmdict.html).
- Kanji dictionary based on [kanjidic2](https://www.edrdg.org/wiki/index.php/KANJIDIC_Project) and
  [KanjiVG](https://kanjivg.tagaini.net/).
- Powerful search engine with many search options: JLPT level, part-of-speech, etc.
- Entries can be marked for study and have a score that evolves as you correctly recognize them.
- Training mode for studied entries, where lowest score entries are more likely to appear.
- Tagging of entries.
- Notes that can be attached to each entry.
- Print feature with connections between studied kanji and vocabulary, in a format that allows both
  studying and use as flashcards.

Building on Linux
-----------------

The only runtime dependency to run Tagaini Jisho is Qt 5.15 or higher. In order to build it, you
will also need CMake 2.8.1 or higher.

First configure the build using `cmake`:

    $ cmake .

By default, the program is installed into `/usr/local`, but you can change this by setting the
`CMAKE_INSTALL_PREFIX` variable. For instance:

    $ cmake . -DCMAKE_INSTALL_PREFIX=/usr

If you want to produce a debug build (useful if you are trying a development version and want to
reports problems), add the `-DCMAKE_BUILD_TYPE=Debug` option to the command line above to produce a
binary with debug symbols built-in.

You can also use `ccmake .` after running `cmake` to change these options or enable more debugging
options.

You can then build the program using `make`.

    $ make -j8

This will take some time, especially to generate the dictionaries databases. Adjust the `-j`
parameter to accurately reflect the number of CPU cores on your machine.

Finally, you can (optionally) install the program:

    # make install

Or if you prefer to run it in-place, just run

    $ ./src/gui/tagainijisho

From the build directory.

Building on Mac OS with Homebrew
--------------------------------

If you don't have Qt5 and installed yet, install them:

    $ brew install qt@5 cmake

Configure:

    $ Qt5_DIR=/usr/local/opt/qt5/lib/cmake cmake -DCMAKE_INSTALL_PREFIX=$HOME/Applications .

Build:

    $ make

Run in-place:

    $ ./src/gui/tagainijisho

Install (application bundle will be installed to `$HOME/Applications`):

    $ make install

Or generate a drag'n drop installer:

    $ cpack -G DragNDrop

Building on Windows with MSYS2
------------------------------

Make sure the following MSYS2 packages are installed:

    gzip mingw-w64-x86_64-toolchain mingw-w64-x86_64-qt5-static mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-nsis

We use a static Qt5 package to facilitate packaging, but feel free to use the non-static Qt5 package
if you don't plan to create an installer.

`/mingw64/bin` will also need to be in your `PATH`, so add it if it is not already there:

    $ export PATH="/mingw64/bin:$PATH"

Invoke CMake:

    $ cmake -G Ninja .

Build:

    $ ninja

The program can now be run in-place:

    $ ./src/gui/tagainijisho

Or you may want to create an installer (static Qt5 only):

    $ cpack -G NSIS

Usage
-----

For a detailed user manual, please see https://www.tagaini.net/manual.

Tagaini supports the following command-line arguments:

`--temp-db` start the program on an empty, temporary database that will be removed once the program
exits. This is useful for testing new things on a clean database.

Known bugs
----------
- Kanji stroke order may not always be accurate. Please report incorrect kanji to
  https://groups.google.com/group/tagaini-jisho.
- Kanji stroke color segmentation is not always accurate.
- JLPT levels may not always be accurate (as there are no official JLPT lists, these are gathered on
  a voluntary basis).

Credits
-------

Tagaini Jisho makes heavy use of the embedded SQLite database. Many thanks to all its developers for
making such a great embedded database available, and for their kind support.

Qt5 is used as a development framework and ensures portability between Linux, Mac OS, and Windows.

Words definitions are provided by the [JMDict](https://www.csse.monash.edu.au/~jwb/jmdict.html).

Kanji information come from the [kanjidic2
project](https://www.edrdg.org/wiki/index.php/KANJIDIC_Project).

Kanji components, and stroke animations come from the [KanjiVG project](https://kanjivg.tagaini.net).

JLPT levels for words come from the now-defunct JLPT Study Page, the [JLPT Resource
Page](http://www.tanos.co.uk/jlpt/), as well as lists provided by [Thierry
Bézecourt](http://www.thbz.org/kanjimots/jlpt.php3) and Alain Côté.

Application icon has been contributed by [Philip Seyfi](https://divita.eu/).

Flag images by [Mark James](http://www.famfamfam.com/lab/icons/flags/).

Fugue Icons Copyright (C) 2010 [Yusuke Kamiyamane](https://p.yusukekamiyamane.com/), licensed under a
[Creative Commons Attribution 3.0 license](https://creativecommons.org/licenses/by/3.0/).

Contact
-------

Website: https://www.tagaini.net

Development, bug reports, feature requests and general questions are handled on GitHub:
https://github.com/Gnurou/tagainijisho
