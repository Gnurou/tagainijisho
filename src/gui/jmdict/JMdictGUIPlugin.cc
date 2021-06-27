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

#include "core/TextTools.h"
#include "core/ASyncQuery.h"
#include "core/QueryBuilder.h"
#include "core/jmdict/JMdictEntrySearcher.h"
#include "core/jmdict/JMdictPlugin.h"
#include "gui/EntryFormatter.h"
#include "gui/jmdict/JMdictEntryFormatter.h"
#include "gui/jmdict/JMdictPreferences.h"
#include "gui/jmdict/JMdictGUIPlugin.h"
#include "gui/TrainSettings.h"
#include "gui/MainWindow.h"

#include <QMessageBox>
#include <QToolTip>
#include <QInputDialog>
#include <QtAlgorithms>
#include <QCoreApplication>
#include <QFile>
#include <QDir>

PreferenceItem<bool> JMdictGUIPlugin::furiganasForTraining("jmdict", "furiganasForTraining", true);

JMdictGUIPlugin::JMdictGUIPlugin() : Plugin("JMdictGUI"), _flashJL(0), _flashJS(0), _flashTL(0), _flashTS(0), _linkhandler(0), _filter(0), _trainer(0)
{
}

JMdictGUIPlugin::~JMdictGUIPlugin()
{
}

bool JMdictGUIPlugin::onRegister()
{
	// Check if the JMdict plugin is loaded
	if (!Plugin::pluginExists("JMdict")) return false;
	// Register the formatter
	if (!EntryFormatter::registerFormatter(JMDICTENTRY_GLOBALID, &JMdictEntryFormatter::instance())) return false;
	// Register the link handler
	_linkhandler = new JMdictLinkHandler();
	if (!DetailedViewLinkManager::registerHandler(_linkhandler)) return false;

	// Add the main window menu entries
	MainWindow *mainWindow = MainWindow::instance();
	QMenu *menu = mainWindow->trainMenu();
	QMenu *menu2 = menu->addMenu(tr("Vocabulary flashcards"));
	_flashJL = menu2->addAction(tr("From &japanese, whole study list"));
	_flashJS = menu2->addAction(tr("From &japanese, current set"));
	connect(_flashJL, SIGNAL(triggered()), this, SLOT(trainingJapaneseList()));
	connect(_flashJS, SIGNAL(triggered()), this, SLOT(trainingJapaneseSet()));
	menu2->addSeparator();
	_flashTL = menu2->addAction(tr("From &translation, whole study list"));
	_flashTS = menu2->addAction(tr("From &translation, current set"));
	connect(_flashTL, SIGNAL(triggered()), this, SLOT(trainingTranslationList()));
	connect(_flashTS, SIGNAL(triggered()), this, SLOT(trainingTranslationSet()));

	// Add the search extender
	_filter = new JMdictFilterWidget(0);
	mainWindow->searchWidget()->addSearchFilter(_filter);

	// Add the preference panel
	PreferencesWindow::addPanel(&JMdictPreferences::staticMetaObject);

	return true;
}

bool JMdictGUIPlugin::onUnregister()
{
	MainWindow *mainWindow = MainWindow::instance();
	// Remove the preference panel
	PreferencesWindow::removePanel(&JMdictPreferences::staticMetaObject);

	// Remove the search extender
	mainWindow->searchWidget()->removeSearchFilterWidget(_filter->name());
	delete _filter; _filter = 0;
	// Remove the main window entries
	delete _flashJS; _flashJS = 0;
	delete _flashJL; _flashJL = 0;
	delete _flashTL; _flashTL = 0;
	delete _flashTS; _flashTS = 0;
	// Remove the link handler
	DetailedViewLinkManager::removeHandler(_linkhandler);
	delete _linkhandler; _linkhandler = 0;
	// Remove the formatter
	EntryFormatter::removeFormatter(JMDICTENTRY_GLOBALID);
	return true;
}

JMdictLinkHandler::JMdictLinkHandler() : DetailedViewLinkHandler("longdesc")
{
}

