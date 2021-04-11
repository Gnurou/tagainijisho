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

#include "core/Database.h"
#include "core/Lang.h"
#include "core/EntrySearcherManager.h"
#include "core/RelativeDate.h"
#include "gui/UpdateChecker.h"
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
#include <QMessageBox>
#include <QFile>

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
	category = new ListsViewPreferences(this);
	addCategory(category);

	// Now add the panels provided by plugins
	foreach (const QMetaObject *objClass, _pluginPanels) {
		category = qobject_cast<PreferencesWindowCategory *>(objClass->newInstance(Q_ARG(QWidget *, this)));
		if (!category) continue;
		addCategory(category);
	}

	// Data preferences
	category = new DataPreferences(this);
	addCategory(category);
	
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

void PreferencesWindow::applySettings()
{
	QList<PreferencesWindowCategory *> cats;
	for (int i = 0; i < stackedWidget->count(); i++)
		cats << qobject_cast<PreferencesWindowCategory *> (stackedWidget->widget(i));
	foreach (PreferencesWindowCategory *category, cats) category->applySettings();
	foreach (PreferencesWindowCategory *category, cats) category->updateUI();
}

static const QMap<QString, QString> _langMapping()
{
	QMap<QString, QString> ret;
	ret.insert("ar", QT_TRANSLATE_NOOP("GeneralPreferences", "Arabic"));
	ret.insert("cs", QT_TRANSLATE_NOOP("GeneralPreferences", "Czech"));
	ret.insert("de", QT_TRANSLATE_NOOP("GeneralPreferences", "German"));
	ret.insert("en", QT_TRANSLATE_NOOP("GeneralPreferences", "English"));
	ret.insert("es", QT_TRANSLATE_NOOP("GeneralPreferences", "Spanish"));
	ret.insert("fa_IR", QT_TRANSLATE_NOOP("GeneralPreferences", "Persian (Iran)"));
	ret.insert("fi_FI", QT_TRANSLATE_NOOP("GeneralPreferences", "Finnish (Finland)"));
	ret.insert("fr", QT_TRANSLATE_NOOP("GeneralPreferences", "French"));
	ret.insert("hu", QT_TRANSLATE_NOOP("GeneralPreferences", "Hungarian"));
	ret.insert("id", QT_TRANSLATE_NOOP("GeneralPreferences", "Indonesian"));
	ret.insert("it", QT_TRANSLATE_NOOP("GeneralPreferences", "Italian"));
	ret.insert("ja", QT_TRANSLATE_NOOP("GeneralPreferences", "Japanese"));
	ret.insert("nb", QT_TRANSLATE_NOOP("GeneralPreferences", "Norvegian Bokmal"));
	ret.insert("nl", QT_TRANSLATE_NOOP("GeneralPreferences", "Dutch"));
	ret.insert("pl", QT_TRANSLATE_NOOP("GeneralPreferences", "Polish"));
	ret.insert("pt_BR", QT_TRANSLATE_NOOP("GeneralPreferences", "Portuguese (Brazil)"));
	ret.insert("ru", QT_TRANSLATE_NOOP("GeneralPreferences", "Russian"));
	ret.insert("sv", QT_TRANSLATE_NOOP("GeneralPreferences", "Swedish"));
	ret.insert("th", QT_TRANSLATE_NOOP("GeneralPreferences", "Thai"));
	ret.insert("tr", QT_TRANSLATE_NOOP("GeneralPreferences", "Turkish"));
	ret.insert("uk", QT_TRANSLATE_NOOP("GeneralPreferences", "Ukrainian"));
	ret.insert("vi", QT_TRANSLATE_NOOP("GeneralPreferences", "Vietnamese"));
	ret.insert("zh", QT_TRANSLATE_NOOP("GeneralPreferences", "Chinese"));
	return ret;
}

static const QMap<QString, QString> langMapping(_langMapping());

typedef QPair<QString, QString> qps;

/* Localized sorted version of langMapping */
static const QList<qps> _prefLangs()
{
	QList<qps> ret;
	foreach (const QString &lang, langMapping.keys()) {
		// FIXME log(n2)
		int i;
		QString trans = QCoreApplication::translate("GeneralPreferences", langMapping[lang].toUtf8().constData());
		for (i = 0; i < ret.size(); i++)
			if (trans <= ret[i].second) break;
		ret.insert(i, qps(lang, trans));
	}

	return ret;
}

