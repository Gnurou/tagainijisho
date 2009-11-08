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
#include "KanjiPlayer.h"
#include "SingleEntryView.h"
#include "AbstractHistory.h"

class GraphicsComponentItem : public QObject, public QGraphicsItem
{
	Q_OBJECT
private:
	int _width;
	EntryPointer<Entry> _kanji;
	const KanjiComponent *_component;
	QString _meanings, _readings;
	int _meaningsHeight, _readingsHeight;
	int _kanjiSize;

	void onChildHoverLeave();

protected:
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

public:
	GraphicsComponentItem(int width, const KanjiComponent *component, QGraphicsItem *parent = 0);
	~GraphicsComponentItem();
	const KanjiComponent *component() const { return _component; }
	const Kanjidic2Entry *kanji() const { return static_cast<Kanjidic2Entry *>(_kanji.data()); }
	Kanjidic2Entry *kanji() { return static_cast<Kanjidic2Entry *>(_kanji.data()); }

	virtual QRectF boundingRect() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
	void hoverEnter(const KanjiComponent *);
	void hoverLeave(const KanjiComponent *);
	void clicked(const KanjiComponent *);
};

class KanjiPopup : public QFrame {
	Q_OBJECT
private:
	static const int maxEltWidth = 200;

	AbstractHistory<QString, QList<QString> > _history;
	SingleEntryView entryView;

	KanjiPlayer *stroke;
	QLabel *propsLabel;
	QLabel *meaningsLabel;
	QLabel *readingsLabel;

	QToolButton *prevButton;
	QToolButton *nextButton;

	void addComponentDescription(const KanjiComponent *component, GraphicsComponentItem *parent, int &vPos);

private slots:
	void showKanji(Kanjidic2Entry *entry);
	void onOpenClick();
	void onPreviousClick();
	void onNextClick();
	void onComponentHighlighted(const KanjiComponent *component);
	void onComponentUnHighlighted(const KanjiComponent *component);
	void onComponentClicked(const KanjiComponent *component);

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
	void requestDisplay(Entry *entry);
};

#endif
