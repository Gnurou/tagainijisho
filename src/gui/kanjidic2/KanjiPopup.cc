/*
 *  Copyright (C) 2008  Alexandre Courbot
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
#include "core/EntriesCache.h"
#include "gui/kanjidic2/KanjiPopup.h"
#include "gui/EntryFormatter.h"
#include "gui/SingleEntryView.h"

#include <QtDebug>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QUrl>
#include <QFontMetrics>
#include <QCursor>
#include <QToolTip>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

GraphicsComponentItem::GraphicsComponentItem(int width, const KanjiComponent *component, QGraphicsItem *parent) : QGraphicsItem(parent), _width(width), _component(component)
{
	setAcceptHoverEvents(true);
	setAcceptedMouseButtons(Qt::LeftButton);
	setHandlesChildEvents(false);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	_kanji = EntriesCache::get(KANJIDIC2ENTRY_GLOBALID, TextTools::singleCharToUnicode(component->element()));

	EntryPointer<const Entry> original;
	const Kanjidic2Entry *kOriginal;
	if (!component->original().isEmpty()) {
		original = EntriesCache::get(KANJIDIC2ENTRY_GLOBALID, TextTools::singleCharToUnicode(component->original()));
		kOriginal = static_cast<const Kanjidic2Entry *>(original.data());
	}
	QFont font;
	QFontMetrics metrics(font);
	_meaningsHeight = metrics.height();
	_readingsHeight = metrics.height();
	_kanjiSize = _meaningsHeight + _readingsHeight;
	QString meanings;
	if (original.data()) meanings = kOriginal->meaningsString();
	else meanings = kanji()->meaningsString();
	if (!meanings.isEmpty()) meanings[0] = meanings[0].toUpper();
	_meanings = metrics.elidedText(meanings, Qt::ElideRight, width - _kanjiSize);

	QString readings;
	readings = kanji()->readings().join(", ");
	_readings = metrics.elidedText(readings, Qt::ElideRight, width - _kanjiSize);
}

GraphicsComponentItem::~GraphicsComponentItem()
{
}

QRectF GraphicsComponentItem::boundingRect() const
{
	return QRectF(0, 0, _width, qMax(_kanjiSize, _meaningsHeight + _readingsHeight));
}

void GraphicsComponentItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	// TODO Draw something in that case!
	if (!kanji()) return;
	painter->save();
	QRectF drawingArea(boundingRect());
	QPen pen(painter->pen());
	QBrush brush;
	brush.setStyle(Qt::SolidPattern);
	QColor bg;
	if (!kanji()->trained()) bg = QColor(Qt::lightGray);
	else {
		const EntryFormatter *formatter(EntryFormatter::getFormatter(kanji()));
		bg = formatter->scoreColor(kanji());
	}
	if (!isSelected()) brush.setColor(bg);
	else {
		brush = widget->palette().highlight();
		pen.setColor(widget->palette().highlightedText().color());
	}
	int rightPartHeight = _meaningsHeight + _readingsHeight;
	pen.setStyle(Qt::NoPen);
	painter->setPen(pen);
	painter->setBrush(brush);
	painter->setRenderHint(QPainter::Antialiasing);
	painter->drawRoundedRect(drawingArea, 5.0, 5.0);

	KanjiRenderer renderer(kanji());
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(5);
	painter->setPen(pen);
	painter->setBrush(QBrush());
	painter->save();
	if (rightPartHeight > _kanjiSize) painter->translate(0.0, (rightPartHeight - _kanjiSize) / 2);
	painter->scale(_kanjiSize / 109.0, _kanjiSize / 109.0);
	renderer.renderStrokes(painter);
	painter->restore();

	QFont font;
	QFontMetrics metrics(font);
	painter->drawText(_kanjiSize, metrics.ascent(), _meanings);
	painter->drawText(_kanjiSize, _meaningsHeight + metrics.ascent(), _readings);
	painter->restore();
}

void GraphicsComponentItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	setSelected(true);
	emit hoverEnter(component());
	event->accept();
}

void GraphicsComponentItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	setSelected(false);
	emit hoverLeave(component());
	event->accept();
}

void GraphicsComponentItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		emit clicked(component());
		event->accept();
	}
}

void GraphicsComponentItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	SingleEntryView view;
	QMenu menu;
	view.setEntry(kanji());
	view.populateMenu(&menu);
	menu.exec(QCursor::pos());
	event->accept();
}

QGraphicsScene *scene;
QGraphicsView *view;
QMap<const KanjiComponent *, GraphicsComponentItem *> itemsMap;

PreferenceItem<int> KanjiPopup::historySize("kanjidic", "popupHistorySize", 1000);
PreferenceItem<int> KanjiPopup::animationSize("kanjidic", "animationSize", 100);
PreferenceItem<bool> KanjiPopup::autoStartAnim("kanjidic", "autoStartAnim", true);

KanjiPopup::KanjiPopup(QWidget *parent) : QFrame(parent), _history(historySize.value()), entryView()
{
	QGridLayout *layout = new QGridLayout(this);

	prevButton = new QToolButton(this);
	prevButton->setIcon(QIcon(":/images/icons/go-previous.png"));
	prevButton->setToolTip(tr("Previous entry"));
	prevButton->setAutoRaise(true);
	prevButton->setEnabled(false);
	connect(prevButton, SIGNAL(clicked()), this, SLOT(onPreviousClick()));
	nextButton = new QToolButton(this);
	nextButton->setIcon(QIcon(":/images/icons/go-next.png"));
	nextButton->setToolTip(tr("Next entry"));
	nextButton->setAutoRaise(true);
	nextButton->setEnabled(false);
	connect(nextButton, SIGNAL(clicked()), this, SLOT(onNextClick()));

	QToolButton *openButton = new QToolButton(this);
	openButton->setIcon(QIcon(":/images/icons/zoom-in.png"));
	openButton->setToolTip(tr("See in detailed view"));
	openButton->setAutoRaise(true);
	connect(openButton, SIGNAL(clicked()), this, SLOT(onOpenClick()));

	QToolButton *moreButton = new QToolButton(this);
	moreButton->setIcon(QIcon(":/images/icons/list-add.png"));
	moreButton->setToolTip(tr("Entry options menu"));
	moreButton->setAutoRaise(true);
	moreButton->setPopupMode(QToolButton::InstantPopup);
	QMenu *menu = new QMenu(openButton);
	entryView.populateMenu(menu);
	moreButton->setMenu(menu);

	QToolButton *closeButton = new QToolButton(this);
	closeButton->setIcon(QIcon(":/images/icons/application-exit.png"));
	closeButton->setToolTip(tr("Close this popup"));
	closeButton->setAutoRaise(true);
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	QHBoxLayout *hLayout = new QHBoxLayout();
	hLayout->addWidget(prevButton);
	hLayout->addWidget(nextButton);
	hLayout->addStretch();
	hLayout->addWidget(openButton);
	hLayout->addWidget(moreButton);
	hLayout->addWidget(closeButton);

	layout->addLayout(hLayout, 0, 0, 1, 2, Qt::AlignRight);

	stroke = new KanjiPlayer(animationSize.value(), this);
	stroke->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	layout->addWidget(stroke, 3, 0, Qt::AlignTop);

	propsLabel = new QLabel(this);
	layout->addWidget(propsLabel, 3, 1);

	meaningsLabel = new QLabel(this);
	meaningsLabel->setAlignment(Qt::AlignHCenter);
	layout->addWidget(meaningsLabel, 1, 0, 1, 2);

	readingsLabel = new QLabel(this);
	readingsLabel->setAlignment(Qt::AlignCenter);
	layout->addWidget(readingsLabel, 2, 0, 1, 2);

	scene = new QGraphicsScene(this);
	view = new QGraphicsView(scene);
	view->viewport()->setAutoFillBackground(false);
	view->setFrameStyle(0);
//	view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setScene(scene);
	view->setMaximumHeight(150);
	layout->addWidget(view, 4, 0, 1, 2);

	connect(&entryView, SIGNAL(entryChanged(Entry *)), &entryView, SLOT(updateMenuEntries()));
	connect(&entryView, SIGNAL(entryChanged(Entry *)), this, SLOT(updateInfo()));
	connect(stroke, SIGNAL(componentHighlighted(const KanjiComponent*)), this, SLOT(onComponentHighlighted(const KanjiComponent*)));
	connect(stroke, SIGNAL(componentUnHighlighted(const KanjiComponent*)), this, SLOT(onComponentUnHighlighted(const KanjiComponent*)));
	connect(stroke, SIGNAL(componentClicked(const KanjiComponent*)), this, SLOT(onComponentClicked(const KanjiComponent*)));
}

KanjiPopup::~KanjiPopup()
{
}

void KanjiPopup::updateInfo()
{
	QString str;
	Kanjidic2Entry *entry = static_cast<Kanjidic2Entry *>(entryView.entry());

	if (entry->kanjiFrequency() != -1)
		str += tr("<b>Freq:</b> %1<br/>").arg(entry->kanjiFrequency());
	if (entry->jlpt() != -1)
		str += tr("<b>Grade:</b> %1<br/>").arg(entry->grade());
	if (entry->jlpt() != -1)
		str += tr("<b>JLPT:</b> %1<br/>").arg(entry->jlpt());
	if (entry->trained())
		str += tr("<b>Score:</b> %1<br/>").arg(entry->score());
	propsLabel->setText(str);
}

void KanjiPopup::showKanji(Kanjidic2Entry *entry)
{
	QString str;

	if (!entry) return;

	entryView.setEntry(entry);

	prevButton->setEnabled(_history.hasPrevious());
	nextButton->setEnabled(_history.hasNext());

	updateInfo();

	if (!entry->inDB() && entry->meanings().isEmpty()) str = tr("No information about this kanji!");
	else {
		str = entry->meaningsString();
		if (!str.isEmpty()) str[0] = str[0].toUpper();
	}

	meaningsLabel->setText(meaningsLabel->fontMetrics().elidedText(str, Qt::ElideRight, maxEltWidth));
	str.clear();

	readingsLabel->setText(readingsLabel->fontMetrics().elidedText(entry->readings().join(", "), Qt::ElideRight, maxEltWidth));

	itemsMap.clear();
	scene->clear();
	view->centerOn(0, 0);
	if (!entry->rootComponents().isEmpty()) {
/*		GraphicsComponentItem *root = new GraphicsComponentItem(300, &entry->components()[0]);
		root->setAcceptedMouseButtons(0);
		root->setAcceptHoverEvents(false);
		scene->addItem(root);
		int i = root->boundingRect().height() + 5;*/
		int i = 0;
		foreach(const KanjiComponent *component, entry->rootComponents())
			addComponentDescription(component, 0, i);
	}
	view->resize(view->sizeHint());

	stroke->stopAnimation();
	stroke->setKanji(entry);
	if (autoStartAnim.value()) stroke->startAnimation();
	else stroke->setPosition(entry->strokeCount());

