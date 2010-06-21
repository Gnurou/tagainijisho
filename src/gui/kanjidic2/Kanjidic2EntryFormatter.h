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
#include "gui/EntryFormatter.h"
#include "gui/DetailedView.h"

class Kanjidic2EntryFormatter : public EntryFormatter
{
	Q_OBJECT
protected:
	Kanjidic2EntryFormatter(QObject *parent = 0);
	virtual ~Kanjidic2EntryFormatter() {}
	
	
	/**
	 * Some components have the shape of a kanji but are actually variations of another one.
	 * This function returns the right shape for a component according to the user's settings.
	 */
	ConstKanjidic2EntryPointer getShapeEntry(const KanjiComponent *comp) const;
	/**
	 * Some components have the shape of a kanji but are actually variations of another one.
	 * This function returns the right meaning for a component according to the user's settings.
	 */
	ConstKanjidic2EntryPointer getMeaningEntry(const KanjiComponent *comp) const;

public:
	static Kanjidic2EntryFormatter &instance();

	virtual QString shortDesc(const ConstEntryPointer &entry) const;
	/**
	 * Variant that takes kanji variations into account.
	 */
	virtual QString shortDesc(const ConstKanjidic2EntryPointer &shape, const ConstKanjidic2EntryPointer &entry) const;
	/**
	 * Shows a tooltip with a short description of the kanji
	 */
	void showToolTip(const ConstKanjidic2EntryPointer entry, const QPoint& pos) const;

	static QString getQueryUsedInWordsSql(int kanji, int limit = maxWordsToDisplay.value(), bool onlyStudied = showOnlyStudiedVocab.value());
	static QString getQueryUsedInKanjiSql(int kanji, int limit = maxCompoundsToDisplay.value(), bool onlyStudied = showOnlyStudiedCompounds.value());

	virtual void draw(const ConstEntryPointer &entry, QPainter &painter, const QRectF &rectangle, QRectF &usedSpace, const QFont &textFont = QFont()) const;
	void drawCustom(const ConstKanjidic2EntryPointer& entry, QPainter& painter, const QRectF& rectangle, QRectF& usedSpace, const QFont& textFont = QFont(), int printSize = printSize.value(), bool printWithFont = printWithFont.value(), bool printMeanings = printMeanings.value(), bool printOnyomi = printOnyomi.value(), bool printKunyomi = printKunyomi.value(), bool printComponents = printComponents.value(), bool printOnlyStudiedComponents = printOnlyStudiedComponents.value(), int maxWordsToPrint = maxWordsToPrint.value(), bool printOnlyStudiedVocab = printOnlyStudiedVocab.value(), bool printStrokesNumbers = printStrokesNumbers.value(), int printStrokesNumbersSize = strokesNumbersSize.value(), bool printGrid = printGrid.value()) const;

	static PreferenceItem<bool> showReadings;
	static PreferenceItem<bool> showNanori;
	static PreferenceItem<bool> showUnicode;
	static PreferenceItem<bool> showSKIP;
	static PreferenceItem<bool> showFourCorner;
	static PreferenceItem<bool> showJLPT;
	static PreferenceItem<bool> showGrade;
	static PreferenceItem<bool> showRadicals;
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
	static PreferenceItem<bool> tooltipShowFourCorner;
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
	static PreferenceItem<bool> printStrokesNumbers;
	static PreferenceItem<int> strokesNumbersSize;
	static PreferenceItem<bool> printGrid;

public slots:
	virtual QString formatMeanings(const ConstEntryPointer &entry) const;
	virtual QString formatOnReadings(const ConstEntryPointer &entry) const;
	virtual QString formatKunReadings(const ConstEntryPointer &entry) const;
	virtual QString formatNanori(const ConstEntryPointer &entry) const;
	virtual QString formatStrokesCount(const ConstEntryPointer &entry) const;
	virtual QString formatFrequency(const ConstEntryPointer &entry) const;
	virtual QString formatGrade(const ConstEntryPointer &entry) const;
	virtual QString formatJLPT(const ConstEntryPointer &entry) const;
	virtual QString formatVariations(const ConstEntryPointer &entry) const;
	virtual QString formatVariationsOf(const ConstEntryPointer &entry) const;
	virtual QString formatUnicode(const ConstEntryPointer &entry) const;
	virtual QString formatSkip(const ConstEntryPointer &entry) const;
	virtual QString formatFourCorner(const ConstEntryPointer &entry) const;
	virtual QString formatRadicals(const ConstEntryPointer &entry) const;
	virtual QString formatComponents(const ConstEntryPointer &entry) const;

	virtual QList<DetailedViewJob *> jobUsedInKanji(const ConstEntryPointer &_entry, const QTextCursor &cursor) const;
	virtual QList<DetailedViewJob *> jobUsedInWords(const ConstEntryPointer &_entry, const QTextCursor &cursor) const;
};

class ShowUsedInKanjiJob : public DetailedViewJob {
	Q_DECLARE_TR_FUNCTIONS(ShowUsedInJob)
private:
	QString _kanji;
	bool gotResults;
public:
	ShowUsedInKanjiJob(const QString &kanji, const QTextCursor &cursor);
	virtual void firstResult();
	virtual void result(EntryPointer entry);
	virtual void completed();
};

class ShowUsedInWordsJob : public DetailedViewJob {
	Q_DECLARE_TR_FUNCTIONS(ShowUsedInWordsJob)
private:
	QString _kanji;
	bool gotResults;
public:
	ShowUsedInWordsJob(const QString &kanji, const QTextCursor &cursor);
	virtual void firstResult();
	virtual void result(EntryPointer entry);
	virtual void completed();
};

#endif
