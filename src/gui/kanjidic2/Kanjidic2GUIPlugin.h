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

#ifndef __GUI_KANJIDIC2GUIPLUGIN_H
#define __GUI_KANJIDIC2GUIPLUGIN_H

#include "core/Plugin.h"
#include "gui/HexSpinBox.h"
#include "gui/ReadingTrainer.h"

#include "YesNoTrainer.h"
#include "SearchBar.h"
#include <QSpinBox>

class Kanjidic2EntryFormatter;
class KanjiLinkHandler;
class KanjiAllWordsHandler;
class KanjiAllComponentsOfHandler;
class Kanjidic2OptionsWidget;

class Kanjidic2GUIPlugin : public QObject, public Plugin
{
	Q_OBJECT
private:
	Kanjidic2EntryFormatter *_formatter;
	QAction *_flashKL, *_flashKS, *_flashML, *_flashMS, *_readingPractice;
	KanjiLinkHandler *_linkHandler;
	KanjiAllWordsHandler *_wordsLinkHandler;
	KanjiAllComponentsOfHandler *_componentsLinkHandler;
	Kanjidic2OptionsWidget *_extender;
	YesNoTrainer *_trainer;
	ReadingTrainer *_readingTrainer;

	void training(YesNoTrainer::TrainingMode mode, const QString &queryString);

private slots:
	void trainerDeleted();
	void readingTrainerDeleted();

protected slots:
	void trainingKanjiList();
	void trainingKanjiSet();
	void trainingMeaningList();
	void trainingMeaningSet();
	void readingPractice();

public:
	Kanjidic2GUIPlugin();
	virtual ~Kanjidic2GUIPlugin();
	virtual QString pluginInfo() const;
	virtual bool onRegister();
	virtual bool onUnregister();

	/**
	 * Used to filter the events of the detailed views and
	 * handle mouse hover and clicks on kanjis.
	 */
	bool eventFilter(QObject *obj, QEvent *_event);
};

class KanjiLinkHandler : public DetailedViewLinkHandler
{
	Q_DECLARE_TR_FUNCTIONS(KanjiLinkHandler)
public:
	KanjiLinkHandler();
	void handleUrl(const QUrl &url, DetailedView *view);
};

class KanjiAllWordsHandler : public DetailedViewLinkHandler
{
public:
	KanjiAllWordsHandler();
	void handleUrl(const QUrl &url, DetailedView *view);
};

class KanjiAllComponentsOfHandler : public DetailedViewLinkHandler
{
public:
	KanjiAllComponentsOfHandler();
	void handleUrl(const QUrl &url, DetailedView *view);
};

class Kanjidic2OptionsWidget : public SearchBarExtender
{
	Q_OBJECT
private:
	QSpinBox *_strokeCountSpinBox;
	QPushButton *_gradeButton;
	QStringList _gradesList;
	QLineEdit *_components;
	HexSpinBox *_unicode;
	QSpinBox *_skip1, *_skip2, *_skip3;
	/// Actiongroup used to store the kanjis grades options
	QActionGroup *actionGroup;
	QAction *allKyouku, *allJouyou;

protected:
	virtual void _reset();

public:
	Kanjidic2OptionsWidget(QWidget *parent = 0);
	virtual QString name() const { return "kanjidicoptions"; }
	virtual QString currentTitle() const;
	virtual QString currentCommand() const;

	virtual void updateFeatures();

	int strokeCount() const { return _strokeCountSpinBox->value(); }
	void setStrokeCount(int value) { _strokeCountSpinBox->setValue(value); }
	Q_PROPERTY(int strokeCount READ strokeCount WRITE setStrokeCount)

	QString components() const { return _components->text(); }
	void setComponents(const QString &value) { _components->setText(value); }
	Q_PROPERTY(QString components READ components WRITE setComponents)

	int unicode() const { return _unicode->value(); }
	void setUnicode(int value) { _unicode->setValue(value); }
	Q_PROPERTY(int unicode READ unicode WRITE setUnicode)

	QStringList grades() const { return _gradesList; }
	void setGrades(const QStringList &list);
	Q_PROPERTY(QStringList grades READ grades WRITE setGrades)

protected slots:
	void onGradeTriggered(QAction *action);
	void allKyoukuKanjis(bool checked);
	void allJouyouKanjis(bool checked);
};

#endif // KANJIDIC2GUIPLUGIN_H
