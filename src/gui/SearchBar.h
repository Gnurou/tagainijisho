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

#ifndef __GUI_SEARCHBAR_H_
#define __GUI_SEARCHBAR_H_

#include "gui/ClickableLabel.h"
#include "gui/RelativeDateEdit.h"
#include "gui/MultiStackedWidget.h"
#include "gui/TagsDialogs.h"

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMovie>
#include <QList>
#include <QCheckBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QTimer>
#include <QGroupBox>
#include <QSet>
#include <QMap>

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

class SearchBar;

/**
 * Let the user select the entry type (vocabulary of kanji) to filter.
 */
class EntryTypeSelectionWidget : public SearchFilterWidget
{
	Q_OBJECT
public:
	typedef enum { All = 0, Vocabulary = 1, Kanjis = 2 } Type;

private:
	QComboBox *_comboBox;
	SearchBar *_bar;

protected:
	virtual void _reset();

public:
	EntryTypeSelectionWidget(SearchBar *parent = 0);
	virtual QString name() const { return "entrytypeselector"; }
	virtual QString currentTitle() const { return tr("Entry type filter"); }
	virtual QString currentCommand() const;

	int type() const { return _comboBox->currentIndex(); }
	void setType(int newType) { _comboBox->setCurrentIndex(newType); }
	Q_PROPERTY(int type READ type WRITE setType)

private slots:
	void onComboBoxChanged(int index);
};

class JLPTOptionsWidget : public SearchFilterWidget {
	Q_OBJECT
private:
	QCheckBox *JLPT4CheckBox, *JLPT3CheckBox, *JLPT2CheckBox, *JLPT1CheckBox;

protected:
	virtual void _reset();

public:
	JLPTOptionsWidget(QWidget *parent = 0);
	virtual QString name() const { return "jlptoptions"; }
	virtual QString currentTitle() const;
	virtual QString currentCommand() const;

	/**
	 * Returns the list of currently checked levels. The returned
	 * list contains integers exclusively.
	 */
	QList<QVariant> levels() const;
	/**
	 * Set the list of currently checked levels. The list
	 * must contains integers exclusively.
	 */
	void setLevels(const QList<QVariant> &filter);
	Q_PROPERTY(QList<QVariant> levels READ levels WRITE setLevels)
};

class StudyOptionsWidget : public SearchFilterWidget {
	Q_OBJECT
public:
	typedef enum { All = 0, Studied = 1, NonStudied = 2 } StudyFilter;

protected:
	virtual void _reset();

private:
	QRadioButton *allEntriesButton, *studiedEntriesButton, *nonStudiedEntriesButton;
	QSpinBox *minSpinBox, *maxSpinBox;

	QGroupBox *studyBox;
	RelativeDateEdit *_studyMinDate, *_studyMaxDate;

	QGroupBox *trainBox;
	RelativeDateEdit *_trainMinDate, *_trainMaxDate;

	QGroupBox *mistakeBox;
	RelativeDateEdit *_mistakenMinDate, *_mistakenMaxDate;

public:
	StudyOptionsWidget(QWidget *parent = 0);
	virtual QString name() const { return "studyoptions"; }
	virtual QString currentTitle() const;
	virtual QString currentCommand() const;

	int studyState() const;
	void setStudyState(int state);
	Q_PROPERTY(int studyState READ studyState WRITE setStudyState)

	int studyMinScore() const { return minSpinBox->value(); }
	void setStudyMinScore(int value) { minSpinBox->setValue(value); }
	Q_PROPERTY(int studyMinScore READ studyMinScore WRITE setStudyMinScore)
	int studyMaxScore() const { return maxSpinBox->value(); }
	void setStudyMaxScore(int value) { maxSpinBox->setValue(value); }
	Q_PROPERTY(int studyMaxScore READ studyMaxScore WRITE setStudyMaxScore)

	QString studyMinDate() const { return _studyMinDate->dateString(); }
	void setStudyMinDate(const QString &date) { _studyMinDate->setDateString(date); }
	Q_PROPERTY(QString studyMinDate READ studyMinDate WRITE setStudyMinDate)
	QString studyMaxDate() const { return _studyMaxDate->dateString(); }
	void setStudyMaxDate(const QString &date) { _studyMaxDate->setDateString(date); }
	Q_PROPERTY(QString studyMaxDate READ studyMaxDate WRITE setStudyMaxDate)

