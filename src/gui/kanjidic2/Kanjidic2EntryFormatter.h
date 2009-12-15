/*
 *  Copyright (C) 2009  Alexandre Courbot
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __GUI_KANJIDIC2_ENTRYFORMATTER_H
#define __GUI_KANJIDIC2_ENTRYFORMATTER_H

#include "core/kanjidic2/Kanjidic2Entry.h"
#include "core/EntriesCache.h"
#include "EntryFormatter.h"
#include "DetailedView.h"

class Kanjidic2EntryFormatter : public EntryFormatter
{
Q_DECLARE_TR_FUNCTIONS(EntryFormatter)
protected:
	virtual void _detailedVersion(const Entry *entry, QTextCursor &cursor, DetailedView *view) const;

public:
	Kanjidic2EntryFormatter();
	virtual ~Kanjidic2EntryFormatter() {}

	void writeJapanese(const Kanjidic2Entry *entry, QTextCursor &cursor, DetailedView *view) const;
	void writeTranslation(const Kanjidic2Entry *entry, QTextCursor &cursor, DetailedView *view) const;
	void writeKanjiInfo(const Kanjidic2Entry *entry, QTextCursor &cursor, DetailedView *view) const;

	virtual void writeShortDesc(const Entry *entry, QTextCursor &cursor) const;
	virtual void draw(const Entry *entry, QPainter &painter, const QRectF &rectangle, QRectF &usedSpace, const QFont &textFont = QFont()) const;
	void drawCustom(const Entry *entry, QPainter &painter, const QRectF &rectangle, QRectF &usedSpace, const QFont &textFont = QFont(), int printSize = Kanjidic2EntryFormatter::printSize.value(), bool printWithFont = Kanjidic2EntryFormatter::printWithFont.value(), bool printMeanings = Kanjidic2EntryFormatter::printMeanings.value(), bool printOnyomi = Kanjidic2EntryFormatter::printOnyomi.value(), bool printKunyomi = Kanjidic2EntryFormatter::printKunyomi.value(), bool printComponents = Kanjidic2EntryFormatter::printComponents.value(), bool printOnlyStudiedComponents = Kanjidic2EntryFormatter::printOnlyStudiedComponents.value(), int maxWordsToPrint = Kanjidic2EntryFormatter::maxWordsToPrint.value(), bool printOnlyStudiedVocab = Kanjidic2EntryFormatter::printOnlyStudiedVocab.value()) const;
	virtual void detailedVersionPart1(const Entry *entry, QTextCursor &cursor, DetailedView *view) const;
	virtual void detailedVersionPart2(const Entry *entry, QTextCursor &cursor, DetailedView *view) const;

	/**
	 * Shows a tooltip with a short description of the kanji
	 */
	void showToolTip(const Kanjidic2Entry *entry, const QPoint &pos) const;

	static QString getQueryUsedInWordsSql(int kanji, int limit = maxWordsToDisplay.value(), bool onlyStudied = showOnlyStudiedVocab.value());
	static QString getQueryUsedInKanjiSql(int kanji, int limit = maxCompoundsToDisplay.value(), bool onlyStudied = showOnlyStudiedCompounds.value());

	static PreferenceItem<bool> showReadings;
	static PreferenceItem<bool> showNanori;
	static PreferenceItem<bool> showUnicode;
	static PreferenceItem<bool> showSKIP;
	static PreferenceItem<bool> showJLPT;
	static PreferenceItem<bool> showGrade;
	static PreferenceItem<bool> showComponents;
	static PreferenceItem<bool> showStrokesNumber;
	static PreferenceItem<bool> showFrequency;
	static PreferenceItem<bool> showVariations;
	static PreferenceItem<bool> showVariationOf;
	static PreferenceItem<int> maxWordsToDisplay;
	static PreferenceItem<bool> showOnlyStudiedVocab;
	static PreferenceItem<int> maxCompoundsToDisplay;
	static PreferenceItem<bool> showOnlyStudiedCompounds;

	static PreferenceItem<bool> tooltipShowScore;
	static PreferenceItem<bool> tooltipShowUnicode;
	static PreferenceItem<bool> tooltipShowSKIP;
	static PreferenceItem<bool> tooltipShowJLPT;
	static PreferenceItem<bool> tooltipShowGrade;
	static PreferenceItem<bool> tooltipShowStrokesNumber;
	static PreferenceItem<bool> tooltipShowFrequency;

	static PreferenceItem<int> printSize;
	static PreferenceItem<bool> printWithFont;
	static PreferenceItem<bool> printMeanings;
	static PreferenceItem<bool> printOnyomi;
	static PreferenceItem<bool> printKunyomi;
	static PreferenceItem<bool> printComponents;
	static PreferenceItem<bool> printOnlyStudiedComponents;
	static PreferenceItem<bool> printOnlyStudiedVocab;
	static PreferenceItem<int> maxWordsToPrint;
};

class ShowUsedInKanjiJob : public DetailedViewJob {
	Q_DECLARE_TR_FUNCTIONS(ShowUsedInJob)
private:
	QString _kanji;
	bool _gotResult;
public:
	ShowUsedInKanjiJob(const QString &kanji, const QTextCursor &cursor);
	virtual void firstResult();
	virtual void result(EntryPointer<Entry> entry);
	virtual void completed();
};

class ShowUsedInWordsJob : public DetailedViewJob {
	Q_DECLARE_TR_FUNCTIONS(ShowUsedInWordsJob)
private:
	QString _kanji;
	bool _gotResult;
public:
	ShowUsedInWordsJob(const QString &kanji, const QTextCursor &cursor);
	virtual void firstResult();
	virtual void result(EntryPointer<Entry> entry);
	virtual void completed();
};

#endif
