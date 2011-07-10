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

#include "core/Database.h"
#include "core/jmdict/JMdictEntry.h"
#include "core/jmdict/JMdictEntrySearcher.h"
#include "core/jmdict/JMdictPlugin.h"
#include "gui/kanjidic2/KanjiRenderer.h"
#include "gui/jmdict/JMdictEntryFormatter.h"
#include "gui/kanjidic2/Kanjidic2GUIPlugin.h"

#include <QUrl>
#include <QTextList>
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
	const QString queryUsedInWordsSql("select distinct " QUOTEMACRO(JMDICTENTRY_GLOBALID) ", jmdict.entries.id "
		"from jmdict.entries "
		"join jmdict.kanjiChar on jmdict.kanjiChar.id = jmdict.entries.id "
		"join jmdict.senses as senses on senses.id = jmdict.entries.id "
			"and senses.priority = 0 "
			"and senses.misc & %4 = 0 "
		"%3join training on training.id = jmdict.entries.id and training.type = " QUOTEMACRO(JMDICTENTRY_GLOBALID) " "
		"left join jmdict.jlpt on jmdict.entries.id = jmdict.jlpt.id "
		"where jmdict.kanjiChar.kanji = %1 and jmdict.kanjiChar.priority = 0 "
		"order by training.dateAdded is null ASC, training.score ASC, jmdict.jlpt.level DESC, jmdict.entries.frequency DESC "
		"limit %2");

	return queryUsedInWordsSql.arg(kanji).arg(limit).arg(onlyStudied ? "" : "left ").arg(JMdictEntrySearcher::miscFilterMask() | (1 << JMdictPlugin::miscBitShifts()["uk"]));
}

