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
#ifndef _GUI__READINGTRAINER_H
#define _GUI__READINGTRAINER_H

#include "gui/ui_ReadingTrainer.h"

#include <QFrame>
#include <QSqlQuery>

class ReadingTrainer : public QFrame
{
	Q_OBJECT
private:
	static PreferenceItem<QByteArray> windowGeometry;

	Ui::ReadingTrainer ui;
	QSqlQuery query;
	EntryPointer<Entry> entry;
	unsigned int _goodCount, _wrongCount, _totalCount;

protected:
	void updateStatusLabel();

public:
	ReadingTrainer(QWidget *parent = 0);
	~ReadingTrainer();

	void newSession();

protected slots:
	void checkAnswer();

public slots:
	void train();
};

#endif
