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
#include "gui/kanjidic2/Kanjidic2GUIPlugin.h"
#include "gui/kanjidic2/KanaSelector.h"

#include "gui/PreferencesWindow.h"

Kanjidic2Preferences::Kanjidic2Preferences(QWidget *parent) : PreferencesWindowCategory(tr("Character entries"), parent)
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
	connect(printStrokesNumbersButton, SIGNAL(toggled(bool)), this, SLOT(updatePrintPreview()));
	connect(printStrokesNumbersSize, SIGNAL(valueChanged(int)), SLOT(updatePrintPreview()));
	connect(printGrid, SIGNAL(toggled(bool)), SLOT(updatePrintPreview()));
	previewLabel->installEventFilter(this);

	connect(printComponents, SIGNAL(toggled(bool)), printOnlyStudiedComponents, SLOT(setEnabled(bool)));

	connect(animSpeedDefault, SIGNAL(toggled(bool)), this, SLOT(onAnimSpeedDefaultChecked(bool)));
	connect(animDelayDefault, SIGNAL(toggled(bool)), this, SLOT(onAnimDelayDefaultChecked(bool)));

	previewLabel->setPicture(previewPic);

	_player = new KanjiPlayer(animationTab);
	_player->setPictureSize(KanjiPopup::animationSize.value());
	previewEntry = KanjiEntryRef(0x9593).get();
	_player->setKanji(previewEntry);
	connect(showGrid, SIGNAL(toggled(bool)), _player, SLOT(setShowGrid(bool)));
	connect(animSpeedSlider, SIGNAL(valueChanged(int)), _player, SLOT(setAnimationSpeed(int)));
	connect(animDelaySlider, SIGNAL(valueChanged(int)), _player, SLOT(setDelayBetweenStrokes(int)));
	connect(animationLoopDelay, SIGNAL(valueChanged(int)), _player, SLOT(setAnimationLoopDelay(int)));
	connect(showStrokesNumbersButton, SIGNAL(toggled(bool)), _player, SLOT(setShowStrokesNumbers(bool)));
	connect(showStrokesNumbersSize, SIGNAL(valueChanged(int)), _player, SLOT(setStrokesNumbersSize(int)));
	animationLayout->addWidget(_player, 0, Qt::AlignVCenter);
	_player->installEventFilter(this);

	connect(animSize, SIGNAL(valueChanged(int)), this, SLOT(onSizeChanged(int)));

	QBoxLayout *bLayout = static_cast<QBoxLayout *>(kanaAppearanceBox->layout());
	QFont f;
	f.fromString(KanaView::characterFont.defaultValue());
	_kanaSelectorFont = new PreferencesFontChooser(tr("Character font"), f, this);
	QHBoxLayout *hLayout = new QHBoxLayout();
	hLayout->addWidget(_kanaSelectorFont);
	bLayout->addLayout(hLayout);

	_kanaSelectorPreview = new KanaView(this, true);
	_kanaSelectorPreview->setShowObsolete(true);
	QLayout *layout = kanaPreviewBox->layout();
	layout->addWidget(_kanaSelectorPreview);

	connect(_kanaSelectorFont, SIGNAL(fontChanged(QFont)), _kanaSelectorPreview, SLOT(setCharacterFont(QFont)));
}

