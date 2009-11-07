/*
 *  Copyright (C) 2008, 2009  Alexandre Courbot
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

#include "core/jmdict/JMdictDefs.h"
#include "gui/jmdict/JMdictPreferences.h"
#include "gui/jmdict/JMdictGUIPlugin.h"

JMdictPreferences::JMdictPreferences(QWidget *parent) : PreferencesWindowCategory(tr("Vocabulary entries"), parent)
{
	setupUi(this);

	// Initialize the definitions list
	for (int i = 0; !JMdictMiscEntitiesLongDesc[i].isEmpty(); i++) {
		QString s(JMdictMiscEntitiesLongDesc[i]);
		s[0] = s[0].toUpper();
		displayedDefs->addItem(s);
	}

	connect(filterButton, SIGNAL(clicked()), this, SLOT(onFilterButtonClicked()));
	connect(unFilterButton, SIGNAL(clicked()), this, SLOT(onUnFilterButtonClicked()));
}

void JMdictPreferences::onFilterButtonClicked()
{
	foreach (QListWidgetItem *item, displayedDefs->selectedItems()) {
		QString s(item->text());
		delete item;
		filteredDefs->addItem(s);
	}
}

void JMdictPreferences::onUnFilterButtonClicked()
{
	foreach (QListWidgetItem *item, filteredDefs->selectedItems()) {
		QString s(item->text());
		delete item;
		displayedDefs->addItem(s);
	}
}

void JMdictPreferences::refresh()
{
}

void JMdictPreferences::applySettings()
{
}
