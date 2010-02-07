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

#include "gui/SearchBar.h"
#include "gui/StudyFilterWidget.h"
#include "gui/JLPTFilterWidget.h"
#include "gui/TagsFilterWidget.h"
#include "gui/NotesFilterWidget.h"

#include <QtDebug>

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSlider>
#include <QCalendarWidget>
#include <QSqlQuery>


PreferenceItem<int> SearchBar::searchBarHistorySize("mainWindow", "searchBarHistorySize", 100);

SearchBar::SearchBar(QWidget *parent) : QWidget(parent)
{
	_searchField = new QComboBox(this);
	_searchField->setMinimumWidth(150);
	_searchField->setEditable(true);
	_searchField->setMaxCount(searchBarHistorySize.value());
	_searchField->setSizePolicy(QSizePolicy::Expanding, _searchField->sizePolicy().verticalPolicy());
	_searchField->setInsertPolicy(QComboBox::NoInsert);

	_entryTypeSelector = new EntryTypeSelectionWidget(this);
	connect(_entryTypeSelector, SIGNAL(enableFeature(const QString &)), this, SLOT(enableFeature(const QString &)));
	connect(_entryTypeSelector, SIGNAL(disableFeature(const QString &)), this, SLOT(disableFeature(const QString &)));

	searchButton = new QPushButton(tr("Search"), this);
	searchButton->setIcon(QIcon(":/images/icons/ldap_lookup.png"));
	searchButton->setShortcut(QKeySequence("Ctrl+Return"));

	resetText = new QToolButton(this);
	resetText->setIcon(QIcon(":/images/icons/reset-search.png"));
	resetText->setToolTip(tr("Clear search text"));
	resetText->setEnabled(false);
	connect(resetText, SIGNAL(clicked()), this, SLOT(resetSearchText()));

	connect(_entryTypeSelector, SIGNAL(commandUpdated()),searchButton, SLOT(click()));

	// The animation shall not be bigger than the size of the button
	int searchAnimSize = searchButton->size().height();
	searchAnim = new QMovie(":/images/search.gif", "gif", this);
	// Only reduce the animation size - do not increase it
	if (searchAnimSize < 35) searchAnim->setScaledSize(QSize(searchAnimSize, searchAnimSize));
	searchAnim->jumpToFrame(0);
	searchStatus = new ClickableLabel(this);
	searchStatus->setMovie(searchAnim);

	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->setSpacing(0);
	vLayout->setContentsMargins(0, 0, 0, 0);
	_searchBarLayout = new QHBoxLayout();
	_searchBarLayout->setContentsMargins(0, 0, 0, 0);
	_searchBarLayout->setSpacing(5);
	_searchBarLayout->addWidget(resetText);
	_searchBarLayout->addWidget(_searchField);
	_searchBarLayout->addWidget(_entryTypeSelector);
	_searchBarLayout->addWidget(searchButton);
	_searchBarLayout->addWidget(searchStatus);
	vLayout->addLayout(_searchBarLayout);

	_extenders = new MultiStackedWidget(this);
	vLayout->addWidget(_extenders);

	connect(searchButton, SIGNAL(clicked()), this, SLOT(searchButtonClicked()));
	connect(searchStatus, SIGNAL(clicked()), this, SIGNAL(stopSearch()));
	connect(_searchField->lineEdit(), SIGNAL(returnPressed()), searchButton, SLOT(click()));
	connect(_searchField->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(onSearchTextChanged(QString)));
//	connect(_searchField, SIGNAL(currentIndexChanged(int)), searchButton, SLOT(click()));

	_extendersList[_entryTypeSelector->name()] = _entryTypeSelector;
	//registerExtender(new StudyFilterWidget(this));
	//registerExtender(new TagsFilterWidget(this));
	//registerExtender(new NotesFilterWidget(this));
	//registerExtender(new JLPTFilterWidget(this));
}

void SearchBar::onComponentSearchKanjiSelected(const QString &text)
{
	_searchField->lineEdit()->insert(text);
}

void SearchBar::onSearchTextChanged(const QString &text)
{
	resetText->setEnabled(!text.isEmpty());
}

void SearchBar::resetSearchText()
{
	if (_searchField->lineEdit()->text().isEmpty()) return;
	_searchField->clearEditText();
	searchButtonClicked();
}

void SearchBar::enableFeature(const QString &feature)
{
	if (!_disabledFeatures.contains(feature)) return;

	SearchFilterWidget *source = qobject_cast<SearchFilterWidget *>(sender());
	QSet<SearchFilterWidget *> &set = _disabledFeatures[feature];
	set.remove(source);
	bool shallEnable = true;
	// Check if any of the feature disablers is active - if not we can enable
	// the feature
	foreach (SearchFilterWidget *extender, set) if (extender->isEnabled()) { shallEnable = false; break; }
	if (shallEnable) {
		_disabledFeatures.remove(feature);
		foreach (SearchFilterWidget *extender, _extendersList.values()) {
			if (extender->feature() == feature) {
					_extenders->setWidgetEnabled(extender, true);
					extender->updateFeatures();
			}
		}
	}
}

