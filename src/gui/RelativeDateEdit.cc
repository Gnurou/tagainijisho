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

#include "core/Preferences.h"
#include "gui/RelativeDateEdit.h"

#include <QtDebug>

#include <QList>
#include <QFontMetrics>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QDesktopWidget>
#include <QLabel>
#include <QApplication>

RelativeDatePopup::RelativeDatePopup(RelativeDate &date, QWidget *parent) : QFrame(parent), _date(date)
{
	_notSetButton = new QRadioButton(tr("Not set"), this);

	_counterBox = new QSpinBox(this);
	_counterBox->setRange(0, 1000000);
	_whatBox = new QComboBox(this);
	_whatBox->addItem(tr("days ago"), RelativeDate::DaysAgo);
	_whatBox->addItem(tr("weeks ago"), RelativeDate::WeeksAgo);
	_whatBox->addItem(tr("months ago"), RelativeDate::MonthsAgo);
	_whatBox->addItem(tr("years ago"), RelativeDate::YearsAgo);
	QPushButton *relativeOkButton = new QPushButton(tr("Ok"), this);
	QHBoxLayout *hLayout = new QHBoxLayout();
	hLayout->addWidget(_counterBox);
	hLayout->addWidget(_whatBox);
	hLayout->addWidget(relativeOkButton);

	_calendar = new QCalendarWidget(this);

	QFrame *sep;
	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->addWidget(_notSetButton);
	sep = new QFrame(this);
	sep->setFrameStyle(QFrame::Sunken | QFrame::HLine);
	vLayout->addWidget(sep);
	vLayout->addWidget(new QLabel(tr("Relative date:"), this));
	vLayout->addLayout(hLayout);
	sep = new QFrame(this);
	sep->setFrameStyle(QFrame::Sunken | QFrame::HLine);
	vLayout->addWidget(sep);
	vLayout->addWidget(new QLabel(tr("Absolute date:"), this));
	vLayout->addWidget(_calendar);

	connect(relativeOkButton, SIGNAL(clicked()), this, SLOT(onRelativeDateSelected()));
	connect(_notSetButton, SIGNAL(clicked()), this, SLOT(onNotSetSelected()));
	connect(_calendar, SIGNAL(clicked(QDate)), this, SLOT(onAbsoluteDateSelected(QDate)));
//	connect(this, SIGNAL(dateSelected()), this, SLOT(sync()));
	sync();
}

void RelativeDatePopup::show()
{
	_calendar->setFirstDayOfWeek(static_cast<Qt::DayOfWeek>(RelativeDate::firstDayOfWeek.value()));
	QWidget::show();
}


void RelativeDatePopup::onNotSetSelected()
{
	_date.setDateType(RelativeDate::NotSet);
	emit dateSelected();
}

void RelativeDatePopup::onRelativeDateSelected()
{
	_notSetButton->setChecked(false);
	_date.setDateType(_whatBox->itemData(_whatBox->currentIndex()).toInt());
	_date.setAgo(_counterBox->value());
	emit dateSelected();
}

void RelativeDatePopup::onAbsoluteDateSelected(const QDate &date)
{
	_notSetButton->setChecked(false);
	_date.setDateType(RelativeDate::AbsoluteDate);
	_date.setAbsoluteDate(date);
	emit dateSelected();
}

void RelativeDatePopup::sync()
{
	switch(_date.dateType()) {
		case RelativeDate::NotSet:
			_notSetButton->setChecked(true);
			break;
		case RelativeDate::DaysAgo:
		case RelativeDate::WeeksAgo:
		case RelativeDate::MonthsAgo:
		case RelativeDate::YearsAgo:
			_notSetButton->setChecked(false);
			_counterBox->setValue(_date.ago());
			break;
		case RelativeDate::AbsoluteDate:
			_notSetButton->setChecked(false);
			break;
	}
	_calendar->setSelectedDate(_date.absoluteDate());
}

RelativeDateEdit::RelativeDateEdit(QWidget *parent) : ElidedPushButton<QPushButton>(parent), _date(), _popup(_date)
{
	_popup.hide();
	_popup.setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	_popup.setWindowModality(Qt::ApplicationModal);
	_popup.setWindowFlags(Qt::Popup);
	_popup.installEventFilter(this);
	setCheckable(true);
	setMaxTextWidth(100);
	connect(this, SIGNAL(toggled(bool)), this, SLOT(togglePopup(bool)));
	connect(&_popup, SIGNAL(dateSelected()), this, SLOT(onDateSelected()));
	updateButtonTitle();
}

void RelativeDateEdit::togglePopup(bool status)
{
	if (status) {
		_popup.move(mapToGlobal(rect().bottomLeft()));
		_popup.show();
		QDesktopWidget *desktopWidget = QApplication::desktop();
		QRect popupRect = _popup.geometry();
		QRect screenRect(desktopWidget->screenGeometry(this));
		if (!screenRect.contains(_popup.geometry())) {
			if (screenRect.right() < popupRect.right()) popupRect.moveRight(screenRect.right());
			if (screenRect.bottom() < popupRect.bottom()) popupRect.moveBottom(screenRect.bottom());
			_popup.setGeometry(popupRect);
		}
	}
	else {
		_popup.hide();
	}
}

void RelativeDateEdit::onDateSelected()
{
	setChecked(false);
	updateButtonTitle();
	emit dateSelected(_date);
}

void RelativeDateEdit::updateButtonTitle()
{
	QString dateString(_date.translatedDateString());
	if (dateString.isEmpty()) dateString = tr("Not set");
	setText(dateString);
}

bool RelativeDateEdit::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::Hide) {
		setChecked(false);
	}
	return false;
}

void RelativeDateEdit::wheelEvent(QWheelEvent *event)
{
	if (event->orientation() != Qt::Vertical) return;
	static int wheelDelta = 0;
	wheelDelta += event->delta();
	int steps = wheelDelta / 120;
	if (steps) {
		wheelDelta -= steps * 120;
		if (_date.dateType() == RelativeDate::NotSet && steps > 0) {
			_date.setDateType(_popup._whatBox->itemData(_popup._whatBox->currentIndex()).toInt());
			_date.setAgo(steps - 1);
		} else if (_date.dateType() == RelativeDate::AbsoluteDate) {
			_date.setAbsoluteDate(_date.absoluteDate().addDays(steps));
		} else {
			int ago = _date.ago() + steps;
			if (ago >= 0) {
				_date.setAgo(ago);
			} else {
				_date.setDateType(RelativeDate::NotSet);
			}
		}
		updateButtonTitle();
		_popup.sync();
		emit dateChanged(_date);
	}
	event->accept();
}
