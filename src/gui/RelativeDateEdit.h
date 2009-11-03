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

#ifndef __RELATIVE_DATE_EDIT_H_
#define __RELATIVE_DATE_EDIT_H_

#include "core/RelativeDate.h"
#include "gui/ElidedPushButton.h"

#include <QPushButton>
#include <QMenu>
#include <QActionGroup>
#include <QCalendarWidget>
#include <QFrame>
#include <QRadioButton>
#include <QSpinBox>
#include <QComboBox>

class RelativeDatePopup : public QFrame {
	Q_OBJECT
friend class RelativeDateEdit;
private:
	QRadioButton *_notSetButton;
	QSpinBox *_counterBox;
	QComboBox *_whatBox;
	QCalendarWidget *_calendar;
	RelativeDate &_date;

public:
	RelativeDatePopup(RelativeDate &date, QWidget *parent = 0);
	void show();

private slots:
	void onNotSetSelected();
	void onRelativeDateSelected();
	void onAbsoluteDateSelected(const QDate &date);

public slots:
	/**
	 * Synchronizes the state of widgets with the date
	 */
	void sync();

signals:
	void dateSelected();
};

class RelativeDateEdit : public ElidedPushButton<QPushButton>
{
	Q_OBJECT
private:
	RelativeDate _date;
	RelativeDatePopup *_popup;

public:
	RelativeDateEdit(QWidget *parent = 0);

	QString dateString() const { return _date.dateString(); }
	void setDateString(const QString &string) { _date.setDateString(string); updateButtonTitle(); _popup->sync(); emit dateChanged(_date); }
	QString translatedDateString() const { return _date.translatedDateString(); }

	const RelativeDate &date() const { return _date; }

	virtual bool eventFilter(QObject *obj, QEvent *event);
	virtual void wheelEvent(QWheelEvent *event);

protected slots:
	void togglePopup(bool status);
	void onDateSelected();
	void updateButtonTitle();

signals:
	/**
	 * Indicates the user clearly selected a date
	 */
	void dateSelected(const RelativeDate &date);

	/**
	 * Indicates the date has changed, but may be changed again in
	 * a short amount of time (e.g. wheel scroll)
	 */
	void dateChanged(const RelativeDate &date);
};

#endif
