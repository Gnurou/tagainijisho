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

#ifndef __GUI_SEARCHBUILDER_H
#define __GUI_SEARCHBUILDER_H

#include "gui/SearchFilterWidget.h"

#include <QMap>
#include <QString>

/**
 * This class handles a set of SearchFilterWidgets
 * and emits a query corresponding to their current state
 * each time the state of one of them changes. This query
 * can then be connected to a ResultsList in order to be
 * executed.
 */
class SearchBuilder : public QObject {
	Q_OBJECT
private:	
	QMap<QString, SearchFilterWidget *> _filters;
	
public:
	SearchBuilder(QObject *parent = 0);
	
	/**
	 * Asks this SearchBuilder to manage a filter widget.
	 * The builder does NOT take ownership of the widget -
	 * it just receives its signals and reacts to them
	 * accordingly.
	 */
	bool addSearchFilter(const QString& name, SearchFilterWidget* filter);
	void removeSearchFilter(const QString& name);
	bool contains(const QString& name) { return _filters.contains(name); }
	SearchFilterWidget *get(const QString &name) { return contains(name) ? _filters[name] : 0; }
	QString commands() const;
	
	QMap<QString, QVariant> getState() const;
	void restoreState(const QMap<QString, QVariant> &state);
	
public slots:
	/**
	 * Emit a query that corresponds to the current state of
	 * the registered filters.
	 */
	void runSearch();

signals:
	void queryRequested(const QString &query);
};

#endif
