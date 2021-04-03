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
#include "core/Lang.h"
#include "core/TextTools.h"
#include "core/jmdict/JMdictEntry.h"
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
#include <qglobal.h>

static QString tatoebaTemplate("http://tatoeba.org/eng/sentences/search?query=%1&from=jpn&to=eng");
static QString jishoTemplate("http://jisho.org/sentences?jap=%1&eng=");

PreferenceItem<bool> JMdictEntryFormatter::showJLPT("jmdict", "showJLPT", true);
PreferenceItem<bool> JMdictEntryFormatter::showKanjis("jmdict", "showKanjis", true);
PreferenceItem<bool> JMdictEntryFormatter::showJMdictID("jmdict", "showJMdictID", false);
PreferenceItem<bool> JMdictEntryFormatter::searchVerbBuddy("jmdict", "searchVerbBuddy", true);
PreferenceItem<int> JMdictEntryFormatter::maxHomophonesToDisplay("jmdict", "maxHomophonesToDisplay", 5);
PreferenceItem<bool> JMdictEntryFormatter::displayStudiedHomophonesOnly("jmdict", "displayStudiedHomophonesOnly", false);
PreferenceItem<int> JMdictEntryFormatter::maxHomographsToDisplay("jmdict", "maxHomographsToDisplay", 5);
PreferenceItem<bool> JMdictEntryFormatter::displayStudiedHomographsOnly("jmdict", "displayStudiedHomographsOnly", false);

PreferenceItem<int> JMdictEntryFormatter::headerPrintSize("jmdict", "headerPrintSize", 20);
PreferenceItem<bool> JMdictEntryFormatter::printKanjis("jmdict", "printKanjis", true);
PreferenceItem<bool> JMdictEntryFormatter::printOnlyStudiedKanjis("jmdict", "printOnlyStudiedKanjis", false);
PreferenceItem<int> JMdictEntryFormatter::maxDefinitionsToPrint("jmdict", "maxDefinitionsToPrint", 0);
PreferenceItem<QString> JMdictEntryFormatter::exampleSentencesService("jmdict", "exampleSentencesService", "");

JMdictEntryFormatter &JMdictEntryFormatter::instance()
{
	static JMdictEntryFormatter _instance;
	return _instance;
}

QString JMdictEntryFormatter::getVerbBuddySql(const QString &matchPattern, quint64 pos, int id)
{
	const QString queryFindVerbBuddySql("select distinct " QUOTEMACRO(JMDICTENTRY_GLOBALID) ", jmdict.entries.id "
		"from jmdict.entries "
		"join jmdict.kanji on jmdict.kanji.id = jmdict.entries.id "
		"join jmdict.senses on jmdict.senses.id = jmdict.entries.id "
		"where jmdict.kanji.docid in "
			"(select docid from jmdict.kanjiText "
			"where jmdict.kanjiText.reading match '\"%1*\"') "
		"and jmdict.kanji.priority = 0 "
		"and jmdict.senses.pos0 & %2 == %2 "
		"and jmdict.senses.misc0 & %4 == 0 "
		"and jmdict.entries.id != %3");

        return queryFindVerbBuddySql.arg(matchPattern)
            .arg(pos)
            .arg(id)
            .arg(JMdictEntrySearcher::miscFilterMask()[0]);
}

