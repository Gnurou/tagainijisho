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

#ifndef __GUI_STUDYFILTERWIDGET_H
#define __GUI_STUDYFILTERWIDGET_H

#include "gui/SearchFilterWidget.h"
#include "gui/RelativeDateEdit.h"

#include <QRadioButton>
#include <QSpinBox>
#include <QGroupBox>

class StudyFilterWidget : public SearchFilterWidget {
	Q_OBJECT
public:
	typedef enum { All = 0, Studied = 1, NonStudied = 2 } StudyFilter;

protected:
	virtual void _reset();

private:
	QRadioButton *allEntriesButton, *studiedEntriesButton, *nonStudiedEntriesButton;
	QSpinBox *minSpinBox, *maxSpinBox;

	QGroupBox *studyBox;
	RelativeDateEdit *_studyMinDate, *_studyMaxDate;

	QGroupBox *trainBox;
	RelativeDateEdit *_trainMinDate, *_trainMaxDate;

	QGroupBox *mistakeBox;
	RelativeDateEdit *_mistakenMinDate, *_mistakenMaxDate;

public:
	StudyFilterWidget(QWidget *parent = 0);
	virtual QString name() const { return "studyoptions"; }
	virtual QString currentTitle() const;
	virtual QString currentCommand() const;

	int studyState() const;
	void setStudyState(int state);
	Q_PROPERTY(int studyState READ studyState WRITE setStudyState)

	int studyMinScore() const { return minSpinBox->value(); }
	void setStudyMinScore(int value) { minSpinBox->setValue(value); }
	Q_PROPERTY(int studyMinScore READ studyMinScore WRITE setStudyMinScore)
	int studyMaxScore() const { return maxSpinBox->value(); }
	void setStudyMaxScore(int value) { maxSpinBox->setValue(value); }
	Q_PROPERTY(int studyMaxScore READ studyMaxScore WRITE setStudyMaxScore)

	QString studyMinDate() const { return _studyMinDate->dateString(); }
	void setStudyMinDate(const QString &date) { _studyMinDate->setDateString(date); }
	Q_PROPERTY(QString studyMinDate READ studyMinDate WRITE setStudyMinDate)
	QString studyMaxDate() const { return _studyMaxDate->dateString(); }
	void setStudyMaxDate(const QString &date) { _studyMaxDate->setDateString(date); }
	Q_PROPERTY(QString studyMaxDate READ studyMaxDate WRITE setStudyMaxDate)

	QString trainMinDate() const { return _trainMinDate->dateString(); }
	void setTrainMinDate(const QString &date) { _trainMinDate->setDateString(date); }
	Q_PROPERTY(QString trainMinDate READ trainMinDate WRITE setTrainMinDate)
	QString trainMaxDate() const { return _trainMaxDate->dateString(); }
	void setTrainMaxDate(const QString &date) { _trainMaxDate->setDateString(date); }
	Q_PROPERTY(QString trainMaxDate READ trainMaxDate WRITE setTrainMaxDate)

	QString mistakenMinDate() const { return _mistakenMinDate->dateString(); }
	void setMistakenMinDate(const QString &date) { _mistakenMinDate->setDateString(date); }
	Q_PROPERTY(QString mistakenMinDate READ mistakenMinDate WRITE setMistakenMinDate)
	QString mistakenMaxDate() const { return _mistakenMaxDate->dateString(); }
	void setMistakenMaxDate(const QString &date) { _mistakenMaxDate->setDateString(date); }
	Q_PROPERTY(QString mistakenMaxDate READ mistakenMaxDate WRITE setMistakenMaxDate)

private slots:
	/**
	 * This slot is used whenever a radio button is toggled
	 * and emits the commandUpdated() signal only when the
	 * button is checked. This avoids emitting two searches
	 * every time a button is toggled.
	 */
	void onRadioButtonToggled(bool toggled);

protected slots:
	void checkMinSpinBoxValue(int newMaxValue);
	void checkMaxSpinBoxValue(int newMinValue);
};

#endif
