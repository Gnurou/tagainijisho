/*
 *  Copyright (C) 2010  Alexandre Courbot
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

#ifndef __GUI_KANJIDIC2FILTERWIDGET_H
#define __GUI_KANJIDIC2FILTERWIDGET_H

#include "gui/SearchWidget.h"
#include "gui/kanjidic2/KanjiSelector.h"

#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QAction>

class Kanjidic2FilterWidget : public SearchFilterWidget
{
	Q_OBJECT
private:
	QSpinBox *_strokeCountSpinBox;
	QSpinBox *_maxStrokeCountSpinBox;
	QCheckBox *_rangeCheckBox;
	QPushButton *_gradeButton;
	QStringList _gradesList;
	QLineEdit *_radicals;
	RadicalKanjiSelector *_radKSelector;
	QLineEdit *_components;
	ComponentKanjiSelector *_compKSelector;
	QSpinBox *_unicode;
	QSpinBox *_skip1, *_skip2, *_skip3;
	QComboBox *_fcTopLeft, *_fcTopRight, *_fcBotLeft, *_fcBotRight, *_fcExtra;
	/// Actiongroup used to store the kanjis grades options
	QActionGroup *actionGroup;
	QAction *allKyouku, *allJouyou;

protected:
	virtual void _reset();
	virtual bool eventFilter(QObject *watched, QEvent *event);
	
public:
	Kanjidic2FilterWidget(QWidget *parent = 0);

	virtual QString name() const { return "kanjidicoptions"; }
	virtual QString currentTitle() const;
	virtual QString currentCommand() const;

	virtual void updateFeatures();

	int strokeCount() const { return _strokeCountSpinBox->value(); }
	void setStrokeCount(int value) { _strokeCountSpinBox->setValue(value); }
	Q_PROPERTY(int strokeCount READ strokeCount WRITE setStrokeCount)

	int maxStrokeCount() const { return _maxStrokeCountSpinBox->value(); }
	void setMaxStrokeCount(int value) { _maxStrokeCountSpinBox->setValue(value); }
	Q_PROPERTY(int maxStrokeCount READ maxStrokeCount WRITE setMaxStrokeCount)
	
	bool isStrokeRange() const { return _rangeCheckBox->isChecked(); }
	void setStrokeRange(bool value) { _rangeCheckBox->setChecked(value); }
	Q_PROPERTY(int isStrokeRange READ isStrokeRange WRITE setStrokeRange)
	
	QString radicals() const { return _radicals->text(); }
	void setRadicals(const QString &value) { _radicals->setText(value); }
	Q_PROPERTY(QString radicals READ radicals WRITE setRadicals)
	
	QString components() const { return _components->text(); }
	void setComponents(const QString &value) { _components->setText(value); }
	Q_PROPERTY(QString components READ components WRITE setComponents)

	int unicode() const { return _unicode->value(); }
	void setUnicode(int value) { _unicode->setValue(value); }
	Q_PROPERTY(int unicode READ unicode WRITE setUnicode)

	QString skip() const;
	void setSkip(const QString &value);
	Q_PROPERTY(QString skip READ skip WRITE setSkip)

	QString fourCorner() const;
	void setFourCorner(const QString &value);
	Q_PROPERTY(QString fourCorner READ fourCorner WRITE setFourCorner)
	
	QStringList grades() const { return _gradesList; }
	void setGrades(const QStringList &list);
	Q_PROPERTY(QStringList grades READ grades WRITE setGrades)

protected slots:
	void onStrokeRangeToggled(bool checked);
	void onStrokeRangeChanged();
	void onGradeTriggered(QAction *action);
	void allKyoukuKanjis(bool checked);
	void allJouyouKanjis(bool checked);
};

#endif