QString JMdictEntryFormatter::getHomophonesSql(const QString &reading, int id, int maxToDisplay, bool studiedOnly)
{
	const QString queryFindHomophonesSql("select distinct " QUOTEMACRO(JMDICTENTRY_GLOBALID) ", jmdict.entries.id from jmdict.entries "
		"join jmdict.kana on jmdict.kana.id = jmdict.entries.id "
		"%4join training on training.id = jmdict.entries.id and training.type = " QUOTEMACRO(JMDICTENTRY_GLOBALID) " "
		"join jmdict.senses on jmdict.senses.id = jmdict.entries.id "
		"left join jmdict.jlpt on jmdict.entries.id = jmdict.jlpt.id "
		"where jmdict.kana.docid in "
			"(select docid from jmdict.kanaText "
			"where jmdict.kanaText.reading match '\"%1\"') "
		"and jmdict.kana.priority = 0 "
		"and jmdict.senses.misc0 & %5 == 0 "
		"and jmdict.entries.id != %3 "
		"order by training.dateAdded is null ASC, training.score ASC, jmdict.jlpt.level DESC, jmdict.entries.frequency DESC "
		"limit %2");

        return queryFindHomophonesSql.arg(reading)
            .arg(maxToDisplay)
            .arg(id)
            .arg(studiedOnly ? "" : "left ")
            .arg(JMdictEntrySearcher::miscFilterMask()[0]);
}

QString JMdictEntryFormatter::getHomographsSql(const QString &writing, int id, int maxToDisplay, bool studiedOnly)
{
	const QString queryFindHomographsSql("select distinct " QUOTEMACRO(JMDICTENTRY_GLOBALID) ", jmdict.entries.id from jmdict.entries "
		"join jmdict.kanji on jmdict.kanji.id = jmdict.entries.id "
		"%4join training on training.id = jmdict.entries.id and training.type = " QUOTEMACRO(JMDICTENTRY_GLOBALID) " "
		"join jmdict.senses on jmdict.senses.id = jmdict.entries.id "
		"left join jmdict.jlpt on jmdict.entries.id = jmdict.jlpt.id "
		"where jmdict.kanji.docid in "
			"(select docid from jmdict.kanjiText "
			"where jmdict.kanjiText.reading match '\"%1\"') "
		"and jmdict.kanji.priority = 0 "
		"and jmdict.senses.misc0 & %5 == 0 "
		"and jmdict.entries.id != %3 "
		"order by training.dateAdded is null ASC, training.score ASC, jmdict.jlpt.level DESC, jmdict.entries.frequency DESC "
		"limit %2");

        return queryFindHomographsSql.arg(writing)
            .arg(maxToDisplay)
            .arg(id)
            .arg(studiedOnly ? "" : "left ")
            .arg(JMdictEntrySearcher::miscFilterMask()[0]);
}

JMdictEntryFormatter::JMdictEntryFormatter(QObject* parent) : EntryFormatter("detailed_jmdict.css", "detailed_jmdict.html", parent)
{
	_exampleSentencesServices["Tatoeba"] = tatoebaTemplate;
	_exampleSentencesServices["Jisho.org"] = jishoTemplate;
}

