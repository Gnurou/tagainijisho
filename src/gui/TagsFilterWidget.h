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

#ifndef __GUI_TAGSFILTERWIDGET_H
#define __GUI_TAGSFILTERWIDGET_H

#include "gui/SearchFilterWidget.h"
#include "gui/TagsDialogs.h"

#include <QPushButton>

class TagsFilterWidget : public SearchFilterWidget {
	Q_OBJECT
private:
	TagsLineInput *lineInput;
	QPushButton *tagsButton;

private slots:
	void populateTagsMenu();

protected:
	virtual void _reset();

public:
	TagsFilterWidget(QWidget *parent = 0);
	virtual QString name() const { return "tagssearch"; }
	virtual QString currentTitle() const;
	virtual QString currentCommand() const;

	QString tags() const { return lineInput->text(); }
	void setTags(const QString &tags);
	Q_PROPERTY(QString tags READ tags WRITE setTags)

public slots:
	void tagMenuClicked(QAction *action);
};

#endif
