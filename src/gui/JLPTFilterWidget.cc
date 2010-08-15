/*
 *  Copyright (C) 2009/2010  Alexandre Courbot
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

#include "gui/JLPTFilterWidget.h"
#include "gui/FlowLayout.h"

#include <QGroupBox>
#include <QHBoxLayout>

JLPTFilterWidget::JLPTFilterWidget(QWidget *parent) : SearchFilterWidget(parent)
{
	_propsToSave << "levels";

	JLPT4CheckBox = new QCheckBox(tr("Level N5"), this);
	connect(JLPT4CheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	JLPT3CheckBox = new QCheckBox(tr("Level N4"), this);
	connect(JLPT3CheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	JLPT2CheckBox = new QCheckBox(tr("Level N2"), this);
	connect(JLPT2CheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	JLPT1CheckBox = new QCheckBox(tr("Level N1"), this);
	connect(JLPT1CheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));

	QHBoxLayout *hLayout = new QHBoxLayout(this);
	hLayout->addWidget(JLPT4CheckBox);
	hLayout->addWidget(JLPT3CheckBox);
	hLayout->addWidget(JLPT2CheckBox);
	hLayout->addWidget(JLPT1CheckBox);
}

QString JLPTFilterWidget::currentCommand() const
{
	QString ret;

	if (JLPT4CheckBox->isChecked() || JLPT3CheckBox->isChecked() || JLPT2CheckBox->isChecked() || JLPT1CheckBox->isChecked()) {
		bool first = true;
		if (JLPT4CheckBox->isChecked()) {
			if (first) first = false;
			else ret += ",";
			ret += "4";
		}
		if (JLPT3CheckBox->isChecked()) {
			if (first) first = false;
			else ret += ",";
			ret += "3";
		}
		if (JLPT2CheckBox->isChecked()) {
			if (first) first = false;
			else ret += ",";
			ret += "2";
		}
		if (JLPT1CheckBox->isChecked()) {
			if (first) first = false;
			else ret += ",";
			ret += "1";
		}
		ret = ":jlpt=" + ret;
	}
	return ret;
}

QString JLPTFilterWidget::currentTitle() const
{
	QString ret;
	bool first = true;

	if (JLPT4CheckBox->isChecked()) {
		if (first) first = false;
		else ret += ",";
		ret += "4";
	}
	if (JLPT3CheckBox->isChecked()) {
		if (first) first = false;
		else ret += ",";
		ret += "3";
	}
	if (JLPT2CheckBox->isChecked()) {
		if (first) first = false;
		else ret += ",";
		ret += "2";
	}
	if (JLPT1CheckBox->isChecked()) {
		if (first) first = false;
		else ret += ",";
		ret += "1";
	}
	if (!ret.isEmpty()) return tr("JLPT:") + ret;
	else return tr("JLPT");
}

QList<QVariant> JLPTFilterWidget::levels() const
{
	QList<QVariant> ret;
	if (JLPT4CheckBox->isChecked()) ret << 4;
	if (JLPT3CheckBox->isChecked()) ret << 3;
	if (JLPT2CheckBox->isChecked()) ret << 2;
	if (JLPT1CheckBox->isChecked()) ret << 1;
	return ret;
}

void JLPTFilterWidget::setLevels(const QList<QVariant> &filter)
{
	JLPT4CheckBox->setChecked(filter.contains(4));
	JLPT3CheckBox->setChecked(filter.contains(3));
	JLPT2CheckBox->setChecked(filter.contains(2));
	JLPT1CheckBox->setChecked(filter.contains(1));
}

void JLPTFilterWidget::_reset()
{
	JLPT4CheckBox->setChecked(false);
	JLPT3CheckBox->setChecked(false);
	JLPT2CheckBox->setChecked(false);
	JLPT1CheckBox->setChecked(false);
}
