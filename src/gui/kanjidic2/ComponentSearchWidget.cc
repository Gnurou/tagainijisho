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

#include <QtDebug>

#include "core/TextTools.h"
#include "gui/kanjidic2/ComponentSearchWidget.h"
#include "gui/kanjidic2/Kanjidic2EntryFormatter.h"
#include "gui/KanjiValidator.h"

#include <QSqlError>

#include <QSet>
#include <QVBoxLayout>
#include <QSplitter>
#include <QSqlQuery>
#include <QCursor>
#include <QScrollBar>
#include <QComboBox>
#include <QFontMetrics>

#include <QDesktopWidget>

#define KANJI_SIZE 50
#define PADDING 5

CandidatesKanjiList::CandidatesKanjiList(QWidget *parent) : QGraphicsView(parent), _scene(), curItem(0), pos(0), wheelDelta(0)
{
	setScene(&_scene);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

//	setSceneRect(-0xfffff, -0xfffff, 2 * 0xfffff, 2 * 0xfffff);
	setResizeAnchor(QGraphicsView::AnchorViewCenter);

	timer.setInterval(20);
	timer.setSingleShot(false);
	connect(&timer, SIGNAL(timeout()), this, SLOT(updateAnimationState()));
	connect(&_scene, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

QSize CandidatesKanjiList::sizeHint() const
{
	QFont font;
	font.setPixelSize(KANJI_SIZE);
	return QSize(QWidget::sizeHint().width(), QFontMetrics(font).height() + horizontalScrollBar()->height());
}

#define ITEM_CENTER(item) (item->pos() + item->boundingRect().center())
#define ITEM_POSITION(x) (4 * PADDING + (x) * (KANJI_SIZE + PADDING))

void CandidatesKanjiList::updateAnimationState()
{
	if (items.isEmpty()) return;

	int dest = ITEM_CENTER(items[curItem]).x();
	if (dest == pos) {
		timer.stop();
		return;
	}
	pos += (dest - pos) / 2;
	if (qAbs(dest - pos) == 1) pos = dest;
	centerOn(pos, ITEM_CENTER(items[curItem]).y());
}

void CandidatesKanjiList::onSelectionChanged()
{
	QList<QGraphicsItem *> selection(_scene.selectedItems());
	if (selection.isEmpty()) return;
	emit kanjiSelected(static_cast<QGraphicsTextItem *>(selection[selection.size() - 1])->toPlainText());
}

void CandidatesKanjiList::addItem(const QString &kanji)
{
	QFont font;
	font.setPixelSize(KANJI_SIZE);
	QGraphicsTextItem *item = _scene.addText(kanji, font);
	item->setPos(ITEM_POSITION(items.size()), 0);
	item->setFlags(QGraphicsItem::ItemIsSelectable);
	items << item;
	if (items.size() == 1) {
		curItem = 0;
		timer.start();
	}
	setSceneRect(_scene.itemsBoundingRect());
}

void CandidatesKanjiList::clear()
{
	_scene.clear();
	items.clear();
	curItem = pos = 0;
	// Used to reset the scrollbar
	setSceneRect(0, 0, 1, 1);
}

#define MOUSE_STEP 120
void CandidatesKanjiList::wheelEvent(QWheelEvent *event)
{
	event->accept();
	if (items.isEmpty()) return;
	wheelDelta += event->delta();
	int steps = wheelDelta / MOUSE_STEP;
	if (steps != 0) {
		wheelDelta -= steps * MOUSE_STEP;
		curItem -= steps;
		if (curItem >= items.size()) curItem = items.size() - 1;
		else if (curItem < 0) curItem = 0;
		timer.start();
	}
}

ComponentSearchWidget::ComponentSearchWidget(QWidget *parent) : QFrame(parent)
{
	setupUi(this);
	KanjiValidator *validator = new KanjiValidator(currentSelection);
	currentSelection->setValidator(validator);
	// The queued connection allows the click signal to be entirely processed and prevents the next complement to appear
	// under the mouse to be selected.
	connect(complementsList, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()), Qt::QueuedConnection);
	connect(currentSelection, SIGNAL(textChanged(QString)), this, SLOT(onSelectedComponentsChanged(QString)));
	connect(candidatesList, SIGNAL(kanjiSelected(QString)), this, SIGNAL(kanjiSelected(QString)));

	onSelectedComponentsChanged("");
}

void ComponentSearchWidget::onSelectionChanged()
{	
	foreach (const QListWidgetItem *selected, complementsList->selectedItems()) {
		currentSelection->setText(currentSelection->text() + selected->text());
	}
}

void ComponentSearchWidget::onSelectedComponentsChanged(const QString &components)
{
	QStringList selection(components.split("", QString::SkipEmptyParts));
	for (int i = 0; i < selection.size(); ++i) selection[i] = QString("%1").arg(TextTools::singleCharToUnicode(selection[i]));

	QSqlQuery query;
	QString queryString;
	// TODO Merge original and element when displaying candidates
	// TODO Add the stroke count of every group into the DB and use that value to sort them
	if (!selection.isEmpty()) queryString = QString("select distinct c.kanji, c2.element, entries.strokeCount from "
		// All kanji for which all requested components are either present as their element or original field
		"(SELECT DISTINCT ks1.kanji FROM kanjidic2.strokeGroups AS ks1 WHERE (ks1.element IN (%1) OR ks1.original IN (%1)) GROUP BY ks1.kanji HAVING UNIQUECOUNT(CASE WHEN ks1.element IN (%1) THEN ks1.element ELSE NULL END, CASE WHEN ks1.original IN (%1) THEN ks1.original ELSE NULL END) >= %2) as c "
		"join kanjidic2.strokeGroups c2 on c.kanji = c2.kanji left join kanjidic2.entries on entries.id = c2.kanji where (c2.element not in (%1) or c2.kanji in (%1)) order by entries.strokeCount, entries.frequency is null ASC, entries.frequency ASC").arg(selection.join(",")).arg(selection.size());
	//if (!selection.isEmpty()) queryString = QString("select distinct c2.element, strokeCount from kanjidic2.strokeGroups c1 join kanjidic2.strokeGroups c2 on c1.kanji = c2.kanji left join kanjidic2.entries on entries.id = c2.element where c1.element in (%1) and c1.kanji in (select kanji from kanjidic2.strokeGroups where element in (%1) group by kanji having count(distinct element) >= %2) and c2.element not in (select element from strokeGroups where kanji in (%1)) order by entries.frequency is null ASC, entries.frequency ASC").arg(l.join(",")).arg(selection.size());
	// If there is no selection, select all elements that are root and employed as a part of another kanji
	else queryString = "select distinct 0, ks.element, 0 from kanjidic2.strokeGroups as ks where ks.element not in (select distinct kanji from strokeGroups)";
	if (!query.exec(queryString)) qDebug() << query.lastError();
	populateList(query);
}

void ComponentSearchWidget::populateList(QSqlQuery &query)
{
	QFont font;
	font.setPointSize(font.pointSize() + 2);
	QFont nbrFont(font);
	nbrFont.setBold(true);
	QColor nbrBg(Qt::yellow);
	nbrBg = nbrBg.lighter();

	candidatesList->clear();
	complementsList->clear();

	QSet<QString> candidates;
	QMap<int, QSet<QString> > complements;
	while (query.next()) {
		// Do we have a new candidate?
		QString val(TextTools::unicodeToSingleChar(query.value(0).toInt()));
		if (!val.isEmpty() && !candidates.contains(val)) {
			candidates << val;
			candidatesList->addItem(val);
		}

		// Do we have a new complement?
		if (query.value(1) == 0) continue;
		ConstKanjidic2EntryPointer kEntry(EntriesCache::get(2, query.value(1).toInt()).staticCast<const Kanjidic2Entry>());
		if (!kEntry) continue;
		val = kEntry->kanji();
		if (!val.isEmpty() && !complements[kEntry->strokeCount()].contains(val)) {
			complements[kEntry->strokeCount()] << val;
		}
	}
	for (int i = 0; i < 32; i++) {
		QSet<QString> comp(complements[i]);
		if (!comp.isEmpty()) {
			QListWidgetItem *item = new QListWidgetItem(QString::number(i), complementsList);
			item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
			item->setBackground(nbrBg);
			item->setFont(nbrFont);
		}
		foreach (const QString &c, comp) {
			QListWidgetItem *item = new QListWidgetItem(c, complementsList);
			item->setFont(font);
		}
	}
}

RadicalSelector::RadicalSelector(QWidget *parent) : QListWidget(parent), baseFont(font()), labelFont(baseFont), _acceptVariants(true)
{
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(updateSelectableComplements()));
	
	// Setup the fonts and size of the grid
	baseFont.setPointSize(baseFont.pointSize() + 2);
	QFontMetrics fm(baseFont);
	labelFont.setBold(true);
	QFontMetrics lfm(labelFont);
	int maxFontSize = qMax(fm.maxWidth(), fm.height()) + 2;
	int maxBoldSize = 0;
	for (int i = 0; i < 10; i++) {
		int w = lfm.width(QString::number(i));
		if (w > maxBoldSize) maxBoldSize = w;
	}
	maxBoldSize *= 2;
	int gridSize = qMax(maxBoldSize, maxFontSize) + 5;
	setFont(baseFont);
	setGridSize(QSize(gridSize, gridSize));
	
	buildRadicalsList();
}

