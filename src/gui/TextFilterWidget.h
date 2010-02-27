/*
 *  Copyright (C) 2008/2009/2010  Alexandre Courbot
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

#ifndef __GUI_TEXTFILTERWIDGET_H
#define __GUI_TEXTFILTERWIDGET_H

#include "gui/SearchFilterWidget.h"
#include "core/Preferences.h"

#include <QComboBox>
#include <QLineEdit>
#include <QToolButton>
#include <QPushButton>
#include <QAction>

class TextFilterWidget : public SearchFilterWidget
{
	Q_OBJECT
private:
	QComboBox *_searchField;
	QToolButton *resetText;
	QPushButton *searchButton;	
	bool _clipboardEnabled;
	bool _reseted;
	QAction *_enableClipboardInputAction;
	
private slots:
	void searchButtonClicked();
	void onItemSelected(int item);
	void resetSearchText();
	void onSearchTextChanged(const QString &text);
	void onClipboardChanged();
	void onClipboardSelectionChanged();
	void enableClipboardInput(bool enable);

protected:
	virtual void _reset();
	
public:
	TextFilterWidget(QWidget *parent = 0);
	virtual QString name() const { return "searchtext"; }
	virtual QString currentTitle() const;
	virtual QString currentCommand() const { return text(); }
	QComboBox *searchField() { return _searchField; }
	
	QString text() const { return _searchField->lineEdit()->text(); }
	void setText(const QString &text) { _searchField->lineEdit()->setText(text); }
	Q_PROPERTY(QString text READ text WRITE setText)
	
	static PreferenceItem<int> textSearchHistorySize;
	QAction *enableClipboardInputAction() { return _enableClipboardInputAction; }
};

#endif
