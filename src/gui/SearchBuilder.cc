/*
 *  Copyright (C) 2008  Alexandre Courbot
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

#include "gui/SearchBuilder.h"

SearchBuilder::SearchBuilder(QObject *parent) : QObject(parent)
{
}

void SearchBuilder::reset()
{
	foreach (SearchFilterWidget *filter, _filters.values()) {
		filter->setAutoUpdateQuery(false);
		filter->reset();
		filter->setAutoUpdateQuery(true);
	}
	runSearch();
}

QString SearchBuilder::commands() const
{
	QStringList cmds;
	foreach (const SearchFilterWidget *filter, _filters.values()) {
		if (filter->isEnabled()) {
			QString command(filter->currentCommand().trimmed());
			if (!command.isEmpty()) cmds << command;
		}
	}
	return cmds.join(" ");
}

void SearchBuilder::runSearch()
{
	emit queryRequested(commands());
}

bool SearchBuilder::addSearchFilter(SearchFilterWidget *filter)
{
	if (_filters.contains(filter->name())) return false;
	connect(filter, SIGNAL(commandUpdated()), this, SLOT(runSearch()));
	_filters[filter->name()] = filter;
	return true;
}

void SearchBuilder::removeSearchFilter(const QString &name)
{
	if (_filters.contains(name)) {
		SearchFilterWidget *filter = _filters[name];
		disconnect(filter, SIGNAL(commandUpdated()), this, SLOT(runSearch()));
		_filters.remove(name);
	}
}

QMap<QString, QVariant> SearchBuilder::getState() const
{
	QMap<QString, QVariant> ret;
	foreach (SearchFilterWidget *filter, _filters.values()) {
		QMap<QString, QVariant> state = filter->getState();
		ret[filter->name()] = state;
	}

	return ret;
}

void SearchBuilder::restoreState(const QMap<QString, QVariant> &state)
{
	foreach (SearchFilterWidget *filter, _filters.values()) {
		filter->setAutoUpdateQuery(false);
		filter->reset();
		if (state.contains(filter->name())) {
			filter->restoreState(state.value(filter->name()).toMap());
		}
		filter->setAutoUpdateQuery(true);
	}
}
