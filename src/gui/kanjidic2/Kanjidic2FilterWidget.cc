/*
 *  Copyright (C) 2010  Alexandre Courbot
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

#include "gui/TJSpinBox.h"
#include "gui/TJLineEdit.h"
#include "gui/KanjiValidator.h"
#include "gui/MainWindow.h"
#include "gui/kanjidic2/Kanjidic2FilterWidget.h"
#include "gui/kanjidic2/Kanjidic2GUIPlugin.h"

#include <QGroupBox>

static void prepareFourCornerComboBox(QComboBox *box)
{
	box->addItem("");
	box->addItem(QString::fromUtf8("亠"));
	box->addItem(QString::fromUtf8("一"));
	box->addItem(QString::fromUtf8("｜"));
	box->addItem(QString::fromUtf8("丶"));
	box->addItem(QString::fromUtf8("十"));
	box->addItem(QString::fromUtf8("キ"));
	box->addItem(QString::fromUtf8("口"));
	box->addItem(QString::fromUtf8("厂"));
	box->addItem(QString::fromUtf8("八"));
	box->addItem(QString::fromUtf8("小"));
}

Kanjidic2FilterWidget::Kanjidic2FilterWidget(QWidget *parent) : SearchFilterWidget(parent, "kanjidic")
{
	_propsToSave << "strokeCount" << "isStrokeRange" << "maxStrokeCount" << "radicals" << "components" << "unicode" << "skip" << "fourCorner" << "grades";

	QGroupBox *_strokeCountGroupBox = new QGroupBox(tr("Stroke count"), this);
	connect(_strokeCountGroupBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	{
		QVBoxLayout *vLayout = new QVBoxLayout(_strokeCountGroupBox);
		QHBoxLayout *hLayout = new QHBoxLayout();
		hLayout->setContentsMargins(0, 0, 0, 0);
		_strokeCountSpinBox = new TJSpinBox(this);
		_strokeCountSpinBox->setSpecialValueText("");
		_strokeCountSpinBox->setRange(0, 34);
		hLayout->addWidget(_strokeCountSpinBox);
		connect(_strokeCountSpinBox, SIGNAL(valueChanged(QString)), this, SLOT(onStrokeRangeChanged()));
		_maxStrokeCountSpinBox = new TJSpinBox(this);
		_maxStrokeCountSpinBox->setSpecialValueText("");
		_maxStrokeCountSpinBox->setRange(0, 34);
		_maxStrokeCountSpinBox->setVisible(false);
		_maxStrokeCountSpinBox->setEnabled(false);
		hLayout->addWidget(_maxStrokeCountSpinBox);
		connect(_maxStrokeCountSpinBox, SIGNAL(valueChanged(QString)), this, SLOT(onStrokeRangeChanged()));
		_rangeCheckBox = new QCheckBox(tr("Range"), _strokeCountGroupBox);
		vLayout->addLayout(hLayout);
		vLayout->addWidget(_rangeCheckBox);
		connect(_rangeCheckBox, SIGNAL(toggled(bool)), this, SLOT(onStrokeRangeToggled(bool)));
	}
	QGroupBox *componentsGroupBox = new QGroupBox(this);
	{
		QVBoxLayout *vLayout = new QVBoxLayout(componentsGroupBox);
		QLabel *label;
		label = new QLabel(tr("Radicals"), componentsGroupBox);
		label->setAlignment(Qt::AlignHCenter);
		vLayout->addWidget(label);
		_radicals = new TJLineEdit(componentsGroupBox);
		connect(_radicals, SIGNAL(textChanged(const QString &)), this, SLOT(commandUpdate()));
		// _radKSelector will be allocated the first time it is used
		_radKSelector = 0;
		_radicals->installEventFilter(this);
		vLayout->addWidget(_radicals);
		label = new QLabel(tr("Components"), componentsGroupBox);
		label->setAlignment(Qt::AlignHCenter);
		vLayout->addWidget(label);
		_components = new TJLineEdit(componentsGroupBox);
		KanjiValidator *kanjiValidator = new KanjiValidator(_components);
		_components->setValidator(kanjiValidator);
		connect(_components, SIGNAL(textChanged(const QString &)), this, SLOT(commandUpdate()));
		// _compKSelector will be allocated the first time it is used
		_compKSelector = 0;
		_components->installEventFilter(this);
		vLayout->addWidget(_components);
	}
	QGroupBox *unicodeGroupBox = new QGroupBox(tr("Unicode"), this);
	{
		QHBoxLayout *hLayout = new QHBoxLayout(unicodeGroupBox);
		_unicode = new TJSpinBox(unicodeGroupBox, "[0-9a-fA-f]{0,6}", 16);
		_unicode->setRange(0, 0x2A6DF);
		_unicode->setPrefix("0x");
		connect(_unicode, SIGNAL(valueChanged(int)), this, SLOT(delayedCommandUpdate()));
		hLayout->addWidget(_unicode);
	}
	QGroupBox *skipGroupBox = new QGroupBox(tr("SKIP code"), this);
	{
		QHBoxLayout *hLayout = new QHBoxLayout(skipGroupBox);
		_skip1 = new TJSpinBox(unicodeGroupBox);
		_skip1->setRange(0, 4);
		_skip1->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
		hLayout->addWidget(_skip1);
		connect(_skip1, SIGNAL(valueChanged(int)), this, SLOT(delayedCommandUpdate()));
		_skip2 = new TJSpinBox(unicodeGroupBox);
		_skip2->setRange(0, 30);
		_skip2->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
		hLayout->addWidget(_skip2);
		connect(_skip2, SIGNAL(valueChanged(int)), this, SLOT(delayedCommandUpdate()));
		_skip3 = new TJSpinBox(unicodeGroupBox);
		_skip3->setRange(0, 30);
		_skip3->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
		hLayout->addWidget(_skip3);
		connect(_skip3, SIGNAL(valueChanged(int)), this, SLOT(delayedCommandUpdate()));
	}
	QGroupBox *fourCornerGroupBox = new QGroupBox(tr("Four corner"), this);
	{
		QHBoxLayout *hLayout = new QHBoxLayout(fourCornerGroupBox);
		QGridLayout *gLayout = new QGridLayout();
		hLayout->addLayout(gLayout);
		_fcTopLeft = new QComboBox(fourCornerGroupBox);
		prepareFourCornerComboBox(_fcTopLeft);
		connect(_fcTopLeft, SIGNAL(currentIndexChanged(int)), this, SLOT(delayedCommandUpdate()));
		gLayout->addWidget(_fcTopLeft, 0, 0);
		_fcTopRight = new QComboBox(fourCornerGroupBox);
		prepareFourCornerComboBox(_fcTopRight);
		connect(_fcTopRight, SIGNAL(currentIndexChanged(int)), this, SLOT(delayedCommandUpdate()));
		gLayout->addWidget(_fcTopRight, 0, 1);
		_fcBotLeft = new QComboBox(fourCornerGroupBox);
		prepareFourCornerComboBox(_fcBotLeft);
		connect(_fcBotLeft, SIGNAL(currentIndexChanged(int)), this, SLOT(delayedCommandUpdate()));
		gLayout->addWidget(_fcBotLeft, 1, 0);
		_fcBotRight = new QComboBox(fourCornerGroupBox);
		prepareFourCornerComboBox(_fcBotRight);
		connect(_fcBotRight, SIGNAL(currentIndexChanged(int)), this, SLOT(delayedCommandUpdate()));
		gLayout->addWidget(_fcBotRight, 1, 1);
		_fcExtra = new QComboBox(fourCornerGroupBox);
		prepareFourCornerComboBox(_fcExtra);
		connect(_fcExtra, SIGNAL(currentIndexChanged(int)), this, SLOT(delayedCommandUpdate()));
		hLayout->addWidget(_fcExtra);
	}
	QGroupBox *gradeGroupBox = new QGroupBox(tr("School grade"), this);
	{
		QHBoxLayout *hLayout = new QHBoxLayout(gradeGroupBox);
		_gradeButton = new QPushButton(tr("Grade"), gradeGroupBox);
		QMenu *menu = new QMenu(_gradeButton);
		QAction *action;
		actionGroup = new QActionGroup(menu);
		actionGroup->setExclusive(false);
		for (int i = 1 ; i <= 10; i++) {
			if (i == 7) continue;
			action = actionGroup->addAction(QCoreApplication::translate("Kanjidic2GUIPlugin", Kanjidic2GUIPlugin::kanjiGrades[i].toLatin1()));
			action->setProperty("Agrade", i);
			action->setCheckable(true);
		}
		connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onGradeTriggered(QAction *)));
		menu->addActions(actionGroup->actions());
		menu->addSeparator();
		allKyouku = menu->addAction(tr("All &Kyouiku kanji"));
		allKyouku->setCheckable(true);
		connect(allKyouku, SIGNAL(toggled(bool)), this, SLOT(allKyoukuKanjis(bool)));
		allJouyou = menu->addAction(tr("All &Jouyou kanji"));
		allJouyou->setCheckable(true);
		connect(allJouyou, SIGNAL(toggled(bool)), this, SLOT(allJouyouKanjis(bool)));

		_gradeButton->setMenu(menu);
		hLayout->addWidget(_gradeButton);
	}
	
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(_strokeCountGroupBox);
	mainLayout->addWidget(componentsGroupBox);
	mainLayout->addWidget(unicodeGroupBox);
	mainLayout->addWidget(skipGroupBox);
	mainLayout->addWidget(fourCornerGroupBox);
	mainLayout->addWidget(gradeGroupBox);
	mainLayout->setContentsMargins(0, 0, 0, 0);
}

void showSelector(QWidget *selector, QWidget *focusWidget)
{
	selector->move(focusWidget->mapToGlobal(QPoint(focusWidget->rect().left() + (focusWidget->rect().width() - selector->rect().width()) / 2, focusWidget->rect().bottom())));	
	MainWindow::fitToScreen(selector);
	selector->show();
}

bool Kanjidic2FilterWidget::eventFilter(QObject *watched, QEvent *event)
{
	if ((watched == _radicals || watched == _components)) {
		QLineEdit *focusWidget = qobject_cast<QLineEdit *>(watched);
		switch (event->type()) {
			// Automatically popup the selector on focus
			case QEvent::FocusIn:
			{
				KanjiSelector *selector = 0;
				bool justCreated = false;
				// Create the selector if this is the first time we use it
				if (focusWidget == _radicals && !_radKSelector) {
					_radKSelector = new RadicalKanjiSelector(_radicals);
					KanjiSelectorValidator *selectorValidator = new KanjiSelectorValidator(_radKSelector, _radicals);
					_radicals->setValidator(selectorValidator);
					justCreated = true;
				} else if (focusWidget == _components && !_compKSelector) {
					_compKSelector = new ComponentKanjiSelector(_components);
					justCreated = true;
				}
				if (focusWidget == _radicals) selector = _radKSelector;
				else if (focusWidget == _components) selector = _compKSelector;
				// If the selector has been created, setup its properties
				if (justCreated) {
					selector->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
					selector->setAttribute(Qt::WA_ShowWithoutActivating);
					selector->setWindowModality(Qt::NonModal);
					selector->setFocusPolicy(Qt::NoFocus);
					selector->installEventFilter(this);
					selector->associateTo(focusWidget);
					selector->reset();
				}
				showSelector(selector, focusWidget);
				break;
			}
			// Automatically hide the selector on focus out
			case QEvent::FocusOut:
				if (focusWidget == _radicals && !(QApplication::widgetAt(QCursor::pos()) != _radKSelector)) _radKSelector->hide();
				else if (focusWidget == _components && !(QApplication::widgetAt(QCursor::pos()) != _compKSelector)) _compKSelector->hide();
				break;
			// Show the selector is the line edit has focus
			case QEvent::Enter:
				if (focusWidget == _radicals && _radicals->hasFocus()) showSelector(_radKSelector, _radicals);
				else if (focusWidget == _components && _components->hasFocus()) showSelector(_compKSelector, _components);
				break;
			// Hide the selector when leaving for something else than the selector
			case QEvent::Leave:
				if (focusWidget == _radicals && _radicals->hasFocus() && QApplication::widgetAt(QCursor::pos()) != _radKSelector) _radKSelector->hide();
				else if (focusWidget == _components && _components->hasFocus() && QApplication::widgetAt(QCursor::pos()) != _compKSelector) _compKSelector->hide();
				break;
			// If ESC is pressed while a radical/component line edit has focus, hide its selector
			case QEvent::KeyPress:
			{
				int key = static_cast<QKeyEvent *>(event)->key();
				if (key != Qt::Key_Escape) break;
				if (focusWidget == _radicals) _radKSelector->hide();
				else if (focusWidget == _components) _compKSelector->hide();
				break;
			}
			default:
				break;
		}
	}
	else if (watched == _radKSelector || watched == _compKSelector) {
		switch (event->type()) {
			// If the mouse leaves a selector for something else than its associated line edit, hide it
			case QEvent::Leave:
				if (watched == _radKSelector && QApplication::widgetAt(QCursor::pos()) != _radicals) _radKSelector->hide();
				else if (watched == _compKSelector && QApplication::widgetAt(QCursor::pos()) != _components) _compKSelector->hide();
				break;
			default:
				break;
		}
	}
	return SearchFilterWidget::eventFilter(watched, event);
}

void Kanjidic2FilterWidget::onStrokeRangeToggled(bool checked)
{
	// We will need to update the query if we uncheck the strokes checkbox and maxStrokes != minStrokes != 0.
	bool needUpdate = (!checked && _maxStrokeCountSpinBox->value() != 0 && _maxStrokeCountSpinBox->value() != _strokeCountSpinBox->value());
	_maxStrokeCountSpinBox->setVisible(checked);
	_maxStrokeCountSpinBox->setEnabled(checked);
	// Do not allow the command update to be called as the state did not really change
	_maxStrokeCountSpinBox->blockSignals(true);
	_maxStrokeCountSpinBox->setValue(checked ? _strokeCountSpinBox->value() : 0);
	_maxStrokeCountSpinBox->blockSignals(false);
	if (needUpdate) commandUpdate();
}

void Kanjidic2FilterWidget::onStrokeRangeChanged()
{
	if (_maxStrokeCountSpinBox->isEnabled()) {
		int minValue = _strokeCountSpinBox->value();
		int maxValue = _maxStrokeCountSpinBox->value();
		if (sender() == _strokeCountSpinBox && minValue > maxValue) _maxStrokeCountSpinBox->setValue(minValue);
		else if (sender() == _maxStrokeCountSpinBox && maxValue < minValue) _strokeCountSpinBox->setValue(maxValue);
	}
	delayedCommandUpdate();
}

void Kanjidic2FilterWidget::allKyoukuKanjis(bool checked)
{
	bool prevStatus = allJouyou->blockSignals(true);
	allJouyou->setChecked(false);
	allJouyou->blockSignals(prevStatus);
	_gradesList.clear();
	foreach(QAction *action, actionGroup->actions()) {
		int grade = action->property("Agrade").toInt();
		if (grade <= 6) {
			action->setChecked(checked);
			if (checked) _gradesList << QString::number(grade);
		}
		else action->setChecked(false);
	}
	actionGroup->setEnabled(!checked);
	commandUpdate();
}

void Kanjidic2FilterWidget::allJouyouKanjis(bool checked)
{
	bool prevStatus = allKyouku->blockSignals(true);
	allKyouku->setChecked(false);
	allKyouku->blockSignals(prevStatus);
	_gradesList.clear();
	foreach(QAction *action, actionGroup->actions()) {
		int grade = action->property("Agrade").toInt();
		if (grade <= 8) {
			action->setChecked(checked);
			if (checked) _gradesList << QString::number(grade);
		}
		else action->setChecked(false);
	}
	actionGroup->setEnabled(!checked);
	commandUpdate();
}

QString Kanjidic2FilterWidget::currentCommand() const
{
	QString ret;

	int minStrokes = _strokeCountSpinBox->value();
	int maxStrokes = _maxStrokeCountSpinBox->value();
	if ((!_maxStrokeCountSpinBox->isEnabled() || maxStrokes == 0 || maxStrokes == minStrokes) && minStrokes > 0) {
		ret += QString(" :stroke=%1").arg(minStrokes);
	}
	else if (minStrokes != 0 && maxStrokes != 0) {
		ret += QString(" :stroke=%1,%2").arg(minStrokes).arg(maxStrokes);
	}
	QString kanjis = _radicals->text();
	if (!kanjis.isEmpty()) {
		bool first = true;
		ret += " :radical=";
		foreach(QChar c, kanjis) {
			if (!first) ret +=",";
			else first = false;
			ret += QString("\"%1\"").arg(c);
		}
	}
	kanjis = _components->text();
	if (!kanjis.isEmpty()) {
		bool first = true;
		ret += " :component=";
		foreach(QChar c, kanjis) {
			if (!first) ret +=",";
			else first = false;
			ret += QString("\"%1\"").arg(c);
		}
	}
	if (_unicode->value()) ret += QString(" :unicode=%1").arg(_unicode->text());
	if (_skip1->value() || _skip2->value() || _skip3->value()) ret += QString(" :skip=%1").arg(skip());
	if (_fcTopLeft->currentIndex() > 0 || _fcTopRight->currentIndex() > 0 || _fcBotLeft->currentIndex() > 0 || _fcBotRight->currentIndex() > 0 || _fcExtra->currentIndex() > 0) ret += QString(" :fourcorner=%1").arg(fourCorner());
	if (!_gradesList.isEmpty()) ret += " :grade=" + _gradesList.join(",");
	return ret;
}

QString Kanjidic2FilterWidget::currentTitle() const
{
	QString ret;

	QString kanjis = _radicals->text() + _components->text();
	if (!kanjis.isEmpty()) {
		bool first = true;
		ret += "[";
		foreach(QChar c, kanjis) {
			if (!first) ret +=",";
			else first = false;
			ret += c;
		}
		ret += "]";
	}
	int minStrokes = _strokeCountSpinBox->value();
	int maxStrokes = _maxStrokeCountSpinBox->value();
	if ((!_maxStrokeCountSpinBox->isEnabled() || maxStrokes == 0 || maxStrokes == minStrokes) && minStrokes > 0) {
		ret += tr(", %1 strokes").arg(minStrokes);
	} else if (minStrokes != 0 && maxStrokes != 0) {
		if (minStrokes == 0) ret += tr(", strokes<=%1").arg(maxStrokes);
		else if (maxStrokes == 0) ret += tr(", strokes>=%1").arg(minStrokes);
		else ret += tr(", %1-%2 strokes").arg(minStrokes).arg(maxStrokes);
	}
	if (_unicode->value()) ret += tr(", unicode: %1").arg(_unicode->text());
	if (_skip1->value() || _skip2->value() || _skip3->value()) ret += tr(", skip: %1").arg(skip());
	if (_fcTopLeft->currentIndex() > 0 || _fcTopRight->currentIndex() > 0 || _fcBotLeft->currentIndex() > 0 || _fcBotRight->currentIndex() > 0 || _fcExtra->currentIndex() > 0) ret += tr(", 4c: %1").arg(fourCorner());
	if (!_gradesList.isEmpty()) ret += tr(", grade: %1").arg(_gradesList.join(","));
	if (!ret.isEmpty()) ret = tr("Kanji") + ret;
	else ret = tr("Kanji");
	return ret;
}

void Kanjidic2FilterWidget::onGradeTriggered(QAction *action)
{
	if (action->isChecked()) {
		int grade = action->property("Agrade").toInt();
		_gradesList << QString::number(grade);
	} else {
		int grade = action->property("Agrade").toInt();
		_gradesList.removeOne(QString::number(grade));
	}
	if (!_gradesList.isEmpty()) _gradeButton->setText(tr("Grade:") + _gradesList.join(","));
	else _gradeButton->setText(tr("Grade"));
	commandUpdate();
}

void Kanjidic2FilterWidget::updateFeatures()
{
	if (!currentCommand().isEmpty()) emit disableFeature("wordsdic");
	else emit enableFeature("wordsdic");
}

void Kanjidic2FilterWidget::_reset()
{
	_strokeCountSpinBox->setValue(0);
	_maxStrokeCountSpinBox->setValue(0);
	_rangeCheckBox->setChecked(false);
	_unicode->setValue(0);
	_skip1->setValue(0);
	_skip2->setValue(0);
	_skip3->setValue(0);
	_fcTopLeft->setCurrentIndex(0);
	_fcTopRight->setCurrentIndex(0);
	_fcBotLeft->setCurrentIndex(0);
	_fcBotRight->setCurrentIndex(0);
	_fcExtra->setCurrentIndex(0);
	_gradesList.clear();
	foreach (QAction *action, _gradeButton->menu()->actions()) if (action->isChecked()) action->trigger();
	_radicals->clear();
	_components->clear();
}

void Kanjidic2FilterWidget::setGrades(const QStringList &list)
{
	_gradesList.clear();
	foreach(QAction *action, _gradeButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(action->property("Agrade").toString())) action->trigger();
	}
}

QString Kanjidic2FilterWidget::skip() const
{
	return QString("%1-%2-%3").arg(_skip1->value() ? QString::number(_skip1->value()) : "?").arg(_skip2->value() ? QString::number(_skip2->value()) : "?").arg(_skip3->value() ? QString::number(_skip3->value()) : "?");
}

void Kanjidic2FilterWidget::setSkip(const QString &value)
{
	QStringList skipList(value.split('-'));
	if (skipList.size() != 3) return;
	bool ok;
	int t, c1, c2;
	t = skipList[0].toInt(&ok); if (!ok && skipList[0] != "?") return;
	c1 = skipList[1].toInt(&ok); if (!ok && skipList[1] != "?") return;
	c2 = skipList[2].toInt(&ok); if (!ok && skipList[2] != "?") return;
	_skip1->setValue(t);
	_skip2->setValue(c1);
	_skip3->setValue(c2);
}

QString Kanjidic2FilterWidget::fourCorner() const
{
	return QString("%1%2%3%4.%5").arg(_fcTopLeft->currentIndex() > 0 ? QString::number(_fcTopLeft->currentIndex() - 1) : "?")
		.arg(_fcTopRight->currentIndex() > 0 ? QString::number(_fcTopRight->currentIndex() - 1) : "?")
		.arg(_fcBotLeft->currentIndex() > 0 ? QString::number(_fcBotLeft->currentIndex() - 1) : "?")
		.arg(_fcBotRight->currentIndex() > 0 ? QString::number(_fcBotRight->currentIndex() - 1) : "?")
		.arg(_fcExtra->currentIndex() > 0 ? QString::number(_fcExtra->currentIndex() - 1) : "?");
}

void Kanjidic2FilterWidget::setFourCorner(const QString &value)
{
	// Sanity check
	if (value.size() != 6 || value[4] != '.') return;
	
	int topLeft, topRight, botLeft, botRight, extra;
	topLeft = value[0].isDigit() ? value[0].toLatin1() - '0' : -1;
	topRight = value[1].isDigit() ? value[1].toLatin1() - '0' : -1;
	botLeft = value[2].isDigit() ? value[2].toLatin1() - '0' : -1;
	botRight = value[3].isDigit() ? value[3].toLatin1() - '0' : -1;
	extra = value[5].isDigit() ? value[5].toLatin1() - '0' : -1;
	_fcTopLeft->setCurrentIndex(topLeft + 1);
	_fcTopRight->setCurrentIndex(topRight + 1);
	_fcBotLeft->setCurrentIndex(botLeft + 1);
	_fcBotRight->setCurrentIndex(botRight + 1);
	_fcExtra->setCurrentIndex(extra + 1);
}
