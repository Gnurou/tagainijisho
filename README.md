README file for Tagaini Jisho
=============================

This is Tagaini Jisho, a free software Japanese dictionary and learning
assistant.

About
-----

Tagaini Jisho is designed to help you remember Japanese vocabulary and kanji
(later referred as 'entries') by presenting them in a way that makes it easy to
create connections between them. It does so by keeping track of all the entries
you already know and want to study, and letting you tag and annote them, in
addition to providing easy navigation between related entries. A powerful
search engine also allows you to search for entries very precisely. Finally,
Tagaini let you produce printed material (including a handy foldable pocket
book) so let you study anywhere.

Tagaini Jisho runs on Linux/Unix, MacOS X, and Microsoft Windows.

Licence
-------

Copyright (C) 2008-2011 Alexandre Courbot.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version. 

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the COPYING file for more details. 

Documentation is Copyright (C) 2010 Neil Caldwell & Alexandre Courbot,
distributed under the Creative Commons Attribution-Share Alike 3.0 licence.

Features
--------

- Japanese dictionary based on JMdict.
- Kanji dictionary based on kanjidic2 and KanjiVG.
- Powerful search engine with many search parameters: JLPT level,
  part-of-speech, etc.
- Entries can be marked for study and have a score that evolves as you
  correctly recognize them.
- Training mode for studied entries, where lowest score entries are more likely
  to be trained.
- Lists based on your training history. Get a convenient list of all the
  entries you mistaken today, yesterday or this week! 
- Tagging of entries.
- Annotation of entries.
- Print feature with connections between studied kanji and vocabulary, in a
  format that allows both studying and flash-carding (thumb-hideable Japanese
  part).

Compiling
---------

The only runtime dependency to run Tagaini Jisho is Qt 4.5 or higher. In order
to compile it, you will also need CMake 2.8.1 or higher.

Compilation requires two steps: first, the program is compiled, and second, the
dictionaries it uses are created.

To compile the program, you first need to invoke cmake in order to generate
Makefiles for your platform:

    $ cmake .

Unix users: by default, the program will be installed into /usr/local. You can
change this by setting the `CMAKE_INSTALL_PREFIX` variable when invoking CMake:

    $ cmake . -DCMAKE_INSTALL_PREFIX=/path/to/install/dir

You may want to produce a debug build, especially if you are trying a
development version and want to be able to reports problems. Adding the
`-DCMAKE_BUILD_TYPE=Debug` option to CMake's command line will produce a binary
with debug symbols built-in.

You can also use `ccmake .` after having run CMake to change these options.  It
also features a couple of further debugging options that you can enable in case
you run into troubles.

After your build directory is configured, compilation may be performed. On Unix
systems, invoking make will be enough:

    $ make

This will take some time, especially to generate the dictionaries databases.

Finally, you can install the program to its destination:

    # make install

But you may prefer to generate installers or packages for your platform using
CPack. Mac users can obtain an installable DMG image by running the following
command:

    $ cpack -G DragNDrop

Linux users can make a nice deb or rpm package:

    $ cpack -G DEB
    $ cpack -G RPM

Usage
-----

For a detailed user manual, please see http://www.tagaini.net/manual.

In addition to Qt's standard arguments, Tagaini also supports the following
command-line arguments:

`--temp-db` start the program on an empty, temporary database that will be
removed once the program exits. This is useful for testing new things on a
clean database.

Known bugs ----------
- Kanji stroke order may not always be accurate. Please report incorrect kanji
  to https://bugs.launchpad.net/tagaini-jisho.
- Kanji stroke color segmentation is not always accurate.
- JLPT levels may not always be accurate.

Credits
-------

Tagaini Jisho makes heavy use of the embedded SQLite database. Many thanks to
all its developers for making such a great embedded database available, and for
their kind support.

Qt4 is used as a development framework and ensures portability between Linux,
Mac OS, and Windows.

Words definitions are provided by the
[JMDict](http://www.csse.monash.edu.au/~jwb/jmdict.html). 

Kanji information come from the [kanjidic2
project](http://www.csse.monash.edu.au/~jwb/kanjidic.html).

Kanji components, and stroke animations come from the [KanjiVG
project](http://kanjivg.tagaini.net).

JLPT levels for words come from the [JLPT study
page](http://www.jlptstudy.com/) for level 4, 3, and 2. Level 1 vocabulary was
extracted from lists provided by [Thierry
Bézecourt](http://www.thbz.org/kanjimots/jlpt.php3) and [Alain
Côté](http://jetsdencredujapon.blogspot.com).

Application icon has been contributed by [Philip Seyfi](http://divita.eu/).

Flag images by [Mark James](http://www.famfamfam.com/lab/icons/flags/).

Fugue Icons Copyright (C) 2010 [Yusuke
Kamiyamane](http://p.yusukekamiyamane.com/). All rights reserved.  The icons
are licensed under a [Creative Commons Attribution 3.0
license](http://creativecommons.org/licenses/by/3.0/).

Contact
-------

Website: http://www.tagaini.net

Development, bug reports, feature requests and general questions are handled on
GitHub: https://github.com/Gnurou/tagainijisho