void SearchBar::disableFeature(const QString &feature)
{
	if (_disabledFeatures.contains(feature)) _disabledFeatures[feature] = QSet<SearchFilterWidget *>();
	QSet<SearchFilterWidget *> &set = _disabledFeatures[feature];
	SearchFilterWidget *source = qobject_cast<SearchFilterWidget *>(sender());
	if (set.isEmpty()) {
			foreach (SearchFilterWidget *extender, _extendersList.values()) {
				if (extender->feature() == feature) _extenders->setWidgetEnabled(extender, false);
		}
	}
	set << source;
}

QString SearchBar::text() const
{
	QString ret;
	foreach(SearchFilterWidget *extender, _extendersList.values()) {
		if (extender->isEnabled()) ret += extender->currentCommand();
	}
	QString sLineText(_searchField->lineEdit()->text());
	if (!sLineText.isEmpty()) ret += " " + sLineText;
	return ret;
}

void SearchBar::searchButtonClicked()
{
	if (!_searchField->lineEdit()->text().isEmpty() && _searchField->itemText(0) != _searchField->lineEdit()->text()) {
		_searchField->insertItem(0, _searchField->lineEdit()->text());
		_searchField->setCurrentIndex(0);
	}
	emit startSearch(text());
	//stealFocus();
}

void SearchBar::reset()
{
	emit stopSearch();
	foreach (SearchFilterWidget *extender, _extendersList.values()) {
		extender->setAutoUpdateQuery(false);
		extender->reset();
		extender->setAutoUpdateQuery(true);
	}
	_searchField->clearEditText();
	_entryTypeSelector->setAutoUpdateQuery(false);
	_entryTypeSelector->reset();
	_entryTypeSelector->setAutoUpdateQuery(true);
	searchButtonClicked();
}

void SearchBar::searchStarted()
{
	searchAnim->start();
}

void SearchBar::searchCompleted()
{
	searchAnim->stop();
	searchAnim->jumpToFrame(0);
}

void SearchBar::stealFocus()
{
	_searchField->setFocus();
	_searchField->lineEdit()->selectAll();
}

void SearchBar::search()
{
	searchButton->click();
}

void SearchBar::searchVocabulary()
{
	_entryTypeSelector->setType(EntryTypeSelectionWidget::Vocabulary);
}

void SearchBar::searchKanjis()
{
	_entryTypeSelector->setType(EntryTypeSelectionWidget::Kanjis);
}

void SearchBar::searchAll()
{
	_entryTypeSelector->setType(EntryTypeSelectionWidget::All);
}

void SearchBar::registerExtender(SearchFilterWidget *extender)
{
	_extendersList[extender->name()] = extender;
	_extenders->addWidget(extender->currentTitle(), extender);
	connect(extender, SIGNAL(commandUpdated()), searchButton, SLOT(click()));
	connect(extender, SIGNAL(updateTitle(const QString &)), _extenders, SLOT(onTitleChanged(const QString &)));

	connect(extender, SIGNAL(enableFeature(const QString &)), this, SLOT(enableFeature(const QString &)));
	connect(extender, SIGNAL(disableFeature(const QString &)), this, SLOT(disableFeature(const QString &)));
}

void SearchBar::removeExtender(SearchFilterWidget *extender)
{
	disconnect(extender, SIGNAL(commandUpdated()), searchButton, SLOT(click()));
	_extenders->removeWidget(extender);
	_extendersList.remove(_extendersList.key(extender));
}

QMap<QString, QVariant> SearchBar::getState() const
{
	QMap<QString, QVariant> ret;
	ret["searchtext"] = _searchField->lineEdit()->text();
	foreach (SearchFilterWidget *extender, _extendersList.values()) {
		QMap<QString, QVariant> state = extender->getState();
		ret[extender->name()] = state;
	}

	return ret;
}

void SearchBar::restoreState(const QMap<QString, QVariant> &state)
{
	QString searchText(state.value("searchtext").toString());
	if (!searchText.isEmpty()) _searchField->lineEdit()->setText(searchText);
	else _searchField->clearEditText();
	foreach (SearchFilterWidget *extender, _extendersList.values()) {
		extender->setAutoUpdateQuery(false);
		extender->reset();
		if (state.contains(extender->name())) {
			extender->restoreState(state.value(extender->name()).toMap());
		}
		extender->setAutoUpdateQuery(true);
	}
}
