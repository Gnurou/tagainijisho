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

#include <QtDebug>

#include "gui/kanjidic2/KanjiResultsView.h"

#include <QScrollBar>
#include <QGraphicsTextItem>
#include <QWheelEvent>

#define KANJI_SIZE 50
#define PADDING 5

KanjiResultsView::KanjiResultsView(QWidget *parent) : QGraphicsView(parent), _scene(), curItem(0), pos(0), wheelDelta(0)
{
	setScene(&_scene);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

//	setSceneRect(-0xfffff, -0xfffff, 2 * 0xfffff, 2 * 0xfffff);
	setResizeAnchor(QGraphicsView::AnchorViewCenter);

	timer.setInterval(20);
	timer.setSingleShot(false);
	connect(&timer, SIGNAL(timeout()), this, SLOT(updateAnimationState()));
	connect(&_scene, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

QSize KanjiResultsView::sizeHint() const
{
	QFont font;
	font.setPixelSize(KANJI_SIZE);
	return QSize(QWidget::sizeHint().width(), QFontMetrics(font).height() + horizontalScrollBar()->height());
}

#define ITEM_CENTER(item) (item->pos() + item->boundingRect().center())
#define ITEM_POSITION(x) (4 * PADDING + (x) * (KANJI_SIZE + PADDING))

void KanjiResultsView::updateAnimationState()
{
	if (items.isEmpty()) return;

	int dest = ITEM_CENTER(items[curItem]).x();
	if (dest == pos) {
		timer.stop();
		return;
	}
	pos += (dest - pos) / 2;
	if (qAbs(dest - pos) == 1) pos = dest;
	centerOn(pos, ITEM_CENTER(items[curItem]).y());
}

void KanjiResultsView::onSelectionChanged()
{
	QList<QGraphicsItem *> selection(_scene.selectedItems());
	if (selection.isEmpty()) return;
	emit kanjiSelected(static_cast<QGraphicsTextItem *>(selection[selection.size() - 1])->toPlainText());
}

void KanjiResultsView::addItem(const QString &kanji)
{
	QFont font;
	font.setPixelSize(KANJI_SIZE);
	QGraphicsTextItem *item = _scene.addText(kanji, font);
	item->setPos(ITEM_POSITION(items.size()), 0);
	item->setFlags(QGraphicsItem::ItemIsSelectable);
	items << item;
	if (items.size() == 1) {
		curItem = 0;
		timer.start();
	}
	setSceneRect(_scene.itemsBoundingRect());
}

void KanjiResultsView::clear()
{
	_scene.clear();
	items.clear();
	curItem = pos = 0;
	// Used to reset the scrollbar
	setSceneRect(0, 0, 1, 1);
}

#define MOUSE_STEP 120
void KanjiResultsView::wheelEvent(QWheelEvent *event)
{
	event->accept();
	if (items.isEmpty()) return;
	wheelDelta += event->delta();
	int steps = wheelDelta / MOUSE_STEP;
	if (steps != 0) {
		wheelDelta -= steps * MOUSE_STEP;
		curItem -= steps;
		if (curItem >= items.size()) curItem = items.size() - 1;
		else if (curItem < 0) curItem = 0;
		timer.start();
	}
}
