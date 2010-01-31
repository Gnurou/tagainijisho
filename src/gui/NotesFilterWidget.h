/*
 *  Copyright (C) 2009/2010  Alexandre Courbot
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

#ifndef __GUI_NOTESFILTERWIDGET_H
#define __GUI_NOTESFILTERWIDGET_H

#include "gui/SearchFilterWidget.h"

#include <QLineEdit>

class NotesFilterWidget : public SearchFilterWidget {
	Q_OBJECT
private:
	QLineEdit *words;

private slots:
	void onTextChanged(const QString &newText);

protected:
	virtual void _reset();
	void focusInEvent(QFocusEvent *event);

public:
	NotesFilterWidget(QWidget *parent = 0);
	virtual QString name() const { return "notessearch"; }
	virtual QString currentTitle() const;
	virtual QString currentCommand() const;

	QString notes() const { return words->text(); }
	void setNotes(const QString &notes);
	Q_PROPERTY(QString notes READ notes WRITE setNotes)
};

#endif
