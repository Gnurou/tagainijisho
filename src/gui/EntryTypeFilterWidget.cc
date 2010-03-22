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

#include "gui/EntryTypeFilterWidget.h"

#include <QHBoxLayout>

EntryTypeFilterWidget::EntryTypeFilterWidget(QWidget *parent) : SearchFilterWidget(parent)
{
	_propsToSave << "type";
	QFont fnt(font());
	fnt.setPointSize(fnt.pointSize() - 1);
	setFont(fnt);
	
	_comboBox = new QComboBox(this);
	_comboBox->setFont(fnt);
	_comboBox->addItem(tr("All"), 0);
	_comboBox->addItem(tr("Vocabulary"), 1);
	_comboBox->addItem(tr("Kanji"), 2);
	connect(_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxChanged(int)));

	QHBoxLayout *hLayout = new QHBoxLayout(this);
	hLayout->addWidget(_comboBox);
}

QString EntryTypeFilterWidget::currentCommand() const
{
	switch (_comboBox->itemData(_comboBox->currentIndex()).toInt())
	{
		case 1:
			return ":jmdict";
		case 2:
			return ":kanjidic";
		default:
			return "";
	}
}

void EntryTypeFilterWidget::onComboBoxChanged(int index)
{
	switch (_comboBox->itemData(_comboBox->currentIndex()).toInt())
	{
		case 1:
			emit disableFeature("kanjidic");
			emit enableFeature("wordsdic");
			break;
		case 2:
			emit disableFeature("wordsdic");
			emit enableFeature("kanjidic");
			break;
		default:
			// TODO
			// We cannot emit the feature that way - we must first check whether other widgets
			// allow us to do so.
			emit enableFeature("wordsdic");
			emit enableFeature("kanjidic");
			break;
	}
	commandUpdate();
}

void EntryTypeFilterWidget::_reset()
{
	_comboBox->setCurrentIndex(0);
}