//	adjustSize();
}

void KanjiPopup::addComponentDescription(const KanjiComponent *component, GraphicsComponentItem *parent, int &vPos)
{
	int width(parent ? parent->boundingRect().width() - 20 : maxEltWidth);
	int pos(parent ? 20 : 0);
	GraphicsComponentItem *item = new GraphicsComponentItem(width, component, parent);
	item->setPos(pos, vPos);
	connect(item, SIGNAL(hoverEnter(const KanjiComponent*)), stroke, SLOT(highlightComponent(const KanjiComponent*)));
	connect(item, SIGNAL(hoverLeave(const KanjiComponent*)), stroke, SLOT(unHighlightComponent()));
	connect(item, SIGNAL(clicked(const KanjiComponent*)), this, SLOT(onComponentClicked(const KanjiComponent*)));
	itemsMap[component] = item;
	if (!parent) scene->addItem(item);
	int cVPos = item->boundingRect().height();
//	foreach (const KanjiComponent *child, component->components()) {
//		addComponentDescription(child, item, cVPos);
//	}

	vPos += cVPos;
}

void KanjiPopup::display(Kanjidic2Entry *entry)
{
	_history.add(entry->kanji());
	showKanji(entry);
}

void KanjiPopup::onOpenClick()
{
	emit requestDisplay(entryView.entry());
	close();
}

