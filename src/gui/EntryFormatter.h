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
#include <QMap>

class DetailedView;

class EntryFormatter : public QObject
{
	Q_OBJECT
private:
	static QMap<int, EntryFormatter *> _formatters;

protected:
	QString _css;
	QString _html;

	/**
	 * Paints additional information about this entry, like notes
	 */
	void drawInfo(const ConstEntryPointer &entry, QPainter &painter, QRectF &rectangle, const QFont &textFont = QFont()) const;

	EntryFormatter(const QString& _cssFile = "", const QString& _htmlFile = "", QObject* parent = 0);
	virtual ~EntryFormatter() {}

	typedef enum { Romaji, Kanji, Kana } formatType;
	QString formatString(const QString &str, formatType type) const;

public:
	const QString &CSS() const { return _css; }
	const QString &htmlTemplate() const { return _html; }
	
	/// Returns the color associated to the score of this entry
	static QColor scoreColor(const Entry &entry);
	static QString colorTriplet(const QColor &color);
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
	/**
	 * Build a line using the given title and content.
	 */
	static QString buildSubInfoLine(const QString &title, const QString &content);
	/**
	 * Build a block using the given title and content.
	 */
	static QString buildSubInfoBlock(const QString &title, const QString &content);

	/**
	 * Paints this entry using the given painter into the given rectangle.
	 *
	 * The default version just paints the short version.
	 */
	virtual void draw(const ConstEntryPointer &entry, QPainter &painter, const QRectF &rectangle, QRectF &usedSpace, const QFont &textFont = QFont()) const;

	static PreferenceItem<bool> shortDescShowJLPT;

public slots:
	virtual QString formatHeadFurigana(const ConstEntryPointer &entry) const;
	virtual QString formatHead(const ConstEntryPointer &entry) const;
	virtual QString formatLists(const ConstEntryPointer &entry) const;
	virtual QString formatTags(const ConstEntryPointer &entry) const;
	virtual QString formatNotes(const ConstEntryPointer &entry) const;
	virtual QString formatTrainingData(const ConstEntryPointer &entry) const;

public:
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
};

#endif
