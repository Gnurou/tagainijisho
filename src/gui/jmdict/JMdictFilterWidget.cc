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
	_propsToSave << "containedKanjis" << "containedComponents" << "studiedKanjisOnly" << "kanaOnlyWords" << "pos" << "dial" << "field" << "misc";
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	{
		_containedKanjis = new QLineEdit(this);
		KanjiValidator *kanjiValidator = new KanjiValidator(this);
		_containedKanjis->setValidator(kanjiValidator);
		_containedComponents = new QLineEdit(this);
		_containedComponents->setValidator(kanjiValidator);

		QGridLayout *gLayout = new QGridLayout();
		gLayout->addWidget(new QLabel(tr("With kanji:"), this), 0, 0);
		gLayout->addWidget(_containedKanjis, 0, 1);
		_studiedKanjisCheckBox = new QCheckBox(tr("Using studied kanji only"));
		gLayout->addWidget(_studiedKanjisCheckBox, 0, 2);

		gLayout->addWidget(new QLabel(tr("With components:"), this), 1, 0);
		gLayout->addWidget(_containedComponents, 1, 1);
		_kanaOnlyCheckBox = new QCheckBox(tr("Include kana-only words"));
		gLayout->addWidget(_kanaOnlyCheckBox, 1, 2);

		mainLayout->addLayout(gLayout);
		connect(_containedKanjis, SIGNAL(textChanged(const QString &)), this, SLOT(commandUpdate()));
		connect(_containedComponents, SIGNAL(textChanged(const QString &)), this, SLOT(commandUpdate()));
		connect(_studiedKanjisCheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
		connect(_kanaOnlyCheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));

	}
	{
		QHBoxLayout *hLayout = new QHBoxLayout();

		_posButton = new QPushButton(tr("Part of speech"), this);
		QMenu *menu = new QMenu(this);
		QActionGroup *actionGroup = addCheckableProperties(JMdictPlugin::posMap(), menu);
		_posButton->setMenu(menu);
		hLayout->addWidget(_posButton);
		connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onPosTriggered(QAction *)));
		_dialButton = new QPushButton(tr("Dialect"), this);
		menu = new QMenu(this);
		actionGroup = addCheckableProperties(JMdictPlugin::dialMap(), menu);
		_dialButton->setMenu(menu);
		hLayout->addWidget(_dialButton);
		connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onDialTriggered(QAction *)));
		_fieldButton = new QPushButton(tr("Field"), this);
		menu = new QMenu(this);
		actionGroup = addCheckableProperties(JMdictPlugin::fieldMap(), menu);
		_fieldButton->setMenu(menu);
		hLayout->addWidget(_fieldButton);
		connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onFieldTriggered(QAction *)));
		_miscButton = new QPushButton(tr("Misc"), this);
		menu = new QMenu(this);
		actionGroup = addCheckableProperties(JMdictPlugin::miscMap(), menu);
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
	if (_kanaOnlyCheckBox->isChecked()) ret += " :withkanaonly";
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
	QString comps = _containedComponents->text();
	if (!kanjis.isEmpty()) {
		bool first = true;
		contains += tr(" with ");
		foreach(QChar c, kanjis) {
			if (!first) contains +=",";
			else first = false;
			contains += c;
		}
	}
	if (!comps.isEmpty()) {
		bool first = true;
		contains += tr(" with component ");
		foreach(QChar c, comps) {
			if (!first) contains +=",";
			else first = false;
			contains += c;
		}
	}
	if (_studiedKanjisCheckBox->isChecked()) {
		if (!kanjis.isEmpty() || !comps.isEmpty()) contains += tr(", studied kanji only");
		else contains += tr(" with studied kanji");
	}
	if (_kanaOnlyCheckBox->isChecked()) {
		if (!kanjis.isEmpty() || !comps.isEmpty() || _studiedKanjisCheckBox->isChecked()) contains += tr(", including kana words");
		else contains += tr(" using kana only");
	}

	QStringList propsList = _posList + _dialList + _fieldList + _miscList;
	QString props = propsList.join(",");

	if (!props.isEmpty()) props = " (" + props + ")";
	QString ret(props + contains);
	if (!ret.isEmpty()) ret = tr("Vocabulary") + ret;
	else ret = tr("Vocabulary");
	return ret;
}

void JMdictFilterWidget::__onPropertyTriggered(QAction *action, QStringList &list, QPushButton *button)
{
	QString tag = action->property("TJpropertyIndex").toString();

	if (action->isChecked())
		list << tag;
	else
		list.removeOne(tag);
	if (!list.isEmpty())
		button->setText(tr("Pos:") + list.join(","));
	else
		button->setText(tr("Part of speech"));
	commandUpdate();
}

void JMdictFilterWidget::onPosTriggered(QAction *action)
{
	__onPropertyTriggered(action, _posList, _posButton);
}

void JMdictFilterWidget::onDialTriggered(QAction *action)
{
	__onPropertyTriggered(action, _dialList, _dialButton);
}

void JMdictFilterWidget::onFieldTriggered(QAction *action)
{
	__onPropertyTriggered(action, _fieldList, _fieldButton);
}

void JMdictFilterWidget::onMiscTriggered(QAction *action)
{
	__onPropertyTriggered(action, _miscList, _miscButton);
}

void JMdictFilterWidget::_reset()
{
	_containedKanjis->clear();
	_containedComponents->clear();
	_studiedKanjisCheckBox->setChecked(false);
	_kanaOnlyCheckBox->setChecked(false);
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
	if (!currentCommand().isEmpty()) emit disableFeature("kanjidic");
	else emit enableFeature("kanjidic");
}

void JMdictFilterWidget::setPos(const QStringList &list)
{
	_posList.clear();
	foreach(QAction *action, _posButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(JMdictPlugin::posMap()[action->property("TJpropertyIndex").toString()].first))
			action->trigger();
	}
}

void JMdictFilterWidget::setDial(const QStringList &list)
{
	_dialList.clear();
	foreach(QAction *action, _dialButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(JMdictPlugin::dialMap()[action->property("TJpropertyIndex").toString()].first))
			action->trigger();
	}
}

void JMdictFilterWidget::setField(const QStringList &list)
{
	_fieldList.clear();
	foreach(QAction *action, _fieldButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(JMdictPlugin::fieldMap()[action->property("TJpropertyIndex").toString()].first))
			action->trigger();
	}
}

void JMdictFilterWidget::setMisc(const QStringList &list)
{
	_miscList.clear();
	foreach(QAction *action, _miscButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(JMdictPlugin::miscMap()[action->property("TJpropertyIndex").toString()].first))
			action->trigger();
	}
}

QActionGroup *JMdictFilterWidget::addCheckableProperties(const QMap<QString, QPair<QString, quint16> >&map, QMenu *menu)
{
	QMap<QString, QString> strMap;
	QStringList strList;
	for (const auto &tag : map.keys()) {
		QString translated = QCoreApplication::translate("JMdictLongDescs", map[tag].first.toLatin1());
		translated = translated.replace(0, 1, translated[0].toUpper());
		strList << translated;
		strMap[translated] = tag;
	}

	std::sort(strList.begin(), strList.end());
	QActionGroup *actionGroup = new QActionGroup(menu);
	actionGroup->setExclusive(false);
	foreach(QString str, strList) {
		QString tag = strMap[str];
		QAction *action = actionGroup->addAction(str);
		action->setCheckable(true);
		menu->addAction(action);
		action->setProperty("TJpropertyIndex", tag);
	}
	return actionGroup;
}
