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
#include "core/jmdict/JMdictPlugin.h"
#include "core/jmdict/JMdictEntrySearcher.h"
#include "gui/jmdict/JMdictEntryFormatter.h"
#include "gui/jmdict/JMdictGUIPlugin.h"

// For getting the kanjidic id
#include "core/kanjidic2/Kanjidic2Entry.h"

#include <QPainter>
#include <QTextBlock>
#include <QTextList>
#include <QPair>

PreferenceItem<bool> JMdictEntryFormatter::showJLPT("jmdict", "showJLPT", true);
PreferenceItem<bool> JMdictEntryFormatter::showKanjis("jmdict", "showKanjis", true);
PreferenceItem<bool> JMdictEntryFormatter::searchVerbBuddy("jmdict", "searchVerbBuddy", true);
PreferenceItem<int> JMdictEntryFormatter::maxHomophonesToDisplay("jmdict", "maxHomophonesToDisplay", 5);
PreferenceItem<bool> JMdictEntryFormatter::displayStudiedHomophonesOnly("jmdict", "displayStudiedHomophonesOnly", false);

PreferenceItem<int> JMdictEntryFormatter::headerPrintSize("jmdict", "headerPrintSize", 20);
PreferenceItem<bool> JMdictEntryFormatter::printKanjis("jmdict", "printKanjis", true);
PreferenceItem<bool> JMdictEntryFormatter::printOnlyStudiedKanjis("jmdict", "printOnlyStudiedKanjis", false);
PreferenceItem<int> JMdictEntryFormatter::maxDefinitionsToPrint("jmdict", "maxDefinitionsToPrint", 0);


QString JMdictEntryFormatter::getVerbBuddySql(const QString &matchPattern, quint64 pos, int id)
{
	const QString queryFindVerbBuddySql("select distinct " QUOTEMACRO(JMDICTENTRY_GLOBALID) ", jmdict.entries.id from jmdict.entries join jmdict.kanji on jmdict.kanji.id = jmdict.entries.id join jmdict.senses on jmdict.senses.id = jmdict.entries.id where jmdict.kanji.docid in (select docid from jmdict.kanjiText where jmdict.kanjiText.reading match '\"%1*\"') and jmdict.kanji.priority = 0 and jmdict.senses.pos & %2 == %2 and jmdict.senses.misc & %4 == 0 and jmdict.entries.id != %3");

	return	queryFindVerbBuddySql.arg(matchPattern).arg(pos).arg(id).arg(JMdictEntrySearcher::miscFilterMask());
}

QString JMdictEntryFormatter::getHomophonesSql(const QString &reading, int id, int maxToDisplay, bool studiedOnly)
{
	const QString queryFindHomonymsSql("select distinct " QUOTEMACRO(JMDICTENTRY_GLOBALID) ", jmdict.entries.id from jmdict.entries join jmdict.kana on jmdict.kana.id = jmdict.entries.id %4join training on training.id = jmdict.entries.id and training.type = " QUOTEMACRO(JMDICTENTRY_GLOBALID) " join jmdict.senses on jmdict.senses.id = jmdict.entries.id where jmdict.kana.docid in (select docid from jmdict.kanaText where jmdict.kanaText.reading match '\"%1\"') and jmdict.kana.priority = 0 and jmdict.senses.misc & %5 == 0 and jmdict.entries.id != %3 order by training.dateAdded is null ASC, training.score ASC, jmdict.entries.frequency DESC limit %2");

	return queryFindHomonymsSql.arg(reading).arg(maxToDisplay).arg(id).arg(studiedOnly ? "" : "left ").arg(JMdictEntrySearcher::miscFilterMask());
}

JMdictEntryFormatter::JMdictEntryFormatter() : EntryFormatter()
{
}

