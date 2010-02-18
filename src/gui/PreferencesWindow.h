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

#ifndef __GUI_PREFERENCESWINDOW_H_
#define __GUI_PREFERENCESWINDOW_H_

#include "core/Preferences.h"
#include "gui/DetailedView.h"
#include "gui/EntryDelegate.h"
#include "gui/ResultsList.h"
#include "gui/ResultsView.h"
#include "gui/ui_PreferencesWindow.h"
#include "gui/ui_GeneralPreferences.h"
#include "gui/ui_ResultsViewPreferences.h"
#include "gui/ui_DetailedViewPreferences.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QLabel>
#include <QTabWidget>
#include <QTextBrowser>
#include <QListWidget>
#include <QStackedLayout>
#include <QSlider>
#include <QComboBox>

class PreferencesWindowCategory : public QWidget
{
	Q_OBJECT
private:
	const QString _name;

public slots:
	virtual void applySettings() = 0;
	virtual void refresh() = 0;
	virtual void updateUI() {}

public:
	PreferencesWindowCategory(const QString &name, QWidget *parent = 0);
	const QString &name() const { return _name; }
};

class PreferencesWindow : public QDialog, private Ui::PreferencesWindow
{
	Q_OBJECT
private:
	static QList<const QMetaObject *> _pluginPanels;

private slots:
	void applySettings();

public:
	PreferencesWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);
	void addCategory(PreferencesWindowCategory *category);

	static void addPanel(const QMetaObject *panel) { _pluginPanels << panel; }
	static void removePanel(const QMetaObject *panel) { _pluginPanels.removeAll(panel); }
};

class PreferencesFontChooser;

class GeneralPreferences : public PreferencesWindowCategory, private Ui::GeneralPreferences
{
	Q_OBJECT
private:
	// Default font
	PreferencesFontChooser *fontChooser;
	static const QStringList langCodes;
	static const QStringList langNames;

public slots:
	void applySettings();
	void refresh();

	void onCheckUpdatesChecked(int checked);
	void updateNextCheckLabel();
	void onCacheSizeDefaultChecked(bool checked);

public:
	GeneralPreferences(QWidget *parent = 0);
};

class ResultsViewPrefsDummyEntry : public Entry
{
	Q_OBJECT
protected:
	void _detailedVersion(QTextCursor &cursor, DetailedView *view) const {}

public:
	ResultsViewPrefsDummyEntry() : Entry(-1, -1) {}
	void writeShortDesc(QTextCursor &cursor) const {}
	virtual QStringList writings() const;
	virtual QStringList readings() const;
	virtual QStringList meanings() const;
};

class ResultsViewPreferences : public PreferencesWindowCategory, private Ui::ResultsViewPreferences
{
	Q_OBJECT
private:
	ResultsList *_list;
	ResultsView *_view;

	PreferencesFontChooser *romajifontChooser;
	PreferencesFontChooser *kanafontChooser;
	PreferencesFontChooser *kanjifontChooser;

	void applyFontSetting(PreferencesFontChooser *fontChooser, PreferenceItem<QString> *prefItem, const EntryDelegateLayout::FontRole fontRole);

protected slots:
	void onSmoothScrollingToggled(bool value) { _view->setSmoothScrolling(value); }

public slots:
	void applySettings();
	void refresh();
	void updateUI();

public:
	ResultsViewPreferences(QWidget *parent = 0);
};

class PreferencesDetailedViewExample : public DetailedView
{
	Q_OBJECT
public:
	PreferencesDetailedViewExample(QWidget *parent = 0);

public slots:
	void redrawKanasHeader(const QFont &font);
	void redrawKanjisHeader(const QFont &font);
	void redrawKanas(const QFont &font);
	void redrawKanjis(const QFont &font);
	void redrawRomajis(const QFont &font);
};

class DetailedViewPreferences : public PreferencesWindowCategory, private Ui::DetailedViewPreferences
{
	Q_OBJECT
private:
	PreferencesFontChooser *romajifontChooser;
	PreferencesFontChooser *kanaHeaderfontChooser;
	PreferencesFontChooser *kanjiHeaderfontChooser;
	PreferencesFontChooser *kanjifontChooser;
	PreferencesFontChooser *kanafontChooser;
	PreferencesDetailedViewExample *detailedExample;

	void applyFontSetting(PreferencesFontChooser *fontChooser, PreferenceItem<QString> *prefItem, const DetailedViewFonts::FontRole fontRole);

protected slots:
	void onSmoothScrollingToggled(bool value) { this->detailedExample->setSmoothScrolling(value); }

public slots:
	void applySettings();
	void refresh();
	void updateUI();

public:
	DetailedViewPreferences(QWidget *parent = 0);
};

class QGridLayout;
class PreferencesFontChooser : public QWidget
{
	Q_OBJECT
private:
	QFont _defaultFont;
	QFont _font;
	QCheckBox *_default;
	QPushButton *_fontButton;

private slots:
	void changeFont();
	void defaultChanged(int state);
	void checkDefaultState(const QFont &font);

public:
	PreferencesFontChooser(const QString &whatFor, const QFont &defaultFont = QFont(), QWidget *parent = 0);
	const QFont &font() const { return _font; }
	void setFont(const QFont &f);
	void setDefault(bool def) { _default->setChecked(def); }
	bool isDefault() const { return _default->isChecked(); }

signals:
	void fontChanged(const QFont &font);
};

class PreferencesEntryDelegateLayout : public EntryDelegateLayout
{
	Q_OBJECT
public:
	PreferencesEntryDelegateLayout(EntryDelegateLayout::DisplayMode displayMode = OneLine, const QString& textFont = "", const QString& kanjiFont = "", const QString& kanaFont = "", QObject* parent = 0);

public slots:
	void setKanjiFont(const QFont &font);
	void setKanaFont(const QFont &font);
	void setTextFont(const QFont &font);
	void setOneLineDisplay();
	void setTwoLinesDisplay();
};
/*
class PreferencesEntryDelegate : public EntryDelegate
{
	Q_OBJECT
protected:
	ResultsView *_watchedView;

public:
	PreferencesEntryDelegate(ResultsView *watchedView);
	void updateLayout();

public slots:
	void setKanjiFont(const QFont &font);
	void setKanaFont(const QFont &font);
	void setTextFont(const QFont &font);
	void setOneLineDisplay();
	void setTwoLinesDisplay();
};
*/
#endif