	QString trainMinDate() const { return _trainMinDate->dateString(); }
	void setTrainMinDate(const QString &date) { _trainMinDate->setDateString(date); }
	Q_PROPERTY(QString trainMinDate READ trainMinDate WRITE setTrainMinDate)
	QString trainMaxDate() const { return _trainMaxDate->dateString(); }
	void setTrainMaxDate(const QString &date) { _trainMaxDate->setDateString(date); }
	Q_PROPERTY(QString trainMaxDate READ trainMaxDate WRITE setTrainMaxDate)

	QString mistakenMinDate() const { return _mistakenMinDate->dateString(); }
	void setMistakenMinDate(const QString &date) { _mistakenMinDate->setDateString(date); }
	Q_PROPERTY(QString mistakenMinDate READ mistakenMinDate WRITE setMistakenMinDate)
	QString mistakenMaxDate() const { return _mistakenMaxDate->dateString(); }
	void setMistakenMaxDate(const QString &date) { _mistakenMaxDate->setDateString(date); }
	Q_PROPERTY(QString mistakenMaxDate READ mistakenMaxDate WRITE setMistakenMaxDate)

private slots:
	/**
	 * This slot is used whenever a radio button is toggled
	 * and emits the commandUpdated() signal only when the
	 * button is checked. This avoids emitting two searches
	 * every time a button is toggled.
	 */
	void onRadioButtonToggled(bool toggled);

protected slots:
	void checkMinSpinBoxValue(int newMaxValue);
	void checkMaxSpinBoxValue(int newMinValue);
};

class TagsSearchWidget : public SearchFilterWidget {
	Q_OBJECT
private:
	TagsLineInput *lineInput;
	QPushButton *tagsButton;

private slots:
	void populateTagsMenu();

protected:
	virtual void _reset();
	void focusInEvent(QFocusEvent *event);

public:
	TagsSearchWidget(QWidget *parent = 0);
	virtual QString name() const { return "tagssearch"; }
	virtual QString currentTitle() const;
	virtual QString currentCommand() const;

	QString tags() const { return lineInput->text(); }
	void setTags(const QString &tags);
	Q_PROPERTY(QString tags READ tags WRITE setTags)

public slots:
	void tagMenuClicked(QAction *action);
};

class NotesSearchWidget : public SearchFilterWidget {
	Q_OBJECT
private:
	QLineEdit *words;

private slots:
	void onTextChanged(const QString &newText);

protected:
	virtual void _reset();
	void focusInEvent(QFocusEvent *event);

public:
	NotesSearchWidget(QWidget *parent = 0);
	virtual QString name() const { return "notessearch"; }
	virtual QString currentTitle() const;
	virtual QString currentCommand() const;

	QString notes() const { return words->text(); }
	void setNotes(const QString &notes);
	Q_PROPERTY(QString notes READ notes WRITE setNotes)
};

class SearchBar : public QWidget
{
	Q_OBJECT
private:
	QComboBox *_searchField;
	QToolButton *resetText;
	EntryTypeSelectionWidget *_entryTypeSelector;
	QPushButton *searchButton;
	QMovie *searchAnim;
	ClickableLabel *searchStatus;
	QMap<QString, QSet<SearchFilterWidget *> > _disabledFeatures;

	MultiStackedWidget *_extenders;
	QMap<QString, SearchFilterWidget *> _extendersList;

private slots:
	void searchButtonClicked();
	void enableFeature(const QString &feature);
	void disableFeature(const QString &feature);
	void resetSearchText();
	void onSearchTextChanged(const QString &text);

public:
	SearchBar(QWidget *parent = 0);
	QString text() const;

	void registerExtender(SearchFilterWidget *extender);
	SearchFilterWidget *getExtender(const QString &name) { return _extendersList.value(name); }
	void removeExtender(SearchFilterWidget *extender);
	bool isFeatureEnabled(const QString &feature);

	QMap<QString, QVariant> getState() const;
	void restoreState(const QMap<QString, QVariant> &state);

	static PreferenceItem<int> searchBarHistorySize;

public slots:
	void search();
	void searchVocabulary();
	void searchKanjis();
	void searchAll();

	void searchStarted();
	void searchCompleted();

	void stealFocus();

	void reset();

signals:
	void startSearch(const QString &search);
	void stopSearch();
};

#endif