void Kanjidic2Preferences::refresh()
{
	showReadings->setChecked(Kanjidic2EntryFormatter::showReadings.value());
	showNanori->setChecked(Kanjidic2EntryFormatter::showNanori.value());
	showRadicals->setChecked(Kanjidic2EntryFormatter::showRadicals.value());
	showComponents->setChecked(Kanjidic2EntryFormatter::showComponents.value());
	showFrequency->setChecked(Kanjidic2EntryFormatter::showFrequency.value());
	showGrade->setChecked(Kanjidic2EntryFormatter::showGrade.value());
	showJLPT->setChecked(Kanjidic2EntryFormatter::showJLPT.value());
	showStrokesNumber->setChecked(Kanjidic2EntryFormatter::showStrokesNumber.value());
	showUnicode->setChecked(Kanjidic2EntryFormatter::showUnicode.value());
	showSKIP->setChecked(Kanjidic2EntryFormatter::showSKIP.value());
	showFourCorner->setChecked(Kanjidic2EntryFormatter::showFourCorner.value());
	showHeisig->setChecked(Kanjidic2EntryFormatter::showHeisig.value());
	showVariations->setChecked(Kanjidic2EntryFormatter::showVariations.value());
	showVariationOf->setChecked(Kanjidic2EntryFormatter::showVariationOf.value());

	maxCompounds->setValue(Kanjidic2EntryFormatter::maxCompoundsToDisplay.value());
	showOnlyStudiedCompounds->setChecked(Kanjidic2EntryFormatter::showOnlyStudiedCompounds.value());
	maxWords->setValue(Kanjidic2EntryFormatter::maxWordsToDisplay.value());
	showOnlyStudiedVocab->setChecked(Kanjidic2EntryFormatter::showOnlyStudiedVocab.value());

	kanjiTooltipEnabled->setChecked(Kanjidic2GUIPlugin::kanjiTooltipEnabled.value());
	tooltipShowScore->setChecked(Kanjidic2EntryFormatter::tooltipShowScore.value());
	tooltipShowFrequency->setChecked(Kanjidic2EntryFormatter::tooltipShowFrequency.value());
	tooltipShowGrade->setChecked(Kanjidic2EntryFormatter::tooltipShowGrade.value());
	tooltipShowJLPT->setChecked(Kanjidic2EntryFormatter::tooltipShowJLPT.value());
	tooltipShowStrokesNumber->setChecked(Kanjidic2EntryFormatter::tooltipShowStrokesNumber.value());
	tooltipShowUnicode->setChecked(Kanjidic2EntryFormatter::tooltipShowUnicode.value());
	tooltipShowSKIP->setChecked(Kanjidic2EntryFormatter::tooltipShowSKIP.value());
	tooltipShowFourCorner->setChecked(Kanjidic2EntryFormatter::tooltipShowFourCorner.value());
	tooltipShowHeisig->setChecked(Kanjidic2EntryFormatter::tooltipShowHeisig.value());

	kanjiPrintSize->setValue(Kanjidic2EntryFormatter::printSize.value());
	printMeanings->setChecked(Kanjidic2EntryFormatter::printMeanings.value());
	printOnyomi->setChecked(Kanjidic2EntryFormatter::printOnyomi.value());
	printKunyomi->setChecked(Kanjidic2EntryFormatter::printKunyomi.value());
	printComponents->setChecked(Kanjidic2EntryFormatter::printComponents.value());
	printOnlyStudiedComponents->setChecked(Kanjidic2EntryFormatter::printOnlyStudiedComponents.value());
	maxWordsPrint->setValue(Kanjidic2EntryFormatter::maxWordsToPrint.value());
	printStrokesNumbersButton->setChecked(Kanjidic2EntryFormatter::printStrokesNumbers.value());
	printStrokesNumbersSize->setValue(Kanjidic2EntryFormatter::strokesNumbersSize.value());
	printGrid->setChecked(Kanjidic2EntryFormatter::printGrid.value());
	if (Kanjidic2EntryFormatter::printWithFont.value()) fontButton->setChecked(true);
	else handWritingButton->setChecked(true);
	printOnlyStudiedVocab->setChecked(Kanjidic2EntryFormatter::printOnlyStudiedVocab.value());

	animSize->setValue(KanjiPopup::animationSize.value());
	animSpeedSlider->setValue(KanjiPlayer::animationSpeed.value());
	animSpeedDefault->setChecked(KanjiPlayer::animationSpeed.isDefault());
	animDelaySlider->setValue(KanjiPlayer::delayBetweenStrokes.value());
	animDelayDefault->setChecked(KanjiPlayer::delayBetweenStrokes.isDefault());
	showGrid->setChecked(KanjiPlayer::showGridPref.value());
	showStrokesNumbersButton->setChecked(KanjiPlayer::showStrokesNumbersPref.value());
	showStrokesNumbersSize->setValue(KanjiPlayer::strokesNumbersSizePref.value());
	autoStartAnim->setChecked(KanjiPopup::autoStartAnim.value());
	animationLoopDelay->setValue(KanjiPlayer::animationLoopDelay.value());

	updatePrintPreview();

	_kanaSelectorFont->setDefault(KanaView::characterFont.isDefault());
	QFont font;
	font.fromString(KanaView::characterFont.value());
	_kanaSelectorFont->setFont(font);
}

