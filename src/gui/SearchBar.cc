/*
 *  Copyright (C) 2008/2009  Alexandre Courbot
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

#include "gui/SearchBar.h"

#include <QtDebug>

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSlider>
#include <QCalendarWidget>
#include <QSqlQuery>

SearchBarExtender::SearchBarExtender(QWidget *parent, const QString &feature) : QWidget(parent), _feature(feature), _autoUpdateQuery(true), _propsToSave()
{
	_timer.setSingleShot(true);
	_timer.setInterval(500);
	connect(&_timer, SIGNAL(timeout()), this, SIGNAL(commandUpdated()));
}

void SearchBarExtender::updateVisualState()
{
	// Update the title
	emit updateTitle(currentTitle());
	// Update the features
	updateFeatures();
}

void SearchBarExtender::commandUpdate()
{
	updateVisualState();
	if (autoUpdateQuery()) emit commandUpdated();
}

void SearchBarExtender::delayedCommandUpdate()
{
	updateVisualState();
	if (autoUpdateQuery()) _timer.start();
}

void SearchBarExtender::reset()
{
	bool autoUpdateStatus = autoUpdateQuery();
	setAutoUpdateQuery(false);
	_reset();
	setAutoUpdateQuery(autoUpdateStatus);
	commandUpdate();
}

QMap<QString, QVariant> SearchBarExtender::getState() const
{
	QStringList toSave = propertiesToSave();
	QMap<QString, QVariant> ret;
	foreach(const QString &prop, toSave) {
		ret[prop] = property(prop.toLatin1().data());
	}
	return ret;
}

void SearchBarExtender::restoreState(const QMap<QString, QVariant> &state)
{
	setAutoUpdateQuery(false);
	foreach(const QString &prop, state.keys()) {
		setProperty(prop.toLatin1().data(), state.value(prop));
	}
	setAutoUpdateQuery(true);
}

void SearchBarExtender::updateFeatures()
{
}

EntryTypeSelectionWidget::EntryTypeSelectionWidget(SearchBar *parent) : SearchBarExtender(parent), _bar(parent)
{
	_propsToSave << "type";

	_comboBox = new QComboBox(this);
	_comboBox->addItem(tr("All"), 0);
	_comboBox->addItem(tr("Vocabulary"), 1);
	_comboBox->addItem(tr("Kanji"), 2);
	connect(_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxChanged(int)));

	QHBoxLayout *hLayout = new QHBoxLayout(this);
	hLayout->addWidget(_comboBox);
}

QString EntryTypeSelectionWidget::currentCommand() const
{
	switch (_comboBox->itemData(_comboBox->currentIndex()).toInt())
	{
		case 1:
			return ":jmdict";
		case 2:
			return ":kanjidic";
		default:
			return "";
	}
}

void EntryTypeSelectionWidget::onComboBoxChanged(int index)
{
	switch (_comboBox->itemData(_comboBox->currentIndex()).toInt())
	{
		case 1:
			emit disableFeature("kanjidic");
			emit enableFeature("wordsdic");
			break;
		case 2:
			emit disableFeature("wordsdic");
			emit enableFeature("kanjidic");
			break;
		default:
			// TODO
			// We cannot emit the feature that way - we must first check whether other widgets
			// allow us to do so.
			emit enableFeature("wordsdic");
			emit enableFeature("kanjidic");
			break;
	}
	commandUpdate();
}

void EntryTypeSelectionWidget::_reset()
{
	_comboBox->setCurrentIndex(0);
}

JLPTOptionsWidget::JLPTOptionsWidget(QWidget *parent) : SearchBarExtender(parent)
{
	_propsToSave << "levels";

	QGroupBox *jlptBox = new QGroupBox(tr("JLPT filter"), this);
	{
		JLPT4CheckBox = new QCheckBox(tr("Level 4"), jlptBox);
		connect(JLPT4CheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
		JLPT3CheckBox = new QCheckBox(tr("Level 3"), jlptBox);
		connect(JLPT3CheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
		JLPT2CheckBox = new QCheckBox(tr("Level 2"), jlptBox);
		connect(JLPT2CheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
		JLPT1CheckBox = new QCheckBox(tr("Level 1"), jlptBox);
		connect(JLPT1CheckBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));

		QHBoxLayout *hLayout = new QHBoxLayout(jlptBox);
		hLayout->addWidget(JLPT4CheckBox);
		hLayout->addWidget(JLPT3CheckBox);
		hLayout->addWidget(JLPT2CheckBox);
		hLayout->addWidget(JLPT1CheckBox);
	}
	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->addWidget(jlptBox);
	vLayout->setContentsMargins(0, 0, 0, 0);
}

QString JLPTOptionsWidget::currentCommand() const
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
		ret = " :jlpt=" + ret;
	}
	return ret;
}

QString JLPTOptionsWidget::currentTitle() const
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

QList<QVariant> JLPTOptionsWidget::levels() const
{
	QList<QVariant> ret;
	if (JLPT4CheckBox->isChecked()) ret << 4;
	if (JLPT3CheckBox->isChecked()) ret << 3;
	if (JLPT2CheckBox->isChecked()) ret << 2;
	if (JLPT1CheckBox->isChecked()) ret << 1;
	return ret;
}

void JLPTOptionsWidget::setLevels(const QList<QVariant> &filter)
{
	JLPT4CheckBox->setChecked(filter.contains(4));
	JLPT3CheckBox->setChecked(filter.contains(3));
	JLPT2CheckBox->setChecked(filter.contains(2));
	JLPT1CheckBox->setChecked(filter.contains(1));
}

void JLPTOptionsWidget::_reset()
{

	JLPT4CheckBox->setChecked(false);
	JLPT3CheckBox->setChecked(false);
	JLPT2CheckBox->setChecked(false);
	JLPT1CheckBox->setChecked(false);
}

StudyOptionsWidget::StudyOptionsWidget(QWidget *parent) : SearchBarExtender(parent)
{
	_propsToSave << "studyState" << "studyMinScore" << "studyMaxScore" << "studyMinDate" << "studyMaxDate" << "trainMinDate" << "trainMaxDate" <<"mistakenMinDate" << "mistakenMaxDate";

	QGroupBox *scoreBox = new QGroupBox(tr("Score"), this);
	scoreBox->setEnabled(false);
//	scoreBox->setVisible(false);
	{
		QSlider *minSlider = new QSlider(scoreBox);
		minSlider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		minSlider->setRange(0, 100);
		minSlider->setOrientation(Qt::Horizontal);
		minSlider->setMinimumWidth(50);
		QSlider *maxSlider = new QSlider(scoreBox);
		maxSlider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		maxSlider->setRange(0, 100);
		maxSlider->setOrientation(Qt::Horizontal);
		maxSlider->setMinimumWidth(50);
		minSpinBox = new QSpinBox(scoreBox);
		minSpinBox->setRange(0, 100);
		maxSpinBox = new QSpinBox(scoreBox);
		maxSpinBox->setRange(0, 100);
		connect(minSlider, SIGNAL(valueChanged(int)), minSpinBox, SLOT(setValue(int)));
		connect(maxSlider, SIGNAL(valueChanged(int)), maxSpinBox, SLOT(setValue(int)));
		connect(minSpinBox, SIGNAL(valueChanged(int)), minSlider, SLOT(setValue(int)));
		connect(maxSpinBox, SIGNAL(valueChanged(int)), maxSlider, SLOT(setValue(int)));
		minSlider->setValue(0);
		maxSlider->setValue(100);
		connect(minSpinBox, SIGNAL(valueChanged(int)), this, SLOT(delayedCommandUpdate()));
		connect(minSpinBox, SIGNAL(valueChanged(int)), this, SLOT(checkMaxSpinBoxValue(int)));
		connect(maxSpinBox, SIGNAL(valueChanged(int)), this, SLOT(delayedCommandUpdate()));
		connect(maxSpinBox, SIGNAL(valueChanged(int)), this, SLOT(checkMinSpinBoxValue(int)));

		QGridLayout *gridLayout = new QGridLayout(scoreBox);
		gridLayout->addWidget(new QLabel(tr("Min"), scoreBox), 0, 0);
		gridLayout->addWidget(new QLabel(tr("Max"), scoreBox), 1, 0);
		gridLayout->addWidget(minSlider, 0, 1);
		gridLayout->addWidget(maxSlider, 1, 1);
		gridLayout->addWidget(minSpinBox, 0, 2);
		gridLayout->addWidget(maxSpinBox, 1, 2);
	}

	studyBox = new QGroupBox(tr("Study date"), this);
	studyBox->setEnabled(false);
//	studyBox->setVisible(false);
	connect(studyBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	{
		QDate infDate, supDate;
		QSqlQuery query;
		query.exec("select min(dateAdded) from training");
		if (query.next()) {
			infDate = QDateTime::fromTime_t(query.value(0).toUInt()).date();
		}
		query.exec("select max(dateAdded) from training");
		if (query.next()) {
			supDate = QDateTime::fromTime_t(query.value(0).toUInt()).date();
		}

		_studyMinDate = new RelativeDateEdit(studyBox);
		connect(_studyMinDate, SIGNAL(dateSelected(const RelativeDate &)), this, SLOT(commandUpdate()));
		connect(_studyMinDate, SIGNAL(dateChanged(const RelativeDate &)), this, SLOT(delayedCommandUpdate()));
		_studyMaxDate = new RelativeDateEdit(studyBox);
		connect(_studyMaxDate, SIGNAL(dateSelected(const RelativeDate &)), this, SLOT(commandUpdate()));
		connect(_studyMaxDate, SIGNAL(dateChanged(const RelativeDate &)), this, SLOT(delayedCommandUpdate()));

		QGridLayout *gridLayout = new QGridLayout(studyBox);
		gridLayout->addWidget(new QLabel(tr("since"), studyBox), 0, 3);
		gridLayout->addWidget(new QLabel(tr("until"), studyBox), 1, 3);
		gridLayout->addWidget(_studyMinDate, 0, 4);
		gridLayout->addWidget(_studyMaxDate, 1, 4);
	}

	trainBox = new QGroupBox(tr("Last trained"), this);
	trainBox->setEnabled(false);
//	trainBox->setVisible(false);
	connect(trainBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	{
		QDate infDate, supDate;
		QSqlQuery query;
		query.exec("select min(dateAdded) from training");
		if (query.next()) {
			infDate = QDateTime::fromTime_t(query.value(0).toUInt()).date();
		}
		query.exec("select max(dateAdded) from training");
		if (query.next()) {
			supDate = QDateTime::fromTime_t(query.value(0).toUInt()).date();
		}

		_trainMinDate = new RelativeDateEdit(trainBox);
		connect(_trainMinDate, SIGNAL(dateSelected(const RelativeDate &)), this, SLOT(commandUpdate()));
		connect(_trainMinDate, SIGNAL(dateChanged(const RelativeDate &)), this, SLOT(delayedCommandUpdate()));
		_trainMaxDate = new RelativeDateEdit(trainBox);
		connect(_trainMaxDate, SIGNAL(dateSelected(const RelativeDate &)), this, SLOT(commandUpdate()));
		connect(_trainMaxDate, SIGNAL(dateChanged(const RelativeDate &)), this, SLOT(delayedCommandUpdate()));

		QGridLayout *gridLayout = new QGridLayout(trainBox);
		gridLayout->addWidget(new QLabel(tr("since"), trainBox), 0, 3);
		gridLayout->addWidget(new QLabel(tr("until"), trainBox), 1, 3);
		gridLayout->addWidget(_trainMinDate, 0, 4);
		gridLayout->addWidget(_trainMaxDate, 1, 4);
	}

	mistakeBox = new QGroupBox(tr("Last mistaken"), this);
	mistakeBox->setEnabled(false);
//	mistakeBox->setVisible(false);
	connect(mistakeBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	{
		QDate infDate, supDate;
		QSqlQuery query;
		query.exec("select min(date) from trainingLog where result=0");
		if (query.next()) {
			infDate = QDateTime::fromTime_t(query.value(0).toUInt()).date();
		}
		query.exec("select max(date) from trainingLog where result=0");
		if (query.next()) {
			supDate = QDateTime::fromTime_t(query.value(0).toUInt()).date();
		}

		_mistakenMinDate = new RelativeDateEdit(mistakeBox);
		connect(_mistakenMinDate, SIGNAL(dateSelected(const RelativeDate &)), this, SLOT(commandUpdate()));
		connect(_mistakenMinDate, SIGNAL(dateChanged(const RelativeDate &)), this, SLOT(delayedCommandUpdate()));

		_mistakenMaxDate = new RelativeDateEdit(mistakeBox);
		connect(_mistakenMaxDate, SIGNAL(dateSelected(const RelativeDate &)), this, SLOT(commandUpdate()));
		connect(_mistakenMaxDate, SIGNAL(dateChanged(const RelativeDate &)), this, SLOT(delayedCommandUpdate()));

		QGridLayout *gridLayout = new QGridLayout(mistakeBox);
		gridLayout->addWidget(new QLabel(tr("since"), mistakeBox), 0, 0);
		gridLayout->addWidget(new QLabel(tr("until"), mistakeBox), 1, 0);
		gridLayout->addWidget(_mistakenMinDate, 0, 1);
		gridLayout->addWidget(_mistakenMaxDate, 1, 1);
	}

//	QGroupBox *entriesBox = new QGroupBox(tr("Study filter"), this);
	QWidget *entriesBox = new QWidget(this);
	{
		allEntriesButton = new QRadioButton(tr("None"), entriesBox);
		allEntriesButton->setChecked(true);
		connect(allEntriesButton, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonToggled(bool)));
		studiedEntriesButton = new QRadioButton(tr("Studied"), entriesBox);
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonToggled(bool)));
		nonStudiedEntriesButton = new QRadioButton(tr("Not studied"), entriesBox);
		connect(nonStudiedEntriesButton, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonToggled(bool)));

		connect(studiedEntriesButton, SIGNAL(toggled(bool)), scoreBox, SLOT(setEnabled(bool)));
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), studyBox, SLOT(setEnabled(bool)));
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), trainBox, SLOT(setEnabled(bool)));
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), mistakeBox, SLOT(setEnabled(bool)));
/*		connect(studiedEntriesButton, SIGNAL(toggled(bool)), scoreBox, SLOT(setVisible(bool)));
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), studyBox, SLOT(setVisible(bool)));
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), trainBox, SLOT(setVisible(bool)));
		connect(studiedEntriesButton, SIGNAL(toggled(bool)), mistakeBox, SLOT(setVisible(bool)));*/

		QHBoxLayout *hLayout = new QHBoxLayout(entriesBox);
		hLayout->addWidget(new QLabel(tr("Study status:"), entriesBox));
		hLayout->addStretch();
		hLayout->addWidget(allEntriesButton);
		hLayout->addStretch();
		hLayout->addWidget(studiedEntriesButton);
		hLayout->addStretch();
		hLayout->addWidget(nonStudiedEntriesButton);
		hLayout->addStretch();
	}

	QHBoxLayout *hLayout = new QHBoxLayout();
	hLayout->addWidget(scoreBox);
	hLayout->addWidget(studyBox);
	hLayout->addWidget(trainBox);
	hLayout->addWidget(mistakeBox);
	hLayout->setContentsMargins(0, 0, 0, 0);
	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->addWidget(entriesBox);
	vLayout->addLayout(hLayout);
	vLayout->setContentsMargins(0, 0, 0, 0);
}

