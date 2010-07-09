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

#ifndef __CORE_ENTRY_H
#define __CORE_ENTRY_H

#include <QColor>
#include <QMetaType>
#include <QDate>
#include <QSet>
#include <QObject>
#include <QSharedPointer>

#include "core/Tag.h"

class Entry : public QObject, public QSharedData
{
	Q_OBJECT
public:
	// TODO move outside of entry!
	class Note
	{
	private:
		int _id;
		QDateTime _dateAdded;
		QDateTime _dateLastChange;
		QString _note;

		/// For notes that are already inside the database - used by EntrySearcher
		Note(int id, const QDateTime &dateAdded, const QDateTime &dateLastChange, const QString &note) : _id(id), _dateAdded(dateAdded), _dateLastChange(dateLastChange), _note(note) {}
		/// For new notes to be created from Entry::addNote()
		Note(const QString &note) : _id(-1), _dateAdded(QDateTime::currentDateTime()), _dateLastChange(QDateTime::currentDateTime()), _note(note) {}
		/// Update the note from Entry::updateNote()
		void update(const QString &newNote);

		/// Resync the DB with the current state of this note. The Entry that the note belongs
		/// to must be given as parameter.
		void writeToDB(const Entry *entry);

		/// Delete this note from the database. To be called from Entry::deleteNote()
		void deleteFromDB(const Entry *entry);

	public:
		const QDateTime &dateAdded() const { return _dateAdded; }
		const QDateTime &dateLastChange() const { return _dateLastChange; }
		const QString &note() const { return _note; }
		bool operator==(const Note &note);

	friend class Entry;
	friend class EntrySearcher;
	};

private:
	quint8 _type;
	quint32 _id;
	QDateTime _dateAdded;
	QDateTime _dateLastTrain;
	QDateTime _dateLastMistake;
	unsigned int _nbTrained;
	unsigned int _nbSuccess;
	int _score;

	QSet<Tag> _tags;
	QList<Note> _notes;
	QSet<quint32> _lists;

	/**
	 * Updates the database with new training information about this
	 * entry. Automatically called by related methods.
	 */
	void updateTrainingData();

	void setDateAdded(const QDateTime &date) { _dateAdded = date; }
	void setDateLastTrained(const QDateTime &date) { _dateLastTrain = date; }
	void setDateLastMistake(const QDateTime &date) { _dateLastMistake = date; }
	void setNbTrained(unsigned int nb) { _nbTrained = nb; }
	void setNbSuccess(unsigned int nb) { _nbSuccess = nb; }

	// No copy, ever!
	Entry(const Entry &);

protected:
	qint32 _frequency;
	Entry(quint8 type, quint32 id);

public:
	// Role used for models that allow accessing entries
	enum { EntryRole = Qt::UserRole };
	
	// Must be public or QSharedPointer won't work
	virtual ~Entry();

	quint8 type() const { return _type; }
	quint32 id() const { return _id; }
	QDateTime dateAdded() const { return _dateAdded; }
	QDateTime dateLastTrain() const { return _dateLastTrain; }
	QDateTime dateLastMistake() const { return _dateLastMistake; }
	unsigned int nbTrained() const { return _nbTrained; }
	unsigned int nbSuccess() const { return _nbSuccess; }

	void addToTraining();
	void removeFromTraining();
	void setAlreadyKnown();
	void resetScore();

	/**
	 * An entry is considered to be under training if it has been added to the
	 * training list at some point.
	 */
	bool trained() const { return dateAdded().isValid(); }
	int score() const { return _score; }
	/// Returns the color associated to the score of this entry
	QColor scoreColor() const;

	/**
	 * An entry is considered as assimilated if its score is at least 95.
	 */
	bool alreadyKnown() const { return score() >= 95; }

	qint32 frequency() const { return _frequency; }

	const QSet<Tag> &tags() const { return _tags; }
	QSet<Tag> &tags() { return _tags; }
	void setTags(const QStringList &tags);
	void addTags(const QStringList &tags);

	const QList<Note> &notes() const { return _notes; }
	QList<Note> &notes() { return _notes; }
	const Note &addNote(const QString &note);
	void updateNote(Note &note, const QString &noteText);
	void deleteNote(Note &note);

	/**
	 * Returns the lists this entry belongs to. Lists are just identified by
	 * their row number, which can be used to retrieve an index in
	 * EntryListModel.
	 */
	const QSet<quint32> &lists() const { return _lists; }
	QSet<quint32> &lists() { return _lists; }

	void train(bool success, float factor = 1.0f);

	typedef enum { ShortVersion, TinyVersion } VersionLength;

	/// Returns the main representation of the entry
	virtual QString mainRepr() const;

	/**
	 * Returns a string that describes the entry in a manner suitable for being displayed
	 * in menus, etc.
	 */
	virtual QString shortVersion(VersionLength length = ShortVersion) const;
	virtual QString name() const;

	virtual QStringList writings() const = 0;
	virtual QStringList readings() const = 0;
	virtual QStringList meanings() const = 0;

signals:
	/**
	 * Emitted when the entry has changed and its views
	 * need to be redrawn.
	 * TODO should we not use a ConstEntryPointer for safety here?
	 */
	void entryChanged(Entry *);

/**
 * EntrySearcher needs to access our private methods in order to completely
 * load the entry.
 */
friend class EntrySearcher;
};

// TODO try to remove this, needed by the notes edit dialog
Q_DECLARE_METATYPE(Entry::Note *)

typedef QSharedPointer<Entry> EntryPointer;
Q_DECLARE_METATYPE(EntryPointer)
typedef QSharedPointer<const Entry> ConstEntryPointer;
Q_DECLARE_METATYPE(ConstEntryPointer)

#endif
