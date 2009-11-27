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

#ifndef __GUI_JMDICT_PREFERENCES_H
#define __GUI_JMDICT_PREFERENCES_H

#include "core/jmdict/JMdictEntry.h"
#include "gui/PreferencesWindow.h"
#include "gui/ui_JMdictPreferences.h"

#include <QPicture>

class JMdictPreferences : public PreferencesWindowCategory, private Ui::JMdictPreferences
{
	Q_OBJECT
private:
	QPicture previewPic;
	EntryPointer<const Entry> _previewEntry;
	const JMdictEntry *previewEntry;

protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);

protected slots:
	void onFilterButtonClicked();
	void onUnFilterButtonClicked();

public slots:
	void applySettings();
	void refresh();
	void updatePrintPreview();

public:
	Q_INVOKABLE JMdictPreferences(QWidget *parent = 0);
};

#endif
