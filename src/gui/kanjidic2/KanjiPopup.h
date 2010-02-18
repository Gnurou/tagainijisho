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

#ifndef __GUI_KANJI_POPUP_H
#define __GUI_KANJI_POPUP_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QList>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>

#include "core/kanjidic2/Kanjidic2Entry.h"
#include "gui/kanjidic2/KanjiPlayer.h"
#include "gui/SingleEntryView.h"
#include "gui/AbstractHistory.h"

class KanjiComponentWidget : public QWidget
{
	Q_OBJECT
private:
	const KanjiComponent *_component;
	EntryPointer _kanji;

protected:
	virtual void paintEvent(QPaintEvent *event);

public:
	KanjiComponentWidget(QWidget *parent = 0);
	virtual QSize sizeHint() const;
	void setComponent(const KanjiComponent *component);
	const KanjiComponent *component() const { return _component; }
	const Kanjidic2Entry *kanji() const { return static_cast<Kanjidic2Entry *>(_kanji.data()); }

};

#include "gui/ui_KanjiPopup.h"

class KanjiPopup : public QFrame, public Ui::KanjiPopup {
	Q_OBJECT
private:
	AbstractHistory<QString, QList<QString> > _history;
	SingleEntryView entryView;

	void setComponentsLabelText(int highlightPos = -1);

protected:
	bool eventFilter(QObject *obj, QEvent *event);

private slots:
	void showKanji(Kanjidic2Entry *entry);
	void onOpenClick();
	void onPreviousClick();
	void onNextClick();
	void onComponentHighlighted(const KanjiComponent *component);
	void onComponentUnHighlighted();
	void onComponentClicked(const KanjiComponent *component);

	void onComponentLinkHovered(const QString &link);
	void onComponentLinkActivated(const QString &link);

public:
	KanjiPopup(QWidget *parent = 0);
	~KanjiPopup();

	void display(Kanjidic2Entry *entry);

	static PreferenceItem<int> historySize;
	static PreferenceItem<int> animationSize;
	static PreferenceItem<bool> autoStartAnim;

public slots:
	void updateInfo();

signals:
	void requestDisplay(EntryPointer);
};

#endif
