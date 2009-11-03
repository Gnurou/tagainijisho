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

MultiStackedWidgetButton::MultiStackedWidgetButton(QWidget *parent) : ElidedPushButton<QToolButton>(parent)
{
	QToolButton *alt = new QToolButton(this);
	alt->setIcon(QIcon(":/images/icons/reset-search.png"));
	alt->setAutoRaise(true);
	alt->setToolTip(tr("Reset this extender"));
	alt->hide();
	setMaxTextWidth(150);
	_altWidget = alt;
	setMinimumHeight(ALTWIDGETSIZE + 2);
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
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);
	_layout = new QVBoxLayout();
	_layout->setContentsMargins(0, 0, 0, 0);
	_layout->setSpacing(0);
	_buttonsLayout = new FlowLayout();
	_buttonsLayout->setContentsMargins(0, 0, 0, 0);
	_buttonsLayout->setSpacing(-1);
	_buttonsLayout->setAlignment(Qt::AlignLeft);
	mainLayout->addLayout(_buttonsLayout);
	mainLayout->addLayout(_layout);

	QAction *popupShortcut = new QAction(this);
	for (int i = 1; i < 10; i++) {
		popupShortcut = new QAction(this);
		//: Shortcut for popping up/down an extender
		popupShortcut->setProperty("buttonNbr", i - 1);
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

void MultiStackedWidget::addWidget(const QString &label, QWidget *widget)
{
	MultiStackedWidgetButton *button = new MultiStackedWidgetButton(this);
	QFont font = button->font();
	font.setPointSize(font.pointSize() - 1);
	button->setFont(font);
	button->setText(label);
	button->setAutoRaise(true);
	button->setCheckable(true);
	_buttonsLayout->addWidget(button);
	connect(button->altWidget(), SIGNAL(clicked()), widget, SLOT(reset()));
	widget->setVisible(false);
	StackedWidgetEntryInfo info;
	info.label = label;
	info.button = button;
	_buttonMap[widget] = info;
	_layout->addWidget(widget);
	connect(button, SIGNAL(toggled(bool)), this, SLOT(onButtonToggled()));
}

void MultiStackedWidget::removeWidget(QWidget *widget)
{
	if (!_buttonMap.contains(widget)) return;
	_layout->removeWidget(widget);
	//hideWidget(widget);
	_buttonsLayout->removeWidget(_buttonMap[widget].button);
	_buttonMap.remove(widget);
}

void MultiStackedWidget::showWidget(QWidget *widget)
{
	if (!_buttonMap.contains(widget)) return;
	if (!widget->isVisible()) {
		foreach (QWidget *otherWidget, _buttonMap.keys()) if (otherWidget->isVisible()) _buttonMap[otherWidget].button->click();
//		_buttonMap[widget].button->setChecked(true);
		widget->setVisible(true);
		widget->setFocus();
	}
}

void MultiStackedWidget::hideWidget(QWidget *widget)
{
	if (!_buttonMap.contains(widget)) return;
	if (widget->isVisible()) {
		widget->setVisible(false);
//		_buttonMap[widget].button->setChecked(false);
	}
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
		if (_buttonMap[key].button == signalSender) {
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
//		info.button->setBackgroundRole(QPalette::Button);
//		info.button->setAutoFillBackground(false);
		info.button->hideAltWidget();
		info.button->setText(_buttonMap[signalSender].label);
	}
	else {
//		info.button->setAutoFillBackground(true);
//		info.button->setBackgroundRole(QPalette::Mid);
		info.button->showAltWidget();
		info.button->setText(newTitle);
	}
}

void MultiStackedWidget::setWidgetEnabled(QWidget *widget, bool enabled)
{
	if (!_buttonMap.contains(widget)) return;
	const StackedWidgetEntryInfo &info = _buttonMap[widget];

	if (!enabled && info.button->isChecked()) {
		info.button->click();
	}
	info.button->setEnabled(enabled);
	widget->setEnabled(enabled);
}

void MultiStackedWidget::onShortcutTriggered()
{
	QAction *action = qobject_cast<QAction *>(sender());
	int nbr = action->property("buttonNbr").toInt();
	if (nbr >= _buttonsLayout->count()) return;
	qobject_cast<QToolButton *>(_buttonsLayout->itemAt(nbr)->widget())->click();
}

void MultiStackedWidget::hideAllExtenders()
{
	foreach(QWidget *key, _buttonMap.keys()) {
		if (_buttonMap[key].button->isChecked()) _buttonMap[key].button->click();
	}
}
