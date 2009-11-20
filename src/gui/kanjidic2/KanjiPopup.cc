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

KanjiComponentWidget::KanjiComponentWidget(QWidget *parent) : QWidget(parent), _component(0)
{
	QFontMetrics metrics(font());
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

QSize KanjiComponentWidget::sizeHint() const
{
	QFontMetrics metrics(font());
	return QSize(metrics.height() * 6, metrics.height() * 2);
}

void KanjiComponentWidget::setComponent(const KanjiComponent *component)
{
	_component = component;
	if (component) _kanji = EntriesCache::get(KANJIDIC2ENTRY_GLOBALID, TextTools::singleCharToUnicode(component->element()));
	else _kanji.reset();
	update();
}

void KanjiComponentWidget::paintEvent(QPaintEvent *event)
{
	if (!component()) return;
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	QFontMetrics metrics(font());
	const int kanjiSize(metrics.height() * 2);

	const Kanjidic2Entry *kEntry(kanji());
	if (kEntry && !kEntry->strokes().isEmpty()) {
		KanjiRenderer renderer(kEntry);
		QPen pen(painter.pen());
		pen.setStyle(Qt::SolidLine);
		pen.setWidth(5);
		painter.setPen(pen);
		painter.setBrush(QBrush());
		painter.save();
		painter.scale(kanjiSize / 109.0, kanjiSize / 109.0);
		renderer.renderStrokes(&painter);
		painter.restore();
	} else {
		QString k(component()->element());
		painter.save();
		QFont f(font());
		f.setPixelSize(kanjiSize);
		QFontMetrics fMetrics(f);
		painter.setFont(f);
		painter.drawText(0, fMetrics.ascent(), k);
		painter.restore();
	}

	if (!kEntry) return;
	QString readings;
	QString meanings;

	QRect textRect(QPoint(kanjiSize, 0), size() - QSize(kanjiSize, 0));
	readings = fontMetrics().elidedText(kEntry->readings().join(", "), Qt::ElideRight, textRect.width());
	meanings = fontMetrics().elidedText(kEntry->meanings().join(", "), Qt::ElideRight, textRect.width());
	if (!meanings.isEmpty()) meanings[0] = meanings[0].toUpper();

	painter.drawText(textRect, 0, QString("%1\n%2").arg(meanings).arg(readings));
}

PreferenceItem<int> KanjiPopup::historySize("kanjidic", "popupHistorySize", 1000);
PreferenceItem<int> KanjiPopup::animationSize("kanjidic", "animationSize", 100);
PreferenceItem<bool> KanjiPopup::autoStartAnim("kanjidic", "autoStartAnim", true);

KanjiPopup::KanjiPopup(QWidget *parent) : QFrame(parent), _history(historySize.value()), entryView()
{
	setupUi(this);

	connect(prevButton, SIGNAL(clicked()), this, SLOT(onPreviousClick()));
	connect(nextButton, SIGNAL(clicked()), this, SLOT(onNextClick()));
	connect(openButton, SIGNAL(clicked()), this, SLOT(onOpenClick()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
	QMenu *menu = new QMenu(moreButton);
	entryView.populateMenu(menu);
	moreButton->setMenu(menu);

	stroke->setPictureSize(animationSize.value());

	connect(&entryView, SIGNAL(entryChanged(Entry *)), &entryView, SLOT(updateMenuEntries()));
	connect(&entryView, SIGNAL(entryChanged(Entry *)), this, SLOT(updateInfo()));
	connect(stroke, SIGNAL(componentHighlighted(const KanjiComponent*)), this, SLOT(onComponentHighlighted(const KanjiComponent*)));
	connect(stroke, SIGNAL(componentUnHighlighted(const KanjiComponent*)), this, SLOT(onComponentUnHighlighted(const KanjiComponent*)));
	connect(stroke, SIGNAL(componentClicked(const KanjiComponent*)), this, SLOT(onComponentClicked(const KanjiComponent*)));
}

KanjiPopup::~KanjiPopup()
{
}

void KanjiPopup::showKanji(Kanjidic2Entry *entry)
{
	QString str;

	if (!entry) return;

	entryView.setEntry(entry);

	prevButton->setEnabled(_history.hasPrevious());
	nextButton->setEnabled(_history.hasNext());

	if (entry->meanings().isEmpty()) str = tr("No information about this kanji!");
	else {
		str = entry->meaningsString();
		if (!str.isEmpty()) str[0] = str[0].toUpper();
	}

	meaningsLabel->setText(meaningsLabel->fontMetrics().elidedText(str, Qt::ElideRight, meaningsLabel->size().width()));
	readingsLabel->setText(readingsLabel->fontMetrics().elidedText(entry->readings().join(", "), Qt::ElideRight, readingsLabel->size().width()));

	updateInfo();

	stroke->stop();
	stroke->setKanji(entry);
	if (autoStartAnim.value()) stroke->play();
	else stroke->setPosition(entry->strokeCount());

	compWidget->setComponent(0);

//	adjustSize();
}

void KanjiPopup::updateInfo()
{
	const Kanjidic2Entry *entry(static_cast<const Kanjidic2Entry *>(entryView.entry()));
	QString str;
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
	if (component) compWidget->setComponent(component);
	stroke->highlightComponent(component);
}

void KanjiPopup::onComponentUnHighlighted(const KanjiComponent *component)
{
	stroke->unHighlightComponent();
	compWidget->setComponent(0);
}

void KanjiPopup::onComponentClicked(const KanjiComponent *component)
{
	EntryPointer<Entry> element = EntriesCache::get(KANJIDIC2ENTRY_GLOBALID, TextTools::singleCharToUnicode(component->element()));
	Kanjidic2Entry *kElement = static_cast<const Kanjidic2Entry *>(element.data());
	if (!kElement) return;
	_history.add(kElement->kanji());
	showKanji(kElement);
}