QString Kanjidic2EntryFormatter::getQueryUsedInKanjiSql(int kanji, int limit, bool onlyStudied)
{
	const QString queryUsedInKanjiSql("select distinct " QUOTEMACRO(KANJIDIC2ENTRY_GLOBALID) ", ks1.kanji "
		"from kanjidic2.strokeGroups as ks1 "
		"join kanjidic2.entries on ks1.isRoot == 1 and ks1.kanji = entries.id "
		"%3join training on training.type = " QUOTEMACRO(KANJIDIC2ENTRY_GLOBALID) " and training.id = entries.id "
		"where (ks1.element = %1 or ks1.original = %1) "
		"and ks1.kanji != %1 "
		"order by training.dateAdded is null ASC, training.score ASC, entries.strokeCount, entries.jlpt DESC "
		"limit %2");

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
		SQLite::Query query(Database::connection());
		query.exec(getQueryUsedInWordsSql(entry->id(), maxWordsToPrint, printOnlyStudiedVocab));
		painter.setFont(textFont);
		while (query.next()) {
			ConstJMdictEntryPointer jmEntry(JMdictEntryRef(query.valueInt(1)).get());

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

void Kanjidic2EntryFormatter::showToolTip(const ConstKanjidic2EntryPointer entry, const QPoint &pos) const
{
	QString writing(DetailedViewFonts::HTML(DetailedViewFonts::KanjiHeader) + entry->kanji() + "</span>"), alt;
	if (!entry->readings().isEmpty()) {
		QStringList readings;
		foreach (const QString &reading, entry->readings())
			readings << DetailedViewFonts::HTML(DetailedViewFonts::Kana) + reading + "</span>";
		alt = "(" + readings.join(", ") + ")";
	}

	QString s;
	if (tooltipShowScore.value() && entry->trained()) {
		QColor scoreColor(entry->scoreColor());
		s += QString("<div background-color: #%1%2%3;\">%4</div> ").arg(QString::number(scoreColor.red(), 16)).arg(QString::number(scoreColor.green(), 16)).arg(QString::number(scoreColor.blue(), 16)).arg(writing);
	} else s += writing;
	s += alt;
	if (!entry->meanings().isEmpty()) {
		QString s2 = entry->meaningsString();
		if (!s2.isEmpty()) s2[0] = s2[0].toUpper();
		s += "<br/>" + s2;
	}
	s += "<table border=\"0\" width=\"100%\">";
	int tCpt = 0;
	#define CLOSE_TAB \
		if (tCpt % 2) s += "<td>" + body + "</td></tr>"; \
		else s += "<tr><td>" + body + "</td>"; \
		++tCpt

	if (entry->strokeCount() != -1 && tooltipShowStrokesNumber.value()) {
		QString body(tr("<b>Strokes:</b> %1").arg(entry->strokeCount()));
		CLOSE_TAB;
	}
	if (entry->frequency() != -1 && tooltipShowFrequency.value()) {
		QString body(tr("<b>Frequency:</b> %1").arg(entry->frequency()));
		CLOSE_TAB;
	}
	if (entry->grade() != -1 && tooltipShowGrade.value()) {
		QString body(tr("<b>Grade:</b> %1").arg(QCoreApplication::translate("Kanjidic2GUIPlugin", Kanjidic2GUIPlugin::kanjiGrades[entry->grade()].toLatin1())));
		CLOSE_TAB;
	}
	if (entry->jlpt() != -1 && tooltipShowJLPT.value()) {
		QString body(tr("<b>JLPT:</b> N%1").arg(entry->jlpt() < 3 ? entry->jlpt() : entry->jlpt() + 1));
		CLOSE_TAB;
	}
	if (tooltipShowUnicode.value()) {
		QString body(tr("<b>Unicode:</b> 0x%1").arg(QString::number(TextTools::singleCharToUnicode(entry->kanji()), 16)));
		CLOSE_TAB;
	}
	if (tooltipShowSKIP.value() && !entry->skipCode().isEmpty()) {
		QString body(tr("<b>SKIP:</b> %1").arg(entry->skipCode()));
		CLOSE_TAB;
	}
	if (tooltipShowFourCorner.value() && !entry->fourCorner().isEmpty()) {
		QString body(tr("<b>4 corner:</b> %1").arg(entry->fourCorner()));
		CLOSE_TAB;
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

	QString str(entry->kanji());
	if (shape != entry) str += QString(" (%1) ").arg(shape->kanji());
	if (!entry->meanings().isEmpty()) str += ": " + entry->meaningsString();
	ret += autoFormat(str);
	
	if (shortDescShowJLPT.value() && entry->jlpt() != -1)
		ret += QString(" <b>%1</b>").arg(autoFormat(tr("(JLPT N%1)").arg(entry->jlpt() < 3 ? entry->jlpt() : entry->jlpt() + 1)));
	ret += QString(" <a href=\"entry://?type=%1&id=%2\"><img src=\"moreicon\"/></a>").arg(entry->type()).arg(entry->id());
	if (entry->trained()) {
		ret = QString("<span style=\"background-color:%1\">%2</span>").arg(colorTriplet(entry->scoreColor())).arg(ret);
	}
	return ret;
}

QString Kanjidic2EntryFormatter::formatHead(const ConstEntryPointer &_entry) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());

	QString res(EntryFormatter::formatHead(entry));
	if (res.isEmpty()) return res;
	else return QString("<a href=\"drawkanji://?kanji=%1\">").arg(entry->kanji()) + res + "</a>";
}

QString Kanjidic2EntryFormatter::formatMeanings(const ConstEntryPointer &_entry) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	if (!entry->kanjiMeanings().isEmpty()) {
		QString s = entry->meaningsString();
		if (!s.isEmpty()) s[0] = s[0].toUpper();
		return s;
	}
	return "";
}

QString Kanjidic2EntryFormatter::formatOnReadings(const ConstEntryPointer &_entry) const
{
	if (showReadings.value()) {
		ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
		QStringList on = entry->onyomiReadings();
		if (!on.isEmpty())
			return QString("<b>%1:</b> %2").arg(tr("On")).arg(autoFormat(on.join(", ")));
	}
	return "";
}

QString Kanjidic2EntryFormatter::formatKunReadings(const ConstEntryPointer &_entry) const
{
	if (showReadings.value()) {
		ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
		QStringList kun = entry->kunyomiReadings();
		if (!kun.isEmpty())
			return QString("<b>%1:</b> %2").arg(tr("Kun")).arg(autoFormat(kun.join(", ")));
	}
	return "";
}

QString Kanjidic2EntryFormatter::formatNanori(const ConstEntryPointer &_entry) const
{
	if (showReadings.value()) {
		ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
		QStringList nanori = entry->nanoris();
		if (!nanori.isEmpty())
			return QString("<b>%1:</b> %2").arg(tr("Nanori")).arg(autoFormat(nanori.join(", ")));
	}
	return "";
}

	
QString Kanjidic2EntryFormatter::formatStrokesCount(const ConstEntryPointer &_entry) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	if (entry->strokeCount() != -1 && showStrokesNumber.value()) {
		return QString("<b>%1:</b> %2").arg(tr("Strokes")).arg(entry->strokeCount());
	}
	return "";
}

