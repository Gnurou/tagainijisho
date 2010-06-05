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
#ifndef __GUI_ENTRYFORMATTER_H
#define __GUI_ENTRYFORMATTER_H

#include "core/EntriesCache.h"
#include "core/Preferences.h"
#include "gui/DetailedView.h"

#include <QPainter>
#include <QTextCursor>
#include <QMap>

class DetailedView;

class EntryFormatter : public QObject
{
	Q_OBJECT
private:
	static QMap<int, EntryFormatter *> _formatters;

protected:
	/**
	 * Actual implementation of the detailed version rendering, to be overloaded
	 * by subclasses
	 */
	virtual void _detailedVersion(const ConstEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const = 0;
	/**
	 * Paints additional information about this entry, like notes
	 */
	void drawInfo(const ConstEntryPointer &entry, QPainter &painter, QRectF &rectangle, const QFont &textFont = QFont()) const;

	EntryFormatter();
	virtual ~EntryFormatter() {}

public:
	EntryFormatter(const QString& _cssFile = "", const QString& _htmlFile = "", QObject* parent = 0);

	/**
	 * Register a new formatter for an entry type.
	 *
	 * @return true if the formatter has been registered successfully, false if a formatter
	 * was already registered for this type of entry.
	 */
	static bool registerFormatter(const int entryType, EntryFormatter *formatter);
	/**
	 * Remove a previously registered formatter.
	 *
	 * @return true if the formatter has been removed successfully, false if there were no
	 * formatter registered for this type of entry.
	 */
	static bool removeFormatter(const int entryType);
	static const EntryFormatter *getFormatter(const int entryType) { return _formatters[entryType]; }
	static const EntryFormatter *getFormatter(const ConstEntryPointer &entry) { return _formatters[entry->type()]; }

	/**
	 * Writes the entry with as many details as possible using the given cursor. When the
	 * function returns, the cursor must be placed at the last position this method wrote
	 * to.
	 */
	void detailedVersion(const ConstEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const;

	/**
	 * Used by the flashcard trainer in order to write the first part of the quizz.
	 * Eventually CSS elements should be used to hide the relevant parts and this
	 * functions should disappear.
	 */
	virtual void detailedVersionPart1(const ConstEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const {}

	/**
	 * Used by the flashcard trainer in order to write the second part of the quizz.
	 * Eventually CSS elements should be used to hide the relevant parts and this
	 * functions should disappear.
	 */
	virtual void detailedVersionPart2(const ConstEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const {}

	/**
	 * Write a short description with all the fancy and interaction the user
	 * can expect on the detailed view.
	 */
	virtual void writeShortDesc(const ConstEntryPointer &entry, QTextCursor &cursor) const = 0;

	/**
	 * Write a as-short as possible title to identify this entry.
	 */
	virtual void writeEntryTitle(const ConstEntryPointer &entry, QTextCursor &cursor) const;

	/**
	 * Writes the meta data of this entry (tags, training data, notes)
	 */
	void writeUserData(const ConstEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const;	
	
	void autoFormat(const ConstEntryPointer &entry, const QString &str, QTextCursor &cursor, const QTextCharFormat &mergeWith = QTextCharFormat()) const;

	/**
	 * Paints this entry using the given painter into the given rectangle.
	 *
	 * The default version just paints the short version.
	 */
	virtual void draw(const ConstEntryPointer &entry, QPainter &painter, const QRectF &rectangle, QRectF &usedSpace, const QFont &textFont = QFont()) const;
	
	static PreferenceItem<bool> shortDescShowJLPT;
	
	
	
protected:
	QString _css;
	QString _html;
	
	static QString colorTriplet(const QColor &color);

public:
	const QString &CSS() const { return _css; }
	const QString &htmlTemplate() const { return _html; }
	
	QString autoFormat(const QString &str) const;

	/**
	 * Return a as-short as possible title to identify this entry.
	 */
	virtual QString entryTitle(const ConstEntryPointer &entry) const;

	/**
	 * Return a short description with all the fancy and interaction the user
	 * can expect on the detailed view.
	 */
	virtual QString shortDesc(const ConstEntryPointer &entry) const;

	QString buildSubInfoLine(const QString &title, const QString &content) const;
	QString buildSubInfoBlock(const QString &title, const QString &content) const;
	// Background jobs should be added through a HTML comment that is parsed later by the detailed view.

public slots:
	virtual QString formatHeadFurigana(const ConstEntryPointer &entry) const;
	virtual QString formatHead(const ConstEntryPointer &entry) const;
	virtual QString formatLists(const ConstEntryPointer &entry) const;
	virtual QString formatTags(const ConstEntryPointer &entry) const;
	virtual QString formatNotes(const ConstEntryPointer &entry) const;
	virtual QString formatTrainingData(const ConstEntryPointer &entry) const;
	
};

#endif
