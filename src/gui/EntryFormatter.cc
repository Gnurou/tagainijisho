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

#include "core/TextTools.h"
#include "gui/EntryFormatter.h"
#include "gui/DetailedView.h"

#include <QToolTip>

PreferenceItem<bool> EntryFormatter::shortDescShowJLPT("mainWindow/detailedView", "shortEntryShowJLPT", false);

QMap<int, EntryFormatter *> EntryFormatter::_formatters;

static const int entryTextProperties = Qt::AlignJustify | Qt::TextWordWrap;
static QFont printFont = QFont("", 14);

EntryFormatter::EntryFormatter()
{
}

bool EntryFormatter::registerFormatter(const int entryType, EntryFormatter *formatter)
{
	if (_formatters.contains(entryType)) return false;
	_formatters[entryType] = formatter;
	return true;
}

bool EntryFormatter::removeFormatter(const int entryType)
{
	if (!_formatters.contains(entryType)) return false;
	_formatters.remove(entryType);
	return true;
}

QColor EntryFormatter::scoreColor(const Entry *entry) const
{
	int sc = entry->score() * 5;
	return QColor(sc > 0xff ? sc < 0x1ff ? 0xff - (sc - 0x100) : 0x00 : 0xff,
					  sc < 0xff ? sc : 0xff, 0x00).lighter(165);
}

void EntryFormatter::writeUserData(const Entry *entry, QTextCursor &cursor, DetailedView *view) const
{
	QTextCharFormat normal(DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText));
	QTextCharFormat bold(normal);
	bold.setFontWeight(QFont::Bold);

	// Tags
	if (!entry->tags().isEmpty()) {
		cursor.insertBlock(QTextBlockFormat());
		cursor.setCharFormat(normal);
		cursor.insertImage("tagicon");
		bool first = true;
		foreach(const Tag &tag, entry->tags()) {
			if (!first) cursor.insertText("  ");
			else first = false;
			QTextCharFormat linkFormat(normal);
			linkFormat.setAnchor(true);
			linkFormat.setAnchorHref(QString("tag://%1").arg(tag.name()));
			cursor.mergeCharFormat(linkFormat);
			cursor.insertText(tag.name());
			cursor.setCharFormat(normal);
		}
	}

	//Notes
	if (!entry->notes().isEmpty()) {
		QTextTableFormat tableFormat;
		tableFormat.setBorder(1);
		tableFormat.setBorderBrush(Qt::black);
		tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
		tableFormat.setBackground(QColor(Qt::yellow).lighter());
		QVector<QTextLength> constraints;
		constraints << QTextLength(QTextLength::PercentageLength, 100);
		tableFormat.setColumnWidthConstraints(constraints);
		cursor.insertTable(1, 1, tableFormat);

		cursor.setCharFormat(bold);
		cursor.insertText(tr("Notes:"));
		cursor.setCharFormat(normal);
		bool first = true;
		cursor.setCharFormat(normal);
		foreach(const Entry::Note &note, entry->notes()) {
			if (!first) cursor.insertText("\n");
			cursor.insertBlock(QTextBlockFormat());
			cursor.setCharFormat(normal);
			autoFormat(entry, note.note(), cursor);
			first = false;
		}
		cursor.movePosition(QTextCursor::NextBlock);
	}

	// Training data
	if (entry->trained()) {
		QTextBlockFormat trainingFormat;
		trainingFormat.setAlignment(Qt::AlignRight);
		QTextCharFormat charFormat(normal);
		QFont font(charFormat.font());
		font.setPointSize(font.pointSize() / 1.5);
		charFormat.setFont(font);
		charFormat.setFontItalic(true);
		cursor.insertBlock(trainingFormat);
		cursor.setCharFormat(charFormat);
		cursor.insertText(tr("Studied since %1.").arg(entry->dateAdded().date().toString(Qt::DefaultLocaleShortDate)));
		cursor.insertText(tr(" Score: %1.").arg(entry->score()));
		if (entry->dateLastTrain().isValid()) {
			cursor.insertText(tr(" Last trained on %1.").arg(entry->dateLastTrain().date().toString(Qt::DefaultLocaleShortDate)));
			if (entry->dateLastMistake().isValid()) {
				cursor.insertText(tr(" Last mistaken on %1.").arg(entry->dateLastMistake().date().toString(Qt::DefaultLocaleShortDate)));
			}
		}
		else cursor.insertText(tr(" Never trained."));
		cursor.setCharFormat(normal);
	}
}

