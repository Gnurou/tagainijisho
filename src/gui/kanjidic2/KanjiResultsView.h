/*
 *  Copyright (C) 2010 Alexandre Courbot
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

#ifndef __GUI_KANJI_RESULTS_VIEW_H
#define __GUI_KANJI_RESULTS_VIEW_H

#include "gui/ScrollBarSmoothScroller.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>

/**
 * A horizontally-scrollable results view designed to display kanji
 * in an easily selectable way.
 */
class KanjiResultsView : public QGraphicsView
{
	Q_OBJECT
private:
	QGraphicsScene _scene;
	QList<QGraphicsTextItem *> items;
	ScrollBarSmoothScroller _smoothScroller;

protected slots:
	void onSelectionChanged();
	void wheelEvent(QWheelEvent *event);

public:
	KanjiResultsView(QWidget *parent = 0);
	virtual QSize sizeHint() const;
	void clear();
	QGraphicsScene *scene() { return &_scene; }

public slots:
	void addItem(const QString &kanji);

signals:
	void kanjiSelected(const QString &kanji);
};

#endif