void JMdictEntryFormatter::writeSensePos(const Sense &sense, QTextCursor &cursor) const
{
	QList<const QPair<QString, QString> *> posEntities(JMdictPlugin::posEntitiesList(sense.partOfSpeech()));
	QList<const QPair<QString, QString> *> miscEntities(JMdictPlugin::miscEntitiesList(sense.misc()));
	QList<const QPair<QString, QString> *> dialectEntities(JMdictPlugin::dialectEntitiesList(sense.dialect()));
	QList<const QPair<QString, QString> *> fieldEntities(JMdictPlugin::fieldEntitiesList(sense.field()));
	
	if (posEntities.isEmpty() && miscEntities.isEmpty() && dialectEntities.isEmpty() &&
		fieldEntities.isEmpty()) return;

	QTextCharFormat posFormat(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	QTextCharFormat saveFormat(cursor.charFormat());
	posFormat.setForeground(Qt::darkGray);
	cursor.insertBlock(QTextBlockFormat());
	cursor.setBlockCharFormat(posFormat);

	bool hasWritten = false;
	for (int i = 0; i < posEntities.size(); i++) {
		if (i != 0 || hasWritten) cursor.insertText(", ");
		posFormat.setAnchor(true);
		posFormat.setAnchorHref(QString("longdesc://pos#%1").arg(JMdictPlugin::posBitShifts()[posEntities[i]->first]));
		QString translated = QCoreApplication::translate("JMdictLongDescs", posEntities[i]->second.toLatin1());
		translated.replace(0, 1, translated[0].toUpper());
		posFormat.setToolTip(translated);
		cursor.setCharFormat(posFormat);
		cursor.insertText(posEntities[i]->first);
		hasWritten = true;
		posFormat.setAnchor(false);
		posFormat.setAnchorHref("");
		posFormat.setToolTip("");
		cursor.setCharFormat(posFormat);
	}
	
	for (int i = 0; i < miscEntities.size(); i++) {
		if (i != 0 || hasWritten) cursor.insertText(", ");
		posFormat.setAnchor(true);
		posFormat.setAnchorHref(QString("longdesc://misc#%1").arg(JMdictPlugin::miscBitShifts()[miscEntities[i]->first]));
		QString translated = QCoreApplication::translate("JMdictLongDescs", miscEntities[i]->second.toLatin1());
		translated.replace(0, 1, translated[0].toUpper());
		posFormat.setToolTip(translated);
		cursor.setCharFormat(posFormat);
		cursor.insertText(miscEntities[i]->first);
		hasWritten = true;
		posFormat.setAnchor(false);
		posFormat.setAnchorHref("");
		posFormat.setToolTip("");
		cursor.setCharFormat(posFormat);
	}
	
	for (int i = 0; i < fieldEntities.size(); i++) {
		if (i != 0 || hasWritten) cursor.insertText(", ");
		posFormat.setAnchor(true);
		posFormat.setAnchorHref(QString("longdesc://field#%1").arg(JMdictPlugin::fieldBitShifts()[fieldEntities[i]->first]));
		QString translated = QCoreApplication::translate("JMdictLongDescs", fieldEntities[i]->second.toLatin1());
		translated.replace(0, 1, translated[0].toUpper());
		posFormat.setToolTip(translated);
		cursor.setCharFormat(posFormat);
		cursor.insertText(fieldEntities[i]->first);
		hasWritten = true;
		posFormat.setAnchor(false);
		posFormat.setAnchorHref("");
		posFormat.setToolTip("");
		cursor.setCharFormat(posFormat);
	}
	
	for (int i = 0; i < dialectEntities.size(); i++) {
		if (i != 0 || hasWritten) cursor.insertText(", ");
		posFormat.setAnchor(true);
		posFormat.setAnchorHref(QString("longdesc://dialect#%1").arg(JMdictPlugin::dialectBitShifts()[dialectEntities[i]->first]));
		QString translated = QCoreApplication::translate("JMdictLongDescs", dialectEntities[i]->second.toLatin1());
		translated.replace(0, 1, translated[0].toUpper());
		posFormat.setToolTip(translated);
		cursor.setCharFormat(posFormat);
		cursor.insertText(dialectEntities[i]->first);
		hasWritten = true;
		posFormat.setAnchor(false);
		posFormat.setAnchorHref("");
		posFormat.setToolTip("");
		cursor.setCharFormat(posFormat);
	}
	cursor.setCharFormat(saveFormat);
	// Needed to avoid having the last tooltip appearing when the user moves the mouse to the empty space after the last definition
	cursor.insertText(" ");
}

void JMdictEntryFormatter::writeKanaHeader(const ConstJMdictEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const
{
	const QList<KanjiReading> &kanjis = entry->getKanjiReadings();
	const QList<KanaReading> &kanas = entry->getKanaReadings();
	cursor.setBlockCharFormat(DetailedViewFonts::charFormat(DetailedViewFonts::KanaHeader));
	if (!kanjis.isEmpty() && !kanjis[0].getKanaReadings().isEmpty()) {
		cursor.insertText(kanas[kanjis[0].getKanaReadings()[0]].getReading());
		cursor.insertBlock();
	}
}

void JMdictEntryFormatter::writeKanjiHeader(const ConstJMdictEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const
{
	const QList<KanjiReading> &kanjis = entry->getKanjiReadings();
	const QList<KanaReading> &kanas = entry->getKanaReadings();
	const KanaReading &kanaReading = kanas[0];
	cursor.setBlockCharFormat(DetailedViewFonts::charFormat(DetailedViewFonts::KanjiHeader));
	if (!kanjis.isEmpty()) {
		const KanjiReading &kanjiReading = kanjis[0];
//	if (!kanaReading.getKanjiReadings().isEmpty()) {
//		const KanjiReading &kanjiReading = kanjis[kanaReading.getKanjiReadings()[0]];
		cursor.insertText(kanjiReading.getReading());
	} else {
		cursor.insertText(kanaReading.getReading());
	}
}

void JMdictEntryFormatter::writeJapanese(const ConstJMdictEntryPointer &entry, QTextCursor &cursor, DetailedView *view) const
{
	const QList<KanjiReading> &kanjis = entry->getKanjiReadings();
	const QList<KanaReading> &kanas = entry->getKanaReadings();

	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	QTextCharFormat kana(DetailedViewFonts::charFormat(DetailedViewFonts::Kana));
	QTextCharFormat kanji(DetailedViewFonts::charFormat(DetailedViewFonts::Kanji));
	QTextCharFormat bold(normal);
	bold.setFontWeight(QFont::Bold);
	QTextBlockFormat header;
	header.setAlignment(Qt::AlignHCenter);
	cursor.setBlockFormat(header);

	writeKanaHeader(entry, cursor, view);
	writeKanjiHeader(entry, cursor, view);
	cursor.setCharFormat(normal);

	// Check for other readings
	// Current kanji reading, other pronunciations
	// No kanji reading, all kanas are alternative
	QList<KanaReading> altReadings;
	if (kanjis.isEmpty()) altReadings = kanas;
	else foreach (qint32 idx, kanjis[0].getKanaReadings()) altReadings << kanas[idx];
	if (altReadings.size() > 1) {
		cursor.insertBlock(QTextBlockFormat());
		cursor.setCharFormat(bold);
		cursor.insertText(tr("Alternate readings: "));
		cursor.setCharFormat(normal);
		for (int i = 1; i < altReadings.size(); i++) {
			if (i != 1) cursor.insertText(", ");
			cursor.setCharFormat(kana);
			cursor.insertText(altReadings[i].getReading());
			cursor.setCharFormat(normal);
		}
	}

	// Other kanji readings
	if (kanjis.size() > 1) {
		cursor.insertBlock(QTextBlockFormat());
		QTextCharFormat bold(normal);
		bold.setFontWeight(QFont::Bold);
		cursor.setCharFormat(bold);
		cursor.insertText(tr("Variants:"));
		cursor.setCharFormat(normal);
		bool notFirstHomophone = false;
		for (int j = 1; j < kanjis.size(); j++) {
			if (notFirstHomophone) cursor.insertText(", ");
			else cursor.insertText(" ");
			notFirstHomophone = true;
			const KanjiReading &kanjiReading = kanjis[j];
			autoFormat(entry, kanjiReading.getReading(), cursor);
			cursor.setCharFormat(normal);
			const QList<qint32> &kanaReadings = kanjiReading.getKanaReadings();
			if (!kanaReadings.isEmpty()) {
				cursor.insertText(" (");
				int notFirst = false;
				foreach(qint32 kanaIdx, kanaReadings) {
					if (notFirst) cursor.insertText(", ");
					cursor.setCharFormat(kana);
					cursor.insertText(kanas[kanaIdx].getReading());
					cursor.setCharFormat(normal);
					notFirst = true;
				}
				cursor.insertText(")");
				cursor.setCharFormat(normal);
			}
		}
	}
	// TODO Orphan pronunciations
}

void JMdictEntryFormatter::writeTranslation(const ConstJMdictEntryPointer& entry, QTextCursor& cursor, DetailedView* view) const
{
	const QList<KanjiReading> &kanjis = entry->getKanjiReadings();
	const QList<KanaReading> &kanas = entry->getKanaReadings();
	const QList<const Sense *> senses = entry->getSenses();

	Q_ASSERT(senses.size() >= 1);
	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	QTextCharFormat save;
	quint64 oldPos = 0;
	quint32 oldMisc = 0;
	quint16 oldDialect = 0;
	quint16 oldField = 0;
	QStringList oldWritingString;

	foreach (const Sense *sense, senses) {
		// Shall we output the part of speech?
		if (oldPos != sense->partOfSpeech() || oldMisc != sense->misc() ||
			oldDialect != sense->dialect() || oldField != sense->field()) {
			writeSensePos(*sense, cursor);
			oldPos = sense->partOfSpeech();
			oldMisc = sense->misc();
			oldDialect = sense->dialect();
			oldField = sense->field();
		}

		// Write the entry
		cursor.insertBlock(QTextBlockFormat());
		QMap<QString, Gloss> glosses = sense->getGlosses();
		QStringList keys;
		// TODO separate different languages with new block
		foreach (const QString &glossKey, glosses.keys()) {
			cursor.insertImage(QString("flag:") + glossKey);
			cursor.setCharFormat(normal);
			QString gloss(glosses[glossKey].gloss());
			if (!gloss.isEmpty()) gloss[0] = gloss[0].toUpper();
			cursor.insertText(QString(" ") + gloss + ".");
		}

		QStringList senseHeaders;
		// Check if the writing is restricted
		if (!sense->stagK().isEmpty()) foreach (int idx, sense->stagK()) {
			const KanjiReading &kReading = kanjis[idx];
			QString str(kReading.getReading());
			const QList<qint32> &kList = kReading.getKanaReadings();
			if (!kList.isEmpty()) {
					QStringList kanasList;
					foreach (qint32 idx2, kList) kanasList << kanas[idx2].getReading();
					str += " (" + kanasList.join(",") + ")";
			}
			senseHeaders << str;
		}
		if (!sense->stagR().isEmpty()) foreach (int idx, sense->stagR()) {
			const KanaReading &kReading = kanas[idx];
			senseHeaders << kReading.getReading();
		}
		if (!senseHeaders.isEmpty()) {
			QTextCharFormat bold;
			bold.setForeground(QColor(60, 60, 60));
			autoFormat(entry, " [ " + senseHeaders.join(", ") + " ]", cursor, bold);

			oldWritingString = senseHeaders;
		}
	}
}

void JMdictEntryFormatter::writeEntryInfo(const ConstJMdictEntryPointer& entry, QTextCursor& cursor, DetailedView* view) const
{
	const QList<const Sense *> senses = entry->getSenses();

	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	QTextCharFormat bold(normal);
	bold.setFontWeight(QFont::Bold);
	if (showJLPT.value() && entry->jlpt() != -1) {
		cursor.insertBlock(QTextBlockFormat());
		cursor.setCharFormat(bold);
		cursor.insertText(tr("JLPT level:"));
		cursor.setCharFormat(normal);
		cursor.insertText(" " + QString::number(entry->jlpt()));
	}
	if (showKanjis.value() && !entry->getKanjiReadings().isEmpty()) {
		const QString &reading(entry->getKanjiReadings()[0].getReading());
		bool headerPrinted(false);
		for (int i = 0; i < reading.size(); ++i) {
			if (TextTools::isKanjiChar(reading, i)) {
				if (!headerPrinted) {
					cursor.insertBlock(QTextBlockFormat());
					cursor.setCharFormat(bold);
					cursor.insertText(tr("Kanji:"));
					cursor.setCharFormat(normal);
					headerPrinted = true;
				}
				QString k(reading[i]);
				if (reading[i].isHighSurrogate()) k += reading[++i];
				cursor.insertText("\n");
				EntryPointer _entry = EntriesCache::get(KANJIDIC2ENTRY_GLOBALID, TextTools::singleCharToUnicode(k));
				view->addWatchEntry(_entry);

				const EntryFormatter *formatter(EntryFormatter::getFormatter(_entry));
				formatter->writeShortDesc(_entry, cursor);
			}
		}
	}
	bool searchVi = true, searchVt = true;
/*	bool hasVi = false, hasVt = false;
	foreach (const Sense &sense, senses) {
		if (sense.partOfSpeech() & JMdict_POS_vi) hasVi = true;
		if (sense.partOfSpeech() & JMdict_POS_vt) hasVt = true;
	}
	if (hasVi && !hasVt) view->addBackgroundJob(new FindVerbBuddyJob(this, JMdict_POS_vt, cursor));
	if (hasVt && !hasVi) view->addBackgroundJob(new FindVerbBuddyJob(this, JMdict_POS_vi, cursor));*/

	if (JMdictPlugin::posBitShifts().contains("vi") && JMdictPlugin::posBitShifts().contains("vt")) {
		quint64 posVi(1 << JMdictPlugin::posBitShifts()["vi"]);
		quint64 posVt(1 << JMdictPlugin::posBitShifts()["vt"]);
		foreach (const Sense *sense, senses) {
			if (sense->partOfSpeech() & posVi && searchVt) {
				view->addBackgroundJob(new FindVerbBuddyJob(entry, "vt", cursor));
				searchVt = false;
			}
			if (sense->partOfSpeech() & posVt && searchVi) {
				view->addBackgroundJob(new FindVerbBuddyJob(entry, "vi", cursor));
				searchVi = false;
			}
		}
	}
	if (maxHomophonesToDisplay.value()) view->addBackgroundJob(new FindHomonymsJob(entry, maxHomophonesToDisplay.value(), displayStudiedHomophonesOnly.value(), cursor));
}

void JMdictEntryFormatter::writeShortDesc(const ConstEntryPointer& entry, QTextCursor& cursor) const
{
	QTextCharFormat scoreFormat;
	if (entry->trained()) {
		scoreFormat.setBackground(entry->scoreColor());
	}
	autoFormat(entry, entry->shortVersion(Entry::TinyVersion), cursor, scoreFormat);
	ConstJMdictEntryPointer jEntry(entry.objectCast<const JMdictEntry>());
	if (shortDescShowJLPT.value() && jEntry->jlpt() != -1) {
		scoreFormat.setFontWeight(QFont::Bold);
		autoFormat(jEntry, tr(" (JLPT %1)").arg(jEntry->jlpt()), cursor, scoreFormat);
	}
	QTextImageFormat imgFormat;
	imgFormat.setAnchor(true);
	imgFormat.setAnchorHref(QString("entry://?type=%1&id=%2").arg(entry->type()).arg(entry->id()));
	imgFormat.setName("moreicon");
	cursor.insertImage(imgFormat);
}

void JMdictEntryFormatter::_detailedVersion(const ConstEntryPointer& entry, QTextCursor& cursor, DetailedView* view) const
{
	ConstJMdictEntryPointer jEntry(entry.objectCast<const JMdictEntry>());
	writeJapanese(jEntry, cursor, view);
	cursor.insertBlock(QTextBlockFormat());
	writeTranslation(jEntry, cursor, view);
	cursor.insertBlock(QTextBlockFormat());
	writeEntryInfo(jEntry, cursor, view);
}

void JMdictEntryFormatter::drawCustom(const ConstEntryPointer& _entry, QPainter& painter, const QRectF& rectangle, QRectF& usedSpace, const QFont& textFont, int headerPrintSize, bool printKanjis, bool printOnlyStudiedKanjis, int maxDefinitionsToPrint) const
{
	ConstJMdictEntryPointer entry(_entry.objectCast<const JMdictEntry>());
	QFont kanjiFont;
	kanjiFont.setPointSize(headerPrintSize);
	QRectF leftArea(rectangle);
	leftArea.setWidth(rectangle.width() / 3.5);
	QRectF rightArea(rectangle);
	rightArea.setLeft(leftArea.right() + leftArea.width() / 20.0);

	QRectF textBB;

	// Print kanji writing
	QString writing;
	if (!entry->writings().isEmpty()) writing = entry->writings()[0];
	else if (!entry->readings().isEmpty()) writing = entry->readings()[0];
	if (!writing.isEmpty()) {
		painter.setFont(kanjiFont);
		textBB = painter.boundingRect(leftArea, Qt::TextWordWrap | Qt::AlignHCenter, writing);
		painter.drawText(leftArea, Qt::TextWordWrap | Qt::AlignHCenter, writing);
		leftArea.setTop(textBB.bottom());
	}
	painter.setFont(textFont);
	// Print reading, if a kanji writing was available
	if (!entry->writings().isEmpty() && !entry->readings().isEmpty()) {
		QString reading(entry->readings()[0]);
		textBB = painter.boundingRect(leftArea, Qt::TextWordWrap | Qt::AlignHCenter, reading);
		painter.drawText(leftArea, Qt::TextWordWrap | Qt::AlignHCenter, reading);
		leftArea.setTop(textBB.bottom());
	}
	// Print meaning of kanjis used in writing
	if (printKanjis) {
		QList<QChar> usedKanjis;
		foreach (const QChar &c, writing) {
			if (TextTools::isKanjiChar(c) && !usedKanjis.contains(c)) {
				usedKanjis << c;
				ConstKanjidic2EntryPointer kanji(EntriesCache::get(KANJIDIC2ENTRY_GLOBALID, c.unicode()).objectCast<const Kanjidic2Entry>());
				if (!kanji) continue;
				if (printOnlyStudiedKanjis && !kanji->trained()) continue;
				QString s = QString(c) + ": " + kanji->meanings().join(", ");
				QFontMetrics metrics(painter.font(), painter.device());
				s = metrics.elidedText(s, Qt::ElideRight, leftArea.width());
				textBB = painter.boundingRect(leftArea, Qt::AlignLeft, s);
				painter.drawText(leftArea, Qt::AlignLeft, s);
				if (!printOnlyStudiedKanjis && kanji->trained()) painter.drawLine(textBB.topLeft() + QPoint(0, metrics.ascent() + metrics.underlinePos()), textBB.topRight() + QPoint(0, metrics.ascent() + metrics.underlinePos()));
				leftArea.setTop(textBB.bottom());
			}
		}
	}
	// Now print definitions.
	foreach (const Sense *sense, entry->getSenses()) {
		QList<const QPair<QString, QString> *> posEntities(JMdictPlugin::posEntitiesList(sense->partOfSpeech()));
		QList<const QPair<QString, QString> *> miscEntities(JMdictPlugin::miscEntitiesList(sense->misc()));
		QList<const QPair<QString, QString> *> dialectEntities(JMdictPlugin::dialectEntitiesList(sense->dialect()));
		QList<const QPair<QString, QString> *> fieldEntities(JMdictPlugin::fieldEntitiesList(sense->field()));

		QStringList posList;
		for (int i = 0; i < posEntities.size(); i++) posList << posEntities[i]->first;
		for (int i = 0; i < miscEntities.size(); i++) posList << miscEntities[i]->first;
		for (int i = 0; i < dialectEntities.size(); i++) posList << dialectEntities[i]->first;
		for (int i = 0; i < fieldEntities.size(); i++) posList << fieldEntities[i]->first;

		QString posText;
		if (!posList.isEmpty()) posText = QString(" (") + posList.join(",") + ") ";
		QString s(sense->senseText());
		if (!s.isEmpty()) s[0] = s[0].toUpper();
		s = " " + posText + s;
		textBB = painter.boundingRect(rightArea, Qt::AlignLeft | Qt::TextWordWrap, s);
		painter.drawText(rightArea, Qt::AlignLeft | Qt::TextWordWrap, s);
		rightArea.setTop(textBB.bottom());
		if (--maxDefinitionsToPrint == 0) break;
	}
	drawInfo(entry, painter, rightArea, textFont);

	usedSpace = rectangle;
	usedSpace.setHeight(qMax(leftArea.top(), rightArea.top()) - rectangle.top());

	painter.drawLine(QPointF(rectangle.left() + leftArea.width(), usedSpace.top()),
					 QPointF(rectangle.left() + leftArea.width(), usedSpace.bottom()));
}

void JMdictEntryFormatter::detailedVersionPart1(const ConstEntryPointer& _entry, QTextCursor& cursor, DetailedView* view) const
{
	ConstJMdictEntryPointer entry(_entry.objectCast<const JMdictEntry>());
	QTextBlockFormat header;
	header.setAlignment(Qt::AlignHCenter);
	cursor.setBlockFormat(header);
	if (JMdictGUIPlugin::furiganasForTraining.value()) writeKanaHeader(entry, cursor, view);
	writeKanjiHeader(entry, cursor, view);
}

void JMdictEntryFormatter::detailedVersionPart2(const ConstEntryPointer& _entry, QTextCursor& cursor, DetailedView* view) const
{
	ConstJMdictEntryPointer entry(_entry.objectCast<const JMdictEntry>());
	writeTranslation(entry, cursor, view);
}


FindVerbBuddyJob::FindVerbBuddyJob(const ConstJMdictEntryPointer& verb, const QString& pos, const QTextCursor& cursor) :
	DetailedViewJob(cursor), lastKanjiPos(0), searchedPos(pos)
{
	// Check that the pos we are looking for actually exists
	if (!JMdictPlugin::posBitShifts().contains(pos)) return;
	quint64 posMask(1 << JMdictPlugin::posBitShifts()[pos]);
		
	// No writing or no reading, no way to compare
	if (verb->writings().isEmpty() || verb->readings().isEmpty()) return;
	matchPattern = verb->writings()[0];
	QList<qint32> readingIndexes(verb->getKanjiReadings()[0].getKanaReadings());
	if (readingIndexes.isEmpty()) return;
	kanaPattern = verb->getKanaReadings()[readingIndexes[0]].getReading();
	initialLength = matchPattern.size();
	// Remove characters from the match pattern until we meet a kanji
	while (!TextTools::isKanjiChar(matchPattern[matchPattern.size() - 1]) && !matchPattern.isEmpty()) {
		matchPattern.resize(matchPattern.size() - 1);
		kanaPattern.resize(kanaPattern.size() - 1);
	}
	// Only continue if the matchpattern is not empty...
	if (!matchPattern.size() || !kanaPattern.size()) return;

		
	_sql = JMdictEntryFormatter::getVerbBuddySql(matchPattern, posMask, verb->id());
	lastKanjiPos = matchPattern.size();
}

void FindVerbBuddyJob::result(ConstEntryPointer entry)
{
	ConstJMdictEntryPointer jEntry = entry.objectCast<const JMdictEntry>();
	if (!jEntry) return;
	// We now that we have a writing, as the SQL request matched with it
	QString writing(jEntry->writings()[0]);
	if (writing.size() < lastKanjiPos) return;
	// Ensure we have no kanji after lastKanjiPos
	for (int i = lastKanjiPos; i < writing.size(); i++)
		if (TextTools::isKanjiChar(writing[i])) return;
	// Candidates which kana reading do not even match a single character are not considered
	{
		QList<qint32> readingIndexes(jEntry->getKanjiReadings()[0].getKanaReadings());
		if (readingIndexes.isEmpty()) return;
		if (jEntry->getKanaReadings()[readingIndexes[0]].getReading()[0] != kanaPattern[0]) return;
	}
	// We are have a best match if
	// 1) There is no candidate yet
	if (!bestMatch) {
		bestMatch = jEntry;
		return;
	}
	// 2) We match longer and better than the current candidate to the original reading
	{
		QString bestMatchReading = bestMatch->readings()[0];
		QString candidateReading = jEntry->readings()[0];
		int bestMatchLength = 0;
		int candidateMatchLength = 0;

		// First calculate the match length of our current best choice
		bool exit = false;
		do {
			if (bestMatchLength >= firstReading.size() || candidateMatchLength >= firstReading.size()) break;
			if (bestMatchReading[bestMatchLength] == firstReading[bestMatchLength]) bestMatchLength++;
			else exit = true;
			if (candidateReading[candidateMatchLength] == firstReading[candidateMatchLength]) candidateMatchLength++;
			else exit = true;
		} while (!exit);

		// We match longer, no contest!
		if (candidateMatchLength > bestMatchLength) {
			bestMatch = jEntry;
			return;
		}
		// We match better if the phoneme after the match is in the same row as the phoneme of the buddy
		else if (candidateMatchLength == bestMatchLength && candidateReading.size() > candidateMatchLength && firstReading.size() > candidateMatchLength) {
			int firstReadingRow = TextTools::kanasTableRow(firstReading[candidateMatchLength]);
			if (TextTools::kanasTableRow(candidateReading[candidateMatchLength]) == firstReadingRow) {
				bestMatch = jEntry;
				return;
			}
		}
	}
	// 3) Our length is closer to the original than the current candidate.
	if (qAbs(writing.size() - initialLength) < qAbs(bestMatch->writings()[0].size() - initialLength)) {
		bestMatch = jEntry;
		return;
	}
}

void FindVerbBuddyJob::completed()
{
	if (!bestMatch) return;
	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	QTextCharFormat bold(normal);
	bold.setFontWeight(QFont::Bold);

	cursor().insertBlock();
	cursor().setCharFormat(bold);
	cursor().insertText(QString(searchedPos == "vt" ? tr("Transitive buddy:") :
						searchedPos == "vi" ? tr("Intransitive buddy:") :
						tr("Buddy:")) + " ");
	cursor().setCharFormat(normal);
	const EntryFormatter *formatter = EntryFormatter::getFormatter(bestMatch);
	if (formatter) formatter->writeShortDesc(bestMatch, cursor());
}

FindHomonymsJob::FindHomonymsJob(const ConstJMdictEntryPointer& entry, int maxToDisplay, bool studiedOnly, const QTextCursor& cursor) :
	DetailedViewJob(cursor)
{
	QString s;
	// No reading? Must be a katakana entry, take the writing in this case.
	if (!entry->readings().isEmpty()) s = entry->readings()[0];
	else if (!entry->writings().isEmpty()) s = entry->writings()[0];
	else return;
	_sql = JMdictEntryFormatter::getHomophonesSql(s, entry->id(), maxToDisplay, studiedOnly);
}

void FindHomonymsJob::firstResult()
{
	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	QTextCharFormat bold(normal);
	bold.setFontWeight(QFont::Bold);
	cursor().insertBlock(QTextBlockFormat());
	cursor().setCharFormat(bold);
	cursor().insertText(tr("Homophones:"));
	cursor().setCharFormat(normal);
}

void FindHomonymsJob::result(ConstEntryPointer entry)
{
	QTextList *currentList = cursor().currentList();
	cursor().insertBlock(QTextBlockFormat());
	cursor().setCharFormat(QTextCharFormat());
	ConstJMdictEntryPointer jmEntry = entry.objectCast<const JMdictEntry>();
	Q_ASSERT(jmEntry != 0);
	const EntryFormatter *formatter = EntryFormatter::getFormatter(jmEntry);
	if (formatter) formatter->writeShortDesc(jmEntry, cursor());
	if (!currentList) {
		QTextListFormat listFormat;
		listFormat.setStyle(QTextListFormat::ListDisc);
		listFormat.setIndent(0);
		cursor().createList(listFormat);
	}
	else currentList->add(cursor().block());
}

void FindHomonymsJob::completed()
{
}
