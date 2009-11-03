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

#ifndef TRAINSETTINGS_H
#define TRAINSETTINGS_H

#include "ui_TrainSettings.h"

class TrainSettings : public QDialog, private Ui::TrainSettings
{
	Q_OBJECT
public:
	static PreferenceItem<QString> minDatePref;
	static PreferenceItem<QString> maxDatePref;
	static PreferenceItem<int> minScorePref;
	static PreferenceItem<int> maxScorePref;
	static PreferenceItem<int> biasPref;

	enum { BIAS_RANDOM = 0, BIAS_SCORE = 1 };
	TrainSettings(QWidget *parent = 0);

	static const QString MINDATE_DEFAULT;
	static const QString MAXDATE_DEFAULT;
	static const int MINSCORE_DEFAULT = 0;
	static const int MAXSCORE_DEFAULT = 100;
	static const int BIAS_DEFAULT = BIAS_SCORE;

	static QString buildQueryString(int entryType);
	static QString buildOrderString(const QString &scoreColumn);

private slots:
	void updateBiasExplanation(int bias);
	void checkMinScoreValue(int newMaxValue);
	void checkMaxScoreValue(int newMinValue);

public slots:
	void restoreDefaults();
	void applySettings();
};

#endif // TRAINSETTINGS_H
