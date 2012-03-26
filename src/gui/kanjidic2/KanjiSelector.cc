/*
 *  Copyright (C) 2010 Alexandre Courbot
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

#include <QtDebug>

#include "sqlite/Query.h"
#include "core/TextTools.h"
#include "core/kanjidic2/KanjiRadicals.h"
#include "core/Database.h"
#include "core/kanjidic2/Kanjidic2Entry.h"
#include "gui/KanjiValidator.h"
#include "gui/kanjidic2/KanjiSelector.h"
#include "gui/MainWindow.h"
#include "gui/TJLineEdit.h"
#include "gui/kanjidic2/Kanjidic2EntryFormatter.h"

#include <QComboBox>
#include <QDesktopWidget>

ComplementsList::ComplementsList(QWidget *parent) : QListWidget(parent), baseFont(font()), labelFont(baseFont), _sscroll(verticalScrollBar())
{
	// Setup the fonts and size of the grid
	baseFont.setPointSize(baseFont.pointSize() + 2);
	setFont(baseFont);
	setupGridSize();
	setMouseTracking(true);
	connect(this, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(onItemEntered(QListWidgetItem*)));
}

void ComplementsList::setupGridSize()
{
	QFontMetrics fm(baseFont);
	labelFont.setBold(true);
	QFontMetrics lfm(labelFont);
	int maxFontSize = qMax(fm.maxWidth(), fm.height()) + 2;
	int maxBoldSize = 0;
	for (int i = 0; i < 10; i++) {
		int w = lfm.width(QString::number(i));
		if (w > maxBoldSize) maxBoldSize = w;
	}
	maxBoldSize += qMax(lfm.width("0"), lfm.width("1"));
	int gridSize = qMax(maxBoldSize, maxFontSize) + 5;
	setGridSize(QSize(gridSize, gridSize));
}

QSet<uint> ComplementsList::currentSelection() const
{
	QSet<uint> ret;
	QList<QListWidgetItem *> selection(selectedItems());
	foreach (const QListWidgetItem *item, selection)
		ret << item->data(Qt::UserRole).toInt();
	return ret;
}

QListWidgetItem *ComplementsList::addComplement(const QString &repr, uint kanji)
{
	QListWidgetItem *item = new QListWidgetItem(repr, this);
	item->setData(Qt::UserRole, kanji);
	return item;
}

QListWidgetItem *ComplementsList::setCurrentStrokeNbr(int strokeNbr)
{
	QListWidgetItem *item = new QListWidgetItem(QString::number(strokeNbr), this);
	item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
	item->setBackground(Qt::yellow);
	item->setFont(labelFont);
	return item;
}

void ComplementsList::onItemEntered(QListWidgetItem *item)
{
	// Disable tooltip for now. It just gets in the way.
	//if (!(item->flags() & Qt::ItemIsSelectable)) return;
	//Kanjidic2EntryFormatter::instance().showToolTip(KanjiEntryRef(item->text()).get(), QCursor::pos());
}

KanjiSelector::KanjiSelector(QWidget *parent) : QFrame(parent), _associate(0), _outOfSyncWithAssociate(false), _ignoreAssociateSignals(false)
{
	setupUi(this);
	connect(_complementsList, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
	resize(complementsList()->gridSize().width() * 10 + complementsList()->verticalScrollBar()->width(), complementsList()->gridSize().height() * 7);
}

void KanjiSelector::showEvent (QShowEvent *event)
{
	if (_outOfSyncWithAssociate) {
		_outOfSyncWithAssociate = false;
		onAssociateChanged();
	}
}

void KanjiSelector::associateTo(QLineEdit *associate)
{
	if (_associate) {
		disconnect(_associate, SIGNAL(textChanged(QString)), this, SLOT(onAssociateChanged()));
		disconnect(this, SIGNAL(selectionChanged(QSet<uint>)), this, SLOT(updateAssociateFromSelection(QSet<uint>)));
	}
	_associate = associate;
	if (!_associate) return;
	connect(_associate, SIGNAL(textChanged(QString)), this, SLOT(onAssociateChanged()));
	connect(this, SIGNAL(selectionChanged(QSet<uint>)), this, SLOT(updateAssociateFromSelection(QSet<uint>)));
}

void KanjiSelector::updateAssociateFromSelection(QSet<uint> selection)
{
	// Codes of the current complements of the associate
	QSet<uint> currentComps(associateComplements());
	// Codes of the complements being selected
	QList<uint> selComps;
	QString compText(_associate->text());
	// Add the missing components to the text search area
	foreach (uint code, selection) {
		selComps << code;
	}
	// Remove text search components that are not selected from the associate
	foreach (uint comp, currentComps) if (!selComps.contains(comp)) compText.remove(TextTools::unicodeToSingleChar(comp));
	QStringList addComps;
	foreach (uint comp, selComps) {
		QString compRepr(TextTools::unicodeToSingleChar(comp));
		if (!compText.contains(compRepr)) compText += compRepr;
	}

	// Modifying the associate will trigger the onAssociateChanged slot,
	// but we don't want to process it this time
	_ignoreAssociateSignals = true;
	_associate->setText(compText);
	_ignoreAssociateSignals = false;
}

void KanjiSelector::onAssociateChanged()
{
	if (!isVisible()) {
		_outOfSyncWithAssociate = true;
		return;
	}
	if (_ignoreAssociateSignals) return;
	_complementsList->setEnabled(false);
	QSet<uint> selection(associateComplements());
	setSelection(selection);
	_complementsList->setEnabled(true);
}

QSet<uint> KanjiSelector::associateComplements() const
{
	QSet<uint> ret;
	if (!_associate) return ret;
	QString cText(_associate->text());
	for (int i = 0; i < cText.size(); ) {
		uint unicode(TextTools::singleCharToUnicode(cText, i));
		QString chr = TextTools::unicodeToSingleChar(unicode);
		ret << TextTools::singleCharToUnicode(chr);
		//ret << complementCode(chr);
		i += chr.size();
	}
	return ret;
}

QString KanjiSelector::complementRepr(uint kanji) const
{
	return TextTools::unicodeToSingleChar(kanji);
}

uint KanjiSelector::complementCode(const QString &repr) const
{
	return TextTools::singleCharToUnicode(repr);
}

QSet<uint> KanjiSelector::getCandidates(const QSet<uint> &selection)
{
	QSet<uint> res;
	// Get the new results
	emit startQuery();
	QSet<uint> realSel;
	foreach (uint kanji, selection) realSel << complementCode(TextTools::unicodeToSingleChar(kanji));
	QString resQuery(getCandidatesQuery(realSel));
	if (!resQuery.isEmpty()) {
		SQLite::Query query(Database::connection());
		if (!query.exec(resQuery)) qDebug() << query.lastError().message();
		while (query.next()) {
			int ch(query.valueInt(0));
			res << ch;
			QString c(TextTools::unicodeToSingleChar(ch));
			emit foundResult(c);
		}
	}
	emit endQuery();
	return res;
}

void KanjiSelector::updateComplementsList(const QSet<uint> &selection, const QSet<uint> &candidates)
{
	_complementsList->blockSignals(true);
	_complementsList->clear();
	_currentComplements = QSet<QPair<uint, QString> >();
	QString compQuery(getComplementsQuery(selection, candidates));
	if (!compQuery.isEmpty()) {
		SQLite::Query query(Database::connection());
		if (!query.exec(compQuery)) qDebug() << query.lastError().message();
		int curStrokes = 0;
		uint curKanji = 0;
		while (query.next()) {
			uint kanji = query.valueUInt(0);
			// Do not display the same kanji twice - useful for radical selector
			if (curKanji == kanji) continue;
			curKanji = kanji;
			// Do not display kanji that are already in candidates, excepted if they
			// are part of the current selection
			if (candidates.contains(kanji) && !selection.contains(kanji)) continue;
			int strokeNbr = query.valueUInt(1);
			if (strokeNbr > curStrokes) {
				_complementsList->setCurrentStrokeNbr(strokeNbr);
				curStrokes = strokeNbr;
			}
			QString repr(TextTools::unicodeToSingleChar(kanji));
			QListWidgetItem *item = _complementsList->addComplement(repr, kanji);
			if (selection.contains(kanji)) item->setSelected(true);
			_currentComplements << QPair<uint, QString>(kanji, repr);
		}
	}
	_complementsList->blockSignals(false);
}

void KanjiSelector::setSelection(const QSet<uint> &selection)
{
	QSet<uint> candidates(getCandidates(selection));
	updateComplementsList(selection, candidates);
}

void KanjiSelector::onSelectionChanged()
{
	_complementsList->setEnabled(false);
	QSet<uint> selection(_complementsList->currentSelection());
	emit selectionChanged(selection);
	setSelection(selection);
	_complementsList->setEnabled(true);
}

KanjiSelectorValidator::KanjiSelectorValidator(KanjiSelector *filter, QObject *parent) : QValidator(parent), _filter(filter)
{
}

QValidator::State KanjiSelectorValidator::validate(QString &input, int &pos) const
{
	typedef QPair<uint, QString> compType;
	const QSet<compType> &complements = _filter->currentComplements();
	QSet<QString> validChars;
	foreach (const compType &comp, complements) validChars << comp.second;
	int i = 0;
	while (i < input.size()) {
		const QChar &c = input[i];
		bool isValid = false;
		int isSurrogate = c.isHighSurrogate() ? 1 : 0;
		// Should never happen, but done for safety
		if (isSurrogate && i == input.size() - 1) isSurrogate = 0;
		if (!isSurrogate) isValid = validChars.contains(c);
		else isValid = validChars.contains(QString(c) + input[i + 1]);
		if (!isValid) {
			input.remove(i, 1 + isSurrogate);
			if (i < pos) pos -= 1 + isSurrogate;
		}
		else i += 1 + isSurrogate;
	}

	return QValidator::Acceptable;
}
	
QString RadicalKanjiSelector::getCandidatesQuery(const QSet<uint> &selection) const
{
	if (selection.isEmpty()) return "";
	QStringList select;
	foreach (uint sel, selection) select << QString::number(sel);
	
	return QString("select r1.kanji from kanjidic2.radicals as r1 join kanjidic2.entries as e on r1.kanji = e.id where r1.number in (%1) and r1.type is not null group by r1.kanji having uniquecount(r1.number) >= %2 order by e.strokeCount, e.frequency, e.id").arg(select.join(", ")).arg(select.size());
}

QString RadicalKanjiSelector::getComplementsQuery(const QSet<uint> &selection, const QSet<uint> &candidates) const
{
	if (candidates.isEmpty()) return "select entries.id, strokeCount from kanjidic2.radicalsList join kanjidic2.entries on radicalsList.kanji = entries.id order by strokeCount, number, radicalsList.rowid";
	else {
		QString selString;
		foreach (uint candidate, candidates) {
			if (selString.isEmpty()) selString = QString::number(candidate);
			else selString += "," + QString::number(candidate);
		}
		return QString("select distinct e.id, strokeCount from kanjidic2.radicals as r join kanjidic2.radicalsList as rl on r.number = rl.number join kanjidic2.entries as e on rl.kanji = e.id where r.kanji in (%1) and r.type is not null order by strokeCount, rl.number, rl.rowid").arg(selString);
	}
}

QString RadicalKanjiSelector::complementRepr(uint kanji) const
{
	return TextTools::unicodeToSingleChar(KanjiRadicals::instance().rad2Kanji(kanji)[0]);
}

uint RadicalKanjiSelector::complementCode(const QString &repr) const
{
	return KanjiRadicals::instance().kanji2Rad(TextTools::singleCharToUnicode(repr));
}

void RadicalKanjiSelector::reset()
{
	_complementsList->selectionModel()->clear();
	// For first appearance, this is needed
	if (_complementsList->count() == 0) onSelectionChanged();
}

ComponentKanjiSelector::ComponentKanjiSelector(QWidget *parent) : KanjiSelector(parent)
{
}

void ComponentKanjiSelector::reset()
{
	_complementsList->selectionModel()->clear();
	// For first appearance, this is needed
	if (_complementsList->count() == 0) onAssociateChanged();
}

QString ComponentKanjiSelector::getCandidatesQuery(const QSet<uint> &selection) const
{
	if (selection.isEmpty()) return "";
	QStringList select;
	foreach (uint sel, selection) select << QString::number(sel);
	
	return QString("select ks1.kanji from kanjidic2.strokeGroups as ks1 left join kanjidic2.entries as e on ks1.kanji = e.id where (ks1.element in (%1) or ks1.original in (%1)) group by ks1.kanji having uniquecount(CASE WHEN ks1.element IN (%1) THEN ks1.element ELSE NULL END, CASE WHEN ks1.original IN (%1) THEN ks1.original ELSE NULL END) >= %2 order by strokeCount").arg(select.join(", ")).arg(select.size());
}


QString ComponentKanjiSelector::getComplementsQuery(const QSet<uint> &selection, const QSet<uint> &candidates) const
{
	if (selection.isEmpty() && candidates.isEmpty()) return "select distinct kanji, strokeCount from kanjidic2.rootComponents as rc join kanjidic2.entries as e on rc.kanji = e.id order by strokeCount";
	// Selection but no candidates - just get the selection
	else if (candidates.isEmpty()) {
		QString selString;
		foreach (uint sel, selection) {
			if (selString.isEmpty()) selString = QString::number(sel);
			else selString += "," + QString::number(sel);
		}
		return QString("select distinct id, strokeCount from kanjidic2.entries where id in (%1)").arg(selString);
	}
	else {
		QString selString;
		foreach (uint candidate, candidates) {
			if (selString.isEmpty()) selString = QString::number(candidate);
			else selString += "," + QString::number(candidate);
		}
		return QString("select distinct ks2.element, strokeCount from kanjidic2.strokeGroups as ks join kanjidic2.strokeGroups as ks2 on ks.kanji = ks2.kanji left join kanjidic2.entries as e on ks2.element = e.id where ks.kanji in (%1) order by strokeCount").arg(selString);
	}
}

KanjiInputter::KanjiInputter(KanjiSelector *selector, bool useLineEdit, QWidget *parent) : QFrame(parent), _selector(selector)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	if (useLineEdit) {
		QLineEdit *associate = new TJLineEdit(this);
		KanjiValidator *validator = new KanjiValidator(associate);
		associate->setValidator(validator);
		layout->addWidget(associate);
		_selector->associateTo(associate);
		setFocusProxy(associate);
	}
	_results = new KanjiResultsView(this);
	layout->addWidget(_results);
	_selector->setParent(this);
	_selector->layout()->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(_selector);
	connect(selector, SIGNAL(startQuery()), _results, SLOT(startReceive()));
	connect(selector, SIGNAL(endQuery()), _results, SLOT(endReceive()));
	connect(selector, SIGNAL(foundResult(QString)), _results, SLOT(addItem(QString)));
	connect(_results, SIGNAL(kanjiSelected(QString)), this, SIGNAL(kanjiSelected(QString)));
	resize(_selector->complementsList()->gridSize().width() * 10 + _selector->complementsList()->verticalScrollBar()->width(), _selector->complementsList()->gridSize().height() * 7 + _results->sizeHint().height());
}

void KanjiInputter::reset()
{
	_selector->reset();
	_results->clear();
}

KanjiInputPopupAction::KanjiInputPopupAction(KanjiInputter *popup, const QString &title, QWidget *parent) : QAction(title, parent), _popup(popup), focusWidget(0)
{
	setIcon(QIcon(":/images/icons/component-selector.png"));
	setToolTip(tr("Triggers the kanji input panel"));
	setCheckable(true);
	connect(this, SIGNAL(toggled(bool)), this, SLOT(togglePopup(bool)));
	connect(QApplication::instance(), SIGNAL(focusChanged(QWidget *, QWidget *)), this, SLOT(onFocusChanged(QWidget *, QWidget *)));
	_popup->hide();
	_popup->installEventFilter(this);
	_popup->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	_popup->setWindowFlags(Qt::Popup);
	connect(_popup, SIGNAL(kanjiSelected(QString)), this, SLOT(onComponentSearchKanjiSelected(QString)));
}

void KanjiInputPopupAction::togglePopup(bool status)
{
	if (status) {
		QWidget *fWidget = QApplication::focusWidget();
		QLineEdit *lEdit = qobject_cast<QLineEdit *>(fWidget);
		QComboBox *cBox = qobject_cast<QComboBox *>(fWidget);
		if (lEdit || (cBox && cBox->isEditable())) {
			focusWidget = fWidget;
			_popup->move(focusWidget->mapToGlobal(QPoint(focusWidget->rect().left() + (focusWidget->rect().width() - _popup->rect().width()) / 2, focusWidget->rect().bottom())));
			_popup->reset();
			_popup->show();
			//_popup->currentSelection->setFocus();
			MainWindow::fitToScreen(_popup);
			_popup->setFocus();
		}
	}
	else {
		focusWidget = 0;
		_popup->hide();
	}
}

void KanjiInputPopupAction::onComponentSearchKanjiSelected(const QString &kString)
{
	if (focusWidget) {
		int kanji(TextTools::singleCharToUnicode(kString));
		QLineEdit *target = 0;
		QLineEdit *lEdit = qobject_cast<QLineEdit *>(focusWidget);
		QComboBox *cBox = qobject_cast<QComboBox *>(focusWidget);
		if (lEdit) target = lEdit;
		else if (cBox && cBox->isEditable()) target = cBox->lineEdit();
		if (target) target->insert(TextTools::unicodeToSingleChar(kanji));
	}
	_popup->hide();
}

bool KanjiInputPopupAction::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::Hide) {
		setChecked(false);
	}
	return false;
}

void KanjiInputPopupAction::onFocusChanged(QWidget *old, QWidget *now)
{
	QLineEdit *lEdit = qobject_cast<QLineEdit *>(now);
	QComboBox *cBox = qobject_cast<QComboBox *>(now);
	setEnabled(lEdit || (cBox && cBox->isEditable()));
}