void KanjiPopup::onPreviousClick()
{
	QString kanji;
	bool ok = _history.previous(kanji);
	if (ok) {
		EntryPointer<Entry> entry = EntriesCache::get(KANJIDIC2ENTRY_GLOBALID, TextTools::singleCharToUnicode(kanji));
		showKanji(static_cast<Kanjidic2Entry *>(entry.data()));
	}
}

void KanjiPopup::onNextClick()
{
	QString kanji;
	bool ok = _history.next(kanji);
	if (ok) {
		EntryPointer<Entry> entry = EntriesCache::get(KANJIDIC2ENTRY_GLOBALID, TextTools::singleCharToUnicode(kanji));
		showKanji(static_cast<Kanjidic2Entry *>(entry.data()));
	}
}

void KanjiPopup::onComponentHighlighted(const KanjiComponent *component)
{
	if (component) {
		GraphicsComponentItem *item = itemsMap[component];
		if (item) {
			item->setSelected(true);
			view->centerOn(item);
		}
	}
	stroke->highlightComponent(component);
}

void KanjiPopup::onComponentUnHighlighted(const KanjiComponent *component)
{
	foreach(GraphicsComponentItem *item, itemsMap.values()) item->setSelected(false);
	stroke->unHighlightComponent();
}

void KanjiPopup::onComponentClicked(const KanjiComponent *component)
{
	EntryPointer<Entry> element = EntriesCache::get(KANJIDIC2ENTRY_GLOBALID, TextTools::singleCharToUnicode(component->element()));
	Kanjidic2Entry *kElement = static_cast<const Kanjidic2Entry *>(element.data());
	if (!kElement) return;
	_history.add(kElement->kanji());
	showKanji(kElement);
}