void StudyOptionsWidget::onRadioButtonToggled(bool toggled)
{
		if (toggled) commandUpdate();
}

QString StudyOptionsWidget::currentCommand() const
{
	QString ret;

	if (studiedEntriesButton->isChecked()) {
		QString minStudyDateString(_studyMinDate->dateString());
		QString maxStudyDateString(_studyMaxDate->dateString());
		ret += QString(" :study=\"%1\",\"%2\"").arg(minStudyDateString).arg(maxStudyDateString);
				
		if (minSpinBox->value() != 0 || maxSpinBox->value() != 100) {
			ret += QString(" :score=%1,%2").arg(minSpinBox->value()).arg(maxSpinBox->value());
		}

		QString minTrainDateString(_trainMinDate->dateString());
		QString maxTrainDateString(_trainMaxDate->dateString());
		if (!minTrainDateString.isEmpty() || !maxTrainDateString.isEmpty())
			ret += QString(" :lasttrained=\"%1\",\"%2\"").arg(minTrainDateString).arg(maxTrainDateString);

		QString minMistakenDateString(_mistakenMinDate->dateString());
		QString maxMistakenDateString(_mistakenMaxDate->dateString());
		if (!minMistakenDateString.isEmpty() || !maxMistakenDateString.isEmpty())
			ret += QString(" :mistaken=\"%1\",\"%2\"").arg(minMistakenDateString).arg(maxMistakenDateString);
	}
	else if (nonStudiedEntriesButton->isChecked()) ret += " :nostudy";

	return ret;
}

