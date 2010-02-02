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

#include "gui/ElidedPushButton.h"

class MultiStackedWidgetButton : public ElidedPushButton<QToolButton> {
	Q_OBJECT
private:
	QWidget *_altWidget;
	QString _currentTitle;

	void rewriteCurrentTitle();

public:
	MultiStackedWidgetButton(QWidget *parent = 0);
	QWidget *altWidget() { return _altWidget; }
	void setAltWidget(QWidget *widget) { _altWidget = widget; }

	void setText(const QString &str);
	void showAltWidget();
	void hideAltWidget();

	virtual void resizeEvent(QResizeEvent *event);
};

struct StackedWidgetEntryInfo {
	QString label;
	MultiStackedWidgetButton *button;
};

class MultiStackedWidget : public QWidget {
	Q_OBJECT
private:
	QVBoxLayout *_layout;
	QList<MultiStackedWidgetButton *> _toolButtons;
	QMap<QWidget *, StackedWidgetEntryInfo> _buttonMap;

protected:
	QWidget *getWidgetByLabel(const QString &label);

public:
	MultiStackedWidget(QWidget *parent = 0);

	MultiStackedWidgetButton* addWidget(const QString &label, QWidget *widget);
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
