# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Tagaini Jisho is a Japanese dictionary and learning assistant application built with Qt6. It features vocabulary lookup from JMdict, kanji information from kanjidic2/KanjiVG, study tracking with scores, tagging, and flashcard training.

## Build Commands

```bash
# Configure (out-of-source build recommended)
mkdir build && cd build
cmake .. -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=1

# Build
ninja

# Run in-place (from build directory)
./src/gui/tagainijisho

# Run with empty temporary database (for testing)
./src/gui/tagainijisho --temp-db

# Format code
ninja format
```

### CMake Options

- `-DCMAKE_BUILD_TYPE=Debug` - Debug build with symbols
- `-DCMAKE_INSTALL_PREFIX=/usr` - Change install prefix
- `-DDICT_LANG="fr;de;es"` - Specify dictionary languages (semicolon-separated 2-letter codes)
- Debug options: `DEBUG_ENTRIES_CACHE`, `DEBUG_PATHS`, `DEBUG_QUERIES`, `DEBUG_TRANSACTIONS`

## Architecture

### Directory Structure

- `src/core/` - Core data layer (entries, searching, database, plugins)
- `src/gui/` - Qt GUI layer (widgets, views, formatters)
- `src/sqlite/` - SQLite wrapper classes
- `src/core/jmdict/` - JMdict vocabulary dictionary plugin
- `src/core/kanjidic2/` - Kanjidic2 kanji dictionary plugin
- `src/gui/jmdict/` - JMdict GUI components
- `src/gui/kanjidic2/` - Kanjidic2 GUI components (including stroke animation)

### Plugin System

The application uses a plugin architecture for dictionary types:

- `Plugin` (base class in `core/Plugin.h`) - Registration and lifecycle
- Each dictionary type (JMdict, Kanjidic2) has:
  - Core plugin: `*Plugin.cc` - Database setup, queries
  - Entry class: `*Entry.cc` - Data model
  - Entry loader: `*EntryLoader.cc` - Database loading
  - Entry searcher: `*EntrySearcher.cc` - Search functionality
  - GUI plugin: `*GUIPlugin.cc` - UI integration
  - Entry formatter: `*EntryFormatter.cc` - Display formatting

### Entry System

- `Entry` (base class) - Common entry properties (id, type, tags, notes, training score)
- `JMdictEntry` - Vocabulary entry with readings, meanings, parts of speech
- `Kanjidic2Entry` - Kanji entry with strokes, components, radicals, readings
- Entries are reference-counted via `QSharedPointer` (`EntryPointer`, `ConstEntryPointer`)
- `EntriesCache` manages loaded entries to avoid duplication

### Kanji Rendering

- `KanjiRenderer` - Renders kanji strokes from SVG path data
- `KanjiPlayer` - Animated stroke order display widget
- `KanjiStroke` - Individual stroke path data
- `KanjiComponent` - Kanji component groupings for semantic breakdown

## Code style and layout

The project is very old (started in 2008), so the C++ style is out-of-date. Improve the code to use more safe and modern patterns whenever possible, especially if this reduces the code complexity and size.

## Commit logs format

Unless the change is in a root file, or several modules are affected, always prefix each commit title with the module it affects, e.g. `core: do something` or `github: fix Windows build`.

Use short but descriptive commit logs, avoiding bullet points unless they absolutely make sense.
