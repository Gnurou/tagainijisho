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

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>

/**
 * A horizontally-scrollable results view designed to display kanji
 * in an easily selectable way.
 */
class KanjiResultsView : public QGraphicsView
{
	Q_OBJECT
private:
	QGraphicsScene _scene;
	int curItem, pos;
	int wheelDelta;
	QList<QGraphicsTextItem *> items;
	QTimer timer;

protected:
	void wheelEvent(QWheelEvent *event);

protected slots:
	void updateAnimationState();
	void onSelectionChanged();

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
