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
#include "core/EntryListModel.h"
#include "gui/EntryFormatter.h"
#include "gui/DetailedView.h"

#include <QFile>

PreferenceItem<bool> EntryFormatter::shortDescShowJLPT("mainWindow/detailedView", "shortEntryShowJLPT", false);

QMap<int, EntryFormatter *> EntryFormatter::_formatters;

static const int entryTextProperties = Qt::AlignJustify | Qt::TextWordWrap;
static QFont printFont = QFont("", 14);

EntryFormatter::EntryFormatter(const QString& _cssFile, const QString& _htmlFile, QObject* parent) : QObject(parent)
{
	// Try to load custom html file
	QString htmlFile;
	if (!_htmlFile.isEmpty()) htmlFile = lookForFile(_htmlFile);
	if (htmlFile.isEmpty()) htmlFile = lookForFile("detailed_default.html");
	if (htmlFile.isEmpty()) qCritical("%s", tr("Cannot find detailed view HTML file!").toUtf8().constData());
	else {
		QFile f(htmlFile);
		f.open(QIODevice::ReadOnly);
		_html = QString::fromUtf8(f.readAll());
	}
	// Load default css file
	QString cssFile(lookForFile("detailed_default.css"));
	if (cssFile.isEmpty()) qCritical("%s", tr("Cannot find detailed view CSS file!").toUtf8().constData());
	else {
		QFile f(cssFile);
		f.open(QIODevice::ReadOnly);
		_css = QString::fromUtf8(f.readAll());
	}
	// Append custom CSS if specified
	if (!_cssFile.isEmpty()) {
		cssFile = lookForFile(_cssFile);
		if (!cssFile.isEmpty()) {
			QFile f(cssFile);
			f.open(QIODevice::ReadOnly);
			_css += QString::fromUtf8(f.readAll());
		}
	}
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

void EntryFormatter::drawInfo(const ConstEntryPointer &entry, QPainter &painter, QRectF &rectangle, const QFont &textFont) const
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

void EntryFormatter::draw(const ConstEntryPointer& entry, QPainter& painter, const QRectF& rectangle, QRectF& usedSpace, const QFont& textFont) const
{
	painter.save();

	painter.setFont(printFont);
	QString text = QString("  ") + entry->shortVersion();
	usedSpace = painter.boundingRect(rectangle, entryTextProperties, text);
	painter.drawText(rectangle, entryTextProperties, text);

	painter.restore();
}

QString EntryFormatter::colorTriplet(const QColor &color)
{
	return QString("#%1%2%3").arg(QString::number(color.red(), 16), 2, '0').arg(QString::number(color.green(), 16), 2, '0').arg(QString::number(color.blue(), 16), 2, '0');
}

typedef enum { Romaji, Kanji, Kana } formatType;

QString formatString(const QString &str, formatType type)
{
	switch (type) {
		case Kanji:
			return QString("<span class=\"kanji\">%1</span>").arg(str);
		case Kana:
			return QString("<span class=\"kana\">%1</span>").arg(str);
		default:
			return str;
	}
}

QString EntryFormatter::autoFormat(const QString &str) const
{
	QString ret;
	formatType curChar = Romaji, nextChar;
	
	int pos = 0;
	int written = 0;
	while (pos < str.size()) {
		// Guess the format of the next char
		if (TextTools::isKanjiChar(str[pos])) nextChar = Kanji;
		else if (TextTools::isKanaChar(str[pos])) nextChar = Kana;
		else nextChar = Romaji;
		// Late initialization of curChar
		if (pos == 0) curChar = nextChar;
		if (nextChar != curChar) {
			ret += formatString(str.mid(written, pos - written), curChar);
			curChar = nextChar;
			written = pos;
		}
		++pos;
	}
	// Write the remainder of the string
	if (written < str.size()) {
		ret += formatString(str.right(str.size() - written), curChar);
	}
	return ret;
}

QString EntryFormatter::entryTitle(const ConstEntryPointer& entry) const
{
	QString title(entry->mainRepr());
	title = autoFormat(title);
	if (entry->trained()) {
		title = QString("<span style=\"background-color:%1\">%2</span>").arg(colorTriplet(entry->scoreColor())).arg(title);
	}
	return title;
}

QString EntryFormatter::shortDesc(const ConstEntryPointer &entry) const
{
	QString ret(autoFormat(entry->shortVersion(Entry::TinyVersion)));
	ret += QString(" <a href=\"entry://?type=%1&id=%2\"><img src=\"moreicon\"/></a>").arg(entry->type()).arg(entry->id());
	if (entry->trained()) {
		ret = QString("<span style=\"background-color:%1\">%2</span>").arg(colorTriplet(entry->scoreColor())).arg(ret);
	}
	return ret;
}

QString EntryFormatter::buildSubInfoLine(const QString &title, const QString &content)
{
	return QString("<table class=\"subinfo\"><tr><td class=\"title\" valign=\"center\">%1:</td><td class=\"contents\"> %2</td></tr></table>").arg(title).arg(content);
}

QString EntryFormatter::buildSubInfoBlock(const QString &title, const QString &content)
{
	return QString("<table width=\"100%\"class=\"subinfo\"><tr><td class=\"title\">%1</td></tr><tr><td class=\"contents\">%2</td></tr></table>").arg(title).arg(content);
}


QString EntryFormatter::formatHeadFurigana(const ConstEntryPointer &entry) const
{
	QStringList writings(entry->writings());
	QStringList readings(entry->readings());
	if (writings.contains(entry->mainRepr()) && !readings.isEmpty())
		return readings[0];
	else return "";
}

QString EntryFormatter::formatHead(const ConstEntryPointer &entry) const
{
	return entry->mainRepr();
}

QString EntryFormatter::formatLists(const ConstEntryPointer &entry) const
{
	if (!entry->lists().isEmpty()) {
		EntryListModel listModel;
		QStringList ret;
		ret << "<img src=\"listicon\">   ";
		foreach (quint32 id, entry->lists()) {
			QModelIndex idx(listModel.index(id));
			QString label(listModel.data(idx.parent(), Qt::DisplayRole).toString());
			if (label.isEmpty()) label = tr("<Root>");
			QUrl url("list://");
			url.addQueryItem("rowid", QString::number(id));
			ret << QString("<a href=\"%1\">%2</a>").arg(QString(url.toEncoded())).arg(autoFormat(label));
		}
		return ret.join(" ");
	}
	else return "";
}

QString EntryFormatter::formatTags(const ConstEntryPointer &entry) const
{
	if (!entry->tags().isEmpty()) {
		QString ret("<img src=\"tagicon\"> ");
		bool first = true;
		foreach(const Tag &tag, entry->tags()) {
			if (!first) ret += "   ";
			else first = false;
			QUrl url("tag://");
			url.addQueryItem("tag", tag.name());
			ret += QString("<a href=\"%1\">%2</a>").arg(QString(url.toEncoded())).arg(autoFormat(tag.name()));
		}
		return ret;
	}
	else return "";
}

QString EntryFormatter::formatNotes(const ConstEntryPointer &entry) const
{
	if (!entry->notes().isEmpty()) {
		/*QString ret("<div class=\"title\">" + tr("Notes:") + "</div>");
		foreach(const Entry::Note &note, entry->notes()) {
			ret += QString("<p>%1</p>").arg(autoFormat(note.note()));
		}
		return ret;*/
		QStringList notes;
		foreach(const Entry::Note &note, entry->notes())
			notes << QString("<p>%1</p>").arg(autoFormat(note.note()));
			return notes.join("<hr/>");
	}
	else return "";
}

QString EntryFormatter::formatTrainingData(const ConstEntryPointer &entry) const
{
	// Training data
	if (entry->trained()) {
		QStringList ret;
		ret << tr("Studied since %1.").arg(entry->dateAdded().date().toString(Qt::DefaultLocaleShortDate));
		ret << tr("Score: %1.").arg(entry->score());
		if (entry->dateLastTrain().isValid()) {
			ret << tr("Last trained on %1.").arg(entry->dateLastTrain().date().toString(Qt::DefaultLocaleShortDate));
			if (entry->dateLastMistake().isValid()) {
				ret << tr("Last mistaken on %1.").arg(entry->dateLastMistake().date().toString(Qt::DefaultLocaleShortDate));
			}
		}
		return ret.join(" ");
	}
	else return "";
}
