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
#include "EntryFormatter.h"
#include "DetailedView.h"

class JMdictEntryFormatter : public EntryFormatter
{
protected:
	virtual void _detailedVersion(const Entry *entry, QTextCursor &cursor, DetailedView *view) const;

public:
	JMdictEntryFormatter();
	virtual ~JMdictEntryFormatter() {}

	/**
	 * Writes the part of speech using the given cursor.
	 */
	void writeSensePos(const Sense &sense, QTextCursor &cursor) const;

	void writeKanaHeader(const JMdictEntry *entry, QTextCursor &cursor, DetailedView *view) const;
	void writeKanjiHeader(const JMdictEntry *entry, QTextCursor &cursor, DetailedView *view) const;
	void writeJapanese(const JMdictEntry *entry, QTextCursor &cursor, DetailedView *view) const;
	void writeTranslation(const JMdictEntry *entry, QTextCursor &cursor, DetailedView *view) const;
	void writeEntryInfo(const JMdictEntry *entry, QTextCursor &cursor, DetailedView *view) const;

	virtual void writeShortDesc(const Entry *entry, QTextCursor &cursor) const;
	virtual void detailedVersionPart1(const Entry *entry, QTextCursor &cursor, DetailedView *view) const;
	virtual void detailedVersionPart2(const Entry *entry, QTextCursor &cursor, DetailedView *view) const;
	virtual void draw(const Entry *entry, QPainter &painter, const QRectF &rectangle, QRectF &usedSpace, const QFont &textFont = QFont(), int headerPrintSize = JMdictEntryFormatter::headerPrintSize.defaultValue(), bool printKanjis = JMdictEntryFormatter::printKanjis.defaultValue(), bool printOnlyStudiedKanjis = JMdictEntryFormatter::printOnlyStudiedKanjis.defaultValue(), int maxDefinitionsToPrint = JMdictEntryFormatter::maxDefinitionsToPrint.defaultValue()) const;

	static const QString queryFindVerbBuddySql;
	static const QString queryFindHomonymsSql;

	static PreferenceItem<bool> showJLPT;
	static PreferenceItem<bool> showKanjis;
	static PreferenceItem<bool> showKanjisJLPT;
	static PreferenceItem<bool> searchVerbBuddy;
	static PreferenceItem<int> maxHomophonesToDisplay;
	static PreferenceItem<bool> displayStudiedHomophonesOnly;

	static PreferenceItem<int> headerPrintSize;
	static PreferenceItem<bool> printKanjis;
	static PreferenceItem<bool> printOnlyStudiedKanjis;
	static PreferenceItem<int> maxDefinitionsToPrint;

	static QString getVerbBuddySql(const QString &matchPattern, JMdictPosTagType pos, int id);
	static QString getHomophonesSql(const QString &reading, int id, int maxToDisplay = maxHomophonesToDisplay.value(), bool studiedOnly = displayStudiedHomophonesOnly.value());
};

class FindVerbBuddyJob : public DetailedViewJob {
	Q_DECLARE_TR_FUNCTIONS(FindVerbBuddyJob)
private:
	EntryPointer<Entry> bestMatch;
	int lastKanjiPos;
	int initialLength;
	JMdictPosTagType searchedPos;
	QString matchPattern;
	QString kanaPattern;
	QString firstReading;

public:
	FindVerbBuddyJob(const JMdictEntry *verb, JMdictPosTagType pos, const QTextCursor &cursor);
	virtual void result(EntryPointer<Entry> entry);
	virtual void completed();
};

class FindHomonymsJob : public DetailedViewJob {
	Q_DECLARE_TR_FUNCTIONS(FindHomonymsJob)
public:
	FindHomonymsJob(const JMdictEntry *entry, int maxToDisplay, bool studiedOnly, const QTextCursor &cursor);
	virtual void firstResult();
	virtual void result(EntryPointer<Entry> entry);
	virtual void completed();
};

#endif
