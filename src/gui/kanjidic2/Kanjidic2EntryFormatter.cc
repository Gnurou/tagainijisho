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

#include "core/Paths.h"
#include "core/TextTools.h"
#include "gui/kanjidic2/Kanjidic2EntryFormatter.h"

#include "core/jmdict/JMdictEntry.h"
#include "core/jmdict/JMdictEntrySearcher.h"
#include "gui/kanjidic2/KanjiRenderer.h"
#include "gui/jmdict/JMdictEntryFormatter.h"
#include "gui/kanjidic2/Kanjidic2GUIPlugin.h"

#include <QUrl>
#include <QTextList>
#include <QTextCursor>
#include <QTextTable>
#include <QPainter>
#include <QToolTip>
#include <QTextBlock>
#include <QTextList>

PreferenceItem<bool> Kanjidic2EntryFormatter::showReadings("kanjidic", "showReadings", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::showNanori("kanjidic", "showNanori", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::showUnicode("kanjidic", "showUnicode", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::showSKIP("kanjidic", "showSKIP", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::showFourCorner("kanjidic", "showFourCorner", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::showJLPT("kanjidic", "showJLPT", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::showGrade("kanjidic", "showGrade", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::showRadicals("kanjidic", "showRadicals", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::showComponents("kanjidic", "showComponents", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::showStrokesNumber("kanjidic", "showStrokesNumber", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::showFrequency("kanjidic", "showFrequency", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::showVariations("kanjidic", "showVariations", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::showVariationOf("kanjidic", "showVariationOf", true);
PreferenceItem<int> Kanjidic2EntryFormatter::maxWordsToDisplay("kanjidic", "maxWordsToDisplay", 5);
PreferenceItem<bool> Kanjidic2EntryFormatter::showOnlyStudiedVocab("kanjidic", "showOnlyStudiedVocab", false);
PreferenceItem<int> Kanjidic2EntryFormatter::maxCompoundsToDisplay("kanjidic", "maxParentKanjiToDisplay", 30);
PreferenceItem<bool> Kanjidic2EntryFormatter::showOnlyStudiedCompounds("kanjidic", "showOnlyStudiedComponents", false);

PreferenceItem<bool> Kanjidic2EntryFormatter::tooltipShowScore("kanjidic", "tooltipShowScore", false);
PreferenceItem<bool> Kanjidic2EntryFormatter::tooltipShowUnicode("kanjidic", "tooltipShowUnicode", false);
PreferenceItem<bool> Kanjidic2EntryFormatter::tooltipShowSKIP("kanjidic", "tooltipShowSKIP", false);
PreferenceItem<bool> Kanjidic2EntryFormatter::tooltipShowFourCorner("kanjidic", "tooltipShowFourCorner", false);
PreferenceItem<bool> Kanjidic2EntryFormatter::tooltipShowJLPT("kanjidic", "tooltipShowJLPT", false);
PreferenceItem<bool> Kanjidic2EntryFormatter::tooltipShowGrade("kanjidic", "tooltipShowGrade", false);
PreferenceItem<bool> Kanjidic2EntryFormatter::tooltipShowStrokesNumber("kanjidic", "tooltipShowStrokesNumber", false);
PreferenceItem<bool> Kanjidic2EntryFormatter::tooltipShowFrequency("kanjidic", "tooltipShowFrequency", false);

PreferenceItem<int> Kanjidic2EntryFormatter::printSize("kanjidic", "printSize", 80);
PreferenceItem<bool> Kanjidic2EntryFormatter::printWithFont("kanjidic", "printWithFont", false);
PreferenceItem<bool> Kanjidic2EntryFormatter::printMeanings("kanjidic", "printMeanings", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::printOnyomi("kanjidic", "printOnyomi", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::printKunyomi("kanjidic", "printKunyomi", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::printComponents("kanjidic", "printComponents", true);
PreferenceItem<bool> Kanjidic2EntryFormatter::printOnlyStudiedComponents("kanjidic", "printOnlyStudiedComponents", false);
PreferenceItem<int> Kanjidic2EntryFormatter::maxWordsToPrint("kanjidic", "maxWordsToPrint", 5);
PreferenceItem<bool> Kanjidic2EntryFormatter::printOnlyStudiedVocab("kanjidic", "printOnlyStudiedVocab", false);
PreferenceItem<bool> Kanjidic2EntryFormatter::printStrokesNumbers("kanjidic", "printStrokesNumbers", false);
PreferenceItem<int> Kanjidic2EntryFormatter::strokesNumbersSize("kanjidic", "strokesNumbersSize", 6);
PreferenceItem<bool> Kanjidic2EntryFormatter::printGrid("kanjidic", "printStrokesGrid", false);

QString Kanjidic2EntryFormatter::getQueryUsedInWordsSql(int kanji, int limit, bool onlyStudied)
{
	const QString queryUsedInWordsSql("select distinct " QUOTEMACRO(JMDICTENTRY_GLOBALID) ", jmdict.entries.id from jmdict.entries join jmdict.kanjiChar on jmdict.kanjiChar.id = jmdict.entries.id join jmdict.senses on jmdict.senses.id = jmdict.entries.id %3join training on training.id = jmdict.entries.id and training.type = " QUOTEMACRO(JMDICTENTRY_GLOBALID) " left join jmdict.jlpt on jmdict.entries.id = jmdict.jlpt.id where jmdict.kanjiChar.kanji = %1 and jmdict.kanjiChar.priority = 0 and jmdict.senses.misc & %4 == 0 order by training.dateAdded is null ASC, training.score ASC, jmdict.jlpt.level DESC, jmdict.entries.frequency DESC limit %2");

	return queryUsedInWordsSql.arg(kanji).arg(limit).arg(onlyStudied ? "" : "left ").arg(JMdictEntrySearcher::miscFilterMask());
}

QString Kanjidic2EntryFormatter::getQueryUsedInKanjiSql(int kanji, int limit, bool onlyStudied)
{
	const QString queryUsedInKanjiSql("select distinct " QUOTEMACRO(KANJIDIC2ENTRY_GLOBALID) ", ks1.kanji from kanjidic2.strokeGroups as ks1 join kanjidic2.entries on ks1.isRoot == 1 and ks1.kanji = entries.id %3join training on training.type = " QUOTEMACRO(KANJIDIC2ENTRY_GLOBALID) " and training.id = entries.id where (ks1.element = %1 or ks1.original = %1) and ks1.kanji != %1 order by training.dateAdded is null ASC, training.score ASC, entries.strokeCount, entries.jlpt DESC limit %2");

	return queryUsedInKanjiSql.arg(kanji).arg(limit).arg(onlyStudied ? "" : "left ");
}

Kanjidic2EntryFormatter &Kanjidic2EntryFormatter::instance()
{
	static Kanjidic2EntryFormatter _instance;
	return _instance;
}

Kanjidic2EntryFormatter::Kanjidic2EntryFormatter(QObject* parent) : EntryFormatter("detailed_kanjidic2.css", "detailed_kanjidic2.html", parent)
{
}

void Kanjidic2EntryFormatter::writeJapanese(const ConstKanjidic2EntryPointer &entry, QTextCursor &cursor, DetailedView *view) const
{
	QTextBlockFormat header;
	header.setAlignment(Qt::AlignHCenter);
	cursor.setBlockFormat(header);

	// Display the kanji
	cursor.setBlockCharFormat(DetailedViewFonts::charFormat(DetailedViewFonts::KanjiHeader));
	cursor.insertText(entry->kanji());
	cursor.setCharFormat(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
}

void Kanjidic2EntryFormatter::writeTranslation(const ConstKanjidic2EntryPointer& entry, QTextCursor& cursor, DetailedView* view) const
{
	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	QTextCharFormat kana(DetailedViewFonts::charFormat(DetailedViewFonts::Kana));
	QTextCharFormat bold(normal);
	bold.setFontWeight(QFont::Bold);

	QTextBlockFormat header;
	header.setAlignment(Qt::AlignHCenter);
	cursor.setCharFormat(QTextCharFormat());

	// Meanings
	if (!entry->kanjiMeanings().isEmpty()) {
		cursor.setBlockFormat(header);
		cursor.setCharFormat(normal);
		QString s = entry->meaningsString();
		if (!s.isEmpty()) s[0] = s[0].toUpper();
		cursor.insertText(s);
		cursor.insertBlock(QTextBlockFormat());
	}

	if (showReadings.value()) {
		// Readings
		QStringList on = entry->onyomiReadings();
		if (!on.isEmpty()) {
			cursor.insertBlock(QTextBlockFormat());
			cursor.setCharFormat(bold);
			cursor.insertText(tr("On:"));
			cursor.setCharFormat(normal);
			cursor.insertText(" ");
			for (int i = 0; i < on.size(); i++) {
				if (i != 0) cursor.insertText(", ");
				cursor.setCharFormat(kana);
				cursor.insertText(on[i]);
				cursor.setCharFormat(normal);
			}
		}

		QStringList kun = entry->kunyomiReadings();
		if (!kun.isEmpty()) {
			cursor.insertBlock(QTextBlockFormat());
			cursor.setCharFormat(bold);
			cursor.insertText(tr("Kun:"));
			cursor.setCharFormat(normal);
			cursor.insertText(" ");
			for (int i = 0; i < kun.size(); i++) {
				if (i != 0) cursor.insertText(", ");
				cursor.setCharFormat(kana);
				cursor.insertText(kun[i]);
				cursor.setCharFormat(normal);
			}
		}
		if (showNanori.value()) {
			// Nanoris
			QStringList nano = entry->nanoris();
			if (!nano.isEmpty()) {
				cursor.insertBlock(QTextBlockFormat());
				cursor.setCharFormat(bold);
				cursor.insertText(tr("Nanori:"));
				cursor.setCharFormat(normal);
				cursor.insertText(" ");
				for (int i = 0; i < nano.size(); i++) {
					if (i != 0) cursor.insertText(", ");
					cursor.setCharFormat(kana);
					cursor.insertText(nano[i]);
					cursor.setCharFormat(normal);
				}
			}
		}
	}
}

void Kanjidic2EntryFormatter::writeKanjiInfo(const ConstKanjidic2EntryPointer& entry, QTextCursor& cursor, DetailedView* view) const
{
	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	QTextCharFormat kana(DetailedViewFonts::charFormat(DetailedViewFonts::Kana));
	QTextCharFormat kanjiF(DetailedViewFonts::charFormat(DetailedViewFonts::Kanji));
	QTextCharFormat bold(normal);
	bold.setFontWeight(QFont::Bold);

	QTextTableFormat tableFormat;
	tableFormat.setBorder(0);
	tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));
	QTextTable *table = cursor.insertTable(1, 2, tableFormat);
	int cellCpt = 0;
	if (entry->strokeCount() != -1 && showStrokesNumber.value()) {
		cursor.setCharFormat(bold);
		cursor.insertText(tr("Strokes:"));
		cursor.setCharFormat(normal);
		cursor.insertText(tr(" %1").arg(entry->strokeCount()));
		if (++cellCpt % 2 == 0) { table->insertRows(table->rows(), 1); cursor.movePosition(QTextCursor::PreviousBlock); }
		else cursor.movePosition(QTextCursor::NextBlock);
	}
	if (entry->frequency() != -1 && showFrequency.value()) {
		cursor.setCharFormat(bold);
		cursor.insertText(tr("Frequency:"));
		cursor.setCharFormat(normal);
		cursor.insertText(tr(" %1").arg(entry->frequency()));
		if (++cellCpt % 2 == 0) { table->insertRows(table->rows(), 1); cursor.movePosition(QTextCursor::PreviousBlock); }
		else cursor.movePosition(QTextCursor::NextBlock);
	}
	if (entry->grade() != -1 && showGrade.value()) {
		cursor.setCharFormat(bold);
		cursor.insertText(tr("Grade:"));
		cursor.setCharFormat(normal);
		cursor.insertText(tr(" %1").arg(tr(Kanjidic2GUIPlugin::kanjiGrades[entry->grade()].toLatin1())));
		cursor.setCharFormat(normal);
		if (++cellCpt % 2 == 0) { table->insertRows(table->rows(), 1); cursor.movePosition(QTextCursor::PreviousBlock); }
		else cursor.movePosition(QTextCursor::NextBlock);
	}
	if (entry->jlpt() != -1 && showJLPT.value()) {
		cursor.setCharFormat(bold);
		cursor.insertText(tr("JLPT:"));
		cursor.setCharFormat(normal);
		cursor.insertText(tr(" %1").arg(entry->jlpt()));
		if (++cellCpt % 2 == 0) { table->insertRows(table->rows(), 1); cursor.movePosition(QTextCursor::PreviousBlock); }
		else cursor.movePosition(QTextCursor::NextBlock);
	}
	if (showVariations.value()) {
		QSqlQuery query;
		query.prepare("select distinct element from strokeGroups where original = ?");
		query.addBindValue(entry->id());
		query.exec();
		if (query.next()) {
			cursor.setCharFormat(bold);
			cursor.insertText(tr("Variations:"));
			cursor.setCharFormat(normal);
			QList<EntryPointer> entries;
			do entries << KanjiEntryRef(query.value(0).toUInt()).get();
			while (query.next());

			for (int i = 0; i < entries.size(); i++) {
				cursor.insertText(" ");
				ConstKanjidic2EntryPointer kEntry(entries[i].staticCast<const Kanjidic2Entry>());
				const EntryFormatter *formatter(EntryFormatter::getFormatter(kEntry));
				formatter->writeEntryTitle(kEntry, cursor);
			}
			if (++cellCpt % 2 == 0) { table->insertRows(table->rows(), 1); cursor.movePosition(QTextCursor::PreviousBlock); }
			else cursor.movePosition(QTextCursor::NextBlock);
		}
	}
	if (showVariationOf.value() && !entry->variationOf().isEmpty()) {
		cursor.setCharFormat(bold);
		cursor.insertText(tr("Variation of:"));
		cursor.setCharFormat(normal);
		foreach (quint32 kid, entry->variationOf()) {
			cursor.insertText(" ");
			ConstKanjidic2EntryPointer kEntry(KanjiEntryRef(kid).get().staticCast<const Kanjidic2Entry>());
			const EntryFormatter *formatter(EntryFormatter::getFormatter(kEntry));
			formatter->writeEntryTitle(kEntry, cursor);
		}
		if (++cellCpt % 2 == 0) { table->insertRows(table->rows(), 1); cursor.movePosition(QTextCursor::PreviousBlock); }
		else cursor.movePosition(QTextCursor::NextBlock);
	}
	if (showUnicode.value()) {
		cursor.setCharFormat(bold);
		cursor.insertText(tr("Unicode:"));
		cursor.setCharFormat(normal);
		cursor.insertText(tr(" 0x%1").arg(QString::number(TextTools::singleCharToUnicode(entry->kanji()), 16)));
		if (++cellCpt % 2 == 0) { table->insertRows(table->rows(), 1); cursor.movePosition(QTextCursor::PreviousBlock); }
		else cursor.movePosition(QTextCursor::NextBlock);
	}
	if (showSKIP.value() && !entry->skipCode().isEmpty()) {
		cursor.setCharFormat(bold);
		cursor.insertText(tr("SKIP:"));
		cursor.setCharFormat(normal);
		cursor.insertText(tr(" %1").arg(entry->skipCode()));
		if (++cellCpt % 2 == 0) { table->insertRows(table->rows(), 1); cursor.movePosition(QTextCursor::PreviousBlock); }
		else cursor.movePosition(QTextCursor::NextBlock);
	}
	if (showFourCorner.value() && !entry->fourCorner().isEmpty()) {
		cursor.setCharFormat(bold);
		cursor.insertText(tr("4 corner:"));
		cursor.setCharFormat(normal);
		cursor.insertText(tr(" %1").arg(entry->fourCorner()));
		if (++cellCpt % 2 == 0) { table->insertRows(table->rows(), 1); cursor.movePosition(QTextCursor::PreviousBlock); }
		else cursor.movePosition(QTextCursor::NextBlock);
	}

	// End of text in table
	if (cellCpt % 2 == 0) table->removeRows(table->rows() - 1, 1);

	cursor.movePosition(QTextCursor::NextBlock);
	if (showRadicals.value() && !entry->radicals().isEmpty()) {
		cursor.setCharFormat(bold);
		cursor.insertText(tr("Radicals:"));
		cursor.setCharFormat(normal);
		typedef QPair<uint, quint8> radicalType;
		foreach (const radicalType &radical, entry->radicals()) {
			cursor.insertText(" ");
			ConstKanjidic2EntryPointer kEntry(KanjiEntryRef(radical.first).get());
			view->addWatchEntry(kEntry);
			const EntryFormatter *formatter(EntryFormatter::getFormatter(kEntry));
			formatter->writeEntryTitle(kEntry, cursor);
			cursor.insertText(QString(" (%1)").arg(radical.second));
		}
	}
	QList<const KanjiComponent *> components(entry->rootComponents());
	if (!components.isEmpty() && showComponents.value()) {
		QList<const KanjiComponent *> singleComponents;
		cursor.insertBlock(QTextBlockFormat());
		cursor.setCharFormat(bold);
		cursor.insertText(tr("Components:"));
		cursor.setCharFormat(normal);
		QSet<uint> listedComps;
		foreach (const KanjiComponent *component, components) {
			if (listedComps.contains(component->unicode())) continue;
			listedComps << component->unicode();
			cursor.insertText("\n");
			ConstKanjidic2EntryPointer kEntry(getMeaningEntry(component));
			ConstKanjidic2EntryPointer shape(getShapeEntry(component));
			view->addWatchEntry(kEntry);
			writeShortDesc(shape, kEntry, cursor);
		}
	}
	if (maxCompoundsToDisplay.value()) view->addBackgroundJob(new ShowUsedInKanjiJob(entry->kanji(), cursor));
	if (maxWordsToDisplay.value()) view->addBackgroundJob(new ShowUsedInWordsJob(entry->kanji(), cursor));
}

void Kanjidic2EntryFormatter::writeShortDesc(const ConstEntryPointer& entry, QTextCursor& cursor) const
{
	ConstKanjidic2EntryPointer kEntry(entry.staticCast<const Kanjidic2Entry>());
	writeShortDesc(kEntry, kEntry, cursor);
}

void Kanjidic2EntryFormatter::writeShortDesc(const ConstKanjidic2EntryPointer& shape, const ConstKanjidic2EntryPointer &kEntry, QTextCursor& cursor) const
{
	QTextCharFormat charFormat;
	if (kEntry->trained()) charFormat.setBackground(kEntry->scoreColor());
	QString str(kEntry->kanji());
	if (shape != kEntry) str += tr(" (%1) ").arg(shape->kanji());
	if (!kEntry->meanings().isEmpty()) str += ": " + kEntry->meaningsString();
	autoFormat(kEntry, str, cursor, charFormat);
	if (shortDescShowJLPT.value() && kEntry->jlpt() != -1) {
		charFormat.setFontWeight(QFont::Bold);
		autoFormat(kEntry, tr(" (JLPT %1)").arg(kEntry->jlpt()), cursor, charFormat);
	}
	QTextImageFormat imgFormat;
	imgFormat.setAnchor(true);
	imgFormat.setAnchorHref(QString("entry://?type=%1&id=%2").arg(kEntry->type()).arg(kEntry->id()));
	imgFormat.setName("moreicon");
	cursor.insertImage(imgFormat);
}

void Kanjidic2EntryFormatter::_detailedVersion(const ConstEntryPointer& _entry, QTextCursor& cursor, DetailedView* view) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	writeJapanese(entry, cursor, view);
	cursor.insertBlock(QTextBlockFormat());
	writeTranslation(entry, cursor, view);
	cursor.insertBlock(QTextBlockFormat());
	writeKanjiInfo(entry, cursor, view);
}

void Kanjidic2EntryFormatter::draw(const ConstEntryPointer &entry, QPainter &painter, const QRectF &rectangle, QRectF &usedSpace, const QFont &textFont) const
{
	drawCustom(entry.staticCast<const Kanjidic2Entry>(), painter, rectangle, usedSpace, textFont);
}

void Kanjidic2EntryFormatter::drawCustom(const ConstKanjidic2EntryPointer& entry, QPainter& painter, const QRectF& rectangle, QRectF& usedSpace, const QFont& textFont, int printSize, bool printWithFont, bool printMeanings, bool printOnyomi, bool printKunyomi, bool printComponents, bool printOnlyStudiedComponents, int maxWordsToPrint, bool printOnlyStudiedVocab, bool printStrokesNumbers, int printStrokesNumbersSize, bool printGrid) const
{
	QFont kanjiFont;
	kanjiFont.setPointSizeF(textFont.pointSize() * 5);
	QFont boldFont(textFont);
	boldFont.setBold(true);
	QFont italFont(textFont);
	italFont.setItalic(true);

	QRectF leftArea = rectangle;
	leftArea.setWidth(rectangle.width() / 3.5);
	// Adjust font size if needed
	while (QFontMetrics(kanjiFont, painter.device()).maxWidth() > leftArea.width())
		kanjiFont.setPointSize(kanjiFont.pointSize() - 1);

	QRectF textBB;
	// Render the grid, if relevant
	if (printGrid) {
		KanjiRenderer renderer(entry);
		painter.save();
		QPen pen;
		pen.setWidth(2);
		pen.setColor(Qt::gray);
		painter.setPen(pen);
		painter.translate((leftArea.width() - printSize) / 2.0, 0.0);
		painter.scale(printSize / 109.0, printSize / 109.0);
		painter.setRenderHint(QPainter::Antialiasing);

		renderer.renderGrid(&painter);
		painter.restore();
	}
	// Draw the kanji
	if (!printWithFont) {
		//painter.setFont(kanjiFont);
		KanjiRenderer renderer(entry);
		painter.save();
		QPen pen(painter.pen());
		pen.setWidth(5);
		pen.setCapStyle(Qt::SquareCap);
		painter.translate((leftArea.width() - printSize) / 2.0, 0.0);
		painter.scale(printSize / 109.0, printSize / 109.0);
		painter.setRenderHint(QPainter::Antialiasing);

		foreach (const KanjiStroke &stroke, entry->strokes()) {
			painter.setPen(pen);
			renderer.strokeFor(stroke)->render(&painter);
		}
		painter.restore();
		textBB.setTop(leftArea.top());
		textBB.setBottom(leftArea.top() + printSize);
	} else {
		painter.save();
		QFont font;
		font.setPixelSize(printSize);
		painter.setFont(font);
		textBB = painter.boundingRect(leftArea, Qt::AlignHCenter | Qt::AlignTop, entry->kanji());
		painter.drawText(leftArea, Qt::AlignHCenter | Qt::AlignTop, entry->kanji());
		painter.restore();
	}
	
	if (printStrokesNumbers) {
		KanjiRenderer renderer(entry);
		painter.save();
		painter.translate((leftArea.width() - printSize) / 2.0, 0.0);
		painter.scale(printSize / 109.0, printSize / 109.0);
		painter.setRenderHint(QPainter::Antialiasing);

		foreach (const KanjiStroke &stroke, entry->strokes()) {
			renderer.renderStrokeNumber(stroke, &painter, printStrokesNumbersSize);
		}
		painter.restore();
	}

	leftArea.setTop(textBB.bottom());

	if (printComponents) {
		// Draw the components
		painter.setFont(textFont);
		foreach(const KanjiComponent *c, entry->rootComponents()) {
			ConstKanjidic2EntryPointer original(getMeaningEntry(c));
			ConstKanjidic2EntryPointer element(getShapeEntry(c));
			
			if (printOnlyStudiedComponents && !original->trained()) continue;
			QString s = original->kanji();
			if (original != element) s += tr(" (%1) ").arg(element->kanji());
			QString meanings(original->meaningsString());
			if (!meanings.isEmpty()) s += ": " + meanings;
			QFontMetrics metrics(painter.font(), painter.device());
			s = metrics.elidedText(s, Qt::ElideRight, leftArea.width());
			textBB = painter.boundingRect(leftArea, Qt::AlignLeft, s);
			painter.drawText(leftArea, Qt::AlignLeft, s);
			if (!printOnlyStudiedComponents && original->trained()) painter.drawLine(textBB.topLeft() + QPoint(0, metrics.ascent() + metrics.underlinePos()), textBB.topRight() + QPoint(0, metrics.ascent() + metrics.underlinePos()));
			leftArea.setTop(textBB.bottom());
		}
	}
	
	QRectF rightArea = rectangle;
	QString str;
	rightArea.setTopLeft(QPointF(rectangle.left() + leftArea.width() + leftArea.width() / 20.0, rectangle.top()));
	// Print the meanings
	if (printMeanings) {
		painter.setFont(italFont);
		str = entry->meaningsString();
		if (!str.isEmpty()) str[0] = str[0].toUpper();
		str = QFontMetrics(painter.font(), painter.device()).elidedText(str, Qt::ElideRight, rightArea.width());
		textBB = painter.boundingRect(rightArea, Qt::AlignHCenter, str);
		painter.drawText(textBB, Qt::AlignHCenter, str);
		rightArea.setTop(textBB.bottom());
	}

	QRectF tempBox(rightArea);
	if (printOnyomi) {
		QStringList onRead = entry->onyomiReadings();
		int onLength = 0;
		if (!onRead.isEmpty()) {
			painter.setFont(boldFont);
			str = "On:";
			textBB = painter.boundingRect(rightArea, Qt::AlignLeft, str);
			onLength += textBB.width();
			painter.drawText(textBB, Qt::AlignLeft, str);
			painter.setFont(textFont);
			tempBox.setLeft(textBB.right());
			tempBox.setRight(rightArea.center().x());
			str = " " + onRead.join(", ");
			str = QFontMetrics(painter.font(), painter.device()).elidedText(str, Qt::ElideRight, tempBox.width());
			onLength += painter.boundingRect(tempBox, Qt::AlignLeft, str).width();
			painter.drawText(tempBox, Qt::AlignLeft, str);
			rightArea.setTop(textBB.bottom());
		}
	}

	if (printKunyomi) {
		QStringList kunRead = entry->kunyomiReadings();
		if (!kunRead.isEmpty()) {
			tempBox.setLeft(rightArea.center().x());
			tempBox.setRight(rightArea.right());
			str = "Kun:";
			painter.setFont(boldFont);
			textBB = painter.boundingRect(tempBox, Qt::AlignLeft, str);
			painter.drawText(tempBox, Qt::AlignLeft, str);
			painter.setFont(textFont);
			tempBox.setLeft(textBB.right());
			str = " " + kunRead.join(", ");
			str = QFontMetrics(painter.font(), painter.device()).elidedText(str, Qt::ElideRight, tempBox.width());
			painter.drawText(tempBox, Qt::AlignLeft, str);
			rightArea.setTop(textBB.bottom());
		}
	}

	// Now display words using this kanji
	if (maxWordsToPrint) {
		QSqlQuery query;
		query.exec(getQueryUsedInWordsSql(entry->id(), maxWordsToPrint, printOnlyStudiedVocab));
		painter.setFont(textFont);
		while (query.next()) {
			ConstJMdictEntryPointer jmEntry(JMdictEntryRef(query.value(1).toInt()).get());

			QString str = QFontMetrics(painter.font(), painter.device()).elidedText(jmEntry->shortVersion(Entry::TinyVersion), Qt::ElideRight, rightArea.width());
			textBB = painter.boundingRect(rightArea, Qt::AlignLeft, str);
			painter.drawText(textBB, Qt::AlignLeft, str);
			rightArea.setTop(textBB.bottom());
		}
	}
	drawInfo(entry, painter, rightArea, textFont);

	usedSpace = rectangle;
	usedSpace.setHeight(qMax(leftArea.top(), rightArea.top()) - rectangle.top());

	painter.drawLine(QPointF(rectangle.left() + leftArea.width(), usedSpace.top()),
					 QPointF(rectangle.left() + leftArea.width(), usedSpace.bottom()));
}


void Kanjidic2EntryFormatter::detailedVersionPart1(const ConstEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const
{
	writeJapanese(entry.staticCast<const Kanjidic2Entry>(), cursor, view);
}

void Kanjidic2EntryFormatter::detailedVersionPart2(const ConstEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const
{
	writeTranslation(entry.staticCast<const Kanjidic2Entry>(), cursor, view);
}

void Kanjidic2EntryFormatter::showToolTip(const ConstKanjidic2EntryPointer entry, const QPoint &pos) const
{
	QString writing(entry->kanji()), alt;
	if (!entry->readings().isEmpty()) {
		alt = "(" + entry->readings().join(", ") + ")";
	}

	QString s;
	if (tooltipShowScore.value() && entry->trained()) {
		QColor scoreColor(entry->scoreColor());
		s += QString("<div style=\"font-size: xx-large; background-color: #%1%2%3;\">%4</div> ").arg(QString::number(scoreColor.red(), 16)).arg(QString::number(scoreColor.green(), 16)).arg(QString::number(scoreColor.blue(), 16)).arg(writing);
	} else s += QString("<div style=\"font-size: xx-large;\">%1</div> ").arg(writing);
	s += alt;
	if (!entry->meanings().isEmpty()) {
		QString s2 = entry->meaningsString();
		if (!s2.isEmpty()) s2[0] = s2[0].toUpper();
		s += "<br/>" + s2;
	}
	s += "<table border=\"0\" width=\"100%\">";
	int tCpt = 0;
	#define BODY(text) { \
		QString body(tr("<b>Strokes:</b> %1").arg(entry->strokeCount())); \
		if (tCpt % 2) s += "<td>" + body + "</td></tr>"; \
		else s += "<tr><td>" + body + "</td>"; \
		++tCpt; }

	if (entry->strokeCount() != -1 && tooltipShowStrokesNumber.value()) {
		BODY(tr("<b>Strokes:</b> %1").arg(entry->strokeCount()));
	}
	if (entry->frequency() != -1 && tooltipShowFrequency.value()) {
		QString body(tr("<b>Frequency:</b> %1").arg(entry->frequency()));
		if (tCpt % 2) s += "<td>" + body + "</td></tr>";
		else s += "<tr><td>" + body + "</td>";
		++tCpt;
	}
	if (entry->grade() != -1 && tooltipShowGrade.value()) {
		QString body(tr("<b>Grade:</b> %1").arg(tr(Kanjidic2GUIPlugin::kanjiGrades[entry->grade()].toLatin1())));
		if (tCpt % 2) s += "<td>" + body + "</td></tr>";
		else s += "<tr><td>" + body + "</td>";
		++tCpt;
	}
	if (entry->jlpt() != -1 && tooltipShowJLPT.value()) {
		QString body(tr("<b>JLPT:</b> %1").arg(entry->jlpt()));
		if (tCpt % 2) s += "<td>" + body + "</td></tr>";
		else s += "<tr><td>" + body + "</td>";
		++tCpt;
	}
	if (tooltipShowUnicode.value()) {
		QString body(tr("<b>Unicode:</b> 0x%1").arg(QString::number(TextTools::singleCharToUnicode(entry->kanji()), 16)));
		if (tCpt % 2) s += "<td>" + body + "</td></tr>";
		else s += "<tr><td>" + body + "</td>";
		++tCpt;
	}
	if (tooltipShowSKIP.value() && !entry->skipCode().isEmpty()) {
		QString body(tr("<b>SKIP:</b> %1").arg(entry->skipCode()));
		if (tCpt % 2) s += "<td>" + body + "</td></tr>";
		else s += "<tr><td>" + body + "</td>";
		++tCpt;
	}
	if (tooltipShowFourCorner.value() && !entry->fourCorner().isEmpty()) {
		QString body(tr("<b>4 corner:</b> %1").arg(entry->fourCorner()));
		if (tCpt % 2) s += "<td>" + body + "</td></tr>";
		else s += "<tr><td>" + body + "</td>";
		++tCpt;
	}
	if (tCpt % 2) s += "<td></td></tr>";
	s += "</table>";
	QToolTip::showText(pos, s);
}

ConstKanjidic2EntryPointer Kanjidic2EntryFormatter::getShapeEntry(const KanjiComponent *comp) const
{
	ConstKanjidic2EntryPointer component(KanjiEntryRef(comp->unicode()).get());
	return component;
}

ConstKanjidic2EntryPointer Kanjidic2EntryFormatter::getMeaningEntry(const KanjiComponent *comp) const
{
	ConstKanjidic2EntryPointer component;
	// If the component has an original, get the meaning from it!
	if (!comp->original().isEmpty()) component = KanjiEntryRef(TextTools::singleCharToUnicode(comp->original())).get();
	else component = KanjiEntryRef(comp->unicode()).get();
	return component;
}

QString Kanjidic2EntryFormatter::shortDesc(const ConstEntryPointer &entry) const
{
	ConstKanjidic2EntryPointer kEntry(entry.staticCast<const Kanjidic2Entry>());
	return shortDesc(kEntry, kEntry);
}

QString Kanjidic2EntryFormatter::shortDesc(const ConstKanjidic2EntryPointer &shape, const ConstKanjidic2EntryPointer &entry) const
{
	QString ret;
	if (entry->trained()) ret += QString("<span style=\"background-color:%1\">").arg(colorTriplet(entry->scoreColor()));

	QString str(entry->kanji());
	if (shape != entry) str += QString(" (%1) ").arg(shape->kanji());
	if (!entry->meanings().isEmpty()) str += ": " + entry->meaningsString();
	ret += autoFormat(str);
	if (shortDescShowJLPT.value() && entry->jlpt() != -1)
		ret += QString("<b>%1</b>").arg(autoFormat(tr(" (JLPT %1)").arg(entry->jlpt())));
	ret += QString(" <a href=\"entry://?type=%1&id=%2\"><img src=\"moreicon\"/></a>").arg(entry->type()).arg(entry->id());
	if (entry->trained()) ret += "</span>";
	return ret;
}

ShowUsedInKanjiJob::ShowUsedInKanjiJob(const QString &kanji, const QTextCursor &cursor) :
		DetailedViewJob(Kanjidic2EntryFormatter::getQueryUsedInKanjiSql(TextTools::singleCharToUnicode(kanji), Kanjidic2EntryFormatter::maxCompoundsToDisplay.value(), Kanjidic2EntryFormatter::showOnlyStudiedCompounds.value()), cursor), _kanji(kanji), _gotResult(false)
{
}

void ShowUsedInKanjiJob::firstResult()
{
	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	QTextCharFormat bold(normal);
	bold.setFontWeight(QFont::Bold);

	_gotResult = true;
	cursor().insertBlock(QTextBlockFormat());
	cursor().setCharFormat(bold);
	cursor().insertText(tr("Direct compounds:"));
	cursor().setCharFormat(normal);
}

void ShowUsedInKanjiJob::result(EntryPointer entry)
{
	ConstKanjidic2EntryPointer kEntry = entry.staticCast<const Kanjidic2Entry>();
	Q_ASSERT(kEntry);
	
	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	cursor().setCharFormat(normal);
	cursor().insertText(" ");
	const EntryFormatter *formatter(EntryFormatter::getFormatter(kEntry));
	formatter->writeEntryTitle(kEntry, cursor());
}

void ShowUsedInKanjiJob::completed()
{
	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	if (!_gotResult) return;
	QTextCursor &cursor = ShowUsedInKanjiJob::cursor();
	cursor.setCharFormat(normal);
	cursor.insertText(" ");
	QTextCharFormat linkFormat(normal);
	linkFormat.setFontUnderline(true);
	linkFormat.setForeground(Qt::blue);
	linkFormat.setAnchor(true);
	linkFormat.setAnchorHref(QString("component:?reset=true&kanji=%1").arg(_kanji));
	linkFormat.setToolTip(tr("Make a new search using only this filter"));
	_cursor.setCharFormat(linkFormat);
	_cursor.insertText(tr("All compounds"));
	_cursor.setCharFormat(normal);
	_cursor.insertText(" ");
	linkFormat.setAnchorHref(QString("component:?kanji=%1").arg(_kanji));
	linkFormat.setToolTip(tr("Add this filter to the current search"));
	_cursor.setCharFormat(linkFormat);
	_cursor.insertText(tr("(+)"));
	_cursor.setCharFormat(normal);
}

ShowUsedInWordsJob::ShowUsedInWordsJob(const QString &kanji, const QTextCursor &cursor) :
		DetailedViewJob(Kanjidic2EntryFormatter::getQueryUsedInWordsSql(TextTools::singleCharToUnicode(kanji), Kanjidic2EntryFormatter::maxWordsToDisplay.value(), Kanjidic2EntryFormatter::showOnlyStudiedVocab.value()), cursor), _kanji(kanji), _gotResult(false)
{
}

void ShowUsedInWordsJob::firstResult()
{
	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	QTextCharFormat bold(normal);
	bold.setFontWeight(QFont::Bold);

	_gotResult = true;
	cursor().insertBlock(QTextBlockFormat());
	cursor().setCharFormat(bold);
	cursor().insertText(tr("Seen in:"));
	cursor().setCharFormat(normal);
}

void ShowUsedInWordsJob::result(EntryPointer entry)
{
	ConstJMdictEntryPointer jmEntry(entry.staticCast<const JMdictEntry>());
	Q_ASSERT(jmEntry);

	QTextList *currentList = cursor().currentList();
	cursor().insertBlock(QTextBlockFormat());
	cursor().setCharFormat(QTextCharFormat());
	const EntryFormatter *formatter(EntryFormatter::getFormatter(jmEntry));
	formatter->writeShortDesc(jmEntry, cursor());
	if (!currentList) {
		QTextListFormat listFormat;
		listFormat.setStyle(QTextListFormat::ListDisc);
		listFormat.setIndent(0);
		cursor().createList(listFormat);
	}
	else currentList->add(cursor().block());
}

void ShowUsedInWordsJob::completed()
{
	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));

	if (!_gotResult) return;
	QTextCursor &_cursor = cursor();
	_cursor.insertBlock(QTextBlockFormat());
	QTextCharFormat linkFormat(normal);
	linkFormat.setFontUnderline(true);
	linkFormat.setForeground(Qt::blue);
	linkFormat.setAnchor(true);
	linkFormat.setAnchorHref(QString("allwords:?reset=true&kanji=%1").arg(_kanji));
	linkFormat.setToolTip(tr("Make a new search using only this filter"));
	_cursor.setCharFormat(linkFormat);
	_cursor.insertText(tr("All words using this kanji"));
	_cursor.setCharFormat(normal);
	_cursor.insertText(" ");
	linkFormat.setAnchorHref(QString("allwords:?kanji=%1").arg(_kanji));
	linkFormat.setToolTip(tr("Add this filter to the current search"));
	_cursor.setCharFormat(linkFormat);
	_cursor.insertText(tr("(+)"));
	_cursor.setCharFormat(normal);
}