void StudyOptionsWidget::checkMinSpinBoxValue(int newMaxValue)
{
	if (newMaxValue < minSpinBox->value()) minSpinBox->setValue(newMaxValue);
}

void StudyOptionsWidget::checkMaxSpinBoxValue(int newMinValue)
{
	if (newMinValue > maxSpinBox->value()) maxSpinBox->setValue(newMinValue);
}

QString StudyOptionsWidget::currentTitle() const
{
	QString newTitle;
	if (studiedEntriesButton->isChecked()) {
		newTitle = tr("Studied");
		QString minStudyDateString = _studyMinDate->translatedDateString();
		if (!minStudyDateString.isEmpty()) {
			newTitle += tr(" since %1").arg(minStudyDateString);
		}
		QString maxStudyDateString = _studyMaxDate->translatedDateString();
		if (!maxStudyDateString.isEmpty()) {
			newTitle += tr(" until %1").arg(maxStudyDateString);
		}
		if (minSpinBox->value() != 0 || maxSpinBox->value() != 100) {
			newTitle += tr(", score:[%1,%2]").arg(minSpinBox->value()).arg(maxSpinBox->value());
		}

		QString minTrainDateString = _trainMinDate->translatedDateString();
		if (!minTrainDateString.isEmpty()) {
			newTitle += tr(", trained since %1").arg(minTrainDateString);
		}
		QString maxTrainDateString = _trainMaxDate->translatedDateString();
		if (!maxTrainDateString.isEmpty()) {
			if (minTrainDateString.isEmpty()) newTitle += tr(", trained until %1").arg(maxTrainDateString);
			else newTitle += tr(" until %1").arg(maxTrainDateString);
		}

		QString minMistakenDateString = _mistakenMinDate->translatedDateString();
		if (!minMistakenDateString.isEmpty()) {
			newTitle += tr(", mistaken since %1").arg(minMistakenDateString);
		}
		QString maxMistakenDateString = _mistakenMaxDate->translatedDateString();
		if (!maxMistakenDateString.isEmpty()) {
			if (minMistakenDateString.isEmpty()) newTitle += tr(", mistaken until %1").arg(maxMistakenDateString);
			else newTitle += tr(" until %1").arg(maxMistakenDateString);
		}
	}
	else if (nonStudiedEntriesButton->isChecked()) newTitle = tr("Not studied");
	else newTitle = tr("Study");

	return newTitle;
}

