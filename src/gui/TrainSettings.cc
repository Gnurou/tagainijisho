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

#include "gui/TrainSettings.h"

PreferenceItem<QString> TrainSettings::minDatePref("training", "minDate", "yesterday");
PreferenceItem<QString> TrainSettings::maxDatePref("training", "maxDate", "");
PreferenceItem<int> TrainSettings::minScorePref("training", "minScore", 0);
PreferenceItem<int> TrainSettings::maxScorePref("training", "maxScore", 100);
PreferenceItem<int> TrainSettings::biasPref("training", "bias", TrainSettings::BIAS_SCORE);

TrainSettings::TrainSettings(QWidget *parent) : QDialog(parent)
{
	setupUi(this);

	connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(restoreDefaults()));
	connect(bias, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBiasExplanation(int)));
	connect(minScore, SIGNAL(valueChanged(int)), this, SLOT(checkMaxScoreValue(int)));
	connect(maxScore, SIGNAL(valueChanged(int)), this, SLOT(checkMinScoreValue(int)));

	minDate->setDateString(minDatePref.value());
	maxDate->setDateString(maxDatePref.value());
	minScore->setValue(minScorePref.value());
	maxScore->setValue(maxScorePref.value());
	bias->setCurrentIndex(biasPref.value());

	updateBiasExplanation(bias->currentIndex());
}

void TrainSettings::applySettings()
{
	minDatePref.set(minDate->dateString());
	maxDatePref.set(maxDate->dateString());
	minScorePref.set(minScore->value());
	maxScorePref.set(maxScore->value());
	biasPref.set(bias->currentIndex());
}

void TrainSettings::restoreDefaults()
{
	minDatePref.reset();
	maxDatePref.reset();
	minScorePref.reset();
	maxScorePref.reset();
	biasPref.reset();
}

void TrainSettings::updateBiasExplanation(int bias)
{
	switch (bias) {
		case BIAS_RANDOM:
			biasLabel->setText(tr("Entries appear totally randomly."));
			break;
		case BIAS_SCORE:
			biasLabel->setText(tr("Entries with a low score are likely to appear first."));
			break;
		default:
			biasLabel->setText("");
			break;
	}
	updateGeometry();
}

void TrainSettings::checkMinScoreValue(int newMaxValue)
{
	if (newMaxValue < minScore->value()) minScore->setValue(newMaxValue);
}

void TrainSettings::checkMaxScoreValue(int newMinValue)
{
	if (newMinValue > maxScore->value()) maxScore->setValue(newMinValue);
}

QString TrainSettings::buildQueryString(int entryType)
{
	QString queryString(QString("select type, id from training where type = %1").arg(entryType));
	RelativeDate minDate(minDatePref.value());
	if (minDate.isSet()) queryString += QString(" and (dateLastTrain < %1 OR dateLastTrain is null)").arg(QDateTime(minDate.date()).toTime_t());
	RelativeDate maxDate(maxDatePref.value());
	if (maxDate.isSet()) queryString += QString(" and dateLastTrain > %1").arg(QDateTime(maxDate.date()).toTime_t());
	int minScore(minScorePref.value());
	if (minScore > minScorePref.defaultValue()) queryString += QString(" and score >= %1").arg(minScore);
	int maxScore(maxScorePref.value());
	if (maxScore < maxScorePref.defaultValue()) queryString += QString(" and score <= %1").arg(maxScore);
	queryString += " " + buildOrderString("score");
	return queryString;
}

QString TrainSettings::buildOrderString(const QString &scoreColumn)
{
	int bias(biasPref.value());
	if (bias == TrainSettings::BIAS_SCORE) return QString("ORDER BY biaised_random(%1) DESC").arg(scoreColumn);
	else return "ORDER BY random()";
}