void JMdictEntryFormatter::drawCustom(const ConstEntryPointer& _entry, QPainter& painter, const QRectF& rectangle, QRectF& usedSpace, const QFont& textFont, int headerPrintSize, bool printKanjis, bool printOnlyStudiedKanjis, int maxDefinitionsToPrint) const
{
	ConstJMdictEntryPointer entry(_entry.staticCast<const JMdictEntry>());
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
				ConstKanjidic2EntryPointer kanji(KanjiEntryRef(c.unicode()).get().staticCast<const Kanjidic2Entry>());
				if (!kanji) continue;
				if (printOnlyStudiedKanjis && !kanji->trained()) continue;
				QString s = QString(c) + ": " + kanji->meanings().join(", ");
				QFontMetrics metrics(painter.font(), painter.device());
				s = metrics.elidedText(s, Qt::ElideRight, (int) leftArea.width());
				textBB = painter.boundingRect(leftArea, Qt::AlignLeft, s);
				painter.drawText(leftArea, Qt::AlignLeft, s);
				if (!printOnlyStudiedKanjis && kanji->trained()) painter.drawLine(textBB.topLeft() + QPoint(0, metrics.ascent() + metrics.underlinePos()), textBB.topRight() + QPoint(0, metrics.ascent() + metrics.underlinePos()));
				leftArea.setTop(textBB.bottom());
			}
		}
	}
	// Now print definitions.
	foreach (const Sense *sense, entry->getSenses()) {
		QStringList posList;
		for (const auto &pos : sense->partOfSpeech())
			posList << pos;
		for (const auto &misc : sense->misc())
			posList << misc;
		for (const auto &dial : sense->dialect())
			posList << dial;
		for (const auto &field : sense->field())
			posList << field;

		QString posText;
		if (!posList.isEmpty()) posText = QString(" (") + posList.join(",") + ") ";
		QString s(sense->senseText());
		if (!s.isEmpty()) s[0] = s[0].toUpper();
		else continue;
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

QString JMdictEntryFormatter::shortDesc(const ConstEntryPointer &_entry) const
{
	ConstJMdictEntryPointer entry(_entry.staticCast<const JMdictEntry>());
	QString ret(autoFormat(entry->shortVersion(Entry::TinyVersion)));
	if (shortDescShowJLPT.value() && entry->jlpt() != -1) {
		ret += QString(" <b>%1</b>").arg(autoFormat(tr("(JLPT N%1)").arg(entry->jlpt())));
	}
	ret += QString(" <a href=\"entry://?type=%1&id=%2\"><img src=\"moreicon\"/></a>").arg(entry->type()).arg(entry->id());
	if (entry->trained()) {
		ret = QString("<span style=\"background-color:%1\">%2</span>").arg(colorTriplet(EntryFormatter::scoreColor(*entry))).arg(ret);
	}
	return ret;
}

QString JMdictEntryFormatter::formatHeadFurigana(const ConstEntryPointer &_entry) const
{
	ConstJMdictEntryPointer entry(_entry.staticCast<const JMdictEntry>());
	const QList<KanjiReading> &kanjis = entry->getKanjiReadings();
	const QList<KanaReading> &kanas = entry->getKanaReadings();
	if (!kanjis.isEmpty() && !kanjis[0].getKanaReadings().isEmpty() && !entry->writtenInKana())
		return kanas[kanjis[0].getKanaReadings()[0]].getReading();
	else return "";
}

QString JMdictEntryFormatter::formatHead(const ConstEntryPointer &_entry) const
{
	ConstJMdictEntryPointer entry(_entry.staticCast<const JMdictEntry>());
	const QList<KanjiReading> &kanjis = entry->getKanjiReadings();
	const QList<KanaReading> &kanas = entry->getKanaReadings();
	if (!kanjis.isEmpty() && !entry->writtenInKana()) return kanjis[0].getReading();
	else if (!kanas.isEmpty()) return kanas[0].getReading();
	else return "";
}

QString JMdictEntryFormatter::formatAltReadings(const ConstEntryPointer &_entry) const
{
	ConstJMdictEntryPointer entry(_entry.staticCast<const JMdictEntry>());
	const QList<KanjiReading> &kanjis = entry->getKanjiReadings();
	const QList<KanaReading> &kanas = entry->getKanaReadings();
	QList<KanaReading> altReadings;
	if (kanjis.isEmpty()) altReadings = kanas;
	else foreach (qint32 idx, kanjis[0].getKanaReadings()) altReadings << kanas[idx];
	if (altReadings.size() > 1) {
		QString ret(QString("<b>%1</b> ").arg(tr("Alternate readings: ")));
		for (int i = 1; i < altReadings.size(); i++) {
			if (i != 1) ret += ", ";
			ret += autoFormat(altReadings[i].getReading());
		}
		return ret;
	}
	else return "";
}

QString JMdictEntryFormatter::formatAltWritings(const ConstEntryPointer &_entry) const
{
	ConstJMdictEntryPointer entry(_entry.staticCast<const JMdictEntry>());
	const QList<KanjiReading> &kanjis = entry->getKanjiReadings();
	const QList<KanaReading> &kanas = entry->getKanaReadings();
	// Other kanji readings
	int start = entry->writtenInKana() ? 0 : 1;
	if (kanjis.size() > start) {
		QString ret(QString("<b>%1</b> ").arg(tr("Alternate writings:")));
		bool notFirstHomophone = false;
		for (int j = start; j < kanjis.size(); j++) {
			if (notFirstHomophone) ret += ", ";
			notFirstHomophone = true;
			const KanjiReading &kanjiReading = kanjis[j];
			ret += autoFormat(kanjiReading.getReading());
			const QList<qint32> &kanaReadings = kanjiReading.getKanaReadings();
			if (!kanaReadings.isEmpty()) {
				ret += " (";
				int notFirst = false;
				foreach(qint32 kanaIdx, kanaReadings) {
					if (notFirst) ret += ", ";
					ret += kanas[kanaIdx].getReading();
					notFirst = true;
				}
				ret += ")";
			}
		}
		return ret;
	}
	else return "";
}

static QString senseProps(const QSet<QString> &entities, const QString &tag, const QMap<QString, QPair<QString, quint16>> &map)
{
	QStringList ret;
	for (const auto &entity : entities) {
		QString translated = QCoreApplication::translate("JMdictLongDescs", map[entity].first.toLatin1());
		translated.replace(0, 1, translated[0].toUpper());
		ret << QString("<a href=\"%1\" title=\"%3\">%2</a>").arg(QString("longdesc://%1#%2").arg(tag).arg(entity)).arg(entity).arg(translated);
	}

	return ret.join(", ");
}

static QString senseProps(const Sense &sense)
{
	QStringList ret;
	ret << senseProps(sense.partOfSpeech(), "pos", JMdictPlugin::posMap());
	ret << senseProps(sense.misc(), "misc", JMdictPlugin::miscMap());
	ret << senseProps(sense.dialect(), "dialect", JMdictPlugin::dialMap());
	ret << senseProps(sense.field(), "field", JMdictPlugin::fieldMap());
	ret.removeAll("");
	if (!ret.isEmpty()) return QString("<span class=\"senseProps\">%1</span>").arg(ret.join(", "));
	else return "";
}

QString JMdictEntryFormatter::formatSenses(const ConstEntryPointer &_entry) const
{
	ConstJMdictEntryPointer entry(_entry.staticCast<const JMdictEntry>());
	const QList<KanjiReading> &kanjis = entry->getKanjiReadings();
	const QList<KanaReading> &kanas = entry->getKanaReadings();
	const QList<const Sense *> senses = entry->getSenses();

	QSet<QString> oldPos;
	QSet<QString> oldMisc;
	QSet<QString> oldDialect;
	QSet<QString> oldField;
	QStringList oldWritingString;
	QString ret;

	if (senses.isEmpty()) {
		ret += "<div class=\"glosses\">\n";
		ret += entry->meanings()[0];
		ret += "</div>";
	}

	foreach (const Sense *sense, senses) {
		ret += "<div class=\"sense\">\n";
		// Shall we output the part of speech?
		if (oldPos != sense->partOfSpeech() || oldMisc != sense->misc() ||
			oldDialect != sense->dialect() || oldField != sense->field()) {
			ret += senseProps(*sense);
			oldPos = sense->partOfSpeech();
			oldMisc = sense->misc();
			oldDialect = sense->dialect();
			oldField = sense->field();
		}

		// Write the entry
		ret += "<div class=\"glosses\">\n";
		QList<Gloss> glosses = sense->getGlosses();
		QStringList gls;
		foreach (const Gloss &gloss, glosses) {
			QString str(gloss.gloss());
			str.replace("\n", ", ");
			if (str.isEmpty()) continue;
			str[0] = str[0].toUpper();
			gls << QString("<span class=\"glossbody\"><img src=\"flag:%1\"/> %2.</span>").arg(gloss.lang()).arg(str.toHtmlEscaped());
		}
		ret += gls.join("<br/>");

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
			ret += QString("<span class=\"altSenseReadings\"> [ %1 ]</span>").arg(autoFormat(senseHeaders.join(", ")));
			oldWritingString = senseHeaders;
		}
		ret += "</div>\n";
		ret += "</div>\n";
	}
	return ret;
}