int StudyOptionsWidget::studyState() const
{
	if (studiedEntriesButton->isChecked()) return Studied;
	else if (nonStudiedEntriesButton->isChecked()) return NonStudied;
	else return All;
}

void StudyOptionsWidget::setStudyState(int state)
{
	switch (state) {
		case All:
			allEntriesButton->setChecked(true);
			break;
		case Studied:
			studiedEntriesButton->setChecked(true);
			break;
		case NonStudied:
			nonStudiedEntriesButton->setChecked(true);
			break;
	}
}

void StudyOptionsWidget::_reset()
{
	allEntriesButton->setChecked(true);
	minSpinBox->setValue(0);
	maxSpinBox->setValue(100);
	_studyMinDate->setDateString("");
	_studyMaxDate->setDateString("");
	_trainMinDate->setDateString("");
	_trainMaxDate->setDateString("");
	_mistakenMinDate->setDateString("");
	_mistakenMaxDate->setDateString("");
}

TagsSearchWidget::TagsSearchWidget(QWidget *parent) : SearchBarExtender(parent)
{
	_propsToSave << "tags";

	QGroupBox *groupBox = new QGroupBox(tr("Tags filter"), this);
	{
		QHBoxLayout *hLayout = new QHBoxLayout(groupBox);
		lineInput = new TagsLineInput(groupBox);
		hLayout->addWidget(lineInput);
		connect(lineInput, SIGNAL(allValidTags()), this, SLOT(commandUpdate()));

		tagsButton = new QPushButton(tr("..."), groupBox);
		QMenu *menu = new QMenu(tagsButton);
		connect(menu, SIGNAL(aboutToShow()), this, SLOT(populateTagsMenu()));
		tagsButton->setMenu(menu);
		hLayout->addWidget(tagsButton);
		connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(tagMenuClicked(QAction*)));
	}
	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->addWidget(groupBox);
	vLayout->setContentsMargins(0, 0, 0, 0);
}

