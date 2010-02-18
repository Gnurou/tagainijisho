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

#ifndef __GUI_TAGSDIALOGS_H
#define __GUI_TAGSDIALOGS_H

#include "core/Tag.h"
#include "core/EntriesCache.h"

#include <QLineEdit>
#include <QDialog>
#include <QQueue>

/**
 * A line edit that provides auto-completion on tags.
 */
class TagsLineInput : public QLineEdit
{
	Q_OBJECT
private:
	QCompleter *tagsCompleter;

public:
	TagsLineInput(QWidget *parent = 0);

public slots:
	void checkCompletion();
	void onTextChanged(const QString &newText);
	void completeText(const QString &completion);

signals:
	/// Emitted when all the tags are valid
	void allValidTags();
};

class TagsInputDialog : public QDialog
{
	Q_OBJECT
private:
	TagsLineInput *lineEdit;

public:
	TagsInputDialog(const QString &title, const QString &message, const QStringList &initialTags = QStringList(), bool selectAll = false, QWidget *parent = 0);
	QString getText() { return lineEdit->text(); }
};

class TagsDialogs {
	Q_DECLARE_TR_FUNCTIONS(TagsDialogs)
public:
	/// List of the last added tags, for tag menu creation
	static QQueue<QStringList> lastAddedTags;

	/**
	 * Splits all the tags given in string and adds valid tags to tagsList,
	 * and invalid tags to invalidTags
	 * Returns true if all the tags were valid.
	 */
	static bool splitTagsString(const QString &string, QStringList &tagsList, QStringList &invalidTags);

	/**
	 * Invokes the dialog to set tags for the list of entries given as parameter.
	 * Returns true if the tags have changed, false otherwise.
	 */
	static bool setTagsDialog(const QList<EntryPointer> &entries, QWidget *parent = 0);

	static bool addTagsDialog(const QList<EntryPointer> &entries, QWidget *parent = 0);
};

#endif
