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
#include "gui/KanjiValidator.h"
#include "gui/TrainSettings.h"
#include "gui/MainWindow.h"

#include <QMessageBox>
#include <QToolTip>
#include <QInputDialog>
#include <QtAlgorithms>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QHBoxLayout>
#include <QVBoxLayout>

PreferenceItem<bool> JMdictGUIPlugin::furiganasForTraining("jmdict", "furiganasForTraining", true);

JMdictGUIPlugin::JMdictGUIPlugin() : Plugin("JMdictGUI"), _formatter(0), _flashJL(0), _flashJS(0), _flashTL(0), _flashTS(0), _linkhandler(0), _filter(0), _trainer(0)
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
	_formatter = new JMdictEntryFormatter();
	if (!EntryFormatter::registerFormatter(JMDICTENTRY_GLOBALID, _formatter)) { delete _formatter; return false; }
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
	_filter = new JMdictOptionsWidget(0);
	mainWindow->addSearchFilter(_filter, mainWindow->getSearchFilter("searchtext"));

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
	mainWindow->removeSearchFilterWidget(_filter->name());
	// TODO does the owner of the filter change??
	//delete _extender; _extender = 0;
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
	delete _formatter; _formatter = 0;
	return true;
}

JMdictLinkHandler::JMdictLinkHandler() : DetailedViewLinkHandler("longdesc")
{
}

void JMdictLinkHandler::handleUrl(const QUrl &url, DetailedView *view)
{
	QString translated;
	if (url.authority() == "pos") {
		translated = QCoreApplication::translate("JMdictLongDescs", JMdictPlugin::posEntities()[url.fragment().toInt()].second.toLatin1());
	}
	else if (url.authority() == "misc") {
		translated = QCoreApplication::translate("JMdictLongDescs", JMdictPlugin::miscEntities()[url.fragment().toInt()].second.toLatin1());
	}
	else if (url.authority() == "dialect") {
		translated = QCoreApplication::translate("JMdictLongDescs", JMdictPlugin::dialectEntities()[url.fragment().toInt()].second.toLatin1());
	}
	else if (url.authority() == "field") {
		translated = QCoreApplication::translate("JMdictLongDescs", JMdictPlugin::fieldEntities()[url.fragment().toInt()].second.toLatin1());
	}
	else return;
	QToolTip::showText(QCursor::pos(), translated.replace(0, 1, translated[0].toUpper()), 0, QRect());
}

void JMdictOptionsWidget::updateMiscFilteredProperties()
{
	const QStringList &filtered(JMdictEntrySearcher::miscPropertiesFilter.value().split(','));
	QFont normalFont;
	QFont italicFont;
	italicFont.setItalic(true);
	foreach (QAction *action, _miscButton->menu()->actions()) {
		if (filtered.contains(JMdictPlugin::miscEntities()[action->property("TJpropertyIndex").toInt()].first))
			action->setFont(italicFont);
		else action->setFont(normalFont);
	}
}

QActionGroup *JMdictOptionsWidget::addCheckableProperties(const QVector<QPair<QString, QString> >&defs, QMenu *menu)
{
	QList<QString> strList;
	for (int i = 0; i < defs.size(); i++) {
		QString translated = QCoreApplication::translate("JMdictLongDescs", defs[i].second.toLatin1());
		strList << QString(translated.replace(0, 1, translated[0].toUpper()));
	}
	QStringList sortedList(strList);
	qSort(sortedList.begin(), sortedList.end());
	QActionGroup *actionGroup = new QActionGroup(menu);
	actionGroup->setExclusive(false);
	foreach(QString str, sortedList) {
		int idx = strList.indexOf(str);
		QAction *action = actionGroup->addAction(str);
		action->setCheckable(true);
		menu->addAction(action);
		action->setProperty("TJpropertyIndex", idx);
	}
	return actionGroup;
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
	QueryBuilder qBuilder(MainWindow::instance()->queryBuilder());
	const QueryBuilder::Statement *stat(qBuilder.getStatementForEntryType(JMDICTENTRY_GLOBALID));
	if (!stat) {
		QMessageBox::information(MainWindow::instance(), tr("Nothing to train"), tr("There are no vocabulary entries in this set to train on."));
		return;
	}

	QString queryString(stat->buildSqlStatement());
	queryString += " " + TrainSettings::buildOrderString("score");
	qDebug() << queryString;
	training(YesNoTrainer::Japanese, queryString);
}

void JMdictGUIPlugin::trainingTranslationList()
{
	training(YesNoTrainer::Translation, TrainSettings::buildQueryString(JMDICTENTRY_GLOBALID));
}

