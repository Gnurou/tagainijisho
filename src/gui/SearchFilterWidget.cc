/*
 *  Copyright (C) 2009/2010  Alexandre Courbot
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

#include <gui/SearchFilterWidget.h>

SearchFilterWidget::SearchFilterWidget(QWidget *parent, const QString &feature) : QWidget(parent), _feature(feature), _autoUpdateQuery(true), _propsToSave()
{
	_timer.setSingleShot(true);
	_timer.setInterval(500);
	connect(&_timer, SIGNAL(timeout()), this, SIGNAL(commandUpdated()));
}

void SearchFilterWidget::updateVisualState()
{
	// Update the title
	emit updateTitle(currentTitle());
	// Update the features
	updateFeatures();
}

void SearchFilterWidget::commandUpdate()
{
	if (_timer.isActive()) _timer.stop();
	updateVisualState();
	if (autoUpdateQuery()) emit commandUpdated();
}

void SearchFilterWidget::delayedCommandUpdate()
{
	updateVisualState();
	if (autoUpdateQuery()) _timer.start();
}

void SearchFilterWidget::reset()
{
	bool autoUpdateStatus = autoUpdateQuery();
	setAutoUpdateQuery(false);
	_reset();
	setAutoUpdateQuery(autoUpdateStatus);
	commandUpdate();
}

QMap<QString, QVariant> SearchFilterWidget::getState() const
{
	QStringList toSave = propertiesToSave();
	QMap<QString, QVariant> ret;
	foreach(const QString &prop, toSave) {
		ret[prop] = property(prop.toLatin1().data());
	}
	return ret;
}

void SearchFilterWidget::restoreState(const QMap<QString, QVariant> &state)
{
	setAutoUpdateQuery(false);
	foreach(const QString &prop, state.keys()) {
		setProperty(prop.toLatin1().data(), state.value(prop));
	}
	updateTitle(currentTitle());
	setAutoUpdateQuery(true);
}

void SearchFilterWidget::updateFeatures()
{
}
