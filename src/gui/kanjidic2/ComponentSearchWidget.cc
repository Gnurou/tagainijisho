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

#include <QSqlError>

#include <QVBoxLayout>
#include <QSplitter>
#include <QSqlQuery>
#include <QCursor>

#define KANJI_SIZE 50
#define PADDING 5

CandidatesKanjiList::CandidatesKanjiList(QWidget *parent) : QGraphicsView(parent), scene(), pos(0)
{
	setScene(&scene);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

QSize CandidatesKanjiList::sizeHint() const
{
	return QSize(QWidget::sizeHint().width(), KANJI_SIZE);
}

void CandidatesKanjiList::addItem(const QString &kanji)
{
	QFont font;
	font.setPixelSize(KANJI_SIZE);
	QGraphicsTextItem *item = scene.addText(kanji, font);
	item->setPos(PADDING + items.size() * (KANJI_SIZE + PADDING), 0);
	setSceneRect(0, 0, PADDING + items.size() * (KANJI_SIZE + PADDING), KANJI_SIZE);
	items << item;
	setSceneRect(-0xfffff, -0xfffff, 2 * 0xfffff, 2 * 0xfffff);
	centerOn(items[pos]);
}

void CandidatesKanjiList::clear()
{
	scene.clear();
	items.clear();
	pos = 0;
//	translate(0, 0);
}

void CandidatesKanjiList::wheelEvent(QWheelEvent *event)
{
	event->accept();
	if (items.isEmpty()) return;
	if (event->delta() < 0 && pos < items.size() - 1) ++pos;
	else if (event->delta() > 0 && pos > 0) --pos;
	centerOn(items[pos]);
}

/*void CandidatesKanjiList::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	scene.render(&painter);
}*/

ComponentSearchWidget::ComponentSearchWidget(QWidget *parent) : QWidget(parent)
{
	setupUi(this);
	connect(complementsList, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
	connect(currentSelection, SIGNAL(textChanged(QString)), this, SLOT(onSelectedComponentsChanged(QString)));
//	connect(candidatesList, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(onItemEntered(QListWidgetItem*)));
//	connect(complementsList, SIGNAL(itemEntered(QListWidgetItem*)), this, SLOT(onItemEntered(QListWidgetItem*)));

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
	if (!selection.isEmpty()) queryString = QString("select distinct element, strokeCount, parentGroup is null from kanjidic2.strokeGroups left join kanjidic2.entries on entries.id = strokeGroups.element where kanjidic2.strokeGroups.element not in (%1) and kanjidic2.strokeGroups.kanji in (SELECT DISTINCT ks1.kanji FROM kanjidic2.strokeGroups AS ks1 WHERE (ks1.element IN (%1) OR ks1.original IN (%1)) AND ks1.parentGroup NOT NULL GROUP BY ks1.kanji HAVING UNIQUECOUNT(CASE WHEN ks1.element IN (%1) THEN ks1.element ELSE NULL END, CASE WHEN ks1.original IN (%1) THEN ks1.original ELSE NULL END) >= %2) order by entries.strokeCount, entries.frequency is null ASC, entries.frequency ASC").arg(selection.join(",")).arg(selection.size());
	//if (!selection.isEmpty()) queryString = QString("select distinct c2.element, strokeCount from kanjidic2.strokeGroups c1 join kanjidic2.strokeGroups c2 on c1.kanji = c2.kanji left join kanjidic2.entries on entries.id = c2.element where c1.element in (%1) and c1.kanji in (select kanji from kanjidic2.strokeGroups where element in (%1) group by kanji having count(distinct element) >= %2) and c2.element not in (select element from strokeGroups where kanji in (%1)) order by entries.frequency is null ASC, entries.frequency ASC").arg(l.join(",")).arg(selection.size());
	// If there is no selection, select all elements that have no components
	else queryString = "select distinct element, strokeCount from kanjidic2.strokeGroups left join kanjidic2.entries on entries.id = strokeGroups.element where kanjidic2.strokeGroups.rowid not in (select distinct(parentGroup) from kanjidic2.strokeGroups where parentGroup is not null) order by entries.strokeCount, entries.frequency is null ASC, entries.frequency ASC";
	if (!query.exec(queryString)) qDebug() << query.lastError();
	populateList(query);
}

void ComponentSearchWidget::populateList(QSqlQuery &query)
{
	QFont font;
	font.setPointSize(font.pointSize() + 2);
	int currentStrokeCount = 0;
	QFont nbrFont(font);
	nbrFont.setBold(true);
	QColor nbrBg(Qt::yellow);
	nbrBg = nbrBg.lighter();

	candidatesList->clear();
	complementsList->clear();

	while (query.next()) {
		uint code = query.value(0).toInt();
		QString val;
		if (code < 0x10000) {
			val += QChar(code);
		} else {
			val += QChar(QChar::highSurrogate(code));
			val += QChar(QChar::lowSurrogate(code));
		}
		bool isRoot(query.value(2).toBool());
		if (isRoot) {
			candidatesList->addItem(val);
//			QListWidgetItem *item = new QListWidgetItem(val, candidatesList);
//			item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
			continue;
		}

		int strokeCount = query.value(1).toInt();
		if (strokeCount > currentStrokeCount) {
			currentStrokeCount = strokeCount;
			QListWidgetItem *item = new QListWidgetItem(QString::number(strokeCount), complementsList);
			item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
			item->setBackground(nbrBg);
			item->setFont(nbrFont);
		}
		QListWidgetItem *item = new QListWidgetItem(val, complementsList);
		item->setFont(font);
	}
}

void ComponentSearchWidget::onItemEntered(QListWidgetItem *item)
{
	EntryPointer<Entry> entry(EntriesCache::get(KANJIDIC2ENTRY_GLOBALID, TextTools::singleCharToUnicode(item->text())).data());
	const Kanjidic2Entry *kanji(static_cast<const Kanjidic2Entry *>(entry.data()));
	if (!kanji) return;
	const Kanjidic2EntryFormatter *formatter(static_cast<const Kanjidic2EntryFormatter *>(EntryFormatter::getFormatter(kanji)));
	formatter->showToolTip(kanji, QCursor::pos());
}
