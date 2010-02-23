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

#include "StudyFilterWidget.h"

#include <QGridLayout>
#include <QSqlQuery>
#include <QLabel>

StudyFilterWidget::StudyFilterWidget(QWidget *parent) : SearchFilterWidget(parent)
{
	_propsToSave << "studyState" << "studyMinScore" << "studyMaxScore" << "studyMinDate" << "studyMaxDate" << "trainMinDate" << "trainMaxDate" <<"mistakenMinDate" << "mistakenMaxDate";

	scoreBox = new QGroupBox(tr("Score"), this);
	scoreBox->setEnabled(false);
	scoreBox->setVisible(false);
	{
		QSlider *minSlider = new QSlider(scoreBox);
		minSlider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		minSlider->setRange(0, 100);
		minSlider->setOrientation(Qt::Horizontal);
		minSlider->setMinimumWidth(50);
		QSlider *maxSlider = new QSlider(scoreBox);
		maxSlider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		maxSlider->setRange(0, 100);
		maxSlider->setOrientation(Qt::Horizontal);
		maxSlider->setMinimumWidth(50);
		minSpinBox = new QSpinBox(scoreBox);
		minSpinBox->setRange(0, 100);
		maxSpinBox = new QSpinBox(scoreBox);
		maxSpinBox->setRange(0, 100);
		connect(minSlider, SIGNAL(valueChanged(int)), minSpinBox, SLOT(setValue(int)));
		connect(maxSlider, SIGNAL(valueChanged(int)), maxSpinBox, SLOT(setValue(int)));
		connect(minSpinBox, SIGNAL(valueChanged(int)), minSlider, SLOT(setValue(int)));
		connect(maxSpinBox, SIGNAL(valueChanged(int)), maxSlider, SLOT(setValue(int)));
		minSlider->setValue(0);
		maxSlider->setValue(100);
		connect(minSpinBox, SIGNAL(valueChanged(int)), this, SLOT(delayedCommandUpdate()));
		connect(minSpinBox, SIGNAL(valueChanged(int)), this, SLOT(checkMaxSpinBoxValue(int)));
		connect(maxSpinBox, SIGNAL(valueChanged(int)), this, SLOT(delayedCommandUpdate()));
		connect(maxSpinBox, SIGNAL(valueChanged(int)), this, SLOT(checkMinSpinBoxValue(int)));

		QGridLayout *gridLayout = new QGridLayout(scoreBox);
		gridLayout->addWidget(new QLabel(tr("Min"), scoreBox), 0, 0);
		gridLayout->addWidget(new QLabel(tr("Max"), scoreBox), 1, 0);
		gridLayout->addWidget(minSlider, 0, 1);
		gridLayout->addWidget(maxSlider, 1, 1);
		gridLayout->addWidget(minSpinBox, 0, 2);
		gridLayout->addWidget(maxSpinBox, 1, 2);
	}

	studyBox = new QGroupBox(tr("Study date"), this);
	studyBox->setEnabled(false);
	studyBox->setVisible(false);
	connect(studyBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	{
		QDate infDate, supDate;
		QSqlQuery query;
		query.exec("select min(dateAdded) from training");
		if (query.next()) {
			infDate = QDateTime::fromTime_t(query.value(0).toUInt()).date();
		}
		query.exec("select max(dateAdded) from training");
		if (query.next()) {
			supDate = QDateTime::fromTime_t(query.value(0).toUInt()).date();
		}

		_studyMinDate = new RelativeDateEdit(studyBox);
		connect(_studyMinDate, SIGNAL(dateSelected(const RelativeDate &)), this, SLOT(commandUpdate()));
		connect(_studyMinDate, SIGNAL(dateChanged(const RelativeDate &)), this, SLOT(delayedCommandUpdate()));
		_studyMaxDate = new RelativeDateEdit(studyBox);
		connect(_studyMaxDate, SIGNAL(dateSelected(const RelativeDate &)), this, SLOT(commandUpdate()));
		connect(_studyMaxDate, SIGNAL(dateChanged(const RelativeDate &)), this, SLOT(delayedCommandUpdate()));

		QGridLayout *gridLayout = new QGridLayout(studyBox);
		gridLayout->addWidget(new QLabel(tr("since"), studyBox), 0, 3);
		gridLayout->addWidget(new QLabel(tr("until"), studyBox), 1, 3);
		gridLayout->addWidget(_studyMinDate, 0, 4);
		gridLayout->addWidget(_studyMaxDate, 1, 4);
	}

	trainBox = new QGroupBox(tr("Last trained"), this);
	trainBox->setEnabled(false);
	trainBox->setVisible(false);
	connect(trainBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	{
		QDate infDate, supDate;
		QSqlQuery query;
		query.exec("select min(dateAdded) from training");
		if (query.next()) {
			infDate = QDateTime::fromTime_t(query.value(0).toUInt()).date();
		}
		query.exec("select max(dateAdded) from training");
		if (query.next()) {
			supDate = QDateTime::fromTime_t(query.value(0).toUInt()).date();
		}

		_trainMinDate = new RelativeDateEdit(trainBox);
		connect(_trainMinDate, SIGNAL(dateSelected(const RelativeDate &)), this, SLOT(commandUpdate()));
		connect(_trainMinDate, SIGNAL(dateChanged(const RelativeDate &)), this, SLOT(delayedCommandUpdate()));
		_trainMaxDate = new RelativeDateEdit(trainBox);
		connect(_trainMaxDate, SIGNAL(dateSelected(const RelativeDate &)), this, SLOT(commandUpdate()));
		connect(_trainMaxDate, SIGNAL(dateChanged(const RelativeDate &)), this, SLOT(delayedCommandUpdate()));

		QGridLayout *gridLayout = new QGridLayout(trainBox);
		gridLayout->addWidget(new QLabel(tr("since"), trainBox), 0, 3);
		gridLayout->addWidget(new QLabel(tr("until"), trainBox), 1, 3);
		gridLayout->addWidget(_trainMinDate, 0, 4);
		gridLayout->addWidget(_trainMaxDate, 1, 4);
	}

	mistakeBox = new QGroupBox(tr("Last mistaken"), this);
	mistakeBox->setEnabled(false);
	mistakeBox->setVisible(false);
	connect(mistakeBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	{
		QDate infDate, supDate;
		QSqlQuery query;
		query.exec("select min(date) from trainingLog where result=0");
		if (query.next()) {
			infDate = QDateTime::fromTime_t(query.value(0).toUInt()).date();
		}
		query.exec("select max(date) from trainingLog where result=0");
		if (query.next()) {
			supDate = QDateTime::fromTime_t(query.value(0).toUInt()).date();
		}

		_mistakenMinDate = new RelativeDateEdit(mistakeBox);
		connect(_mistakenMinDate, SIGNAL(dateSelected(const RelativeDate &)), this, SLOT(commandUpdate()));
		connect(_mistakenMinDate, SIGNAL(dateChanged(const RelativeDate &)), this, SLOT(delayedCommandUpdate()));

		_mistakenMaxDate = new RelativeDateEdit(mistakeBox);
		connect(_mistakenMaxDate, SIGNAL(dateSelected(const RelativeDate &)), this, SLOT(commandUpdate()));
		connect(_mistakenMaxDate, SIGNAL(dateChanged(const RelativeDate &)), this, SLOT(delayedCommandUpdate()));

		QGridLayout *gridLayout = new QGridLayout(mistakeBox);
		gridLayout->addWidget(new QLabel(tr("since"), mistakeBox), 0, 0);
		gridLayout->addWidget(new QLabel(tr("until"), mistakeBox), 1, 0);
		gridLayout->addWidget(_mistakenMinDate, 0, 1);
		gridLayout->addWidget(_mistakenMaxDate, 1, 1);
	}

//	QGroupBox *entriesBox = new QGroupBox(tr("Study filter"), this);
	QWidget *entriesBox = new QWidget(this);
	{
		allEntriesButton = new QRadioButton(tr("None"), entriesBox);
		allEntriesButton->setChecked(true);
		connect(allEntriesButton, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonToggled(bool)));
		studiedEntriesButton = new QRadioButton(tr("Studied"), entriesBox);
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonToggled(bool)));
		nonStudiedEntriesButton = new QRadioButton(tr("Not studied"), entriesBox);
		connect(nonStudiedEntriesButton, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonToggled(bool)));

		connect(studiedEntriesButton, SIGNAL(toggled(bool)), scoreBox, SLOT(setEnabled(bool)));
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), studyBox, SLOT(setEnabled(bool)));
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), trainBox, SLOT(setEnabled(bool)));
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), mistakeBox, SLOT(setEnabled(bool)));
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), scoreBox, SLOT(setVisible(bool)));
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), studyBox, SLOT(setVisible(bool)));
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), trainBox, SLOT(setVisible(bool)));
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), mistakeBox, SLOT(setVisible(bool)));

		QHBoxLayout *hLayout = new QHBoxLayout(entriesBox);
		hLayout->addWidget(new QLabel(tr("Study status:"), entriesBox));
		hLayout->addStretch();
		hLayout->addWidget(allEntriesButton);
		hLayout->addStretch();
		hLayout->addWidget(studiedEntriesButton);
		hLayout->addStretch();
		hLayout->addWidget(nonStudiedEntriesButton);
		hLayout->addStretch();
	}

	QHBoxLayout *hLayout = new QHBoxLayout();
	hLayout->addWidget(scoreBox);
	hLayout->addWidget(studyBox);
	hLayout->addWidget(trainBox);
	hLayout->addWidget(mistakeBox);
	hLayout->setContentsMargins(0, 0, 0, 0);
	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->addWidget(entriesBox);
	vLayout->addLayout(hLayout);
	vLayout->setContentsMargins(0, 0, 0, 0);
}

