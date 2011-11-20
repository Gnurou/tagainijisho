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

	JLPTN5CheckBox = new QCheckBox(tr("N5"), this);
	connect(JLPTN5CheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	JLPTN4CheckBox = new QCheckBox(tr("N4"), this);
	connect(JLPTN4CheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	JLPTN3CheckBox = new QCheckBox(tr("N3"), this);
	connect(JLPTN3CheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	JLPTN2CheckBox = new QCheckBox(tr("N2"), this);
	connect(JLPTN2CheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	JLPTN1CheckBox = new QCheckBox(tr("N1"), this);
	connect(JLPTN1CheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));

	QHBoxLayout *hLayout = new QHBoxLayout(this);
	hLayout->addWidget(JLPTN5CheckBox);
	hLayout->addWidget(JLPTN4CheckBox);
	hLayout->addWidget(JLPTN3CheckBox);
	hLayout->addWidget(JLPTN2CheckBox);
	hLayout->addWidget(JLPTN1CheckBox);
}

QString JLPTFilterWidget::currentCommand() const
{
	QString ret;

	if (JLPTN5CheckBox->isChecked() || JLPTN4CheckBox->isChecked() || JLPTN3CheckBox->isChecked() ||
		JLPTN2CheckBox->isChecked() || JLPTN1CheckBox->isChecked()) {
		bool first = true;
		if (JLPTN5CheckBox->isChecked()) {
			if (first) first = false;
			else ret += ",";
			ret += "5";
		}
		if (JLPTN4CheckBox->isChecked()) {
			if (first) first = false;
			else ret += ",";
			ret += "4";
		}
		if (JLPTN3CheckBox->isChecked()) {
			if (first) first = false;
			else ret += ",";
			ret += "3";
		}
		if (JLPTN2CheckBox->isChecked()) {
			if (first) first = false;
			else ret += ",";
			ret += "2";
		}
		if (JLPTN1CheckBox->isChecked()) {
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

	if (JLPTN5CheckBox->isChecked()) {
		if (first) first = false;
		else ret += ",";
		ret += "5";
	}
	if (JLPTN4CheckBox->isChecked()) {
		if (first) first = false;
		else ret += ",";
		ret += "4";
	}
	if (JLPTN3CheckBox->isChecked()) {
		if (first) first = false;
		else ret += ",";
		ret += "3";
	}
	if (JLPTN2CheckBox->isChecked()) {
		if (first) first = false;
		else ret += ",";
		ret += "2";
	}
	if (JLPTN1CheckBox->isChecked()) {
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
	if (JLPTN5CheckBox->isChecked()) ret << 5;
	if (JLPTN4CheckBox->isChecked()) ret << 4;
	if (JLPTN3CheckBox->isChecked()) ret << 3;
	if (JLPTN2CheckBox->isChecked()) ret << 2;
	if (JLPTN1CheckBox->isChecked()) ret << 1;
	return ret;
}

void JLPTFilterWidget::setLevels(const QList<QVariant> &filter)
{
	   JLPTN5CheckBox->setChecked(filter.contains(5));
	   JLPTN4CheckBox->setChecked(filter.contains(4));
	   JLPTN3CheckBox->setChecked(filter.contains(3));
	   JLPTN2CheckBox->setChecked(filter.contains(2));
	   JLPTN1CheckBox->setChecked(filter.contains(1));
}

void JLPTFilterWidget::_reset()
{
	   JLPTN5CheckBox->setChecked(false);
	   JLPTN4CheckBox->setChecked(false);
	   JLPTN3CheckBox->setChecked(false);
	   JLPTN2CheckBox->setChecked(false);
	   JLPTN1CheckBox->setChecked(false);
}