void TagsSearchWidget::populateTagsMenu()
{
	QMenu *menu = tagsButton->menu();
	menu->clear();
	QStringList tags(lineInput->text().toLower().split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts));
	foreach(QString tag, Tag::knownTagsModel()->contents()) {
		QString tagStr = tag;
		QAction *action = menu->addAction(tag.replace(0, 1, tag[0].toUpper()));
		action->setProperty("TJtag", tagStr);
		action->setCheckable(true);
		if (tags.contains(tag.toLower())) action->setChecked(true);
	}
}

void TagsSearchWidget::focusInEvent(QFocusEvent *event)
{
	lineInput->setFocus(event->reason());
}

void TagsSearchWidget::tagMenuClicked(QAction *action)
{
	QStringList tags(lineInput->text().toLower().split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts));
	QString tag(action->property("TJtag").toString());
	if (!tags.contains(tag)) {
		QString str(lineInput->text());
		if (str.size() > 0 && !str.endsWith(' ')) str += " ";
		lineInput->setText(str + tag + " ");
	}
	else {
		QRegExp regexp(QString("\\b%1\\b").arg(tag));
		QString text(lineInput->text());
		text.remove(regexp);
		text.remove(QRegExp("^ *"));
		text.replace(QRegExp("  +"), " ");
		regexp.setCaseSensitivity(Qt::CaseInsensitive);
		lineInput->setText(text);
	}
}