GeneralPreferences::GeneralPreferences(QWidget *parent) : PreferencesWindowCategory(tr("General"), parent)
{
	setupUi(this);

	fontChooser = new PreferencesFontChooser(tr("Application-wide default font"), QFont(), this);
	static_cast<QBoxLayout*>(generalGroupBox->layout())->insertWidget(0, fontChooser);

	QList<qps> prefLangs = _prefLangs();
	foreach (const qps &langPair, prefLangs) {
		QString lang(langPair.second);
		const QString &enLang = langMapping[langPair.first];
		QIcon langIcon(QString(":/images/flags/%1.png").arg(langPair.first));
		if (lang != enLang) lang += QString(" (%1)").arg(enLang);
		if (langPair.first == "en" || Lang::supportedGUILanguages().contains(langPair.first)) {
			guiLanguage->addItem(langIcon, lang);
			guiLanguage->setItemData(guiLanguage->count() - 1, langPair.first);
		}
		if (langPair.first == "en" || Lang::supportedDictLanguages().contains(langPair.first)) {
			dictLanguage->addItem(langIcon, lang);
			dictLanguage->setItemData(dictLanguage->count() - 1, langPair.first);
		}
	}

	firstDayOfWeek->addItem(tr("Monday"), Qt::Monday);
	firstDayOfWeek->addItem(tr("Sunday"), Qt::Sunday);

	connect(checkForUpdates, SIGNAL(stateChanged(int)), this, SLOT(onCheckUpdatesChecked(int)));
	connect(checkInterval, SIGNAL(valueChanged(int)), this, SLOT(updateNextCheckLabel()));
}

