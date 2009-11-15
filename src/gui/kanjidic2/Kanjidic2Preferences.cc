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

#include "gui/kanjidic2/Kanjidic2EntryFormatter.h"
#include "gui/kanjidic2/KanjiPlayer.h"
#include "gui/kanjidic2/KanjiPopup.h"
#include "gui/kanjidic2/Kanjidic2Preferences.h"

Kanjidic2Preferences::Kanjidic2Preferences(QWidget *parent) : PreferencesWindowCategory(tr("Kanji entries"), parent)
{
	setupUi(this);

	connect(kanjiPrintSize, SIGNAL(valueChanged(int)), this, SLOT(updatePrintPreview()));
	connect(printMeanings, SIGNAL(toggled(bool)), this, SLOT(updatePrintPreview()));
	connect(printOnyomi, SIGNAL(toggled(bool)), this, SLOT(updatePrintPreview()));
	connect(printKunyomi, SIGNAL(toggled(bool)), this, SLOT(updatePrintPreview()));
	connect(printComponents, SIGNAL(toggled(bool)), this, SLOT(updatePrintPreview()));
	connect(printOnlyStudiedComponents, SIGNAL(toggled(bool)), this, SLOT(updatePrintPreview()));
	connect(maxWordsPrint, SIGNAL(valueChanged(int)), this, SLOT(updatePrintPreview()));
	connect(fontButton, SIGNAL(toggled(bool)), this, SLOT(updatePrintPreview()));
	connect(printOnlyStudiedVocab, SIGNAL(toggled(bool)), this, SLOT(updatePrintPreview()));
	previewLabel->installEventFilter(this);

	connect(printComponents, SIGNAL(toggled(bool)), printOnlyStudiedComponents, SLOT(setEnabled(bool)));

	connect(animSpeedDefault, SIGNAL(toggled(bool)), this, SLOT(onAnimSpeedDefaultChecked(bool)));
	connect(animDelayDefault, SIGNAL(toggled(bool)), this, SLOT(onAnimDelayDefaultChecked(bool)));

	previewLabel->setPicture(previewPic);

	_player = new KanjiPlayer(animationTab);
	_player->setPictureSize(KanjiPopup::animationSize.value());
	_previewEntry = EntriesCache::get(KANJIDIC2ENTRY_GLOBALID, 0x9593);
	previewEntry = static_cast<const Kanjidic2Entry *>(_previewEntry.data());
	_player->setKanji(previewEntry);
	connect(animSpeedSlider, SIGNAL(valueChanged(int)), _player, SLOT(setAnimationSpeed(int)));
	connect(animDelaySlider, SIGNAL(valueChanged(int)), _player, SLOT(setDelayBetweenStrokes(int)));
	animationLayout->addWidget(_player, 0, Qt::AlignVCenter);
	_player->installEventFilter(this);

	_endOfPlayTimer.setSingleShot(true);
	connect(&_endOfPlayTimer, SIGNAL(timeout()), this, SLOT(onStartAnimation()));
	connect(animSize, SIGNAL(valueChanged(int)), this, SLOT(onSizeChanged(int)));
}

void Kanjidic2Preferences::refresh()
{
	showReadings->setChecked(Kanjidic2EntryFormatter::showReadings.value());
	showNanori->setChecked(Kanjidic2EntryFormatter::showNanori.value());
	showComponents->setChecked(Kanjidic2EntryFormatter::showComponents.value());
	showFrequency->setChecked(Kanjidic2EntryFormatter::showFrequency.value());
	showGrade->setChecked(Kanjidic2EntryFormatter::showGrade.value());
	showJLPT->setChecked(Kanjidic2EntryFormatter::showJLPT.value());
	showStrokesNumber->setChecked(Kanjidic2EntryFormatter::showStrokesNumber.value());
	showUnicode->setChecked(Kanjidic2EntryFormatter::showUnicode.value());
	showSKIP->setChecked(Kanjidic2EntryFormatter::showSKIP.value());
	showVariations->setChecked(Kanjidic2EntryFormatter::showVariations.value());
	showVariationOf->setChecked(Kanjidic2EntryFormatter::showVariationOf.value());

	maxCompounds->setValue(Kanjidic2EntryFormatter::maxParentKanjiToDisplay.value());
	maxWords->setValue(Kanjidic2EntryFormatter::maxWordsToDisplay.value());

	kanjiPrintSize->setValue(Kanjidic2EntryFormatter::printSize.value());
	printMeanings->setChecked(Kanjidic2EntryFormatter::printMeanings.value());
	printOnyomi->setChecked(Kanjidic2EntryFormatter::printOnyomi.value());
	printKunyomi->setChecked(Kanjidic2EntryFormatter::printKunyomi.value());
	printComponents->setChecked(Kanjidic2EntryFormatter::printComponents.value());
	printOnlyStudiedComponents->setChecked(Kanjidic2EntryFormatter::printOnlyStudiedComponents.value());
	maxWordsPrint->setValue(Kanjidic2EntryFormatter::maxWordsToPrint.value());
	if (Kanjidic2EntryFormatter::printWithFont.value()) fontButton->setChecked(true);
	else handWritingButton->setChecked(true);
	printOnlyStudiedVocab->setChecked(Kanjidic2EntryFormatter::printOnlyStudiedVocab.value());

	animSize->setValue(KanjiPopup::animationSize.value());
	animSpeedSlider->setValue(KanjiPlayer::animationSpeed.value());
	animSpeedDefault->setChecked(KanjiPlayer::animationSpeed.isDefault());
	animDelaySlider->setValue(KanjiPlayer::delayBetweenStrokes.value());
	animDelayDefault->setChecked(KanjiPlayer::delayBetweenStrokes.isDefault());
	autoStartAnim->setChecked(KanjiPopup::autoStartAnim.value());

	updatePrintPreview();
}

