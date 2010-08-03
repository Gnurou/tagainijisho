/*
 *  Copyright (C) 2009  Alexandre Courbot
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

#include "gui/jmdict/JMdictYesNoTrainer.h"
#include "gui/jmdict/JMdictGUIPlugin.h"

JMdictYesNoTrainer::JMdictYesNoTrainer(QWidget *parent) : YesNoTrainer(parent)
{
	_showFuriganas = new QCheckBox(tr("Show &furigana"), this);
	connect(_showFuriganas, SIGNAL(toggled(bool)), this, SLOT(onShowFuriganasChecked(bool)));
	_showFuriganas->setChecked(JMdictGUIPlugin::furiganasForTraining.value());
	_showFuriganas->setShortcut(QKeySequence(_showFuriganas->shortcut()[0] & 0x00ffffff));
	_showFuriganasAction = _detailedView->toolBar()->addWidget(_showFuriganas);
}

void JMdictYesNoTrainer::onShowFuriganasChecked(bool checked)
{
	JMdictGUIPlugin::furiganasForTraining.set(checked);
	if (checked) frontParts << "furigana";
	else frontParts.removeAll("furigana");

	if (showAnswerButton->isEnabled()) {
		// Necessary to instanciate a new EntryPointer because train received a 
		// const reference to an EntryPointer and starts by reseting currentEntry -
		// therefore if we pass it here we will lose the value
		train(EntryPointer(currentEntry));
	}
}

void JMdictYesNoTrainer::setTrainingMode(TrainingMode mode)
{
	YesNoTrainer::setTrainingMode(mode);
	_showFuriganasAction->setVisible(mode == YesNoTrainer::Japanese);
}
