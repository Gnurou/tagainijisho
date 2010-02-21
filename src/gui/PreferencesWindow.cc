/*
 *  Copyright (C) 2009,2010  Alexandre Courbot
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

#include "core/EntrySearcherManager.h"
#include "core/RelativeDate.h"
#include "gui/UpdateChecker.h"
#include "gui/ResultsView.h"
#include "gui/DetailedView.h"
#include "gui/PreferencesWindow.h"
#include "gui/EntryFormatter.h"
#include "gui/MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QFontDialog>
#include <QRadioButton>

QList<const QMetaObject *> PreferencesWindow::_pluginPanels;

PreferencesWindowCategory::PreferencesWindowCategory(const QString &name, QWidget *parent) : QWidget(parent), _name(name)
{
}

PreferencesWindow::PreferencesWindow(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);
	PreferencesWindowCategory *category;
	category = new GeneralPreferences(this);
	addCategory(category);
	category = new ResultsViewPreferences(this);
	addCategory(category);
	category = new DetailedViewPreferences(this);
	addCategory(category);
	category = new ListsPreferences(this);
	addCategory(category);

	// Now add the panels provided by plugins
	foreach (const QMetaObject *objClass, _pluginPanels) {
		category = qobject_cast<PreferencesWindowCategory *>(objClass->newInstance(Q_ARG(QWidget *, this)));
		if (!category) continue;
		addCategory(category);
	}

	categories->setCurrentRow(0);

	connect(this, SIGNAL(accepted()), this, SLOT(applySettings()));
}

void PreferencesWindow::addCategory(PreferencesWindowCategory *category)
{
	Q_ASSERT(category != 0);

	categories->addItem(category->name());
	stackedWidget->addWidget(category);
	category->refresh();
}

void PreferencesWindow::applySettings() {
	QList<PreferencesWindowCategory *> cats;
	for (int i = 0; i < stackedWidget->count(); i++)
		cats << qobject_cast<PreferencesWindowCategory *> (stackedWidget->widget(i));
	foreach (PreferencesWindowCategory *category, cats) category->applySettings();
	foreach (PreferencesWindowCategory *category, cats) category->updateUI();
}

const QStringList GeneralPreferences::langCodes = (QStringList() <<
	"en" <<
	"fr" <<
	"nl");

const QStringList GeneralPreferences::langNames = (QStringList() <<
	QT_TR_NOOP("English") <<
	QT_TR_NOOP("French") <<
	QT_TR_NOOP("Dutch"));

GeneralPreferences::GeneralPreferences(QWidget *parent) : PreferencesWindowCategory(tr("General"), parent)
{
	setupUi(this);

	fontChooser = new PreferencesFontChooser(tr("Application-wide default font"), QFont(), this);
	static_cast<QBoxLayout*>(generalGroupBox->layout())->insertWidget(0, fontChooser);

	for (int i = 0; i < langNames.size(); i++) {
		guiLanguage->addItem(langNames[i]);
		guiLanguage->setItemData(guiLanguage->count() - 1, langCodes[i]);
	}

	firstDayOfWeek->addItem(tr("Monday"), Qt::Monday);
	firstDayOfWeek->addItem(tr("Sunday"), Qt::Sunday);

	connect(checkForUpdates, SIGNAL(stateChanged(int)), this, SLOT(onCheckUpdatesChecked(int)));
	connect(checkInterval, SIGNAL(valueChanged(int)), this, SLOT(updateNextCheckLabel()));

	connect(cacheSizeDefault, SIGNAL(toggled(bool)), this, SLOT(onCacheSizeDefaultChecked(bool)));
}

void GeneralPreferences::refresh()
{
	fontChooser->setDefault(MainWindow::applicationFont.isDefault());
	fontChooser->setFont(QFont());

	if (MainWindow::guiLanguage.value() == "") guiLanguage->setCurrentIndex(0);
	else {
		int idx(langCodes.indexOf(MainWindow::guiLanguage.value()));
		guiLanguage->setCurrentIndex(idx + 1);
	}

	firstDayOfWeek->setCurrentIndex(firstDayOfWeek->findData(RelativeDate::firstDayOfWeek.value()));

	checkForUpdates->setChecked(MainWindow::autoCheckUpdates.value());
	checkForBetaUpdates->setChecked(MainWindow::autoCheckBetaUpdates.value());
	checkInterval->setValue(MainWindow::updateCheckInterval.value());

	cacheSize->setValue(EntriesCache::cacheSize.value());
	cacheSizeDefault->setChecked(EntriesCache::cacheSize.isDefault());
}

void GeneralPreferences::onCheckUpdatesChecked(int checked)
{ 
	if (checked) {
		checkInterval->setEnabled(true);
	}
	else {
		checkInterval->setEnabled(false);
	}
	updateNextCheckLabel();	
}

void GeneralPreferences::updateNextCheckLabel()
{
	QDateTime dt(MainWindow::lastUpdateCheck.value());
	int updateCheckInterval = checkInterval->value();

	if (checkForUpdates->isChecked()) {
		nextCheck->setText(tr("Next check: %1").arg(dt.addDays(updateCheckInterval).toString()));
	} else {
		nextCheck->setText("");
	}
}

void GeneralPreferences::onCacheSizeDefaultChecked(bool checked)
{
	if (checked) {
		cacheSize->setValue(EntriesCache::cacheSize.defaultValue());
		cacheSize->setEnabled(false);
	}
	else {
		cacheSize->setEnabled(true);
	}
}

void GeneralPreferences::applySettings()
{
	// Default font
	const QFont &font = fontChooser->font();
	if (fontChooser->isDefault()) MainWindow::applicationFont.reset();
	else MainWindow::applicationFont.set(font.toString());

	// Application language
	if (guiLanguage->currentIndex() == 0) MainWindow::guiLanguage.reset();
	else MainWindow::guiLanguage.set(guiLanguage->itemData(guiLanguage->currentIndex()).toString());

	// First day of week
	RelativeDate::firstDayOfWeek.set(static_cast<Qt::DayOfWeek>(firstDayOfWeek->itemData(firstDayOfWeek->currentIndex()).toInt()));

	// Updates check
	MainWindow::autoCheckUpdates.set(checkForUpdates->isChecked());
	MainWindow::autoCheckBetaUpdates.set(checkForBetaUpdates->isChecked());
	MainWindow::updateCheckInterval.set(checkInterval->value());

	// Cache size
	if (cacheSizeDefault->isChecked()) EntriesCache::cacheSize.reset();
	else EntriesCache::cacheSize.set(cacheSize->value());
}

QStringList ResultsViewPrefsDummyEntry::writings() const
{
	QStringList res;
	res << QString::fromUtf8("漢字");
	res << QString::fromUtf8("漢字");
	return res;
}

QStringList ResultsViewPrefsDummyEntry::readings() const
{
	QStringList res;
	res << QString::fromUtf8("ひらがなカタカナ");
	return res;
}

QStringList ResultsViewPrefsDummyEntry::meanings() const
{
	QStringList res;
	res << tr("Romaji text");
	return res;
}

EntryDelegatePreferences::EntryDelegatePreferences(QWidget *parent) : QWidget(parent)
{
	setupUi(this);
	_delegateLayout = new EntryDelegateLayout(EntryDelegateLayout::OneLine, "", "", "", this);
	
	QLayout *layout = layoutBox->layout();
	QFont kFont;
	kFont.fromString(ResultsView::kanjiFont.defaultValue());
	kanjifontChooser = new PreferencesFontChooser(tr("Main writing"), kFont, layoutBox);
	connect(kanjifontChooser, SIGNAL(fontChanged(const QFont &)), this, SLOT(setKanjiFont(const QFont &)));
	layout->addWidget(kanjifontChooser);

	kFont = QFont();
	kFont.fromString(ResultsView::kanaFont.defaultValue());
	kanafontChooser = new PreferencesFontChooser(tr("Readings and alternate writings"), kFont, layoutBox);
	connect(kanafontChooser, SIGNAL(fontChanged(const QFont &)), this, SLOT(setKanaFont(const QFont &)));
	layout->addWidget(kanafontChooser);

	kFont = QFont();
	kFont.fromString(ResultsView::textFont.defaultValue());
	romajifontChooser = new PreferencesFontChooser(tr("Definitions"), kFont, layoutBox);
	connect(romajifontChooser, SIGNAL(fontChanged(const QFont &)), this, SLOT(setTextFont(const QFont &)));
	layout->addWidget(romajifontChooser);
}

void EntryDelegatePreferences::setPrefsToWatch(PreferenceItem<int> *twoLinesPref, PreferenceItem<QString> *defaultFontPref, PreferenceItem<QString> *kanjiFontPref, PreferenceItem<QString> *kanaFontPref)
{
	_twoLinesPref = twoLinesPref;
	_defaultFontPref = defaultFontPref;
	_kanjiFontPref = kanjiFontPref;
	_kanaFontPref = kanaFontPref;
}

void EntryDelegatePreferences::setKanjiFont(const QFont &font)
{
	_delegateLayout->setFont(EntryDelegateLayout::Kanji, font);
}

void EntryDelegatePreferences::setKanaFont(const QFont &font)
{
	_delegateLayout->setFont(EntryDelegateLayout::Kana, font);
}

void EntryDelegatePreferences::setTextFont(const QFont &font)
{
	_delegateLayout->setFont(EntryDelegateLayout::DefaultText, font);
}

void EntryDelegatePreferences::setOneLineDisplay()
{
	_delegateLayout->setDisplayMode(EntryDelegateLayout::OneLine);
}

void EntryDelegatePreferences::setTwoLinesDisplay()
{
	_delegateLayout->setDisplayMode(EntryDelegateLayout::TwoLines);
}

void EntryDelegatePreferences::applyFontSetting(PreferencesFontChooser *fontChooser, PreferenceItem<QString> *prefItem, const EntryDelegateLayout::FontRole fontRole)
{
	const QFont &font = fontChooser->font();
	if (fontChooser->isDefault()) prefItem->reset();
	else prefItem->set(font.toString());
}

void EntryDelegatePreferences::refresh()
{
	if (_twoLinesPref->value() == EntryDelegateLayout::OneLine) oneLine->click();
	else twoLines->click();
	
	QFont font;
	kanjifontChooser->setDefault(_kanjiFontPref->isDefault());
	font.fromString(_kanjiFontPref->value());
	kanjifontChooser->setFont(font);
	
	font = QFont();
	kanafontChooser->setDefault(_kanaFontPref->isDefault());
	font.fromString(_kanaFontPref->value());
	kanafontChooser->setFont(font);
	
	font = QFont();
	romajifontChooser->setDefault(_defaultFontPref->isDefault());
	font.fromString(_defaultFontPref->value());
	romajifontChooser->setFont(font);	
}

void EntryDelegatePreferences::applySettings()
{
	if (oneLine->isChecked()) _twoLinesPref->set(EntryDelegateLayout::OneLine);
	else _twoLinesPref->set(EntryDelegateLayout::TwoLines);
	applyFontSetting(romajifontChooser, _defaultFontPref, EntryDelegateLayout::DefaultText);
	applyFontSetting(kanafontChooser, _kanaFontPref, EntryDelegateLayout::Kana);
	applyFontSetting(kanjifontChooser, _kanjiFontPref, EntryDelegateLayout::Kanji);
}

ResultsViewPreferences::ResultsViewPreferences(QWidget *parent) : PreferencesWindowCategory(tr("Results View"), parent)
{
	setupUi(this);

	// Configure the delegate layout
	entryDelegatePrefs->setPrefsToWatch(&ResultsView::displayMode, &ResultsView::textFont, &ResultsView::kanjiFont, &ResultsView::kanaFont);
	// Prepare the preview list
	_list = new ResultsList(this);
	_view = new ResultsView(this, entryDelegatePrefs->delegateLayout(), true);
	_view->setModel(_list);
	EntryPointer ePtr(new ResultsViewPrefsDummyEntry());
	for (int i = 0; i < 20; i++) _list->addResult(ePtr);
	QVBoxLayout *vLayout = new QVBoxLayout(previewBox);
	vLayout->addWidget(_view);
	
	connect(smoothScrolling, SIGNAL(toggled(bool)), this, SLOT(onSmoothScrollingToggled(bool)));
}

void ResultsViewPreferences::refresh()
{
	nbResults->setValue(ResultsList::resultsPerPagePref.value());
	resultsOrder->setCurrentIndex(EntrySearcherManager::studiedEntriesFirst.value());
	smoothScrolling->setChecked(ResultsView::smoothScrolling.value());

	entryDelegatePrefs->refresh();
}

void ResultsViewPreferences::applySettings()
{
	ResultsList::resultsPerPagePref.set(nbResults->value());
	ResultsView::smoothScrolling.set(smoothScrolling->isChecked());
	EntrySearcherManager::studiedEntriesFirst.set(resultsOrder->currentIndex());
	
	entryDelegatePrefs->applySettings();
}

// TODO replace with preferences signals?
void ResultsViewPreferences::updateUI()
{
	MainWindow::instance()->resultsView()->setSmoothScrolling(ResultsView::smoothScrolling.value());

	EntryDelegateLayout *mwDelegateLayout = MainWindow::instance()->resultsView()->delegateLayout();
	mwDelegateLayout->setDisplayMode(static_cast<EntryDelegateLayout::DisplayMode>(ResultsView::displayMode.value()));
	QFont font;
	font.fromString(ResultsView::textFont.value());
	mwDelegateLayout->setFont(EntryDelegateLayout::DefaultText, font);
	
	font = QFont();
	font.fromString(ResultsView::kanjiFont.value());
	mwDelegateLayout->setFont(EntryDelegateLayout::Kanji, font);
	
	font = QFont();
	font.fromString(ResultsView::kanaFont.value());
	mwDelegateLayout->setFont(EntryDelegateLayout::Kana, font);
}

DetailedViewPreferences::DetailedViewPreferences(QWidget *parent) : PreferencesWindowCategory(tr("Detailed View"), parent)
{
	setupUi(this);

	detailedExample = new PreferencesDetailedViewExample(previewBox);
	{
		QVBoxLayout *layout = new QVBoxLayout(previewBox);
		layout->addWidget(detailedExample);
	}

	{
		QGridLayout *gridLayout = new QGridLayout(fontsBox);

		QFont kFont;
		kFont.fromString(DetailedViewFonts::textFont.defaultValue());
		romajifontChooser = new PreferencesFontChooser(tr("Default text"), kFont, fontsBox);
		connect(romajifontChooser, SIGNAL(fontChanged(const QFont &)), detailedExample, SLOT(redrawRomajis(const QFont &)));
		gridLayout->addWidget(romajifontChooser, 0, 0, 1, 2);

		kFont = QFont();
		kFont.fromString(DetailedViewFonts::kanaHeaderFont.defaultValue());
		kanaHeaderfontChooser = new PreferencesFontChooser(tr("Kana header"), kFont, fontsBox);
		connect(kanaHeaderfontChooser, SIGNAL(fontChanged(const QFont &)), detailedExample, SLOT(redrawKanasHeader(const QFont &)));
		gridLayout->addWidget(kanaHeaderfontChooser, 1, 0);

		kFont = QFont();
		kFont.fromString(DetailedViewFonts::kanjiHeaderFont.defaultValue());
		kanjiHeaderfontChooser = new PreferencesFontChooser(tr("Kanji header"), kFont, fontsBox);
		connect(kanjiHeaderfontChooser, SIGNAL(fontChanged(const QFont &)), detailedExample, SLOT(redrawKanjisHeader(const QFont &)));
		gridLayout->addWidget(kanjiHeaderfontChooser, 2, 0);

		kFont = QFont();
		kFont.fromString(DetailedViewFonts::kanjiFont.defaultValue());
		kanjifontChooser = new PreferencesFontChooser(tr("Kanji"), kFont, fontsBox);
		connect(kanjifontChooser, SIGNAL(fontChanged(const QFont &)), detailedExample, SLOT(redrawKanjis(const QFont &)));
		connect(romajifontChooser, SIGNAL(fontChanged(const QFont &)), kanjifontChooser, SLOT(checkDefaultState(const QFont &)));
		gridLayout->addWidget(kanjifontChooser, 1, 1);

		kFont = QFont();
		kFont.fromString(DetailedViewFonts::kanaFont.defaultValue());
		kanafontChooser = new PreferencesFontChooser(tr("Kana"), kFont, fontsBox);
		connect(kanafontChooser, SIGNAL(fontChanged(const QFont &)), detailedExample, SLOT(redrawKanas(const QFont &)));
		connect(romajifontChooser, SIGNAL(fontChanged(const QFont &)), kanafontChooser, SLOT(checkDefaultState(const QFont &)));
		gridLayout->addWidget(kanafontChooser, 2, 1);
	}
}

void DetailedViewPreferences::refresh()
{
	smoothScrolling->setChecked(DetailedView::smoothScrolling.value());
	shortDescShowJLPT->setChecked(EntryFormatter::shortDescShowJLPT.value());
	romajifontChooser->setDefault(DetailedViewFonts::textFont.isDefault());
	romajifontChooser->setFont(DetailedViewFonts::font(DetailedViewFonts::DefaultText));
	kanaHeaderfontChooser->setDefault(DetailedViewFonts::kanaHeaderFont.isDefault());
	kanaHeaderfontChooser->setFont(DetailedViewFonts::font(DetailedViewFonts::KanaHeader));
	kanjiHeaderfontChooser->setDefault(DetailedViewFonts::kanjiHeaderFont.isDefault());
	kanjiHeaderfontChooser->setFont(DetailedViewFonts::font(DetailedViewFonts::KanjiHeader));
	kanjifontChooser->setDefault(DetailedViewFonts::kanjiFont.isDefault());
	kanjifontChooser->setFont(DetailedViewFonts::font(DetailedViewFonts::Kanji));
	kanafontChooser->setDefault(DetailedViewFonts::kanaFont.isDefault());
	kanafontChooser->setFont(DetailedViewFonts::font(DetailedViewFonts::Kana));
}

void DetailedViewPreferences::applyFontSetting(PreferencesFontChooser *fontChooser, PreferenceItem<QString> *prefItem,
		const DetailedViewFonts::FontRole fontRole)
{
	const QFont &font = fontChooser->font();
	if (fontChooser->isDefault()) prefItem->reset();
	else prefItem->set(font.toString());
	DetailedViewFonts::setFont(fontRole, font);
}

void DetailedViewPreferences::applySettings()
{
	DetailedView::smoothScrolling.set(smoothScrolling->isChecked());
	EntryFormatter::shortDescShowJLPT.set(shortDescShowJLPT->isChecked());
	// Detailed view fonts
	applyFontSetting(romajifontChooser, &DetailedViewFonts::textFont, DetailedViewFonts::DefaultText);
	applyFontSetting(kanaHeaderfontChooser, &DetailedViewFonts::kanaHeaderFont, DetailedViewFonts::KanaHeader);
	applyFontSetting(kanjiHeaderfontChooser, &DetailedViewFonts::kanjiHeaderFont, DetailedViewFonts::KanjiHeader);
	applyFontSetting(kanafontChooser, &DetailedViewFonts::kanaFont, DetailedViewFonts::Kana);
	applyFontSetting(kanjifontChooser, &DetailedViewFonts::kanjiFont, DetailedViewFonts::Kanji);
}

void DetailedViewPreferences::updateUI()
{
	foreach (DetailedView *view, DetailedView::instances()) view->setSmoothScrolling(smoothScrolling->isChecked());
	DetailedViewFonts::fontsChanged();
}

PreferencesFontChooser::PreferencesFontChooser(const QString &whatFor, const QFont &defaultFont, QWidget *parent) : QWidget(parent), _defaultFont(defaultFont)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(new QLabel(QString(tr("%1:").arg(whatFor))));
	layout->addStretch();
	_default = new QCheckBox(tr("Default"), this);
	connect(_default, SIGNAL(stateChanged(int)), this, SLOT(defaultChanged(int)));
	layout->addWidget(_default);
	_fontButton = new QPushButton(tr("Change..."), this);
	connect(_fontButton, SIGNAL(clicked()), this, SLOT(changeFont()));
	setFont(_defaultFont);
	layout->addWidget(_fontButton);
}

ListsPreferences::ListsPreferences(QWidget *parent) : PreferencesWindowCategory(tr("Lists"), parent)
{
}

void ListsPreferences::refresh()
{
}

void ListsPreferences::applySettings()
{
}

void ListsPreferences::updateUI()
{
}

void PreferencesFontChooser::setFont(const QFont &f)
{
	_font = f;
	emit fontChanged(font());
}

void PreferencesFontChooser::defaultChanged(int state)
{
	if (state == Qt::Unchecked) {
		_fontButton->setEnabled(true);
	}
	else if (state == Qt::Checked) {
		_fontButton->setEnabled(false);
		_font = _defaultFont;
		emit fontChanged(font());
	}
}

void PreferencesFontChooser::changeFont()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, _font, this);
	if (ok) {
		setFont(f);
	}
}

void PreferencesFontChooser::checkDefaultState(const QFont &f)
{
	if (_default->isChecked()) {
		_font = f;
		emit fontChanged(font());
	}
}

PreferencesDetailedViewExample::PreferencesDetailedViewExample(QWidget *parent) : DetailedView(parent)
{
	setKanjisClickable(false);
	QTextCursor cursor(document());
	QTextBlockFormat blockFormat;
	blockFormat.setAlignment(Qt::AlignHCenter);
	cursor.setBlockFormat(blockFormat);
	cursor.insertText(QString::fromUtf8("ひらがなカタカナ"));
	cursor.insertBlock(blockFormat);
	cursor.insertText(QString::fromUtf8("漢字"));
	cursor.insertBlock(QTextBlockFormat());
	cursor.insertBlock(QTextBlockFormat());
	cursor.insertText(QString::fromUtf8("漢字 (ひらがなカタカナ): %1").arg("Romaji text"));
}

void PreferencesDetailedViewExample::redrawKanasHeader(const QFont &font)
{
	QTextCursor cursor(document());
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	QTextCharFormat charFormat;
	charFormat.setFont(font);
	charFormat.setForeground(DetailedViewFonts::color(DetailedViewFonts::KanaHeader));
	cursor.setCharFormat(charFormat);
}

void PreferencesDetailedViewExample::redrawKanjisHeader(const QFont &font)
{
	QTextCursor cursor(document());
	cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	QTextCharFormat charFormat;
	charFormat.setFont(font);
	cursor.setCharFormat(charFormat);
}

void PreferencesDetailedViewExample::redrawKanjis(const QFont &font)
{
	QTextCursor cursor(document());
	cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, 3);
	cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 2);
	QTextCharFormat charFormat;
	charFormat.setFont(font);
	cursor.setCharFormat(charFormat);
}

void PreferencesDetailedViewExample::redrawKanas(const QFont &font)
{
	QTextCursor cursor(document());
	cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, 3);
	cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, 4);
	cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 8);
	QTextCharFormat charFormat;
	charFormat.setFont(font);
	cursor.setCharFormat(charFormat);
}

void PreferencesDetailedViewExample::redrawRomajis(const QFont &font)
{
	QTextCursor cursor(document());
	cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, 3);
	cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, 15);
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	QTextCharFormat charFormat;
	charFormat.setFont(font);
	cursor.setCharFormat(charFormat);
}
