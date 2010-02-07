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

#include "gui/TextFilterWidget.h"

#include <QHBoxLayout>

PreferenceItem<int> TextFilterWidget::textSearchHistorySize("mainWindow", "searchBarHistorySize", 100);

TextFilterWidget::TextFilterWidget(QWidget *parent) : SearchFilterWidget(parent)
{
	_propsToSave << "text";

	searchButton = new QPushButton(tr("Search"), this);
	searchButton->setIcon(QIcon(":/images/icons/ldap_lookup.png"));
	searchButton->setShortcut(QKeySequence("Ctrl+Return"));
	connect(searchButton, SIGNAL(clicked()), this, SLOT(searchButtonClicked()));
	
	_searchField = new QComboBox(this);
	_searchField->setMinimumWidth(150);
	_searchField->setEditable(true);
	_searchField->setMaxCount(textSearchHistorySize.value());
	_searchField->setSizePolicy(QSizePolicy::Expanding, _searchField->sizePolicy().verticalPolicy());
	_searchField->setInsertPolicy(QComboBox::NoInsert);
	connect(_searchField->lineEdit(), SIGNAL(returnPressed()), searchButton, SLOT(click()));
	connect(_searchField->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(onSearchTextChanged(QString)));
	connect(_searchField, SIGNAL(activated(int)), this, SLOT(onItemSelected(int)));

	resetText = new QToolButton(this);
	resetText->setIcon(QIcon(":/images/icons/reset-search.png"));
	resetText->setToolTip(tr("Clear search text"));
	resetText->setEnabled(false);
	connect(resetText, SIGNAL(clicked()), this, SLOT(resetSearchText()));
	
	QHBoxLayout *hLayout = new QHBoxLayout(this);
	hLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->setSpacing(5);
	hLayout->addWidget(resetText);
	hLayout->addWidget(_searchField);
	hLayout->addWidget(searchButton);
}

QString TextFilterWidget::currentTitle() const
{
	if (!_searchField->lineEdit()->text().isEmpty()) return _searchField->lineEdit()->text();
	return tr("Text search");
}

void TextFilterWidget::onItemSelected(int item)
{
	if (item != 0) {
		QString itemString(_searchField->itemText(item));
		_searchField->removeItem(item);
		setText(itemString);
		searchButtonClicked();
	}
}

void TextFilterWidget::searchButtonClicked()
{
	QString input(_searchField->lineEdit()->text().trimmed());
	if (!input.isEmpty() && _searchField->itemText(0) != input) {
		_searchField->insertItem(0, input);
		_searchField->setCurrentIndex(0);
		commandUpdate();
	}
}

void TextFilterWidget::onSearchTextChanged(const QString &text)
{
	resetText->setEnabled(!text.isEmpty());
}

void TextFilterWidget::resetSearchText()
{
	if (text().isEmpty()) return;
	_searchField->clearEditText();
	_searchField->setFocus();
	commandUpdate();
}

void TextFilterWidget::_reset()
{
	resetSearchText();
}
