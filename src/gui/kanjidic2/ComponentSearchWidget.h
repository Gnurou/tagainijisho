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

#ifndef __COMPONENT_SEARCH_WIDGET_H
#define __COMPONENT_SEARCH_WIDGET_H

#include "core/EntriesCache.h"
#include "core/kanjidic2/Kanjidic2Entry.h"

#include <QWidget>
#include <QListWidget>
#include <QList>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QTimer>

#include <QPushButton>

class CandidatesKanjiList : public QGraphicsView
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
	CandidatesKanjiList(QWidget *parent = 0);
	virtual QSize sizeHint() const;
	void clear();
	QGraphicsScene *scene() { return &_scene; }

public slots:
	void addItem(const QString &kanji);

signals:
	void kanjiSelected(const QString &kanji);
};

#include "gui/ui_ComponentSearchWidget.h"

class QSqlQuery;

class ComponentSearchWidget : public QFrame, public Ui::ComponentSearchWidget
{
	Q_OBJECT
protected:
	void populateList(QSqlQuery &query);

protected slots:
	void onSelectionChanged();
	void onSelectedComponentsChanged(const QString &components);

public:
	ComponentSearchWidget(QWidget *parent = 0);

signals:
	void kanjiSelected(const QString &kanji);
};

class ComponentSearchButton : public QPushButton
{
	Q_OBJECT
private:
	ComponentSearchWidget _popup;

protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);

protected slots:
	void togglePopup(bool status);

public:
	ComponentSearchButton(QWidget *parent);
	virtual ~ComponentSearchButton();
	const ComponentSearchWidget *componentSearchWidget() const { return &_popup; }
};

#endif
