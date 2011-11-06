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

#include "gui/FlowLayout.h"
#include "gui/MultiStackedWidget.h"

#include <QtDebug>

#include <QAction>
#include <QResizeEvent>

#define ALTWIDGETSIZE 20

MultiStackedWidgetButton::MultiStackedWidgetButton(QAction *action, QAction *resetAction, QWidget *parent) : ElidedPushButton<QToolButton>(parent), _action(action), _resetAction(resetAction)
{
	setAttribute(Qt::WA_MacMiniSize);
#ifndef Q_WS_MAC
	QFont fnt(font());
	fnt.setPointSize(fnt.pointSize() - 1);
	setFont(fnt);
#endif

	setAutoRaise(true);
	setCheckable(true);
	setMaxTextWidth(150);
	setDefaultAction(action);
	QToolButton *alt = new QToolButton(this);
	alt->setAutoRaise(true);
	alt->setDefaultAction(resetAction);
	alt->setVisible(resetAction->isEnabled());
	_altWidget = alt;
	setMinimumHeight(ALTWIDGETSIZE + 2);
	connect(action, SIGNAL(changed()), this, SLOT(onActionChanged()));
	connect(resetAction, SIGNAL(changed()), this, SLOT(onResetActionChanged()));
}

void MultiStackedWidgetButton::onResetActionChanged()
{
	if (_resetAction->isEnabled()) showAltWidget();
	else hideAltWidget();
}

void MultiStackedWidgetButton::onActionChanged()
{
	setText(_action->text());
}

void MultiStackedWidgetButton::resizeEvent(QResizeEvent *event)
{
	QToolButton::resizeEvent(event);
	if (!_altWidget) return;
	_altWidget->setGeometry((event->size().width() - 3 - ALTWIDGETSIZE), (event->size().height() - ALTWIDGETSIZE) / 2, ALTWIDGETSIZE, ALTWIDGETSIZE);
}

void MultiStackedWidgetButton::setText(const QString &str)
{
	_currentTitle = str;
	rewriteCurrentTitle();
}

void MultiStackedWidgetButton::rewriteCurrentTitle()
{
	if (_altWidget && _altWidget->isVisible()) {
		ElidedPushButton<QToolButton>::setText(_currentTitle);
		QString elidedText = ElidedPushButton<QToolButton>::text();
		QFontMetrics metrics(_altWidget->font());
		int nbSpaces = ALTWIDGETSIZE / metrics.width(' ');
		QString spaces;
		for (int i = 0; i < nbSpaces; i++) spaces += ' ';
		QToolButton::setText(elidedText + spaces);
	}
	else {
		ElidedPushButton<QToolButton>::setText(_currentTitle);
	}
}

void MultiStackedWidgetButton::showAltWidget()
{
	if (_altWidget) {
		_altWidget->show();
		rewriteCurrentTitle();
	}
}

void MultiStackedWidgetButton::hideAltWidget()
{
	if (_altWidget) {
		_altWidget->hide();
	}
}

MultiStackedWidget::MultiStackedWidget(QWidget *parent) : QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	_layout = new QVBoxLayout(this);
	_layout->setContentsMargins(0, 0, 0, 0);
	_layout->setSpacing(0);

	QAction *popupShortcut = new QAction(this);
	for (int i = 1; i < 10; i++) {
		popupShortcut = new QAction(this);
		popupShortcut->setProperty("buttonNbr", i - 1);
		//: Shortcut for popping up/down an extender
		popupShortcut->setShortcut(QKeySequence(tr("Ctrl+%1").arg(i)));
		popupShortcut->setAutoRepeat(false);
		connect(popupShortcut, SIGNAL(triggered()), this, SLOT(onShortcutTriggered()));
		addAction(popupShortcut);
		if (i == 0) break;
	}
	popupShortcut = new QAction(this);
	//: Shortcut for hiding all extenders
	popupShortcut->setShortcut(QKeySequence(tr("Ctrl+0")));
	popupShortcut->setAutoRepeat(false);
	connect(popupShortcut, SIGNAL(triggered()), this, SLOT(hideAllExtenders()));
	addAction(popupShortcut);
}

