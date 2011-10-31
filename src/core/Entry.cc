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

#include "core/Tag.h"
#include "core/Entry.h"
#include "core/Database.h"
#include "sqlite/Query.h"

#include <QDebug>

Entry::Entry(EntryType type, EntryId id) : QObject(0), _type(type), _id(id), _dateAdded(), _dateLastTrain(), _dateLastMistake(), _nbTrained(0), _nbSuccess(0), _score(0), _frequency(-1)
{
}

Entry::~Entry()
{
}

QColor Entry::scoreColor() const
{
	int sc = score() * 5;
	return QColor(sc > 0xff ? sc < 0x1ff ? 0xff - (sc - 0x100) : 0x00 : 0xff,
	              sc < 0xff ? sc : 0xff, 0x00).lighter(165);
}

static QString dateToString(const QDateTime &date)
{
	if (!date.isValid()) return "null";
	return QString::number(date.toTime_t());
}

void Entry::updateTrainingData()
{
	SQLite::Query query(Database::connection());

	QString qString;
	if (!trained()) removeFromTraining();
	else {
		qString = "insert or replace into training values(" + QString::number(type()) + ", " + QString::number(id()) + ", " + QString::number(score()) + ", " + dateToString(dateAdded()) + ", " + dateToString(dateLastTrain()) + ", " + QString::number(nbTrained()) + ", " + QString::number(nbSuccess()) + ", " + dateToString(dateLastMistake()) + ")";
		if (!query.exec(qString)) qCritical() << "Error executing query: " << query.lastError().message();
		emit entryChanged(this);
	}
}

void Entry::train(bool success, float factor)
{
	// Can not train entries that are not in our study list
	if (!trained()) return;
	QDateTime currentTime(QDateTime::currentDateTime());
	QDateTime lastTrainTime(dateLastTrain());
	// If this is the first time we train the entry
	if (!lastTrainTime.isValid()) lastTrainTime = dateAdded();
	// Should never happen
	if (!lastTrainTime.isValid()) lastTrainTime = currentTime.addDays(-7);

	// The new score depends on:
	// - Whether we succeeded or not (of course!)
	// - When was the last time we trained - a long time means a bigger positive factor
	// - The factor argument.
	int daysNotSeen = lastTrainTime.daysTo(currentTime);

	int scoreChange = (int) (5 + (daysNotSeen * 2) * factor);
	if (scoreChange > 30) scoreChange = 30;
	if (!success) scoreChange = -scoreChange;

	int newScore(score());

	newScore += scoreChange;
	if (newScore < 0) newScore = 0;
	else if (newScore > 100) newScore = 100;

	_score = newScore;

	_nbTrained++;
	if (success) _nbSuccess++;
	setDateLastTrained(currentTime);
	if (!success) setDateLastMistake(currentTime);
	updateTrainingData();
}

void Entry::addToTraining()
{
	if (trained()) return;
	setDateAdded(QDateTime::currentDateTime());
	setDateLastTrained(QDateTime());
	setDateLastMistake(QDateTime());
	updateTrainingData();
}

void Entry::removeFromTraining()
{
	if (!trained()) return;
	// Reset all values to those of a non-trained entry
	setDateAdded(QDateTime());
	setDateLastTrained(QDateTime());
	setDateLastMistake(QDateTime());
	setNbTrained(0);
	setNbSuccess(0);
	_score = 0;
	// And delete the entry row from the training table
	QString qString = QString("delete from training where type = %1 and id = %2").arg(type()).arg(id());
	SQLite::Query query(Database::connection());
	if (!query.exec(qString)) qCritical() << "Error executing query: " << query.lastError().message();
	emit entryChanged(this);
}

void Entry::setAlreadyKnown()
{
	if (!trained()) addToTraining();
	if (score() < 95) {
		_score = 95;
	}
	updateTrainingData();
}

void Entry::resetScore()
{
	if (!trained()) return;
	_score = 0;
	updateTrainingData();
}

const Entry::Note &Entry::addNote(const QString &note)
{
	Note newNote(note);
	newNote.writeToDB(this);
	_notes << newNote;
	emit entryChanged(this);
	return _notes.last();
}

void Entry::updateNote(Note &note, const QString &noteText)
{
	note.update(noteText);
	note.writeToDB(this);
	emit entryChanged(this);
}

void Entry::deleteNote(Note &note)
{
	note.deleteFromDB(this);
	_notes.removeOne(note);
	emit entryChanged(this);
}

