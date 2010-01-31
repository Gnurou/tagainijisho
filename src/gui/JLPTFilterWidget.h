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

#ifndef __GUI_JLPTHFILTERWIDGET_H
#define __GUI_JLPTHFILTERWIDGET_H

#include "gui/SearchFilterWidget.h"

#include <QCheckBox>

class JLPTFilterWidget : public SearchFilterWidget {
	Q_OBJECT
private:
	QCheckBox *JLPT4CheckBox, *JLPT3CheckBox, *JLPT2CheckBox, *JLPT1CheckBox;

protected:
	virtual void _reset();

public:
	JLPTFilterWidget(QWidget *parent = 0);
	virtual QString name() const { return "jlptoptions"; }
	virtual QString currentTitle() const;
	virtual QString currentCommand() const;

	/**
	 * Returns the list of currently checked levels. The returned
	 * list contains integers exclusively.
	 */
	QList<QVariant> levels() const;
	/**
	 * Set the list of currently checked levels. The list
	 * must contains integers exclusively.
	 */
	void setLevels(const QList<QVariant> &filter);
	Q_PROPERTY(QList<QVariant> levels READ levels WRITE setLevels)
};

#endif