void Kanjidic2Preferences::applySettings()
{
	Kanjidic2EntryFormatter::showReadings.set(showReadings->isChecked());
	Kanjidic2EntryFormatter::showNanori.set(showNanori->isChecked());
	Kanjidic2EntryFormatter::showComponents.set(showComponents->isChecked());
	Kanjidic2EntryFormatter::showFrequency.set(showFrequency->isChecked());
	Kanjidic2EntryFormatter::showGrade.set(showGrade->isChecked());
	Kanjidic2EntryFormatter::showJLPT.set(showJLPT->isChecked());
	Kanjidic2EntryFormatter::showStrokesNumber.set(showStrokesNumber->isChecked());
	Kanjidic2EntryFormatter::showUnicode.set(showUnicode->isChecked());
	Kanjidic2EntryFormatter::showSKIP.set(showSKIP->isChecked());
	Kanjidic2EntryFormatter::showVariations.set(showVariations->isChecked());
	Kanjidic2EntryFormatter::showVariationOf.set(showVariationOf->isChecked());

	Kanjidic2EntryFormatter::maxParentKanjiToDisplay.set(maxCompounds->value());
	Kanjidic2EntryFormatter::maxWordsToDisplay.set(maxWords->value());

	Kanjidic2EntryFormatter::printSize.set(kanjiPrintSize->value());
	Kanjidic2EntryFormatter::printMeanings.set(printMeanings->isChecked());
	Kanjidic2EntryFormatter::printOnyomi.set(printOnyomi->isChecked());
	Kanjidic2EntryFormatter::printKunyomi.set(printKunyomi->isChecked());
	Kanjidic2EntryFormatter::printComponents.set(printComponents->isChecked());
	Kanjidic2EntryFormatter::printOnlyStudiedComponents.set(printOnlyStudiedComponents->isChecked());
	Kanjidic2EntryFormatter::maxWordsToPrint.set(maxWordsPrint->value());
	Kanjidic2EntryFormatter::printWithFont.set(fontButton->isChecked());
	Kanjidic2EntryFormatter::printOnlyStudiedVocab.set(printOnlyStudiedVocab->isChecked());

	KanjiPopup::animationSize.set(animSize->value());
	if (animSpeedDefault->isChecked()) KanjiPlayer::animationSpeed.reset();
	else KanjiPlayer::animationSpeed.set(animSpeedSlider->value());
	if (animDelayDefault->isChecked()) KanjiPlayer::delayBetweenStrokes.reset();
	else KanjiPlayer::delayBetweenStrokes.set(animDelaySlider->value());
	KanjiPopup::autoStartAnim.set(autoStartAnim->isChecked());
}

bool Kanjidic2Preferences::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == previewLabel) {
		switch (event->type()) {
		case QEvent::Show:
		case QEvent::Resize:
			updatePrintPreview();
			return false;
		default:
			return false;

		}
	}
	else if (obj == _player) {
		switch (event->type()) {
		case QEvent::Show:
			connect(_player, SIGNAL(animationStopped()), this, SLOT(onAnimationCompleted()));
			_player->startAnimation();
			return false;
		case QEvent::Hide:
			disconnect(_player, SIGNAL(animationStopped()), this, SLOT(onAnimationCompleted()));
			_endOfPlayTimer.stop();
			_player->stopAnimation();
			_player->resetAnimation();
			return false;
		default:
			return false;
		}
	}
	return false;
}

void Kanjidic2Preferences::updatePrintPreview()
{
	if (!isVisible()) return;

	const Kanjidic2EntryFormatter *formatter = static_cast<const Kanjidic2EntryFormatter *>(EntryFormatter::getFormatter(previewEntry));
	QPainter painter(&previewPic);
	QRectF usedSpace;
	formatter->drawCustom(previewEntry, painter, QRectF(0, 0, printPreviewScrollArea->viewport()->contentsRect().width() - 20, 300), usedSpace, QFont(), kanjiPrintSize->value(), fontButton->isChecked(), printMeanings->isChecked(), printOnyomi->isChecked(), printKunyomi->isChecked(), printComponents->isChecked(), printOnlyStudiedComponents->isChecked(), maxWordsPrint->value(), printOnlyStudiedVocab->isChecked());
	previewPic.setBoundingRect(usedSpace.toRect());
	previewLabel->clear();
	previewLabel->setPicture(previewPic);
}

void Kanjidic2Preferences::onAnimationCompleted()
{
	_endOfPlayTimer.start(1000);
}

void Kanjidic2Preferences::onStartAnimation()
{
	_player->resetAnimation();
	_player->startAnimation();
}

void Kanjidic2Preferences::onAnimSpeedDefaultChecked(bool checked)
{
	if (!checked) {
		animSpeedSlider->setEnabled(true);
	} else {
		animSpeedSlider->setValue(KanjiPlayer::animationSpeed.defaultValue());
		animSpeedSlider->setEnabled(false);
	}
}

void Kanjidic2Preferences::onAnimDelayDefaultChecked(bool checked)
{
	if (!checked) {
		animDelaySlider->setEnabled(true);
	} else {
		animDelaySlider->setValue(KanjiPlayer::delayBetweenStrokes.defaultValue());
		animDelaySlider->setEnabled(false);
	}
}

void Kanjidic2Preferences::onSizeChanged(int newSize)
{
	_player->setPictureSize(newSize);
}
