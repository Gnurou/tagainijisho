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
#include "gui/TJLineEdit.h"

#include <QHBoxLayout>
#include <QApplication>

PreferenceItem<int> TextFilterWidget::textSearchHistorySize("mainWindow", "searchBarHistorySize", 100);

TextFilterWidget::TextFilterWidget(QWidget *parent) : SearchFilterWidget(parent), _reseted(true)
{
	_propsToSave << "text";

	_searchField = new QComboBox(this);
	_searchField->setMinimumWidth(150);
	_searchField->setEditable(true);
	_searchField->setMaxCount(textSearchHistorySize.value());
	_searchField->setSizePolicy(QSizePolicy::Expanding, _searchField->sizePolicy().verticalPolicy());
	_searchField->setInsertPolicy(QComboBox::NoInsert);
	_searchField->setLineEdit(new TJLineEdit());
	connect(_searchField->lineEdit(), SIGNAL(returnPressed()), this, SLOT(runSearch()));
	connect(_searchField->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(onSearchTextChanged(QString)));
	connect(_searchField, SIGNAL(activated(int)), this, SLOT(onItemSelected(int)));

	QHBoxLayout *hLayout = new QHBoxLayout(this);
	hLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->setSpacing(5);
	hLayout->addWidget(_searchField);
	
	setFocusProxy(_searchField);	
}

QString TextFilterWidget::currentTitle() const
{
	if (!_searchField->lineEdit()->text().isEmpty()) return _searchField->lineEdit()->text();
	return tr("Text search");
}

void TextFilterWidget::setText(const QString &text)
{
	_searchField->lineEdit()->setText(text);
	commandUpdate();
}

void TextFilterWidget::onItemSelected(int item)
{
	if (item != 0) {
		QString itemString(_searchField->itemText(item));
		_searchField->removeItem(item);
		setText(itemString);
		runSearch();
	}
}

void TextFilterWidget::runSearch()
{
	QString input(_searchField->lineEdit()->text().trimmed());
	if (!input.isEmpty() && (_reseted || _searchField->itemText(0) != input)) {
		_searchField->insertItem(0, input);
		_searchField->setCurrentIndex(0);
		_reseted = false;
		commandUpdate();
	}
}

void TextFilterWidget::onSearchTextChanged(const QString &text)
{
	delayedCommandUpdate();
}

void TextFilterWidget::resetSearchText()
{
	if (text().isEmpty()) return;
	_reseted = true;
	_searchField->clearEditText();
	_searchField->setFocus();
	commandUpdate();
}

void TextFilterWidget::_reset()
{
	resetSearchText();
}