void Entry::Note::update(const QString &newNote)
{
	_note = newNote;
	_dateLastChange = QDateTime::currentDateTime();
}

void Entry::Note::writeToDB(const Entry *entry)
{
	SQLite::Query query(Database::connection());
	QString qString;

	// Insert the notes properties
	qString = QString("insert or replace into notes values(%1, ?, ?, ?, ?)").arg(_id == -1 ? "null" : "?");
	query.prepare(qString);
	if (_id != -1) query.bindValue(_id);
	query.bindValue(entry->type());
	query.bindValue(entry->id());
	query.bindValue(QString::number(dateAdded().toTime_t()));
	query.bindValue(QString::number(dateLastChange().toTime_t()));
	if (!query.exec()) {
		qCritical() << "Error executing query: " << query.lastError().message();
		return;
	}

	if (_id == -1) {
		_id = query.lastInsertId();
		qString = QString("insert or replace into notesText(docid, note) values(%1, ?)").arg(_id);
	}
	else qString = QString("update notesText set note = ? where docid = %1").arg(_id);
	// Now insert the note text
	query.prepare(qString);
	query.bindValue(note());
	if (!query.exec()) qCritical() << "Error executing query: " << query.lastError().message();
}

void Entry::Note::deleteFromDB(const Entry *entry)
{
	SQLite::Query query(Database::connection());
	query.prepare("delete from notes where noteId = ?");
	query.bindValue(_id);
	if (!query.exec()) qCritical() << "Error executing query: " << query.lastError().message();
	query.prepare("delete from notesText where docid = ?");
	query.bindValue(_id);
	if (!query.exec()) qCritical() << "Error executing query: " << query.lastError().message();
}

void Entry::setTags(const QStringList &tags)
{
	SQLite::Query query(Database::connection());
	if (!query.exec(QString("delete from taggedEntries where type = %1 and id = %2").arg(type()).arg(id()))) qCritical() << "Error executing query: " << query.lastError().message();
	_tags.clear();
	addTags(tags);
}

void Entry::addTags(const QStringList &tags)
{
	SQLite::Query query(Database::connection());
	query.prepare(QString("insert into taggedEntries values(%1, %2, ?, %3)").arg(type()).arg(id()).arg(QDateTime::currentDateTime().toTime_t()));
	foreach(const QString &tag, tags) {
		Tag t = Tag::getOrCreateTag(tag);
		if (!t.isValid()) {
			qCritical() << "Could not get valid tag for" << t.name();
			continue;
		}
		// Do not add tags that we already have
		if (_tags.contains(t)) continue;
		query.bindValue(t.id());
		if (!query.exec()) qCritical() << "Error executing query: " << query.lastError().message();
		_tags << t;
	}
	emit entryChanged(this);
}

bool Entry::Note::operator==(const Note &note)
{
	return _id == note._id;
}

QString Entry::mainRepr() const {
	QStringList strList(writings());
	if (!strList.isEmpty()) return strList[0];
	strList = readings();
	if (!strList.isEmpty()) return strList[0];
	return "";
}

QString Entry::shortVersion(VersionLength length) const {
	QString text;
	QStringList writes;
	QString mRepr(mainRepr());
	
	text += mRepr;

	writes = writings();
	QStringList reads(readings());
	bool reprIsWriting = writes.contains(mRepr);
	if (reprIsWriting) writes.removeAll(mRepr);
	else reads.removeAll(mRepr);
	// Show alternate writings if needed
	if (!writes.isEmpty() && length != TinyVersion) text += ", " + writes.join(", ");
	
	if (reprIsWriting && !reads.isEmpty()) {
		text += " (";
		if (length != TinyVersion) text += reads.join(", ");
		else text += reads[0];
		text += ")";
	}
	else if (!reads.isEmpty() && length != TinyVersion) text += ", " + reads.join(", ");

	// Senses
	QStringList means = meanings();
	bool hasMeaning = !means.isEmpty();
	if (hasMeaning) {
		text += ":";
		if (means.size() == 1 || length == TinyVersion) text += " " + means[0];
		else {
			int cpt = 1;
			foreach (const QString &str, means)
				text += QString(" (%1) %2").arg(cpt++).arg(str);
		}
	}
	return text;
}

QString Entry::name() const
{
	if (!writings().isEmpty()) {
		return writings()[0];
	}

	if (!readings().isEmpty()) {
		return readings()[0];
	}
	return "";
}