QString JMdictEntryFormatter::formatJLPT(const ConstEntryPointer &_entry) const
{
	ConstJMdictEntryPointer entry(_entry.staticCast<const JMdictEntry>());
	if (showJLPT.value() && entry->jlpt() != -1)
		return QString("<b>%1:</b> N%2").arg(tr("JLPT")).arg(entry->jlpt());
	else return "";
}

QString JMdictEntryFormatter::formatKanji(const ConstEntryPointer &entry) const
{
	ConstJMdictEntryPointer jEntry(entry.staticCast<const JMdictEntry>());
	QString contents;
	if (showKanjis.value() && !jEntry->getKanjiReadings().isEmpty()) {
		const QString &reading(jEntry->getKanjiReadings()[0].getReading());
		for (int i = 0; i < reading.size(); ++i) {
			if (TextTools::isKanjiChar(reading, i)) {
				QString k(reading[i]);
				if (reading[i].isHighSurrogate()) k += reading[++i];
				ConstKanjidic2EntryPointer kanji = KanjiEntryRef(TextTools::singleCharToUnicode(k)).get();
				if (kanji) {
					const EntryFormatter *formatter(EntryFormatter::getFormatter(kanji));
					contents += QString("<div class=\"kanjiinfo\">%1</div>").arg(formatter->shortDesc(kanji));
					// TODO
					//view->addWatchEntry(_entry);
				}
			}
		}
	}
	if (!contents.isEmpty()) return buildSubInfoBlock(tr("Kanji"), contents);
	else return "";
}

