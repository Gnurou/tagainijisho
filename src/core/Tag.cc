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
#include "core/Database.h"
#include "core/Tag.h"
#include "core/Entry.h"

Tag Tag::_invalid(0, "");
TagsListModel Tag::knownTags;

void TagsListModel::operator<<(const QString &str)
{
	 if (!_data.contains(str)) {
		 _data << str;
		 qSort(_data.begin(), _data.end());
	 }
}

QVariant TagsListModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole || role == Qt::EditRole) return _data[index.row()];
	return QVariant();
}

void Tag::init()
{
	SQLite::Query query(Database::connection());
	query.exec("select tag from tags");
	while (query.next()) {
		knownTags << QString::fromUtf8(query.valueString(0).c_str());
	}
}

void Tag::cleanup()
{
}

Tag Tag::getTag(const QString &tagString)
{
	SQLite::Query query(Database::connection());

	query.exec(QString("select docid, tag from tags where tag match '%1'").arg(tagString));
	if (!query.next()) return _invalid;
	return Tag(query.valueInt(0), QString::fromUtf8(query.valueString(1).c_str()));
}

Tag Tag::getTag(quint32 id)
{
	SQLite::Query query(Database::connection());

	query.exec(QString("select docid, tag from tags where docid = %1").arg(id));
	if (!query.next()) return _invalid;
	return Tag(query.valueInt(0), QString::fromUtf8(query.valueString(1).c_str()));
}

Tag Tag::getOrCreateTag(const QString &tagString)
{
	Tag tag(getTag(tagString));
	if (tag.isValid()) return tag;

	SQLite::Query query(Database::connection());
	if (!query.exec(QString("insert into tags values('%1')").arg(tagString))) {
		qCritical() << "Error executing query: " << query.lastError().message();
		return _invalid;
	}
	if (!query.exec(QString("select docid from tags where tag match '%1'").arg(tagString))) {
		qCritical() << "Error executing query: " << query.lastError().message();
		return _invalid;
	}
	query.next();
	int id = query.valueInt(0);
	knownTags << tagString;
	return Tag(id, tagString);
}

bool Tag::operator==(const Tag &tag) const
{
	return id() == tag.id();
}

bool Tag::isValidTag(const QString &string)
{
	if (string.isEmpty()) return false;

	//return (TextTools::isRomaji(string) || TextTools::isJapanese(string));

	// A tag is valid if it is made of letters, digits, or Japanese characters
	foreach(const QChar &c, string) {
		switch (c.category()) {
			case QChar::Punctuation_InitialQuote:
			case QChar::Punctuation_FinalQuote:
			case QChar::Separator_Space:
			case QChar::Separator_Line:
				return false;
			default:
				continue;
		}
		//if (!(c.category() & (QChar::Number_DecimalDigit | QChar::Letter_Lowercase | QChar::Letter_Uppercase) ||
		//		TextTools::isJapaneseChar(c))) return false;
	}
	return true;
}