void TagsSearchWidget::setTags(const QString &tags)
{
	QString rTags(tags);
	if (tags.size() && tags[tags.size() - 1] != ' ') rTags += " ";
	lineInput->setText(rTags);
}

QString TagsSearchWidget::currentTitle() const
{
	QStringList tags(lineInput->text().split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts));
	if (tags.isEmpty()) return tr("Tags");
	else return tr("Tagged %1").arg(tags.join(","));
}

QString TagsSearchWidget::currentCommand() const
{
	QStringList tags(lineInput->text().split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts));
	if (tags.isEmpty()) return "";
	return QString(":tag%1%2").arg(tags.isEmpty() ? "" : "=").arg(tags.join(","));
}

void TagsSearchWidget::_reset()
{
	lineInput->clear();
}

NotesSearchWidget::NotesSearchWidget(QWidget *parent) : SearchBarExtender(parent)
{
	_propsToSave << "notes";

	QGroupBox *groupBox = new QGroupBox(tr("Notes filter"), this);
	{
		QHBoxLayout *hLayout = new QHBoxLayout(groupBox);
		words = new QLineEdit(groupBox);
		connect(words, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
		connect(words, SIGNAL(returnPressed()), this, SLOT(commandUpdate()));
		hLayout->addWidget(words);

	}
	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->addWidget(groupBox);
	vLayout->setContentsMargins(0, 0, 0, 0);
}

void NotesSearchWidget::focusInEvent(QFocusEvent *event)
{
	words->setFocus(event->reason());
}

void NotesSearchWidget::setNotes(const QString &notes)
{
	QString rNotes(notes);
	if (notes.size() && notes[notes.size() - 1] != ' ') rNotes += " ";
	words->setText(rNotes);
}

QString NotesSearchWidget::currentTitle() const
{
	if (words->text().isEmpty()) return tr("Notes");
	QStringList args(words->text().split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts));
	if (args.size() == 1 && args[0] == "*") return tr("Has note");
	return tr("Note contains %1").arg(words->text());
}

QString NotesSearchWidget::currentCommand() const
{
	if (words->text().isEmpty()) return "";
	QStringList args(words->text().split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts));
	if (args.size() == 1 && args[0] == "*") return ":note";
	return QString(":note=%1").arg(args.join(","));
}

void NotesSearchWidget::onTextChanged(const QString &newText)
{
	if (newText.isEmpty() || QRegExp("^.*[ ,\\.\\*]$").exactMatch(newText)) commandUpdate();
}

void NotesSearchWidget::_reset()
{
	words->clear();
}

PreferenceItem<int> SearchBar::searchBarHistorySize("mainWindow", "searchBarHistorySize", 100);

