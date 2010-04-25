/*
 *  Copyright (C) 2008  Alexandre Courbot
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

#ifndef __GUI_YESNOTRAINER_H__
#define __GUI_YESNOTRAINER_H__

#include "gui/ToolBarDetailedView.h"

#include <QFrame>
#include <QPushButton>
#include <QTextCursor>
#include <QSqlQuery>
#include <QLabel>

class YesNoTrainer : public QWidget {
	Q_OBJECT
public:
	typedef enum { Japanese, Translation } TrainingMode;

private:
	static PreferenceItem<QByteArray> windowGeometry;

	TrainingMode _trainingMode;
	unsigned int _goodCount, _wrongCount, _totalCount;

	void getNextEntry();
	void _train();

protected:
	EntryPointer currentEntry;
	QSqlQuery _query;
	ToolBarDetailedView *_detailedView;

	QPushButton *showAnswerButton;
	QPushButton *goodAnswerButton;
	QPushButton *wrongAnswerButton;
	QPushButton *skipButton;
	QLabel *_counterLabel;
	QString _queryString;

public:
	YesNoTrainer(QWidget *parent = 0);
	~YesNoTrainer();

	const QString &query() const { return _queryString; }
	void setQuery(QString queryString);
	virtual void setTrainingMode(TrainingMode mode) { _trainingMode = mode; }
	TrainingMode trainingMode() const { return _trainingMode; }
	DetailedView *detailedView() { return _detailedView->detailedView(); }
	bool answerShown() const { return !showAnswerButton->isEnabled(); }

protected slots:
	/**
	 * Show the answer and enable the correct/wrong
	 * buttons.
	 */
	void showAnswer();

	/**
	 * Update the entry for a good answer.
	 */
	void goodAnswer();

	/**
	 * Update the entry for a wrong answer.
	 */
	void wrongAnswer();

	/**
	 * Just skip without recording anything.
	 */
	void skip();

	/**
	 * Train the given entry. Its score will be modified
	 * and its database entry will be updated according to
	 * the result.
	 */
	void train(const EntryPointer& entry);

	void hasResults(unsigned int nbResults);

public slots:
	/**
	 * Starts training.
	 */
	void train();

	void clear();

signals:
	void currentEntryChanged();
};

#endif