void JMdictLinkHandler::handleUrl(const QUrl &url, DetailedView *view)
{
	QString translated;
	if (url.authority() == "pos") {
		translated = QCoreApplication::translate("JMdictLongDescs", JMdictPlugin::posMap()[url.fragment()].first.toLatin1());
	}
	else if (url.authority() == "misc") {
		translated = QCoreApplication::translate("JMdictLongDescs", JMdictPlugin::miscMap()[url.fragment()].first.toLatin1());
	}
	else if (url.authority() == "dialect") {
		translated = QCoreApplication::translate("JMdictLongDescs", JMdictPlugin::dialMap()[url.fragment()].first.toLatin1());
	}
	else if (url.authority() == "field") {
		translated = QCoreApplication::translate("JMdictLongDescs", JMdictPlugin::fieldMap()[url.fragment()].first.toLatin1());
	}
	else return;
	QToolTip::showText(QCursor::pos(), translated.replace(0, 1, translated[0].toUpper()), 0, QRect());
}

void JMdictFilterWidget::updateMiscFilteredProperties()
{
	const QStringList &filtered(JMdictEntrySearcher::miscPropertiesFilter.value().split(','));
	QFont normalFont;
	QFont italicFont;
	italicFont.setItalic(true);
	foreach (QAction *action, _miscButton->menu()->actions()) {
		if (filtered.contains(action->property("TJpropertyIndex").toString()))
			action->setFont(italicFont);
		else action->setFont(normalFont);
	}
}

void JMdictGUIPlugin::training(YesNoTrainer::TrainingMode mode, const QString &queryString)
{
	bool restart = false;
	// Trainer is automatically set to 0 by the destroyed() slot
	if (_trainer && (_trainer->trainingMode() != mode || _trainer->query() != queryString)) delete _trainer;
	if (!_trainer) {
		restart = true;
		_trainer = new JMdictYesNoTrainer(MainWindow::instance());
		_trainer->setAttribute(Qt::WA_DeleteOnClose);
		_trainer->setWindowFlags(Qt::Window);
		connect(_trainer, SIGNAL(destroyed()), this, SLOT(trainerDeleted()));
		_trainer->setTrainingMode(mode);
		_trainer->setQuery(queryString);
	}

	_trainer->show();
	_trainer->activateWindow();
	_trainer->raise();

	if (restart) _trainer->train();
}

void JMdictGUIPlugin::trainingJapaneseList()
{
	training(YesNoTrainer::Japanese, TrainSettings::buildQueryString(JMDICTENTRY_GLOBALID));
}

void JMdictGUIPlugin::trainingJapaneseSet()
{
	QueryBuilder *qBuilder(MainWindow::instance()->searchWidget()->queryBuilder());
	const QueryBuilder::Statement *stat(qBuilder->getStatementForEntryType(JMDICTENTRY_GLOBALID));
	if (!stat) {
		QMessageBox::information(MainWindow::instance(), tr("Nothing to train"), tr("There are no vocabulary entries in this set to train on."));
		return;
	}

	QString queryString(stat->buildSqlStatement());
	queryString += " " + TrainSettings::buildOrderString("score");
	training(YesNoTrainer::Japanese, queryString);
}

void JMdictGUIPlugin::trainingTranslationList()
{
	training(YesNoTrainer::Translation, TrainSettings::buildQueryString(JMDICTENTRY_GLOBALID));
}

void JMdictGUIPlugin::trainingTranslationSet()
{
	QueryBuilder *qBuilder(MainWindow::instance()->searchWidget()->queryBuilder());
	const QueryBuilder::Statement *stat(qBuilder->getStatementForEntryType(JMDICTENTRY_GLOBALID));
	if (!stat) {
		QMessageBox::information(MainWindow::instance(), tr("Nothing to train"), tr("There are no vocabulary entries in this set to train on."));
		return;
	}

	QString queryString(stat->buildSqlStatement());
	queryString += " " + TrainSettings::buildOrderString("score");
	training(YesNoTrainer::Translation, queryString);
}

void JMdictGUIPlugin::trainerDeleted()
{
	_trainer = 0;
}