QString JMdictEntryFormatter::formatExampleSentencesLink(const ConstEntryPointer &_entry) const
{
	if (!exampleSentencesService.value().isEmpty()) {
		ConstJMdictEntryPointer entry(_entry.staticCast<const JMdictEntry>());
		QString tpl(_exampleSentencesServices[exampleSentencesService.value()]);
		if (!tpl.isEmpty()) {
			const QList<KanjiReading> &kanji(entry->getKanjiReadings());
			const QList<KanaReading> &kana(entry->getKanaReadings());
			const QList<const Sense *> &senses(entry->getSenses());
			if (kanji.size() == 0 || senses[0]->misc().contains("uk"))
				tpl = tpl.arg(kana[0].getReading());
			else
				tpl = tpl.arg(kanji[0].getReading());
		}
		return QString("<a href=\"%1\">%2</a>").arg(tpl).arg(tr("Example sentences"));
	}
	else return "";
}

QString JMdictEntryFormatter::formatJMdictID(const ConstEntryPointer &_entry) const
{
	ConstJMdictEntryPointer entry(_entry.staticCast<const JMdictEntry>());
	if (showJMdictID.value())
		return QString("<b>%1:</b> %2").arg(tr("JMdict ID")).arg(entry->id());
	else return "";
}

QList<DetailedViewJob *> JMdictEntryFormatter::jobVerbBuddy(const ConstEntryPointer &_entry, const QTextCursor &cursor) const
{
	QList<DetailedViewJob *> ret;
	ConstJMdictEntryPointer entry(_entry.staticCast<const JMdictEntry>());
	const QList<const Sense *> senses = entry->getSenses();

	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	QTextCharFormat bold(normal);
	bold.setFontWeight(QFont::Bold);
	bool searchVi = true, searchVt = true;

	foreach (const Sense *sense, senses) {
		if (sense->partOfSpeech().contains("vi") && searchVt) {
			ret << new FindVerbBuddyJob(entry, "vt", cursor);
			searchVt = false;
		}
		if (sense->partOfSpeech().contains("vt") && searchVi) {
			ret << new FindVerbBuddyJob(entry, "vi", cursor);
			searchVi = false;
		}
	}
	return ret;
}

