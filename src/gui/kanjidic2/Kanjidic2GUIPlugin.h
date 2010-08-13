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
#include "core/kanjidic2/Kanjidic2Entry.h"
#include "gui/ReadingTrainer.h"
#include "gui/SearchFilterWidget.h"
#include "gui/YesNoTrainer.h"
#include "gui/kanjidic2/Kanjidic2FilterWidget.h"

#include <QDockWidget>

class KanjiLinkHandler;
class KanjiAllWordsHandler;
class KanjiAllComponentsOfHandler;
class Kanjidic2FilterWidget;
class KanaSelector;

class Kanjidic2GUIPlugin : public QObject, public Plugin
{
	Q_OBJECT
private:
	QAction *_flashKL, *_flashKS, *_flashML, *_flashMS, *_readingPractice, *_showKanjiPopup;
	KanjiLinkHandler *_linkHandler;
	KanjiAllWordsHandler *_wordsLinkHandler;
	KanjiAllComponentsOfHandler *_componentsLinkHandler;
	Kanjidic2FilterWidget *_filter;
	YesNoTrainer *_trainer;
	ReadingTrainer *_readingTrainer;
	KanjiInputPopupAction * _cAction, *_kAction;
	KanaSelector *_kanaSelector;
	QDockWidget *_kanaDockWidget;
	/// Used for drag'n drop of kanji from the detailed view
	bool _dragStarted;
	QPoint _dragPos;
	KanjiEntryRef _dragEntryRef;

	static Kanjidic2GUIPlugin *_instance;

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
	void popupDetailedViewKanjiEntry();

public:
	Kanjidic2GUIPlugin();
	virtual ~Kanjidic2GUIPlugin();
	virtual bool onRegister();
	virtual bool onUnregister();

	static Kanjidic2GUIPlugin *instance() { return _instance; }

	/**
	 * Used to filter the events of the detailed views and
	 * handle mouse hover and clicks on kanjis.
	 */
	bool eventFilter(QObject *obj, QEvent *_event);

	KanaSelector *kanaSelector() { return _kanaSelector; }

	static const QString kanjiGrades[];

	static PreferenceItem<bool> kanjiTooltipEnabled;
	static void showPopup(const Kanjidic2EntryPointer &entry, const QPoint &pos);
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

#endif // KANJIDIC2GUIPLUGIN_H
