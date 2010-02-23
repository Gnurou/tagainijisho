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
#include <QClipboard>
#include <QApplication>

PreferenceItem<int> TextFilterWidget::textSearchHistorySize("mainWindow", "searchBarHistorySize", 100);

TextFilterWidget::TextFilterWidget(QWidget *parent) : SearchFilterWidget(parent), _clipboardEnabled(false), _reseted(true)
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
	
	setTabOrder(_searchField, searchButton);
	setTabOrder(searchButton, resetText);
	setFocusProxy(_searchField);
	
	_enableClipboardInputAction = new QAction(tr("Auto-search on clipboard content"), this);
	_enableClipboardInputAction->setCheckable(true);
	connect(_enableClipboardInputAction, SIGNAL(toggled(bool)), this, SLOT(enableClipboardInput(bool)));
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
	if (!input.isEmpty() && (_reseted || _searchField->itemText(0) != input)) {
		_searchField->insertItem(0, input);
		_searchField->setCurrentIndex(0);
		_reseted = false;
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
	_reseted = true;
	_searchField->clearEditText();
	_searchField->setFocus();
	commandUpdate();
}

void TextFilterWidget::_reset()
{
	resetSearchText();
}

void TextFilterWidget::enableClipboardInput(bool enable)
{
	QClipboard *clipboard = QApplication::clipboard();
	if (enable && !_clipboardEnabled) {
		connect(clipboard, SIGNAL(dataChanged()), this, SLOT(onClipboardChanged()));
		connect(clipboard, SIGNAL(selectionChanged()), this, SLOT(onClipboardSelectionChanged()));
		_clipboardEnabled = true;
	}
	else if (!enable && _clipboardEnabled ) {
		disconnect(clipboard, SIGNAL(dataChanged()), this, SLOT(onClipboardChanged()));
		disconnect(clipboard, SIGNAL(selectionChanged()), this, SLOT(onClipboardSelectionChanged()));
		_clipboardEnabled = false;
	}
}

void TextFilterWidget::onClipboardChanged()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString cText(clipboard->text(QClipboard::Clipboard));
	if (cText.isEmpty() || cText == text()) return;
	setText(cText);
	commandUpdate();
}

void TextFilterWidget::onClipboardSelectionChanged()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString cText(clipboard->text(QClipboard::Selection));
	if (cText.isEmpty() || cText == text()) return;
	setText(cText);
	commandUpdate();
}
