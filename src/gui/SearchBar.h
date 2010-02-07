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

#include "gui/SearchFilterWidget.h"

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

class SearchBar;



class SearchBar : public QWidget
{
	Q_OBJECT
private:
	QHBoxLayout *_searchBarLayout;
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
	void onComponentSearchKanjiSelected(const QString &text);

public:
	SearchBar(QWidget *parent = 0);
	QString text() const;

	void registerExtender(SearchFilterWidget *extender);
	SearchFilterWidget *getExtender(const QString &name) { return _extendersList.value(name); }
	void removeExtender(SearchFilterWidget *extender);
	bool isFeatureEnabled(const QString &feature);
	QBoxLayout *layout() { return _searchBarLayout; }
	QComboBox *searchField() { return _searchField; }

	QMap<QString, QVariant> getState() const;
	void restoreState(const QMap<QString, QVariant> &state);


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