void JMdictGUIPlugin::trainingTranslationSet()
{
	QueryBuilder qBuilder(MainWindow::instance()->queryBuilder());
	const QueryBuilder::Statement *stat(qBuilder.getStatementForEntryType(JMDICTENTRY_GLOBALID));
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

QString JMdictGUIPlugin::pluginInfo() const
{
	return "<p>The <a href=\"http://www.csse.monash.edu.au/~jwb/jmdict.html\">JMDict</a> is distributed under the <a href=\"http://creativecommons.org/licenses/by-sa/3.0/\">Creative Common Attribution Share Alike Licence, version 3.0</a>.</p>";
}

JMdictOptionsWidget::JMdictOptionsWidget(QWidget *parent) : SearchFilterWidget(parent, "wordsdic")
{
	_propsToSave << "containedKanjis" << "containedComponents" << "studiedKanjisOnly" << "pos" << "dial" << "field" << "misc";
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	{
		QHBoxLayout *hLayout = new QHBoxLayout();
		_containedKanjis = new QLineEdit(this);
		KanjiValidator *kanjiValidator = new KanjiValidator(this);
		_containedKanjis->setValidator(kanjiValidator);
		_containedComponents = new QLineEdit(this);
		_containedComponents->setValidator(kanjiValidator);

		_studiedKanjisCheckBox = new QCheckBox(tr("Using studied kanji only"));
		hLayout->addWidget(new QLabel(tr("With kanji:"), this));
		hLayout->addWidget(_containedKanjis);
		hLayout->addWidget(_studiedKanjisCheckBox);
		mainLayout->addLayout(hLayout);

		hLayout = new QHBoxLayout();
		hLayout->addWidget(new QLabel(tr("With components:"), this));
		hLayout->addWidget(_containedComponents);
		mainLayout->addLayout(hLayout);

		connect(_containedKanjis, SIGNAL(textChanged(const QString &)), this, SLOT(commandUpdate()));
		connect(_containedComponents, SIGNAL(textChanged(const QString &)), this, SLOT(commandUpdate()));
		connect(_studiedKanjisCheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));

	}
	{
		QHBoxLayout *hLayout = new QHBoxLayout();

		_posButton = new QPushButton(tr("Part of speech"), this);
		QMenu *menu = new QMenu(this);
		QActionGroup *actionGroup = addCheckableProperties(JMdictPlugin::posEntities(), menu);
		_posButton->setMenu(menu);
		hLayout->addWidget(_posButton);
		connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onPosTriggered(QAction *)));
		_dialButton = new QPushButton(tr("Dialect"), this);
		menu = new QMenu(this);
		actionGroup = addCheckableProperties(JMdictPlugin::dialectEntities(), menu);
		_dialButton->setMenu(menu);
		hLayout->addWidget(_dialButton);
		connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onDialTriggered(QAction *)));
		_fieldButton = new QPushButton(tr("Field"), this);
		menu = new QMenu(this);
		actionGroup = addCheckableProperties(JMdictPlugin::fieldEntities(), menu);
		_fieldButton->setMenu(menu);
		hLayout->addWidget(_fieldButton);
		connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onFieldTriggered(QAction *)));
		_miscButton = new QPushButton(tr("Misc"), this);
		menu = new QMenu(this);
		actionGroup = addCheckableProperties(JMdictPlugin::miscEntities(), menu);
		_miscButton->setMenu(menu);
		hLayout->addWidget(_miscButton);
		connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onMiscTriggered(QAction *)));
		updateMiscFilteredProperties();
		connect(&JMdictEntrySearcher::miscPropertiesFilter, SIGNAL(valueChanged(QVariant)), this, SLOT(updateMiscFilteredProperties()));

		mainLayout->addLayout(hLayout);
	}
	mainLayout->setContentsMargins(0, 0, 0, 0);
}

QString JMdictOptionsWidget::currentCommand() const
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

QString JMdictOptionsWidget::currentTitle() const
{
	QString contains;
	QString kanjis = _containedKanjis->text();
	if (!kanjis.isEmpty()) {
		bool first = true;
		contains += tr(" with ");
		foreach(QChar c, kanjis) {
			if (!first) contains +=",";
			else first = false;
			contains += c;
		}
	}
	if (_studiedKanjisCheckBox->isChecked()) {
		if (!kanjis.isEmpty()) contains += tr(", studied kanji only");
		else contains += tr(" with studied kanji");
	}
	kanjis = _containedComponents->text();
	if (!kanjis.isEmpty()) {
		bool first = true;
		contains += tr(" with component ");
		foreach(QChar c, kanjis) {
			if (!first) contains +=",";
			else first = false;
			contains += c;
		}
	}

	QStringList propsList = _posList + _dialList + _fieldList + _miscList;
	QString props = propsList.join(",");

	if (!props.isEmpty()) props = " (" + props + ")";
	QString ret(props + contains);
	if (!ret.isEmpty()) ret = tr("Vocabulary") + ret;
	else ret = tr("Vocabulary");
	return ret;
}