QPair<QAction *, QAction *> MultiStackedWidget::addWidget(const QString &label, QWidget *widget)
{
	QAction *action = new QAction(label, this);
	action->setCheckable(true);
	connect(action, SIGNAL(toggled(bool)), this, SLOT(onButtonToggled()));
	QAction *resetAction = new QAction(QIcon(":/images/icons/eraser-small.png"), tr("Reset this filter"), this);
	resetAction->setEnabled(false);
	connect(resetAction, SIGNAL(triggered()), widget, SLOT(reset()));
	QPair<QAction *, QAction *> actions(action, resetAction);
	widget->setVisible(false);
	StackedWidgetEntryInfo info;
	info.label = label;
	info.actions = actions;
	_buttonMap[widget] = info;
	_orderedActions << action;
	_layout->addWidget(widget);
	widget->installEventFilter(this);
	return actions;
}

void MultiStackedWidget::removeWidget(QWidget *widget)
{
	if (!_buttonMap.contains(widget)) return;
	widget->removeEventFilter(this);
	_layout->removeWidget(widget);
	_orderedActions.removeOne(_buttonMap[widget].actions.first);
	_buttonMap.remove(widget);
}

bool MultiStackedWidget::eventFilter(QObject *watched, QEvent *event)
{
	if (event->type() == QEvent::EnabledChange) {
		QWidget *widget(qobject_cast<QWidget *>(watched));
		bool enabled(widget->isEnabled());
		_buttonMap[widget].actions.first->setEnabled(enabled);
	}
	return false;
}

void MultiStackedWidget::showWidget(QWidget *widget)
{
	if (!_buttonMap.contains(widget)) return;
	if (!widget->isVisible()) {
		foreach (QWidget *otherWidget, _buttonMap.keys()) if (otherWidget->isVisible()) _buttonMap[otherWidget].actions.first->toggle();
		_buttonMap[widget].actions.first->setChecked(true);
		widget->setVisible(true);
		widget->setFocus();
	}
	updateGeometry();
}

void MultiStackedWidget::hideWidget(QWidget *widget)
{
	if (!_buttonMap.contains(widget)) return;
	if (widget->isVisible()) {
		widget->setVisible(false);
		_buttonMap[widget].actions.first->setChecked(false);
	}
	updateGeometry();
}

QWidget *MultiStackedWidget::getWidgetByLabel(const QString &label)
{
	QWidget *widget = 0;
	foreach(QWidget *key, _buttonMap.keys()) {
		if (_buttonMap[key].label == label) {
			widget = key;
			break;
		}
	}
	return widget;
}

void MultiStackedWidget::onButtonToggled()
{
	QObject *signalSender = sender();
	QWidget *widget = 0;
	foreach(QWidget *key, _buttonMap.keys()) {
		if (_buttonMap[key].actions.first == signalSender) {
			widget = key;
			break;
		}
	}
	if (!widget) return;
	if (!widget->isVisible()) showWidget(widget);
	else hideWidget(widget);
}

void MultiStackedWidget::onTitleChanged(const QString &newTitle)
{
	QWidget *signalSender = qobject_cast<QWidget *>(sender());
	if (!_buttonMap.contains(signalSender)) return;
	const StackedWidgetEntryInfo &info = _buttonMap[signalSender];
	if (newTitle == info.label) {
		info.actions.second->setEnabled(false);
		info.actions.first->setText(_buttonMap[signalSender].label);
	}
	else {
		info.actions.second->setEnabled(true);
		info.actions.first->setText(newTitle);
	}
}

void MultiStackedWidget::setWidgetEnabled(QWidget *widget, bool enabled)
{
	if (!_buttonMap.contains(widget)) return;
	const StackedWidgetEntryInfo &info = _buttonMap[widget];

	if (!enabled && info.actions.first->isChecked()) {
		info.actions.first->toggle();
	}
	info.actions.first->setEnabled(enabled);
	widget->setEnabled(enabled);
}

void MultiStackedWidget::onShortcutTriggered()
{
	QAction *action = qobject_cast<QAction *>(sender());
	int nbr = action->property("buttonNbr").toInt();
	if (nbr >= _orderedActions.size()) return;
	_orderedActions[nbr]->trigger();
}

void MultiStackedWidget::hideAllExtenders()
{
	foreach(QWidget *key, _buttonMap.keys()) {
		if (_buttonMap[key].actions.first->isChecked()) _buttonMap[key].actions.first->trigger();
	}
}
