/*
 *  Copyright (C) 2008, 2009, 2010  Alexandre Courbot
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

#ifndef __GUI_ENTRYDELEGATE_H
#define __GUI_ENTRYDELEGATE_H

#include "core/EntriesCache.h"
#include <QStyledItemDelegate>

class EntryDelegateLayout : public QObject
{
	Q_OBJECT
public:
	typedef enum { DefaultText = 0, Kana, Kanji, MAX_FONTS } FontRole;
	typedef enum { OneLine = 0, TwoLines, MAX_MODES } DisplayMode;
	
private:
	QFont _font[MAX_FONTS];
	DisplayMode _displayMode;

	void _fontsChanged();
	QFont _defaultFont(FontRole role) const;

public:
	EntryDelegateLayout(QObject* parent = 0, EntryDelegateLayout::DisplayMode displayMode = OneLine, const QString& textFont = "", const QString& kanjiFont = "", const QString& kanaFont = "");
	const QFont &font(FontRole role) const { return _font[role]; }
	const QFont &textFont() const { return _font[DefaultText]; }
	const QFont &kanaFont() const { return _font[Kana]; }
	const QFont &kanjiFont() const { return _font[Kanji]; }
	DisplayMode displayMode() const { return _displayMode; }

	void setFont(FontRole role, const QFont &font);
	void setDisplayMode(DisplayMode mode);
	void setDisplayModeInt(int mode) { setDisplayMode(static_cast<DisplayMode>(mode)); }
	Q_PROPERTY(int displayMode READ displayMode WRITE setDisplayModeInt);
	QString getKanjiFont() const { return font(Kanji).toString(); }
	void setKanjiFont(const QString &font) { QFont f; if (!font.isEmpty()) f.fromString(font); setFont(Kanji, f); }
	Q_PROPERTY(QString kanjiFont READ getKanjiFont WRITE setKanjiFont);
	QString getKanaFont() const { return font(Kana).toString(); }
	void setKanaFont(const QString &font) { QFont f; if (!font.isEmpty()) f.fromString(font); setFont(Kana, f); }
	Q_PROPERTY(QString kanaFont READ getKanaFont WRITE setKanaFont);
	QString getTextFont() const { return font(DefaultText).toString(); }
	void setTextFont(const QString &font) { QFont f; if (!font.isEmpty()) f.fromString(font); setFont(DefaultText, f); }
	Q_PROPERTY(QString textFont READ getTextFont WRITE setTextFont);

public slots:
	void updateConfig(const QVariant &value);
	
signals:
	void layoutHasChanged();
};

class EntryDelegate : public QStyledItemDelegate
{
	Q_OBJECT
protected:
	EntryDelegateLayout *layout;
	QPixmap _tagsIcon, _notesIcon, _listsIcon;
	/**
	 * Used to prevent displaying some icons in views where they are implicit, e.g. list views
	 */
	quint8 _hiddenIcons;

public:
	EntryDelegate(EntryDelegateLayout *dLayout, QObject *parent = 0);
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const;
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	
	static const quint8 TAGS_ICON = 1;
	static const quint8 NOTES_ICON = 2;
	static const quint8 LISTS_ICON = 3;

	bool isHidden(quint8 icon) const { return _hiddenIcons & icon; }
	void setHidden(quint8 icon, bool hide) { _hiddenIcons = (hide ? _hiddenIcons | icon : _hiddenIcons & ~icon); }
};

#endif