void Kanjidic2Preferences::applySettings()
{
	Kanjidic2EntryFormatter::showReadings.set(showReadings->isChecked());
	Kanjidic2EntryFormatter::showNanori.set(showNanori->isChecked());
	Kanjidic2EntryFormatter::showRadicals.set(showRadicals->isChecked());
	Kanjidic2EntryFormatter::showComponents.set(showComponents->isChecked());
	Kanjidic2EntryFormatter::showFrequency.set(showFrequency->isChecked());
	Kanjidic2EntryFormatter::showGrade.set(showGrade->isChecked());
	Kanjidic2EntryFormatter::showJLPT.set(showJLPT->isChecked());
	Kanjidic2EntryFormatter::showStrokesNumber.set(showStrokesNumber->isChecked());
	Kanjidic2EntryFormatter::showUnicode.set(showUnicode->isChecked());
	Kanjidic2EntryFormatter::showSKIP.set(showSKIP->isChecked());
	Kanjidic2EntryFormatter::showFourCorner.set(showFourCorner->isChecked());
	Kanjidic2EntryFormatter::showHeisig.set(showHeisig->isChecked());
	Kanjidic2EntryFormatter::showVariations.set(showVariations->isChecked());
	Kanjidic2EntryFormatter::showVariationOf.set(showVariationOf->isChecked());

	Kanjidic2EntryFormatter::maxCompoundsToDisplay.set(maxCompounds->value());
	Kanjidic2EntryFormatter::showOnlyStudiedCompounds.set(showOnlyStudiedCompounds->isChecked());
	Kanjidic2EntryFormatter::maxWordsToDisplay.set(maxWords->value());
	Kanjidic2EntryFormatter::showOnlyStudiedVocab.set(showOnlyStudiedVocab->isChecked());

	Kanjidic2GUIPlugin::kanjiTooltipEnabled.set(kanjiTooltipEnabled->isChecked());
	Kanjidic2EntryFormatter::tooltipShowScore.set(tooltipShowScore->isChecked());
	Kanjidic2EntryFormatter::tooltipShowFrequency.set(tooltipShowFrequency->isChecked());
	Kanjidic2EntryFormatter::tooltipShowGrade.set(tooltipShowGrade->isChecked());
	Kanjidic2EntryFormatter::tooltipShowJLPT.set(tooltipShowJLPT->isChecked());
	Kanjidic2EntryFormatter::tooltipShowStrokesNumber.set(tooltipShowStrokesNumber->isChecked());
	Kanjidic2EntryFormatter::tooltipShowUnicode.set(tooltipShowUnicode->isChecked());
	Kanjidic2EntryFormatter::tooltipShowSKIP.set(tooltipShowSKIP->isChecked());
	Kanjidic2EntryFormatter::tooltipShowFourCorner.set(tooltipShowFourCorner->isChecked());
	Kanjidic2EntryFormatter::tooltipShowHeisig.set(tooltipShowHeisig->isChecked());

	Kanjidic2EntryFormatter::printSize.set(kanjiPrintSize->value());
	Kanjidic2EntryFormatter::printMeanings.set(printMeanings->isChecked());
	Kanjidic2EntryFormatter::printOnyomi.set(printOnyomi->isChecked());
	Kanjidic2EntryFormatter::printKunyomi.set(printKunyomi->isChecked());
	Kanjidic2EntryFormatter::printComponents.set(printComponents->isChecked());
	Kanjidic2EntryFormatter::printOnlyStudiedComponents.set(printOnlyStudiedComponents->isChecked());
	Kanjidic2EntryFormatter::maxWordsToPrint.set(maxWordsPrint->value());
	Kanjidic2EntryFormatter::printWithFont.set(fontButton->isChecked());
	Kanjidic2EntryFormatter::printOnlyStudiedVocab.set(printOnlyStudiedVocab->isChecked());
	Kanjidic2EntryFormatter::printStrokesNumbers.set(printStrokesNumbersButton->isChecked());
	Kanjidic2EntryFormatter::strokesNumbersSize.set(printStrokesNumbersSize->value());
	Kanjidic2EntryFormatter::printGrid.set(printGrid->isChecked());

	KanjiPopup::animationSize.set(animSize->value());
	if (animSpeedDefault->isChecked()) KanjiPlayer::animationSpeed.reset();
	else KanjiPlayer::animationSpeed.set(animSpeedSlider->value());
	if (animDelayDefault->isChecked()) KanjiPlayer::delayBetweenStrokes.reset();
	else KanjiPlayer::delayBetweenStrokes.set(animDelaySlider->value());
	KanjiPlayer::showGridPref.set(showGrid->isChecked());
	KanjiPlayer::showStrokesNumbersPref.set(showStrokesNumbersButton->isChecked());
	KanjiPlayer::strokesNumbersSizePref.set(showStrokesNumbersSize->value());
	KanjiPopup::autoStartAnim.set(autoStartAnim->isChecked());
	KanjiPlayer::animationLoopDelay.set(animationLoopDelay->value());

	const QFont &font = _kanaSelectorFont->font();
	if (_kanaSelectorFont->isDefault()) KanaView::characterFont.reset();
	else KanaView::characterFont.set(font.toString());

	Kanjidic2GUIPlugin::instance()->kanaSelector()->kanaView()->setCharacterFont(font);
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
			_player->play();
			return false;
		case QEvent::Hide:
			_player->stop();
			_player->reset();
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
	formatter->drawCustom(previewEntry, painter, QRectF(0, 0, printPreviewScrollArea->viewport()->contentsRect().width() - 20, 300), usedSpace, QFont(), kanjiPrintSize->value(), fontButton->isChecked(), printMeanings->isChecked(), printOnyomi->isChecked(), printKunyomi->isChecked(), printComponents->isChecked(), printOnlyStudiedComponents->isChecked(), maxWordsPrint->value(), printOnlyStudiedVocab->isChecked(), printStrokesNumbersButton->isChecked(), printStrokesNumbersSize->value(), printGrid->isChecked());
	previewPic.setBoundingRect(usedSpace.toRect());
	previewLabel->clear();
	previewLabel->setPicture(previewPic);
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
