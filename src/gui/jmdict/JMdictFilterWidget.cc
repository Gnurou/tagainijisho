/*
 *  Copyright (C) 2010  Alexandre Courbot
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

#include "gui/jmdict/JMdictFilterWidget.h"
#include "core/jmdict/JMdictPlugin.h"
#include "core/jmdict/JMdictEntrySearcher.h"
#include "gui/KanjiValidator.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

JMdictFilterWidget::JMdictFilterWidget(QWidget *parent) : SearchFilterWidget(parent, "wordsdic")
{
	_propsToSave << "containedKanjis" << "containedComponents" << "studiedKanjisOnly" << "pos" << "dial" << "field" << "misc";
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	{
		_containedKanjis = new QLineEdit(this);
		KanjiValidator *kanjiValidator = new KanjiValidator(this);
		_containedKanjis->setValidator(kanjiValidator);
		_containedComponents = new QLineEdit(this);
		_containedComponents->setValidator(kanjiValidator);

		QGridLayout *gLayout = new QGridLayout();
		gLayout->addWidget(new QLabel(tr("With kanji:"), this), 0, 0);
		QHBoxLayout *hLayout = new QHBoxLayout();
		_studiedKanjisCheckBox = new QCheckBox(tr("Using studied kanji only"));
		hLayout->addWidget(_containedKanjis);
		hLayout->addWidget(_studiedKanjisCheckBox);
		gLayout->addLayout(hLayout, 0, 1);

		gLayout->addWidget(new QLabel(tr("With components:"), this), 1, 0);
		gLayout->addWidget(_containedComponents, 1, 1);

		mainLayout->addLayout(gLayout);
		connect(_containedKanjis, SIGNAL(textChanged(const QString &)), this, SLOT(commandUpdate()));
		connect(_containedComponents, SIGNAL(textChanged(const QString &)), this, SLOT(commandUpdate()));
		connect(_studiedKanjisCheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));

	}
	{
		QHBoxLayout *hLayout = new QHBoxLayout();

		_posButton = new QPushButton(tr("Part of speech"), this);
		QMenu *menu = new QMenu(this);
		QActionGroup *actionGroup = addCheckableProperties(JMdictPlugin::posEntities(), menu);
		_posButton->setMenu(menu);
		hLayout->addWidget(_posButton);
		connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onPosTriggered(QAction *)));
		_dialButton = new QPushButton(tr("Dialect"), this);
		menu = new QMenu(this);
		actionGroup = addCheckableProperties(JMdictPlugin::dialectEntities(), menu);
		_dialButton->setMenu(menu);
		hLayout->addWidget(_dialButton);
		connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onDialTriggered(QAction *)));
		_fieldButton = new QPushButton(tr("Field"), this);
		menu = new QMenu(this);
		actionGroup = addCheckableProperties(JMdictPlugin::fieldEntities(), menu);
		_fieldButton->setMenu(menu);
		hLayout->addWidget(_fieldButton);
		connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onFieldTriggered(QAction *)));
		_miscButton = new QPushButton(tr("Misc"), this);
		menu = new QMenu(this);
		actionGroup = addCheckableProperties(JMdictPlugin::miscEntities(), menu);
		_miscButton->setMenu(menu);
		hLayout->addWidget(_miscButton);
		connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onMiscTriggered(QAction *)));
		updateMiscFilteredProperties();
		connect(&JMdictEntrySearcher::miscPropertiesFilter, SIGNAL(valueChanged(QVariant)), this, SLOT(updateMiscFilteredProperties()));

		mainLayout->addLayout(hLayout);
	}
	mainLayout->setContentsMargins(0, 0, 0, 0);
}

QString JMdictFilterWidget::currentCommand() const
{
	QString ret;
	QString kanjis = _containedKanjis->text();
	if (!kanjis.isEmpty()) {
		bool first = true;
		ret += " :haskanji=";
		foreach(QChar c, kanjis) {
			if (!first) ret +=",";
			else first = false;
			ret += QString("\"%1\"").arg(c);
		}
	}
	if (_studiedKanjisCheckBox->isChecked()) ret += " :withstudiedkanjis";
	kanjis = _containedComponents->text();
	if (!kanjis.isEmpty()) {
		bool first = true;
		ret += " :hascomponent=";
		foreach(QChar c, kanjis) {
			if (!first) ret +=",";
			else first = false;
			ret += QString("\"%1\"").arg(c);
		}
	}
	if (!_posList.isEmpty()) ret += " :pos=" + _posList.join(",");
	if (!_dialList.isEmpty()) ret += " :dial=" + _dialList.join(",");
	if (!_fieldList.isEmpty()) ret += " :field=" + _fieldList.join(",");
	if (!_miscList.isEmpty()) ret += " :misc=" + _miscList.join(",");
	return ret;
}

QString JMdictFilterWidget::currentTitle() const
{
	QString contains;
	QString kanjis = _containedKanjis->text();
	if (!kanjis.isEmpty()) {
		bool first = true;
		contains += tr(" with ");
		foreach(QChar c, kanjis) {
			if (!first) contains +=",";
			else first = false;
			contains += c;
		}
	}
	if (_studiedKanjisCheckBox->isChecked()) {
		if (!kanjis.isEmpty()) contains += tr(", studied kanji only");
		else contains += tr(" with studied kanji");
	}
	kanjis = _containedComponents->text();
	if (!kanjis.isEmpty()) {
		bool first = true;
		contains += tr(" with component ");
		foreach(QChar c, kanjis) {
			if (!first) contains +=",";
			else first = false;
			contains += c;
		}
	}

	QStringList propsList = _posList + _dialList + _fieldList + _miscList;
	QString props = propsList.join(",");

	if (!props.isEmpty()) props = " (" + props + ")";
	QString ret(props + contains);
	if (!ret.isEmpty()) ret = tr("Vocabulary") + ret;
	else ret = tr("Vocabulary");
	return ret;
}

void JMdictFilterWidget::onPosTriggered(QAction *action)
{
	if (action->isChecked()) {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_posList << JMdictPlugin::posEntities()[propertyIndex].first;
	}
	else {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_posList.removeOne(JMdictPlugin::posEntities()[propertyIndex].first);
	}
	if (!_posList.isEmpty()) _posButton->setText(tr("Pos:") + _posList.join(","));
	else _posButton->setText(tr("Part of speech"));
	commandUpdate();
}

void JMdictFilterWidget::onDialTriggered(QAction *action)
{
	if (action->isChecked()) {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_dialList << JMdictPlugin::dialectEntities()[propertyIndex].first;
	}
	else {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_dialList.removeOne(JMdictPlugin::dialectEntities()[propertyIndex].first);
	}
	if (!_dialList.isEmpty()) _dialButton->setText(tr("Dial:") + _dialList.join(","));
	else _dialButton->setText(tr("Dialect"));
	commandUpdate();
}

void JMdictFilterWidget::onFieldTriggered(QAction *action)
{
	if (action->isChecked()) {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_fieldList << JMdictPlugin::fieldEntities()[propertyIndex].first;
	}
	else {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_fieldList.removeOne(JMdictPlugin::fieldEntities()[propertyIndex].first);
	}
	if (!_fieldList.isEmpty()) _fieldButton->setText(tr("Field:") + _fieldList.join(","));
	else _fieldButton->setText(tr("Field"));
	emit commandUpdate();
}

void JMdictFilterWidget::onMiscTriggered(QAction *action)
{
	if (action->isChecked()) {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_miscList << JMdictPlugin::miscEntities()[propertyIndex].first;
	}
	else {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_miscList.removeOne(JMdictPlugin::miscEntities()[propertyIndex].first);
	}
	if (!_miscList.isEmpty()) _miscButton->setText(tr("Misc:") + _miscList.join(","));
	else _miscButton->setText(tr("Misc"));
	emit commandUpdate();
}

void JMdictFilterWidget::_reset()
{
	_containedKanjis->clear();
	_containedComponents->clear();
	_studiedKanjisCheckBox->setChecked(false);
	foreach (QAction *action, _posButton->menu()->actions()) if (action->isChecked()) action->trigger();
	_posList.clear();
	foreach (QAction *action, _dialButton->menu()->actions()) if (action->isChecked()) action->trigger();
	_dialList.clear();
	foreach (QAction *action, _fieldButton->menu()->actions()) if (action->isChecked()) action->trigger();
	_fieldList.clear();
	foreach (QAction *action, _miscButton->menu()->actions()) if (action->isChecked()) action->trigger();
	_miscList.clear();
}

void JMdictFilterWidget::updateFeatures()
{
	if (!_containedKanjis->text().isEmpty() || !_containedComponents->text().isEmpty() || _studiedKanjisCheckBox->isChecked() || !_posList.isEmpty() || !_dialList.isEmpty() || !_fieldList.isEmpty() || !_miscList.isEmpty()) emit disableFeature("kanjidic");
	else emit enableFeature("kanjidic");
}

void JMdictFilterWidget::setPos(const QStringList &list)
{
	_posList.clear();
	foreach(QAction *action, _posButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(JMdictPlugin::posEntities()[action->property("TJpropertyIndex").toInt()].first))
			action->trigger();
	}
}

void JMdictFilterWidget::setDial(const QStringList &list)
{
	_dialList.clear();
	foreach(QAction *action, _dialButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(JMdictPlugin::dialectEntities()[action->property("TJpropertyIndex").toInt()].first))
			action->trigger();
	}
}

void JMdictFilterWidget::setField(const QStringList &list)
{
	_fieldList.clear();
	foreach(QAction *action, _fieldButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(JMdictPlugin::fieldEntities()[action->property("TJpropertyIndex").toInt()].first))
			action->trigger();
	}
}

void JMdictFilterWidget::setMisc(const QStringList &list)
{
	_miscList.clear();
	foreach(QAction *action, _miscButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(JMdictPlugin::miscEntities()[action->property("TJpropertyIndex").toInt()].first))
			action->trigger();
	}
}

