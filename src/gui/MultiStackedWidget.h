/*
 *  Copyright (C) 2008/2009  Alexandre Courbot
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

#ifndef __GUI_MULTISTACKEDWIDGET_H_
#define __GUI_MULTISTACKEDWIDGET_H_

#include <QWidget>
#include <QMap>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

#include "gui/ElidedPushButton.h"

class MultiStackedWidgetButton : public ElidedPushButton<QToolButton> {
	Q_OBJECT
private:
	QWidget *_altWidget;
	QString _currentTitle;
	QAction *const _action, *const _resetAction;

	void rewriteCurrentTitle();

private slots:
	void onActionChanged();
	void onResetActionChanged();

public:
	MultiStackedWidgetButton(QAction* action, QAction* resetAction, QWidget* parent = 0);
	QWidget *altWidget() { return _altWidget; }
	void setAltWidget(QWidget *widget) { _altWidget = widget; }

	void setText(const QString &str);
	void showAltWidget();
	void hideAltWidget();

	virtual void resizeEvent(QResizeEvent *event);
};

class ToolBarWidget : public QWidgetAction
{
	Q_OBJECT
private:
	QPair<QAction *, QAction *> _actions;
protected:
	virtual QWidget *createWidget(QWidget *parent)
	{
		MultiStackedWidgetButton *button = new MultiStackedWidgetButton(_actions.first, _actions.second, parent);
		return button;
	}
public:
	ToolBarWidget(QPair<QAction *, QAction *> actions, QObject *parent = 0) : QWidgetAction(parent), _actions(actions) { }
};

struct StackedWidgetEntryInfo {
	QString label;
	QPair<QAction *, QAction *> actions;
};

class MultiStackedWidget : public QWidget {
	Q_OBJECT
private:
	QVBoxLayout *_layout;
	QMap<QWidget *, StackedWidgetEntryInfo> _buttonMap;
	QList<QAction *> _orderedActions;

protected:
	QWidget *getWidgetByLabel(const QString &label);
	virtual bool eventFilter(QObject *watched, QEvent *event);

public:
	MultiStackedWidget(QWidget *parent = 0);

	QPair<QAction *, QAction *> addWidget(const QString &label, QWidget *widget);
	void removeWidget(QWidget *widget);

	void setWidgetEnabled(QWidget *widget, bool enabled);

private slots:
	void onButtonToggled();
	void onShortcutTriggered();

public slots:
	void showWidget(QWidget *widget);
	void hideWidget(QWidget *widget);
	/**
	 * Changes the title of the sender widget
	 */
	void onTitleChanged(const QString &newTitle);

	void hideAllExtenders();
signals:
	
};

#endif