QString Kanjidic2EntryFormatter::formatFrequency(const ConstEntryPointer &_entry) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	if (entry->frequency() != -1 && showFrequency.value()) {
		return QString("<b>%1:</b> %2").arg(tr("Frequency")).arg(entry->frequency());
	}
	return "";
}

QString Kanjidic2EntryFormatter::formatGrade(const ConstEntryPointer &_entry) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	if (entry->grade() != -1 && showGrade.value()) {
		return QString("<b>%1:</b> %2").arg(tr("Grade")).arg(QCoreApplication::translate("Kanjidic2GUIPlugin", Kanjidic2GUIPlugin::kanjiGrades[entry->grade()].toLatin1()));
	}
	return "";
}

QString Kanjidic2EntryFormatter::formatJLPT(const ConstEntryPointer &_entry) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	if (entry->jlpt() != -1 && showJLPT.value()) {
		return QString("<b>%1:</b> N%2").arg(tr("JLPT")).arg(entry->jlpt() < 3 ? entry->jlpt() : entry->jlpt() + 1);
	}
	return "";
}

QString Kanjidic2EntryFormatter::formatVariations(const ConstEntryPointer &_entry) const
{
	if (showVariations.value()) {
		ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
		SQLite::Query query(Database::connection());
		query.prepare("select distinct element from kanjidic2.strokeGroups where original = ?");
		query.bindValue(entry->id());
		query.exec();
		QList<EntryPointer> entries;
		QStringList formats;
		while (query.next()) {
			ConstKanjidic2EntryPointer kEntry(KanjiEntryRef(query.valueUInt(0)).get());
			formats << entryTitle(kEntry);
		}
		if (!formats.isEmpty()) {
			return QString("<b>%1:</b> %2").arg(tr("Variations")).arg(formats.join(" "));
		}
	}
	return "";
}

QString Kanjidic2EntryFormatter::formatVariationsOf(const ConstEntryPointer &_entry) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	if (showVariationOf.value() && !entry->variationOf().isEmpty()) {
		QStringList formats;
		foreach (quint32 kid, entry->variationOf()) {
			ConstKanjidic2EntryPointer kEntry(KanjiEntryRef(kid).get());
			formats << entryTitle(kEntry);
		}
		QString("<b>%1:</b> %2").arg(tr("Variation of")).arg(formats.join(" "));
	}
	return "";
}

QString Kanjidic2EntryFormatter::formatUnicode(const ConstEntryPointer &_entry) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	if (showUnicode.value()) {
		return QString("<b>%1:</b> 0x%2").arg(tr("Unicode")).arg(QString::number(TextTools::singleCharToUnicode(entry->kanji()), 16));
	}
	return "";
}

QString Kanjidic2EntryFormatter::formatSkip(const ConstEntryPointer &_entry) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	if (!entry->skipCode().isEmpty() && showSKIP.value()) {
		return QString("<b>%1:</b> %2").arg(tr("SKIP")).arg(entry->skipCode());
	}
	return "";
}

QString Kanjidic2EntryFormatter::formatFourCorner(const ConstEntryPointer &_entry) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	if (!entry->fourCorner().isEmpty() && showFourCorner.value()) {
		return QString("<b>%1:</b> %2").arg(tr("4 corner")).arg(entry->fourCorner());
	}
	return "";
}

QString Kanjidic2EntryFormatter::formatRadicals(const ConstEntryPointer &_entry) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	if (!entry->radicals().isEmpty() && showRadicals.value()) {
		QStringList formats;
		typedef QPair<uint, quint8> radicalType;
		foreach (const radicalType &radical, entry->radicals()) {
			ConstKanjidic2EntryPointer kEntry(KanjiEntryRef(radical.first).get());
			// TODO
			//view->addWatchEntry(kEntry);
			formats << QString("%1 (%2)").arg(entryTitle(kEntry)).arg(radical.second);
		} 
		return QString("<b>%1:</b> %2").arg(tr("Radicals")).arg(formats.join(" "));
	}
	return "";
}

QString Kanjidic2EntryFormatter::formatComponents(const ConstEntryPointer &_entry) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	QList<const KanjiComponent *> components(entry->rootComponents());
	if (!components.isEmpty() && showComponents.value()) {
		QStringList formats;
		QList<const KanjiComponent *> singleComponents;
		QSet<uint> listedComps;
		foreach (const KanjiComponent *component, components) {
			if (listedComps.contains(component->unicode())) continue;
			listedComps << component->unicode();
			ConstKanjidic2EntryPointer kEntry(getMeaningEntry(component));
			ConstKanjidic2EntryPointer shape(getShapeEntry(component));
			// TODO
			//view->addWatchEntry(kEntry);
			formats << shortDesc(shape, kEntry);
		}
		return buildSubInfoBlock(tr("Components"), formats.join("<br/>"));
	}
	return "";
}

