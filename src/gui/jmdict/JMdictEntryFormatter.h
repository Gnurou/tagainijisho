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

#ifndef __GUI_JMDICT_ENTRYFORMATTER_H
#define __GUI_JMDICT_ENTRYFORMATTER_H

#include "core/jmdict/JMdictEntry.h"
#include "core/EntriesCache.h"
#include "gui/EntryFormatter.h"
#include "gui/DetailedView.h"

class JMdictEntryFormatter : public EntryFormatter
{
	Q_OBJECT
protected:
	virtual void _detailedVersion(const ConstEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const;

	JMdictEntryFormatter(QObject *parent = 0);
	virtual ~JMdictEntryFormatter() {}
public:
	static JMdictEntryFormatter &instance();
	
	/**
	 * Writes the part of speech using the given cursor.
	 */
	void writeSensePos(const Sense &sense, QTextCursor &cursor) const;

	void writeKanaHeader(const ConstJMdictEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const;
	void writeKanjiHeader(const ConstJMdictEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const;
	void writeJapanese(const ConstJMdictEntryPointer &entry, QTextCursor& cursor, DetailedView* view) const;
	void writeTranslation(const ConstJMdictEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const;
	void writeEntryInfo(const ConstJMdictEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const;

	virtual void writeShortDesc(const ConstEntryPointer &entry, QTextCursor &cursor) const;
	virtual void detailedVersionPart1(const ConstEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const;
	virtual void detailedVersionPart2(const ConstEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const;
	virtual void draw(const ConstEntryPointer &entry, QPainter &painter, const QRectF &rectangle, QRectF &usedSpace, const QFont &textFont = QFont()) const { drawCustom(entry, painter, rectangle, usedSpace, textFont); }
	void drawCustom(const ConstEntryPointer &entry, QPainter &painter, const QRectF &rectangle, QRectF &usedSpace, const QFont &textFont = QFont(), int headerPrintSize = headerPrintSize.defaultValue(), bool printKanjis = printKanjis.defaultValue(), bool printOnlyStudiedKanjis = printOnlyStudiedKanjis.defaultValue(), int maxDefinitionsToPrint = maxDefinitionsToPrint.defaultValue()) const;

	static PreferenceItem<bool> showJLPT;
	static PreferenceItem<bool> showKanjis;
	static PreferenceItem<bool> searchVerbBuddy;
	static PreferenceItem<int> maxHomophonesToDisplay;
	static PreferenceItem<bool> displayStudiedHomophonesOnly;
	static PreferenceItem<int> maxHomographsToDisplay;
	static PreferenceItem<bool> displayStudiedHomographsOnly;

	static PreferenceItem<int> headerPrintSize;
	static PreferenceItem<bool> printKanjis;
	static PreferenceItem<bool> printOnlyStudiedKanjis;
	static PreferenceItem<int> maxDefinitionsToPrint;

	static QString getVerbBuddySql(const QString &matchPattern, quint64 pos, int id);
	static QString getHomophonesSql(const QString &reading, int id, int maxToDisplay = maxHomophonesToDisplay.value(), bool studiedOnly = displayStudiedHomophonesOnly.value());
	static QString getHomographsSql(const QString &writing, int id, int maxToDisplay = maxHomophonesToDisplay.value(), bool studiedOnly = displayStudiedHomophonesOnly.value());
	
public slots:
	virtual QString formatHeadFurigana(const ConstEntryPointer &entry) const;
	virtual QString formatHead(const ConstEntryPointer &entry) const;
	virtual QString formatAltReadings(const ConstEntryPointer &entry) const;
	virtual QString formatAltWritings(const ConstEntryPointer &entry) const;
	virtual QString formatSenses(const ConstEntryPointer &entry) const;
	virtual QString formatJLPT(const ConstEntryPointer &entry) const;
	virtual QString formatKanji(const ConstEntryPointer &entry) const;
};

class FindVerbBuddyJob : public DetailedViewJob {
	Q_DECLARE_TR_FUNCTIONS(FindVerbBuddyJob)
private:
	ConstEntryPointer bestMatch;
	int lastKanjiPos;
	int initialLength;
	QString searchedPos;
	QString matchPattern;
	QString kanaPattern;
	QString firstReading;

public:
	FindVerbBuddyJob(const ConstJMdictEntryPointer &verb, const QString &pos, const QTextCursor &cursor);
	virtual void result(EntryPointer entry);
	virtual void completed();
};

class FindHomophonesJob : public DetailedViewJob {
	Q_DECLARE_TR_FUNCTIONS(FindHomophonesJob)
public:
	FindHomophonesJob(const ConstJMdictEntryPointer &entry, int maxToDisplay, bool studiedOnly, const QTextCursor &cursor);
	virtual void firstResult();
	virtual void result(EntryPointer entry);
};

class FindHomographsJob : public DetailedViewJob {
	Q_DECLARE_TR_FUNCTIONS(FindHomographsJob)
public:
	FindHomographsJob(const ConstJMdictEntryPointer &entry, int maxToDisplay, bool studiedOnly, const QTextCursor &cursor);
	virtual void firstResult();
	virtual void result(EntryPointer entry);
};

#endif