void EntryFormatter::detailedVersion(const Entry *entry, QTextCursor &cursor, DetailedView *view) const
{
	_detailedVersion(entry, cursor, view);

	cursor.insertBlock(QTextBlockFormat());
	// Now write user data
	writeUserData(entry, cursor, view);

	cursor.insertBlock(QTextBlockFormat());
}

void EntryFormatter::autoFormat(const Entry *entry, const QString &str, QTextCursor &cursor, const QTextCharFormat &mergeWith) const
{
	if (str.isEmpty()) return;

	QTextCharFormat saveFormat(cursor.charFormat());
	enum { None, Kanji, Kana, Romaji } curChar = None, nextChar;
	QTextCharFormat format;
	int pos = 0;
	int written = 0;
	while (pos < str.size()) {
		// Guess the format of the next char
		if (TextTools::isKanjiChar(str[pos])) nextChar = Kanji;
		else if (TextTools::isKanaChar(str[pos])) nextChar = Kana;
		else nextChar = Romaji;
		if (nextChar != curChar) {
			if (curChar == Kanji) format = DetailedViewFonts::charFormat(DetailedViewFonts::Kanji);
			else if (curChar == Kana) format = DetailedViewFonts::charFormat(DetailedViewFonts::Kana);
			else format = DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText);
			format.merge(mergeWith);
			cursor.setCharFormat(format);
			cursor.insertText(str.mid(written, pos - written));
			curChar = nextChar;
			written = pos;
		}
		++pos;
	}
	if (written < str.size()) {
		if (curChar == Kanji) format = DetailedViewFonts::charFormat(DetailedViewFonts::Kanji);
		else if (curChar == Kana) format = DetailedViewFonts::charFormat(DetailedViewFonts::Kana);
		else format = DetailedViewFonts::charFormat(DetailedViewFonts::DefaultText);
		format.merge(mergeWith);
		cursor.setCharFormat(format);
		cursor.insertText(str.mid(written, str.size() - written));
	}
	cursor.setCharFormat(saveFormat);
}

void EntryFormatter::drawInfo(const Entry *entry, QPainter &painter, QRectF &rectangle, const QFont &textFont) const
{
	// Draw notes
	if (!entry->notes().isEmpty()) {
		QFont italic(textFont);
		italic.setItalic(true);
		painter.setFont(italic);
		foreach (const Entry::Note &note, entry->notes()) {
			QString s(note.note());
			QRectF textBB = painter.boundingRect(rectangle, Qt::AlignLeft | Qt::TextWordWrap, s);
			painter.drawText(rectangle, Qt::AlignLeft | Qt::TextWordWrap, s);
			rectangle.setTop(textBB.bottom());
		}
	}
}

void EntryFormatter::draw(const Entry *entry, QPainter &painter, const QRectF &rectangle, QRectF &usedSpace, const QFont &textFont) const
{
	painter.save();

	painter.setFont(printFont);
	QString text = QString("  ") + entry->shortVersion();
	usedSpace = painter.boundingRect(rectangle, entryTextProperties, text);
	painter.drawText(rectangle, entryTextProperties, text);

	painter.restore();
}

void EntryFormatter::writeEntryTitle(const Entry *entry, QTextCursor &cursor) const
{
	QTextCharFormat scoreFormat;
	if (entry->trained()) {
		scoreFormat.setBackground(scoreColor(entry));
	}
	QString title;
	if (!entry->writings().isEmpty()) title = entry->writings()[0];
	else if (!entry->readings().isEmpty()) title = entry->readings()[0];
	autoFormat(entry, title, cursor, scoreFormat);
}
