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

#include <QAction>
#include <QHash>
#include <QValidator>
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

protected slots:
	void onItemEntered(QListWidgetItem *item);

public:
	ComplementsList(QWidget *parent = 0);
	QSet<uint> currentSelection() const;

public slots:
	/// Add a complement character to the list
	QListWidgetItem *addComplement(const QString &repr, uint kanji);
	/// Start a new series of characters with the given stroke number
	QListWidgetItem *setCurrentStrokeNbr(int strokeNbr);
};

#include "gui/kanjidic2/ui_KanjiSelector.h"

class QLineEdit;
/**
 * A base class for building kanji selectors based on some of their components.
 */
class KanjiSelector : public QFrame, protected Ui::KanjiSelector
{
	Q_OBJECT
private:
	QLineEdit *_associate;
	/// Used when the widget is hidden to tell it should update
	/// itself once shown again
	bool _outOfSyncWithAssociate;
	/// The currently displayed complements, code and representation
	QSet<QPair<uint, QString > > _currentComplements;
	bool _ignoreAssociateSignals;
	/**
	 * Returns all the complements that have been inputted into
	 * the associate
	 */
	QSet<uint> associateComplements() const;

protected:
	KanjiSelector(QWidget *parent = 0);
	/// Returns the string representation suitable for the given complement kanji.
	/// The default is to return the string corresponding to the unicode of the
	/// given character.
	virtual QString complementRepr(uint kanji) const;
	/// Invert method of complementRepr
	virtual uint complementCode(const QString &repr) const;
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
	virtual void showEvent (QShowEvent *event);
	
protected slots:
	virtual void onSelectionChanged();
	virtual void onAssociateChanged();
	void updateAssociateFromSelection(QSet<uint> selection);

public:
	virtual ~KanjiSelector() {}
	virtual void reset() = 0;
	const QSet<QPair<uint, QString> > &currentComplements() const { return _currentComplements; }
	/**
	 * Set the current selection to the given list of complements.
	 */
	void setSelection(const QSet<uint> &selection);
	/**
	 * Associate this selector to a given line edit. Doing so provides
	 * an additional selection method for the user, who can input or
	 * delete complementary characters using the line edit.
	 */
	void associateTo(QLineEdit *associate);
	QLineEdit *associate() { return _associate; }
	ComplementsList *complementsList() { return _complementsList; }

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
 * A validator that only accepts input within the complements list
 * of a given kanji selector.
 */
class KanjiSelectorValidator : public QValidator {
Q_OBJECT
private:
	KanjiSelector *_filter;
public:
	KanjiSelectorValidator(KanjiSelector *filter, QObject *parent = 0);
	KanjiSelector *filter() { return _filter; }
	virtual State validate(QString &input, int &pos) const;
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
	/// Returns the kanji associated with the given radical code
	virtual QString complementRepr(uint kanji) const;
	virtual uint complementCode(const QString &repr) const;
	
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
	virtual QString getCandidatesQuery(const QSet<uint> &selection) const;
	virtual QString getComplementsQuery(const QSet<uint> &selection, const QSet<uint> &candidates) const;

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
	/**
	 * Constructor. The inputter takes ownership of the passed selector which
	 * cannot be used elsewhere.
	 * If useLineEdit is true, a line edit will be associated with the selector
	 * in order to allow the user to directly input kanji.
	 */
	KanjiInputter(KanjiSelector *selector, bool useLineEdit = false, QWidget *parent = 0);
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
