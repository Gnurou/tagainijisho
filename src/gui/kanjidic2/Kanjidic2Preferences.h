/*
 *  Copyright (C) 2008, 2009  Alexandre Courbot
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

#ifndef __GUI_KANJIDIC2_PREFERENCES_H
#define __GUI_KANJIDIC2_PREFERENCES_H

#include "core/kanjidic2/Kanjidic2Entry.h"
#include "core/EntriesCache.h"
#include "gui/PreferencesWindow.h"
#include "gui/ui_Kanjidic2Preferences.h"

#include <QPicture>

class KanjiPlayer;
class Kanjidic2Preferences : public PreferencesWindowCategory, private Ui::Kanjidic2Preferences
{
	Q_OBJECT
private:
	KanjiPlayer *_player;
	QPicture previewPic;
	EntryPointer<const Entry> _previewEntry;
	const Kanjidic2Entry *previewEntry;

protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);

protected slots:
	void updatePrintPreview();
	void onAnimSpeedDefaultChecked(bool checked);
	void onAnimDelayDefaultChecked(bool checked);
	void onSizeChanged(int newSize);

public slots:
	void applySettings();
	void refresh();

public:
	Q_INVOKABLE Kanjidic2Preferences(QWidget *parent = 0);
};

#endif