SearchBar::SearchBar(QWidget *parent) : QWidget(parent)
{
	_searchField = new QComboBox(this);
	_searchField->setMinimumWidth(150);
	_searchField->setEditable(true);
	_searchField->setMaxCount(searchBarHistorySize.value());
	_searchField->setSizePolicy(QSizePolicy::Expanding, _searchField->sizePolicy().verticalPolicy());
	_searchField->setInsertPolicy(QComboBox::NoInsert);

	_entryTypeSelector = new EntryTypeSelectionWidget(this);
	connect(_entryTypeSelector, SIGNAL(enableFeature(const QString &)), this, SLOT(enableFeature(const QString &)));
	connect(_entryTypeSelector, SIGNAL(disableFeature(const QString &)), this, SLOT(disableFeature(const QString &)));

	searchButton = new QPushButton(tr("Search"), this);
	searchButton->setIcon(QIcon(":/images/icons/ldap_lookup.png"));
	searchButton->setShortcut(QKeySequence("Ctrl+Return"));

	resetText = new QToolButton(this);
	resetText->setIcon(QIcon(":/images/icons/reset-search.png"));
	resetText->setToolTip(tr("Clear search text"));
	resetText->setEnabled(false);
	connect(resetText, SIGNAL(clicked()), this, SLOT(resetSearchText()));

	connect(_entryTypeSelector, SIGNAL(commandUpdated()),searchButton, SLOT(click()));

	// The animation shall not be bigger than the size of the button
	int searchAnimSize = searchButton->size().height();
	searchAnim = new QMovie(":/images/search.gif", "gif", this);
	// Only reduce the animation size - do not increase it
	if (searchAnimSize < 35) searchAnim->setScaledSize(QSize(searchAnimSize, searchAnimSize));
	searchAnim->jumpToFrame(0);
	searchStatus = new ClickableLabel(this);
	searchStatus->setMovie(searchAnim);

	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->setSpacing(0);
	vLayout->setContentsMargins(0, 0, 0, 0);
	_searchBarLayout = new QHBoxLayout();
	_searchBarLayout->setContentsMargins(0, 0, 0, 0);
	_searchBarLayout->setSpacing(5);
	_searchBarLayout->addWidget(resetText);
	_searchBarLayout->addWidget(_searchField);
	_searchBarLayout->addWidget(_entryTypeSelector);
	_searchBarLayout->addWidget(searchButton);
	_searchBarLayout->addWidget(searchStatus);
	vLayout->addLayout(_searchBarLayout);

	_extenders = new MultiStackedWidget(this);
	vLayout->addWidget(_extenders);

	connect(searchButton, SIGNAL(clicked()), this, SLOT(searchButtonClicked()));
	connect(searchStatus, SIGNAL(clicked()), this, SIGNAL(stopSearch()));
	connect(_searchField->lineEdit(), SIGNAL(returnPressed()), searchButton, SLOT(click()));
	connect(_searchField->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(onSearchTextChanged(QString)));
//	connect(_searchField, SIGNAL(currentIndexChanged(int)), searchButton, SLOT(click()));

	_extendersList[_entryTypeSelector->name()] = _entryTypeSelector;
	registerExtender(new StudyOptionsWidget(this));
	registerExtender(new TagsSearchWidget(this));
	registerExtender(new NotesSearchWidget(this));
	registerExtender(new JLPTOptionsWidget(this));
}

void SearchBar::onComponentSearchKanjiSelected(const QString &text)
{
	_searchField->lineEdit()->insert(text);
}

void SearchBar::onSearchTextChanged(const QString &text)
{
	resetText->setEnabled(!text.isEmpty());
}

void SearchBar::resetSearchText()
{
	if (_searchField->lineEdit()->text().isEmpty()) return;
	_searchField->clearEditText();
	searchButtonClicked();
}

void SearchBar::enableFeature(const QString &feature)
{
	if (!_disabledFeatures.contains(feature)) return;

	SearchBarExtender *source = qobject_cast<SearchBarExtender *>(sender());
	QSet<SearchBarExtender *> &set = _disabledFeatures[feature];
	set.remove(source);
	bool shallEnable = true;
	// Check if any of the feature disablers is active - if not we can enable
	// the feature
	foreach (SearchBarExtender *extender, set) if (extender->isEnabled()) { shallEnable = false; break; }
	if (shallEnable) {
		_disabledFeatures.remove(feature);
		foreach (SearchBarExtender *extender, _extendersList.values()) {
			if (extender->feature() == feature) {
					_extenders->setWidgetEnabled(extender, true);
					extender->updateFeatures();
			}
		}
	}
}