void RadicalSelector::buildRadicalsList()
{
	clear();
	QSqlQuery query;
	query.exec("select kanji, number, strokeCount from kanjidic2.radicalsList join kanjidic2.entries on radicalsList.kanji = entries.id order by strokeCount, number, radicalsList.rowid");
	int lastNbr = 0;
	int lastStrokeNbr = 0;
	while (query.next()) {
		QString car(TextTools::unicodeToSingleChar(query.value(0).toInt()));
		int curNbr = query.value(1).toInt();
		int strokeNbr = query.value(2).toInt();
		// Skip variations for now
		//if (curNbr == lastNbr) continue;
		if (curNbr != lastNbr && lastStrokeNbr < strokeNbr) {
			QListWidgetItem *item = new QListWidgetItem(QString::number(strokeNbr), this);
			item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
			item->setBackground(Qt::yellow);
			item->setFont(labelFont);
		}
		QListWidgetItem *item = new QListWidgetItem(car, this);
		item->setData(Qt::UserRole, curNbr);
		if (lastNbr != curNbr) lastStrokeNbr = strokeNbr;
		lastNbr = curNbr;
	}
}

void RadicalSelector::setAcceptVariants(bool accept)
{
	_acceptVariants = accept;
	buildRadicalsList();
}

void RadicalSelector::updateSelectableComplements()
{
	QSet<int> selection;
	// Build the lists of selected radicals
	foreach (const QListWidgetItem *selected, selectedItems()) {
		int number = selected->data(Qt::UserRole).toInt();
		selection << number;
	}

	// If nothing is selected, enable everyone and exit
	if (selection.isEmpty()) {
		for (int i = 0; i < count(); i++) {
			QListWidgetItem *it = item(i);
			it->setFlags(it->flags() | Qt::ItemIsEnabled);
		}
	} else {
		QSet<int> selectable;
		QStringList selStrings;
		foreach (int n, selection) selStrings << QString::number(n);
		QSqlQuery query;
		query.exec(QString("select r2.number from kanjidic2.radicals as r1 join kanjidic2.radicals as r2 on r1.kanji = r2.kanji where r1.number in (%1) group by r1.kanji, r2.number having uniquecount(r1.number) >= %2").arg(selStrings.join(", ")).arg(selStrings.size()));
		while (query.next()) selectable << query.value(0).toInt();
		selectable -= selection;
		// Enable/disable items in the list
		for (int i = 0; i < count(); i++) {
			QListWidgetItem *it = item(i);
			// Skip stroke number items
			if (!(it->flags() & Qt::ItemIsSelectable)) continue;
			if (selectable.contains(it->data(Qt::UserRole).toInt()) || it->isSelected())
				it->setFlags(it->flags() | Qt::ItemIsEnabled);
			else it->setFlags(it->flags() & ~Qt::ItemIsEnabled);
		}
	}
}

