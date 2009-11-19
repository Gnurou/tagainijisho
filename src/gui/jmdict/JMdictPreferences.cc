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
#include "core/jmdict/JMdictEntrySearcher.h"
#include "gui/jmdict/JMdictEntryFormatter.h"
#include "gui/jmdict/JMdictPreferences.h"
#include "gui/jmdict/JMdictGUIPlugin.h"

JMdictPreferences::JMdictPreferences(QWidget *parent) : PreferencesWindowCategory(tr("Vocabulary entries"), parent)
{
	setupUi(this);

	connect(filterButton, SIGNAL(clicked()), this, SLOT(onFilterButtonClicked()));
	connect(unFilterButton, SIGNAL(clicked()), this, SLOT(onUnFilterButtonClicked()));
}

void JMdictPreferences::onFilterButtonClicked()
{
	foreach (QListWidgetItem *item, displayedDefs->selectedItems()) {
		filteredDefs->addItem(item->text());
		delete item;
	}
}

void JMdictPreferences::onUnFilterButtonClicked()
{
	foreach (QListWidgetItem *item, filteredDefs->selectedItems()) {
		displayedDefs->addItem(item->text());
		delete item;
	}
}

void JMdictPreferences::refresh()
{
	showJLPT->setChecked(JMdictEntryFormatter::showJLPT.value());
	showKanjis->setChecked(JMdictEntryFormatter::showKanjis.value());
	homophonesCount->setValue(JMdictEntryFormatter::maxHomophonesToDisplay.value());
	studiedHomophonesOnly->setChecked(JMdictEntryFormatter::displayStudiedHomophonesOnly.value());
	lookupVerbBuddy->setChecked(JMdictEntryFormatter::searchVerbBuddy.value());

	filteredDefs->clear();
	displayedDefs->clear();
	const QStringList &filtered(JMdictEntrySearcher::miscPropertiesFilter.value().split(','));
	for (int i = 0; !JMdictMiscEntitiesLongDesc[i].isEmpty(); i++) {
		QString s(JMdictMiscEntitiesLongDesc[i]);
		s[0] = s[0].toUpper();
		if (filtered.contains(JMdictMiscEntitiesShortDesc[i])) filteredDefs->addItem(s);
		else displayedDefs->addItem(s);
	}
}

void JMdictPreferences::applySettings()
{
	JMdictEntryFormatter::showJLPT.set(showJLPT->isChecked());
	JMdictEntryFormatter::showKanjis.set(showKanjis->isChecked());
	JMdictEntryFormatter::maxHomophonesToDisplay.set(homophonesCount->value());
	JMdictEntryFormatter::displayStudiedHomophonesOnly.set(studiedHomophonesOnly->isChecked());
	JMdictEntryFormatter::searchVerbBuddy.set(lookupVerbBuddy->isChecked());
	QStringList filtered, res;
	for (int i = 0; i < filteredDefs->model()->rowCount(); i++) filtered << filteredDefs->item(i)->text();
	for (int i = 0; JMdictMiscEntitiesLongDesc[i] != ""; i++) {
		QString s(JMdictMiscEntitiesLongDesc[i]);
		s[0] = s[0].toUpper();
		if (filtered.contains(s)) res << JMdictMiscEntitiesShortDesc[i];
	}
	JMdictEntrySearcher::miscPropertiesFilter.set(res.join(","));
}