void SearchBar::disableFeature(const QString &feature)
{
	if (_disabledFeatures.contains(feature)) _disabledFeatures[feature] = QSet<SearchBarExtender *>();
	QSet<SearchBarExtender *> &set = _disabledFeatures[feature];
	SearchBarExtender *source = qobject_cast<SearchBarExtender *>(sender());
	if (set.isEmpty()) {
			foreach (SearchBarExtender *extender, _extendersList.values()) {
				if (extender->feature() == feature) _extenders->setWidgetEnabled(extender, false);
		}
	}
	set << source;
}

QString SearchBar::text() const
{
	QString ret;
	foreach(SearchBarExtender *extender, _extendersList.values()) {
		if (extender->isEnabled()) ret += extender->currentCommand();
	}
	QString sLineText(_searchField->lineEdit()->text());
	if (!sLineText.isEmpty()) ret += " " + sLineText;
	return ret;
}

void SearchBar::searchButtonClicked()
{
	if (!_searchField->lineEdit()->text().isEmpty() && _searchField->itemText(0) != _searchField->lineEdit()->text()) {
		_searchField->insertItem(0, _searchField->lineEdit()->text());
		_searchField->setCurrentIndex(0);
	}
	emit startSearch(text());
	//stealFocus();
}

void SearchBar::reset()
{
	emit stopSearch();
	foreach (SearchBarExtender *extender, _extendersList.values()) {
		extender->setAutoUpdateQuery(false);
		extender->reset();
		extender->setAutoUpdateQuery(true);
	}
	_searchField->clearEditText();
	_entryTypeSelector->setAutoUpdateQuery(false);
	_entryTypeSelector->reset();
	_entryTypeSelector->setAutoUpdateQuery(true);
	searchButtonClicked();
}

void SearchBar::searchStarted()
{
	searchAnim->start();
}

void SearchBar::searchCompleted()
{
	searchAnim->stop();
	searchAnim->jumpToFrame(0);
}

void SearchBar::stealFocus()
{
	_searchField->setFocus();
	_searchField->lineEdit()->selectAll();
}

void SearchBar::search()
{
	searchButton->click();
}

void SearchBar::searchVocabulary()
{
	_entryTypeSelector->setType(EntryTypeSelectionWidget::Vocabulary);
}

void SearchBar::searchKanjis()
{
	_entryTypeSelector->setType(EntryTypeSelectionWidget::Kanjis);
}

void SearchBar::searchAll()
{
	_entryTypeSelector->setType(EntryTypeSelectionWidget::All);
}

void SearchBar::registerExtender(SearchBarExtender *extender)
{
	_extendersList[extender->name()] = extender;
	_extenders->addWidget(extender->currentTitle(), extender);
	connect(extender, SIGNAL(commandUpdated()), searchButton, SLOT(click()));
	connect(extender, SIGNAL(updateTitle(const QString &)), _extenders, SLOT(onTitleChanged(const QString &)));

	connect(extender, SIGNAL(enableFeature(const QString &)), this, SLOT(enableFeature(const QString &)));
	connect(extender, SIGNAL(disableFeature(const QString &)), this, SLOT(disableFeature(const QString &)));
}

void SearchBar::removeExtender(SearchBarExtender *extender)
{
	disconnect(extender, SIGNAL(commandUpdated()), searchButton, SLOT(click()));
	_extenders->removeWidget(extender);
	_extendersList.remove(_extendersList.key(extender));
}

QMap<QString, QVariant> SearchBar::getState() const
{
	QMap<QString, QVariant> ret;
	ret["searchtext"] = _searchField->lineEdit()->text();
	foreach (SearchBarExtender *extender, _extendersList.values()) {
		QMap<QString, QVariant> state = extender->getState();
		ret[extender->name()] = state;
	}

	return ret;
}

void SearchBar::restoreState(const QMap<QString, QVariant> &state)
{
	QString searchText(state.value("searchtext").toString());
	if (!searchText.isEmpty()) _searchField->lineEdit()->setText(searchText);
	else _searchField->clearEditText();
	foreach (SearchBarExtender *extender, _extendersList.values()) {
		extender->setAutoUpdateQuery(false);
		extender->reset();
		if (state.contains(extender->name())) {
			extender->restoreState(state.value(extender->name()).toMap());
		}
		extender->setAutoUpdateQuery(true);
	}
}