QList<DetailedViewJob *> JMdictEntryFormatter::jobHomophones(const ConstEntryPointer& _entry, const QTextCursor& cursor) const
{
	ConstJMdictEntryPointer entry(_entry.staticCast<const JMdictEntry>());
	QList<DetailedViewJob *> ret;
	if (maxHomophonesToDisplay.value()) ret << new FindHomophonesJob(entry, maxHomophonesToDisplay.value(), displayStudiedHomophonesOnly.value(), cursor);
	return ret;
}

QList<DetailedViewJob *> JMdictEntryFormatter::jobHomographs(const ConstEntryPointer& _entry, const QTextCursor& cursor) const
{
	ConstJMdictEntryPointer entry(_entry.staticCast<const JMdictEntry>());
	QList<DetailedViewJob *> ret;
	if (maxHomographsToDisplay.value()) ret << new FindHomographsJob(entry, maxHomographsToDisplay.value(), displayStudiedHomographsOnly.value(), cursor);
	return ret;
}


FindVerbBuddyJob::FindVerbBuddyJob(const ConstJMdictEntryPointer& verb, const QString& pos, const QTextCursor& cursor) :
	DetailedViewJob(cursor), lastKanjiPos(0), searchedPos(pos)
{
	// Check that the pos we are looking for actually exists
	quint64 posMask(1 << JMdictPlugin::posMap()[pos].second);

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

void FindVerbBuddyJob::result(EntryPointer entry)
{
	ConstJMdictEntryPointer jEntry = entry.staticCast<const JMdictEntry>();
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
	const EntryFormatter *formatter = EntryFormatter::getFormatter(bestMatch);
	if (formatter) cursor().insertHtml(QString("<br/>%1").arg(EntryFormatter::buildSubInfoLine(QString(searchedPos == "vt" ? tr("Transitive buddy") : searchedPos == "vi" ? tr("Intransitive buddy") : tr("Buddy")), formatter->shortDesc(bestMatch))));
}

FindHomophonesJob::FindHomophonesJob(const ConstJMdictEntryPointer& entry, int maxToDisplay, bool studiedOnly, const QTextCursor& cursor) : DetailedViewJob(cursor), gotResults(false)
{
	QString s;
	// No reading? Must be a katakana entry, take the writing in this case.
	if (!entry->readings().isEmpty()) s = entry->readings()[0];
	else if (!entry->writings().isEmpty()) s = entry->writings()[0];
	else return;
	_sql = JMdictEntryFormatter::getHomophonesSql(s, entry->id(), maxToDisplay, studiedOnly);
}

void FindHomophonesJob::firstResult()
{
	cursor().insertHtml(QString("<br/>%1").arg(EntryFormatter::buildSubInfoBlock(tr("Homophones"), "")));
	_cursor.movePosition(QTextCursor::PreviousBlock);
}

void FindHomophonesJob::result(EntryPointer entry)
{
	const EntryFormatter *formatter = EntryFormatter::getFormatter(entry);
	QString format;
	if (!gotResults) gotResults = true;
	else format = "<br/>\n";
	format += formatter->shortDesc(entry);
	cursor().insertHtml(format);
}

FindHomographsJob::FindHomographsJob(const ConstJMdictEntryPointer& entry, int maxToDisplay, bool studiedOnly, const QTextCursor& cursor) : DetailedViewJob(cursor), gotResults(false)
{
	// No writing? Nothing we can possibly do here.
	if (entry->writings().isEmpty()) return;
	_sql = JMdictEntryFormatter::getHomographsSql(entry->writings()[0], entry->id(), maxToDisplay, studiedOnly);
}

void FindHomographsJob::firstResult()
{
	cursor().insertHtml(QString("<br/>%1").arg(EntryFormatter::buildSubInfoBlock(tr("Homographs"), "")));
	_cursor.movePosition(QTextCursor::PreviousBlock);
}

void FindHomographsJob::result(EntryPointer entry)
{
	const EntryFormatter *formatter = EntryFormatter::getFormatter(entry);
	QString format;
	if (!gotResults) gotResults = true;
	else format = "<br/>\n";
	format += formatter->shortDesc(entry);
	cursor().insertHtml(format);
}
