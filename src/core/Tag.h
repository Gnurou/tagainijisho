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

#ifndef __CORE_TAG_H
#define __CORE_TAG_H

#include <QString>
#include <QHash>
#include <QDateTime>
#include <QList>
#include <QStringList>
#include <QAbstractItemModel>
#include <QCoreApplication>

/**
 * Provides a model of all the tags that we met so far for the completer.
 */
class TagsListModel : public QAbstractListModel
{
	Q_OBJECT
private:
	QStringList _data;

public:
	TagsListModel(QObject *parent = 0) : QAbstractListModel(parent) {}
	virtual ~TagsListModel() {}
	int rowCount(const QModelIndex &parent = QModelIndex()) const { return _data.size(); }
	QVariant data(const QModelIndex &index, int role) const;
	bool contains(const QString &str) const { return _data.contains(str, Qt::CaseInsensitive); }
	bool containsMatch(const QString &str) const { return _data.indexOf(QRegExp(str, Qt::CaseInsensitive, QRegExp::Wildcard)) != -1; }
	const QStringList &contents() const { return _data; }

	void operator<<(const QString &str);
};

class Entry;

/**
 * A tag that can be applied to entries, giving a way to group them
 * according to the user's preference.
 */
class Tag
{
	Q_DECLARE_TR_FUNCTIONS(Tag)
private:
	static Tag _invalid;
	/// A set of all the tags we know, used for auto-completion
	static TagsListModel knownTags;

	quint32 _id;
	QString _name;

	const Tag &__createTag(QString tagName);
	Tag(quint32 id, const QString &name) : _id(id), _name(name) {}

public:
	static void init();
	static void cleanup();
	static TagsListModel *knownTagsModel() { return &knownTags; }

	quint32 id() const { return _id; }
	const QString &name() const { return _name; }

	/**
	 * Returns whether this tag is valid.
	 */
	bool isValid() const { return id() != 0; }

	bool operator==(const Tag &tag) const;

	/**
	 * Returns true if the given string is valid for a tag.
	 */
	static bool isValidTag(const QString &string);

	/**
	 * Returns the tag corresponding to the string given as argument,
	 * or the invalid tag if none is found.
	 */
	static Tag getTag(const QString &tagString);

	static Tag getTag(quint32 id);

	/**
	 * Returns the tag corresponding to the string given as argument,
	 * creating it if none exists yet. The tag returned by this method
	 * is always valid.
	 */
	static Tag getOrCreateTag(const QString &tagString);
};

inline uint qHash(const Tag &tag)
{
	return tag.id();
}

#endif