QStringList RadicalSelector::selectedRadicals() const
{
	QStringList ret;
	foreach (const QListWidgetItem *selected, selectedItems()) ret << selected->text();
	return ret;
}

QSet<int> RadicalSelector::selectedRadicalsNumbers() const
{
	QSet<int> ret;
	foreach (const QListWidgetItem *selected, selectedItems()) {
		int number = selected->data(Qt::UserRole).toInt();
		ret << number;
	}
	return ret;
}

RadicalSearchWidget::RadicalSearchWidget(QWidget *parent) : QFrame(parent)
{
	setupUi(this);
	connect(complementsList, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
	connect(candidatesList, SIGNAL(kanjiSelected(QString)), this, SIGNAL(kanjiSelected(QString)));
	connect(this, SIGNAL(resultFound(QString)), candidatesList, SLOT(addItem(QString)));
	
}

void RadicalSearchWidget::onSelectionChanged()
{
	complementsList->setEnabled(false);

	// First of all clear the candidates list
	candidatesList->clear();
	
	QSet<int> selected(complementsList->selectedRadicalsNumbers());
	QStringList select;
	foreach (int sel, selected) select << QString::number(sel);
	
	// Run the SQL query if we have something selected and emit the results
	if (!selected.isEmpty()) {
		QSqlQuery query;
		QString queryString(QString("select r1.kanji from kanjidic2.radicals as r1 join kanjidic2.entries as e on r1.kanji = e.id where r1.number in (%1) group by r1.kanji having uniquecount(r1.number) >= %2 order by e.strokeCount, e.frequency, e.id").arg(select.join(", ")).arg(select.size()));
		query.exec(queryString);
		while (query.next()) emit resultFound(TextTools::unicodeToSingleChar(query.value(0).toInt()));
	}
	
	complementsList->setEnabled(true);
}

KanjiInputPopupAction::KanjiInputPopupAction(QFrame *popup, const QString &title, QWidget *parent) : QAction(title, parent), _popup(popup), focusWidget(0)
{
	setIcon(QIcon(":/images/icons/component-selector.png"));
	setToolTip(tr("Triggers the kanji input panel"));
	setCheckable(true);
	connect(this, SIGNAL(toggled(bool)), this, SLOT(togglePopup(bool)));
	connect(QApplication::instance(), SIGNAL(focusChanged(QWidget *, QWidget *)), this, SLOT(onFocusChanged(QWidget *, QWidget *)));
	_popup->hide();
	_popup->installEventFilter(this);
	_popup->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	_popup->setWindowModality(Qt::ApplicationModal);
	_popup->setWindowFlags(Qt::Popup);
	connect(_popup, SIGNAL(kanjiSelected(QString)), this, SLOT(onComponentSearchKanjiSelected(QString)));
}

KanjiInputPopupAction::~KanjiInputPopupAction()
{
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
			_popup->show();
			//_popup->currentSelection->setFocus();
			QDesktopWidget *desktopWidget = QApplication::desktop();
			QRect popupRect = _popup->geometry();
			QRect screenRect(desktopWidget->screenGeometry());
			if (!screenRect.contains(_popup->geometry())) {
				if (screenRect.left() > popupRect.left()) popupRect.moveLeft(screenRect.left());
				if (screenRect.top() > popupRect.top()) popupRect.moveTop(screenRect.top());
				if (screenRect.right() < popupRect.right()) popupRect.moveRight(screenRect.right());
				if (screenRect.bottom() < popupRect.bottom()) popupRect.moveBottom(screenRect.bottom());
				_popup->setGeometry(popupRect);
			}
		}
	}
	else {
		focusWidget = 0;
		_popup->hide();
	}
}

void KanjiInputPopupAction::onComponentSearchKanjiSelected(const QString &kanji)
{
	if (focusWidget) {
		QLineEdit *target = 0;
		QLineEdit *lEdit = qobject_cast<QLineEdit *>(focusWidget);
		QComboBox *cBox = qobject_cast<QComboBox *>(focusWidget);
		if (lEdit) target = lEdit;
		else if (cBox && cBox->isEditable()) target = cBox->lineEdit();
		if (target) target->insert(kanji);
	}
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