void StudyFilterWidget::onRadioButtonToggled(bool toggled)
{
	if (toggled) commandUpdate();
}

QString StudyFilterWidget::currentCommand() const
{
	QString ret;

	if (studiedEntriesButton->isChecked()) {
		QString minStudyDateString(_studyMinDate->dateString());
		QString maxStudyDateString(_studyMaxDate->dateString());
		ret += QString(" :study=\"%1\",\"%2\"").arg(minStudyDateString).arg(maxStudyDateString);
				
		if (minSpinBox->value() != 0 || maxSpinBox->value() != 100) {
			ret += QString(" :score=%1,%2").arg(minSpinBox->value()).arg(maxSpinBox->value());
		}

		QString minTrainDateString(_trainMinDate->dateString());
		QString maxTrainDateString(_trainMaxDate->dateString());
		if (!minTrainDateString.isEmpty() || !maxTrainDateString.isEmpty())
			ret += QString(" :lasttrained=\"%1\",\"%2\"").arg(minTrainDateString).arg(maxTrainDateString);

		QString minMistakenDateString(_mistakenMinDate->dateString());
		QString maxMistakenDateString(_mistakenMaxDate->dateString());
		if (!minMistakenDateString.isEmpty() || !maxMistakenDateString.isEmpty())
			ret += QString(" :mistaken=\"%1\",\"%2\"").arg(minMistakenDateString).arg(maxMistakenDateString);
	}
	else if (nonStudiedEntriesButton->isChecked()) ret += " :nostudy";

	return ret;
}