QList<DetailedViewJob *> Kanjidic2EntryFormatter::jobUsedInKanji(const ConstEntryPointer &_entry, const QTextCursor &cursor) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	QList<DetailedViewJob *> ret;
	if (maxCompoundsToDisplay.value()) ret << new ShowUsedInKanjiJob(entry->kanji(), cursor);
	return ret;
}

QList<DetailedViewJob *> Kanjidic2EntryFormatter::jobUsedInWords(const ConstEntryPointer &_entry, const QTextCursor &cursor) const
{
	ConstKanjidic2EntryPointer entry(_entry.staticCast<const Kanjidic2Entry>());
	QList<DetailedViewJob *> ret;
	if (maxWordsToDisplay.value()) ret << new ShowUsedInWordsJob(entry->kanji(), cursor);
	return ret;
}

ShowUsedInKanjiJob::ShowUsedInKanjiJob(const QString &kanji, const QTextCursor &cursor) :
		DetailedViewJob(Kanjidic2EntryFormatter::getQueryUsedInKanjiSql(TextTools::singleCharToUnicode(kanji), Kanjidic2EntryFormatter::maxCompoundsToDisplay.value(), Kanjidic2EntryFormatter::showOnlyStudiedCompounds.value()), cursor), _kanji(kanji), gotResults(false)
{
}

void ShowUsedInKanjiJob::firstResult()
{
	cursor().insertHtml(QString("<br/>%1").arg(EntryFormatter::buildSubInfoLine(tr("Direct compounds"), "")));
	_cursor.movePosition(QTextCursor::PreviousBlock);
	gotResults = true;
}

void ShowUsedInKanjiJob::result(EntryPointer entry)
{
	ConstKanjidic2EntryPointer kEntry = entry.staticCast<const Kanjidic2Entry>();
	Q_ASSERT(kEntry);
	const EntryFormatter *formatter(EntryFormatter::getFormatter(kEntry));
	cursor().insertHtml(formatter->entryTitle(kEntry) + "&nbsp;");
}

void ShowUsedInKanjiJob::completed()
{
	if (!gotResults) return;
	cursor().insertHtml(QString("<a href=\"component:?reset=true&kanji=%1\" title=\"%4\">%2</a> <a href=\"component:?kanji=%1\" title=\"%5\">%3</a>").arg(_kanji).arg(tr("All compounds")).arg(tr("(+)")).arg("Make a new search using only this filter").arg("Add this filter to the current search"));
}

ShowUsedInWordsJob::ShowUsedInWordsJob(const QString &kanji, const QTextCursor &cursor) :
		DetailedViewJob(Kanjidic2EntryFormatter::getQueryUsedInWordsSql(TextTools::singleCharToUnicode(kanji), Kanjidic2EntryFormatter::maxWordsToDisplay.value(), Kanjidic2EntryFormatter::showOnlyStudiedVocab.value()), cursor), _kanji(kanji), gotResults(false)
{
}

void ShowUsedInWordsJob::firstResult()
{
	cursor().insertHtml(QString("<br/>%1").arg(EntryFormatter::buildSubInfoBlock(tr("Seen in"), "")));
	_cursor.movePosition(QTextCursor::PreviousBlock);
	gotResults = true;
}

void ShowUsedInWordsJob::result(EntryPointer entry)
{
	ConstJMdictEntryPointer jmEntry(entry.staticCast<const JMdictEntry>());
	Q_ASSERT(jmEntry);
	const EntryFormatter *formatter(EntryFormatter::getFormatter(jmEntry));
	cursor().insertHtml(formatter->shortDesc(jmEntry) + "<br/>\n");
}

void ShowUsedInWordsJob::completed()
{
	if (!gotResults) return;
	cursor().insertHtml(QString("<a href=\"allwords:?reset=true&kanji=%1\" title=\"%4\">%2</a> <a href=\"allwords:?kanji=%1\" title=\"%5\">%3</a>").arg(_kanji).arg(tr("All words using this kanji")).arg(tr("(+)")).arg("Make a new search using only this filter").arg("Add this filter to the current search"));
}
