/*
 *  Copyright (C) 2010 Alexandre Courbot
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

#ifndef __GUI_KANJI_SELECTOR_H
#define __GUI_KANJI_SELECTOR_H

#include "gui/ScrollBarSmoothScroller.h"
#include "gui/kanjidic2/KanjiResultsView.h"

#include <QListWidget>

/**
 * A List view designed to display the complements of the current selection.
 */
class ComplementsList : public QListWidget
{
	Q_OBJECT
private:
	QFont baseFont;
	QFont labelFont;
	ScrollBarSmoothScroller _sscroll;
	void setupGridSize();

public:
	ComplementsList(QWidget *parent = 0);
	QSet<uint> currentSelection() const;

public slots:
	/// Add a complement character to the list
	QListWidgetItem *addComplement(int kanji);
	/// Start a new series of characters with the given stroke number
	QListWidgetItem *setCurrentStrokeNbr(int strokeNbr);
};

#include "gui/ui_KanjiSelector.h"

/**
 * A base class for building kanji selectors based on some of their components.
 */
class KanjiSelector : public QFrame, protected Ui::KanjiSelector
{
	Q_OBJECT
protected slots:
	virtual void onSelectionChanged();

protected:
	/// Returns the SQL query that should be run in order to get the results list
	/// corresponding to the given selection. Results should comprehend a single
	/// column with the kanji ids.
	virtual QString getCandidatesQuery(const QSet<uint> &selection) const = 0;
	/// Returns the SQL query that should be run in order to get the complements list
	/// corresponding to the given selection
	virtual QString getComplementsQuery(const QSet<uint> &selection, const QSet<uint> &candidates) const = 0;
	
	/**
	 * Returns the list of candidates corresponding to the given selection. Also
	 * emits the startQuery, foundResult and endQuery signals as results are found.
	 */
	virtual QSet<uint> getCandidates(const QSet<uint> &selection);
	virtual void updateComplementsList(const QSet<uint> &selection, const QSet<uint> &candidates);

public:
	KanjiSelector(QWidget *parent = 0);
	virtual ~KanjiSelector() {}
	virtual void reset() = 0;
	/**
	 * Set the current selection to the given list of complements.
	 */
	void setSelection(const QSet<uint> &selection);

signals:
	/**
	 * Emitted when the complements selection is changed.
	 */
	void selectionChanged(const QSet<uint> &selection);

	void startQuery();
	void foundResult(const QString &kanji);
	void endQuery();
};

/**
 * A kanji selector based on radicals.
 */
class RadicalKanjiSelector : public KanjiSelector
{
	Q_OBJECT
protected:
	virtual QString getCandidatesQuery(const QSet<uint> &selection) const;
	virtual QString getComplementsQuery(const QSet<uint> &selection, const QSet<uint> &candidates) const;

public:
	RadicalKanjiSelector(QWidget *parent = 0) : KanjiSelector(parent) {}
	virtual void reset();
};

/**
 * A kanji selector based on components.
 */
class ComponentKanjiSelector : public KanjiSelector
{
	Q_OBJECT
protected:
	QLineEdit *_components;
	QSet<uint> currentComponents() const;
	virtual QString getCandidatesQuery(const QSet<uint> &selection) const;
	virtual QString getComplementsQuery(const QSet<uint> &selection, const QSet<uint> &candidates) const;

protected slots:
	virtual void onComponentsListChanged();
	void updateComponentsFromSelection(QSet<uint> selection);

public:
	ComponentKanjiSelector(QWidget *parent = 0);
	virtual void reset();
};

/**
 * Combines a kanji selector with a kanji results list to make
 * a complete component-based kanji input widget.
 */
class KanjiInputter : public QFrame {
	Q_OBJECT
private:
	KanjiSelector *_selector;
	KanjiResultsView *_results;

public:
	/// The inputter takes ownership of the passed selector.
	KanjiInputter(KanjiSelector *selector, QWidget *parent = 0);
	KanjiSelector *selector() { return _selector; }
	void reset();

signals:
	void kanjiSelected(const QString &kanji);
};

/**
 * An action that shows/hides a given kanji selector.
 */
class KanjiInputPopupAction : public QAction
{
	Q_OBJECT
private:
	KanjiInputter * _popup;
	QWidget *focusWidget;

protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);

protected slots:
	void togglePopup(bool status);
	void onComponentSearchKanjiSelected(const QString &kString);
	void onFocusChanged(QWidget *old, QWidget *now);

public:
	KanjiInputPopupAction(KanjiInputter* popup, const QString& title, QWidget* parent = 0);
};

#endif
