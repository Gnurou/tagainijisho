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

#ifndef __GUI_SEARCHFILTERWIDGET_H
#define __GUI_SEARCHFILTERWIDGET_H

#include <QVariant>
#include <QString>
#include <QMap>
#include <QWidget>
#include <QTimer>

class SearchFilterWidget : public QWidget {
	Q_OBJECT
private:
	QTimer _timer;
	const QString _feature;
	bool _autoUpdateQuery;

	/**
	 * Updates all the visual aspects of this widget
	 * according to its state, that is its title and
	 * enabled features requests. Do not emit any command.
	 */
	void updateVisualState();

protected:
	/**
	 * Subclasses can put the name of the properties that define their
	 * state here to avoid having to overload propertiesToSave().
	 */
	QStringList _propsToSave;

	/**
	 * Return a list of the properties that define the state of this
	 * extender, i.e. properties that will be returned by getState().
	 *
	 * This default version just returns the static list _propsToSave.
	 */
	virtual QStringList propertiesToSave() const { return _propsToSave; }

	/**
	 * Actual reset method, to be implemented by subclasses. It is guaranteed
	 * that auto-update is disabled when this method is called.
	 */
	virtual void _reset() = 0;

public:
	SearchFilterWidget(QWidget *parent = 0, const QString &feature = "");

	/**
	 * Returns the particular exclusive feature implemented by
	 * this extender, if any.
	 */
	const QString &feature() const { return _feature; }

	/**
	 * Emits the enabledFeature() and disableFeature() signals to
	 * reflect the restrictions imposed by this widget according to
	 * its current state.
	 */
	virtual void updateFeatures();

	/**
	 * Returns the name of the extender. As it is used as an identifier,
	 * it must be unique.
	 */
	virtual QString name() const = 0;

	/**
	 * Returns the command(s) that reflect the current
	 * state of the widget.
	 */
	virtual QString currentCommand() const = 0;

	/**
	 * Returns the current title of the widget, that reflects its
	 * current state.
	 */
	virtual QString currentTitle() const = 0;

	/**
	 * Returns the current state of the extender.
	 */
	QMap<QString, QVariant> getState() const;

	/**
	 * Restore the state of the extender from a state
	 * obtained using getState().
	 */
	void restoreState(const QMap<QString, QVariant> &state);

	/**
	 * Returns whether the search query is automatically updated
	 * when the state of this extender changes.
	 */
	bool autoUpdateQuery() const { return _autoUpdateQuery; }

	/**
	 * Sets the auto-update query status of this extender on or off.
	 * It may be desirable to switch auto update off in some cases
	 * in order to avoid bloating
	 * the history.
	 */
	void setAutoUpdateQuery(bool status) { _autoUpdateQuery = status; }

protected slots:
	/**
	 * This slot shall be called every time the state of the
	 * widget has changed and the search should be refined
	 * according to its state.
	 */
	void commandUpdate();

	/**
	 * Some widgets may trigger the commandUpdated() signal
	 * when their value change but a single user action 
	 * would generate a lot of signal emissions and many
	 * database run. For these widgets it is desirable to
	 * wait a short moment before emitting the commandUpdate()
	 * signal - this is what this slot does.
	 */
	void delayedCommandUpdate();

public slots:
	/**
	 * Resets the widget to its default state.
	 */
	void reset();

signals:
	/**
	 * Emitted by the widget to request the entry searcher
	 * manager to update the command. This signal MUST NOT
	 * be called from somewhere else than commandUpdate()!
	 */
	void commandUpdated();

	/**
	 * Update the title displayed on the extender button.
	 * This is used in order to reflect the current state of
	 * the extender when closed.
	 */
	void updateTitle(const QString &newTitle);

	/**
	 * Emitted in order to request other extenders to enable
	 * a given feature.
	 */
	void enableFeature(const QString &feature);

	/**
	 * Emitted in order to request other extenders to disable
	 * a given feature.
	 */
	void disableFeature(const QString &feature);
};

#endif