void GeneralPreferences::refresh()
{
	fontChooser->setDefault(MainWindow::applicationFont.isDefault());
	fontChooser->setFont(QFont());

	for (int idx = 1; idx < guiLanguage->count(); idx++)
		if (guiLanguage->itemData(idx) == Lang::preferredGUILanguage.value()) {
			guiLanguage->setCurrentIndex(idx);
			break;
		}
	for (int idx = 1; idx < dictLanguage->count(); idx++)
		if (dictLanguage->itemData(idx) == Lang::preferredDictLanguage.value()) {
			dictLanguage->setCurrentIndex(idx);
			break;
		}

	alwaysShowEnglish->setChecked(Lang::alwaysShowEnglish.value());

	firstDayOfWeek->setCurrentIndex(firstDayOfWeek->findData(RelativeDate::firstDayOfWeek.value()));

	checkForUpdates->setChecked(MainWindow::autoCheckUpdates.value());
	checkForBetaUpdates->setChecked(MainWindow::autoCheckBetaUpdates.value());
	checkInterval->setValue(MainWindow::updateCheckInterval.value());
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

void GeneralPreferences::applySettings()
{
	// Default font
	const QFont &font = fontChooser->font();
	if (fontChooser->isDefault()) MainWindow::applicationFont.reset();
	else MainWindow::applicationFont.set(font.toString());

	// Application language
	if (guiLanguage->currentIndex() == 0) Lang::preferredGUILanguage.reset();
	else Lang::preferredGUILanguage.set(guiLanguage->itemData(guiLanguage->currentIndex()).toString());
	if (dictLanguage->currentIndex() == 0) Lang::preferredDictLanguage.reset();
	else Lang::preferredDictLanguage.set(dictLanguage->itemData(dictLanguage->currentIndex()).toString());

	// Always show English
	Lang::alwaysShowEnglish.set(alwaysShowEnglish->isChecked());

	// First day of week
	RelativeDate::firstDayOfWeek.set(static_cast<Qt::DayOfWeek>(firstDayOfWeek->itemData(firstDayOfWeek->currentIndex()).toInt()));

	// Updates check
	MainWindow::autoCheckUpdates.set(checkForUpdates->isChecked());
	MainWindow::autoCheckBetaUpdates.set(checkForBetaUpdates->isChecked());
	MainWindow::updateCheckInterval.set(checkInterval->value());
}

EntryDelegatePreferences::EntryDelegatePreferences(QWidget *parent) : QWidget(parent)
{
	setupUi(this);
	_delegateLayout = new EntryDelegateLayout(this, EntryDelegateLayout::OneLine, "", "", "");
	
	QLayout *topLayout = layout();
	QFont kFont;
	kFont.fromString(ResultsView::kanjiFontSetting.defaultValue());
	kanjifontChooser = new PreferencesFontChooser(tr("Main writing"), kFont, this);
	connect(kanjifontChooser, SIGNAL(fontChanged(const QFont &)), this, SLOT(setKanjiFont(const QFont &)));
	topLayout->addWidget(kanjifontChooser);

	kFont = QFont();
	kFont.fromString(ResultsView::kanaFontSetting.defaultValue());
	kanafontChooser = new PreferencesFontChooser(tr("Readings and alternate writings"), kFont, this);
	connect(kanafontChooser, SIGNAL(fontChanged(const QFont &)), this, SLOT(setKanaFont(const QFont &)));
	topLayout->addWidget(kanafontChooser);

	kFont = QFont();
	kFont.fromString(ResultsView::textFontSetting.defaultValue());
	romajifontChooser = new PreferencesFontChooser(tr("Definitions"), kFont, this);
	connect(romajifontChooser, SIGNAL(fontChanged(const QFont &)), this, SLOT(setTextFont(const QFont &)));
	topLayout->addWidget(romajifontChooser);
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

// BUG direct dependency to JMdict!
static EntryRef tagainiRef(1, 1268780);
ResultsViewPreferences::ResultsViewPreferences(QWidget *parent) : PreferencesWindowCategory(tr("Results view"), parent)
{
	setupUi(this);

	// Configure the delegate layout
	entryDelegatePrefs->setPrefsToWatch(&ResultsView::displayModeSetting, &ResultsView::textFontSetting, &ResultsView::kanjiFontSetting, &ResultsView::kanaFontSetting);
	// Prepare the preview list
	_list = new ResultsList(this);
	_view = new ResultsView(this, entryDelegatePrefs->delegateLayout(), true);
	_view->setModel(_list);
	_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
	for (int i = 0; i < 20; i++) _list->addResult(tagainiRef);
	QVBoxLayout *vLayout = new QVBoxLayout(previewBox);
	vLayout->addWidget(_view);
	
	connect(smoothScrolling, SIGNAL(toggled(bool)), this, SLOT(onSmoothScrollingToggled(bool)));
}

void ResultsViewPreferences::refresh()
{
	resultsOrder->setCurrentIndex(EntrySearcherManager::studiedEntriesFirst.value());
	smoothScrolling->setChecked(ResultsView::smoothScrollingSetting.value());

	entryDelegatePrefs->refresh();
}

void ResultsViewPreferences::applySettings()
{
	ResultsView::smoothScrollingSetting.set(smoothScrolling->isChecked());
	EntrySearcherManager::studiedEntriesFirst.set(resultsOrder->currentIndex());
	
	entryDelegatePrefs->applySettings();
}

ListsViewPreferences::ListsViewPreferences(QWidget *parent) : PreferencesWindowCategory(tr("Lists"), parent)
{
	setupUi(this);

	// Configure the delegate layout
	entryDelegatePrefs->setPrefsToWatch(&EntryListView::displayModeSetting, &EntryListView::textFontSetting, &EntryListView::kanjiFontSetting, &EntryListView::kanaFontSetting);
	// Prepare the preview list
	_list = new ResultsList(this);
	_view = new EntryListView(this, entryDelegatePrefs->delegateLayout(), true);
	// Otherwise scrollbar appears when the display mode changes
	_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_view->setModel(_list);
	for (int i = 0; i < 20; i++) _list->addResult(tagainiRef);
	QVBoxLayout *vLayout = new QVBoxLayout(previewBox);
	vLayout->addWidget(_view);

	connect(smoothScrolling, SIGNAL(toggled(bool)), this, SLOT(onSmoothScrollingToggled(bool)));
}

void ListsViewPreferences::refresh()
{
	smoothScrolling->setChecked(EntryListView::smoothScrollingSetting.value());

	entryDelegatePrefs->refresh();
}

void ListsViewPreferences::applySettings()
{
	EntryListView::smoothScrollingSetting.set(smoothScrolling->isChecked());
	
	entryDelegatePrefs->applySettings();
}

DetailedViewPreferences::DetailedViewPreferences(QWidget *parent) : PreferencesWindowCategory(tr("Detailed view"), parent)
{
	setupUi(this);

	detailedExample = new PreferencesDetailedViewExample(previewBox);
	{
		QVBoxLayout *layout = new QVBoxLayout(previewBox);
		layout->addWidget(detailedExample);
	}

	{
		QGridLayout *gridLayout = new QGridLayout(fontsBox);

		romajifontSizeChooser = new PreferencesFontSizeChooser(tr("Default text"), DetailedViewFonts::font(DetailedViewFonts::DefaultText), fontsBox);
		connect(romajifontSizeChooser, SIGNAL(fontChanged(const QFont &)), detailedExample, SLOT(redrawRomajis(const QFont &)));
		gridLayout->addWidget(romajifontSizeChooser, 0, 0, 1, 2);

		kanaHeaderfontSizeChooser = new PreferencesFontSizeChooser(tr("Kana header"), DetailedViewFonts::font(DetailedViewFonts::KanaHeader), fontsBox);
		connect(kanaHeaderfontSizeChooser, SIGNAL(fontChanged(const QFont &)), detailedExample, SLOT(redrawKanasHeader(const QFont &)));
		gridLayout->addWidget(kanaHeaderfontSizeChooser, 1, 0);

		kanjiHeaderfontSizeChooser = new PreferencesFontSizeChooser(tr("Kanji header"), DetailedViewFonts::font(DetailedViewFonts::KanjiHeader), fontsBox);
		connect(kanjiHeaderfontSizeChooser, SIGNAL(fontChanged(const QFont &)), detailedExample, SLOT(redrawKanjisHeader(const QFont &)));
		gridLayout->addWidget(kanjiHeaderfontSizeChooser, 2, 0);

		kanjifontSizeChooser = new PreferencesFontSizeChooser(tr("Kanji"), DetailedViewFonts::font(DetailedViewFonts::Kanji), fontsBox);
		connect(kanjifontSizeChooser, SIGNAL(fontChanged(const QFont &)), detailedExample, SLOT(redrawKanjis(const QFont &)));
		//connect(kanjifontSizeChooser, SIGNAL(fontChanged(const QFont &)), kanafontSizeChooser, SLOT(checkDefaultState(const QFont &)));
		gridLayout->addWidget(kanjifontSizeChooser, 1, 1);

		kanafontSizeChooser = new PreferencesFontSizeChooser(tr("Kana"), DetailedViewFonts::font(DetailedViewFonts::Kana), fontsBox);
		connect(kanafontSizeChooser, SIGNAL(fontChanged(const QFont &)), detailedExample, SLOT(redrawKanas(const QFont &)));
		//connect(kanafontSizeChooser, SIGNAL(fontChanged(const QFont &)), kanafontSizeChooser, SLOT(checkDefaultState(const QFont &)));
		gridLayout->addWidget(kanafontSizeChooser, 2, 1);
	}
}

void DetailedViewPreferences::refresh()
{
	smoothScrolling->setChecked(DetailedView::smoothScrolling.value());
	shortDescShowJLPT->setChecked(EntryFormatter::shortDescShowJLPT.value());

	//romajifontChooser->setDefault(DetailedViewFonts::textFont.isDefault());
	romajifontSizeChooser->changeSize(DetailedViewFonts::font(DetailedViewFonts::DefaultText).pointSize());
	//kanaHeaderfontChooser->setDefault(DetailedViewFonts::kanaHeaderFont.isDefault());
	kanaHeaderfontSizeChooser->changeSize(DetailedViewFonts::font(DetailedViewFonts::KanaHeader).pointSize());
	//kanjiHeaderfontChooser->setDefault(DetailedViewFonts::kanjiHeaderFont.isDefault());
	kanjiHeaderfontSizeChooser->changeSize(DetailedViewFonts::font(DetailedViewFonts::KanjiHeader).pointSize());
	//kanjifontChooser->setDefault(DetailedViewFonts::kanjiFont.isDefault());
	kanjifontSizeChooser->changeSize(DetailedViewFonts::font(DetailedViewFonts::Kanji).pointSize());
	//kanafontChooser->setDefault(DetailedViewFonts::kanaFont.isDefault());
	kanafontSizeChooser->changeSize(DetailedViewFonts::font(DetailedViewFonts::Kana).pointSize());
}

void DetailedViewPreferences::applyFontSetting(PreferencesFontSizeChooser *fontChooser, PreferenceItem<int> *prefItem,
		const DetailedViewFonts::FontRole fontRole)
{
	const QFont &font = fontChooser->font();
	prefItem->set(font.pointSize());
	DetailedViewFonts::setFont(fontRole, font);
}

void DetailedViewPreferences::applySettings()
{
	DetailedView::smoothScrolling.set(smoothScrolling->isChecked());
	EntryFormatter::shortDescShowJLPT.set(shortDescShowJLPT->isChecked());

	// Detailed view fonts
	applyFontSetting(romajifontSizeChooser, &DetailedViewFonts::textFontSize, DetailedViewFonts::DefaultText);
	applyFontSetting(kanaHeaderfontSizeChooser, &DetailedViewFonts::kanaHeaderFontSize, DetailedViewFonts::KanaHeader);
	applyFontSetting(kanjiHeaderfontSizeChooser, &DetailedViewFonts::kanjiHeaderFontSize, DetailedViewFonts::KanjiHeader);
	applyFontSetting(kanafontSizeChooser, &DetailedViewFonts::kanaFontSize, DetailedViewFonts::Kana);
	applyFontSetting(kanjifontSizeChooser, &DetailedViewFonts::kanjiFontSize, DetailedViewFonts::Kanji);
}

void DetailedViewPreferences::updateUI()
{
	foreach (DetailedView *view, DetailedView::instances()) view->setSmoothScrolling(smoothScrolling->isChecked());
	DetailedViewFonts::fontsChanged();
}

DataPreferences::DataPreferences(QWidget *parent) : PreferencesWindowCategory(tr("User data"), parent)
{
	setupUi(this);
	connect(wipeUserDataButton, SIGNAL(pressed()), this, SLOT(onWipeUserDataButtonPushed()));
	userDBFileLabel->setText(Database::defaultDBFile());
}

void DataPreferences::onWipeUserDataButtonPushed()
{
	if (QMessageBox::question(this, tr("Really erase user data?"), tr("This will erase all your user data. Are you sure you want to do this?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
		if (QMessageBox::question(this, tr("REALLY erase it?"), tr("Are you absolutely, positively, definitely sure?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
			QFile f(Database::defaultDBFile());
			if (!f.exists()) {
				QMessageBox::information(this, tr("No user database file"), tr("The user database file has already been erased before."));
			}
			else if (!f.remove()) {
				QMessageBox::warning(this, tr("Cannot erase user database"), tr("Unable to erase user database. Please see what is wrong and try to do it manually."));
			} else {
				QMessageBox::information(this, tr("User data erased"), tr("User data has been erased. Tagaini Jisho will now exit. You will be back to a blank database when it is restarted."));
				QApplication::quit();
			}
		}
	}
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

PreferencesFontSizeChooser::PreferencesFontSizeChooser(const QString &whatFor, const QFont &font, QWidget *parent) : QWidget(parent), _font(font)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(new QLabel(QString(tr("%1:").arg(whatFor))));
	layout->addStretch();
	_size = new QSpinBox(this);
	_size->setMinimum(1);
	_size->setMaximum(40);
	_size->setValue(_font.pointSize());
	connect(_size, SIGNAL(valueChanged(int)), this, SLOT(changeSize(int)));
	layout->addWidget(_size);
}

void PreferencesFontSizeChooser::changeSize(int size)
{
	_font.setPointSize(size);
	emit fontChanged(_font);
}


PreferencesDetailedViewExample::PreferencesDetailedViewExample(QWidget *parent) : DetailedView(parent)
{
	setKanjiClickable(false);
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