void StudyFilterWidget::checkMinSpinBoxValue(int newMaxValue)
{
	if (newMaxValue < minSpinBox->value()) minSpinBox->setValue(newMaxValue);
}

void StudyFilterWidget::checkMaxSpinBoxValue(int newMinValue)
{
	if (newMinValue > maxSpinBox->value()) maxSpinBox->setValue(newMinValue);
}

QString StudyFilterWidget::currentTitle() const
{
	QString newTitle;
	if (studiedEntriesButton->isChecked()) {
		newTitle = tr("Studied");
		QString minStudyDateString = _studyMinDate->translatedDateString();
		if (!minStudyDateString.isEmpty()) {
			newTitle += tr(" since %1").arg(minStudyDateString);
		}
		QString maxStudyDateString = _studyMaxDate->translatedDateString();
		if (!maxStudyDateString.isEmpty()) {
			newTitle += tr(" until %1").arg(maxStudyDateString);
		}
		if (minSpinBox->value() != 0 || maxSpinBox->value() != 100) {
			newTitle += tr(", score:[%1,%2]").arg(minSpinBox->value()).arg(maxSpinBox->value());
		}

		QString minTrainDateString = _trainMinDate->translatedDateString();
		if (!minTrainDateString.isEmpty()) {
			newTitle += tr(", trained since %1").arg(minTrainDateString);
		}
		QString maxTrainDateString = _trainMaxDate->translatedDateString();
		if (!maxTrainDateString.isEmpty()) {
			if (minTrainDateString.isEmpty()) newTitle += tr(", trained until %1").arg(maxTrainDateString);
			else newTitle += tr(" until %1").arg(maxTrainDateString);
		}

		QString minMistakenDateString = _mistakenMinDate->translatedDateString();
		if (!minMistakenDateString.isEmpty()) {
			newTitle += tr(", mistaken since %1").arg(minMistakenDateString);
		}
		QString maxMistakenDateString = _mistakenMaxDate->translatedDateString();
		if (!maxMistakenDateString.isEmpty()) {
			if (minMistakenDateString.isEmpty()) newTitle += tr(", mistaken until %1").arg(maxMistakenDateString);
			else newTitle += tr(" until %1").arg(maxMistakenDateString);
		}
	}
	else if (nonStudiedEntriesButton->isChecked()) newTitle = tr("Not studied");
	else newTitle = tr("Study");

	return newTitle;
}

int StudyFilterWidget::studyState() const
{
	if (studiedEntriesButton->isChecked()) return Studied;
	else if (nonStudiedEntriesButton->isChecked()) return NonStudied;
	else return All;
}

void StudyFilterWidget::setStudyState(int state)
{
	switch (state) {
		case All:
			allEntriesButton->setChecked(true);
			break;
		case Studied:
			studiedEntriesButton->setChecked(true);
			break;
		case NonStudied:
			nonStudiedEntriesButton->setChecked(true);
			break;
	}
}

void StudyFilterWidget::_reset()
{
	allEntriesButton->setChecked(true);
	minSpinBox->setValue(0);
	maxSpinBox->setValue(100);
	_studyMinDate->setDateString("");
	_studyMaxDate->setDateString("");
	_trainMinDate->setDateString("");
	_trainMaxDate->setDateString("");
	_mistakenMinDate->setDateString("");
	_mistakenMaxDate->setDateString("");
}