void JMdictOptionsWidget::onPosTriggered(QAction *action)
{
	if (action->isChecked()) {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_posList << JMdictPlugin::posEntities()[propertyIndex].first;
	}
	else {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_posList.removeOne(JMdictPlugin::posEntities()[propertyIndex].first);
	}
	if (!_posList.isEmpty()) _posButton->setText(tr("Pos:") + _posList.join(","));
	else _posButton->setText(tr("Part of speech"));
	commandUpdate();
}

void JMdictOptionsWidget::onDialTriggered(QAction *action)
{
	if (action->isChecked()) {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_dialList << JMdictPlugin::dialectEntities()[propertyIndex].first;
	}
	else {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_dialList.removeOne(JMdictPlugin::dialectEntities()[propertyIndex].first);
	}
	if (!_dialList.isEmpty()) _dialButton->setText(tr("Dial:") + _dialList.join(","));
	else _dialButton->setText(tr("Dialect"));
	commandUpdate();
}

void JMdictOptionsWidget::onFieldTriggered(QAction *action)
{
	if (action->isChecked()) {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_fieldList << JMdictPlugin::fieldEntities()[propertyIndex].first;
	}
	else {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_fieldList.removeOne(JMdictPlugin::fieldEntities()[propertyIndex].first);
	}
	if (!_fieldList.isEmpty()) _fieldButton->setText(tr("Field:") + _fieldList.join(","));
	else _fieldButton->setText(tr("Field"));
	emit commandUpdate();
}

void JMdictOptionsWidget::onMiscTriggered(QAction *action)
{
	if (action->isChecked()) {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_miscList << JMdictPlugin::miscEntities()[propertyIndex].first;
	}
	else {
		int propertyIndex = action->property("TJpropertyIndex").toInt();
		_miscList.removeOne(JMdictPlugin::miscEntities()[propertyIndex].first);
	}
	if (!_miscList.isEmpty()) _miscButton->setText(tr("Misc:") + _miscList.join(","));
	else _miscButton->setText(tr("Misc"));
	emit commandUpdate();
}

void JMdictOptionsWidget::_reset()
{
	_containedKanjis->clear();
	_containedComponents->clear();
	_studiedKanjisCheckBox->setChecked(false);
	foreach (QAction *action, _posButton->menu()->actions()) if (action->isChecked()) action->trigger();
	_posList.clear();
	foreach (QAction *action, _dialButton->menu()->actions()) if (action->isChecked()) action->trigger();
	_dialList.clear();
	foreach (QAction *action, _fieldButton->menu()->actions()) if (action->isChecked()) action->trigger();
	_fieldList.clear();
	foreach (QAction *action, _miscButton->menu()->actions()) if (action->isChecked()) action->trigger();
	_miscList.clear();
}

void JMdictOptionsWidget::updateFeatures()
{
	if (!_containedKanjis->text().isEmpty() || !_containedComponents->text().isEmpty() || _studiedKanjisCheckBox->isChecked() || !_posList.isEmpty() || !_dialList.isEmpty() || !_fieldList.isEmpty() || !_miscList.isEmpty()) emit disableFeature("kanjidic");
	else emit enableFeature("kanjidic");
}

void JMdictOptionsWidget::setPos(const QStringList &list)
{
	_posList.clear();
	foreach(QAction *action, _posButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(JMdictPlugin::posEntities()[action->property("TJpropertyIndex").toInt()].first))
			action->trigger();
	}
}

void JMdictOptionsWidget::setDial(const QStringList &list)
{
	_dialList.clear();
	foreach(QAction *action, _dialButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(JMdictPlugin::dialectEntities()[action->property("TJpropertyIndex").toInt()].first))
			action->trigger();
	}
}

void JMdictOptionsWidget::setField(const QStringList &list)
{
	_fieldList.clear();
	foreach(QAction *action, _fieldButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(JMdictPlugin::fieldEntities()[action->property("TJpropertyIndex").toInt()].first))
			action->trigger();
	}
}

void JMdictOptionsWidget::setMisc(const QStringList &list)
{
	_miscList.clear();
	foreach(QAction *action, _miscButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(JMdictPlugin::miscEntities()[action->property("TJpropertyIndex").toInt()].first))
			action->trigger();
	}
}
