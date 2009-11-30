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

#include <QDebug>
#include <QtSql>

Entry::Entry() : QObject(0), _type(0), _id(0), _frequency(0), _dateAdded(), _dateLastTrain(), _dateLastMistake(), _nbTrained(0), _nbSuccess(0), _score(0)
{
}

Entry::Entry(int type, int id) : QObject(0), _type(type), _id(id), _frequency(0), _dateAdded(), _dateLastTrain(), _dateLastMistake(), _nbTrained(0), _nbSuccess(0), _score(0)
{
}

Entry::~Entry()
{
}

static QString dateToString(const QDateTime &date)
{
	if (!date.isValid()) return "null";
	return QString::number(date.toTime_t());
}

void Entry::updateTrainingData()
{
	QSqlQuery query;
	QString qString;
	if (!trained()) removeFromTraining();
	else {
		qString = "insert or replace into training values(" + QString::number(type()) + ", " + QString::number(id()) + ", " + QString::number(score()) + ", " + dateToString(dateAdded()) + ", " + dateToString(dateLastTrain()) + ", " + QString::number(nbTrained()) + ", " + QString::number(nbSuccess()) + ", " + dateToString(dateLastMistake()) + ")";
		if (!query.exec(qString)) qCritical() << "Error executing query: " << query.lastError().text();
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

	int scoreChange = 5 + (daysNotSeen * 2) * factor;
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
	QSqlQuery query;
	if (!query.exec(qString)) qCritical() << "Error executing query: " << query.lastError().text();
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
	QSqlQuery query;
	QString qString;

	// Insert the notes properties
	qString = QString("insert or replace into notes values(%1, ?, ?, ?, ?)").arg(_id == -1 ? "null" : "?");
	query.prepare(qString);
	if (_id != -1) query.addBindValue(_id);
	query.addBindValue(entry->type());
	query.addBindValue(entry->id());
	query.addBindValue(QString::number(dateAdded().toTime_t()));
	query.addBindValue(QString::number(dateLastChange().toTime_t()));
	if (!query.exec()) {
		qCritical() << "Error executing query: " << query.lastError().text();
		return;
	}

	if (_id == -1) {
		_id = query.lastInsertId().toInt();
		qString = QString("insert or replace into notesText(docid, note) values(%1, ?)").arg(_id);
	}
	else qString = QString("update notesText set note = ? where docid = %1").arg(_id);
	// Now insert the note text
	query.prepare(qString);
	query.addBindValue(note());
	if (!query.exec()) qCritical() << "Error executing query: " << query.lastError().text();
}

void Entry::Note::deleteFromDB(const Entry *entry)
{
	QSqlQuery query;
	query.prepare("delete from notes where noteId = ?");
	query.addBindValue(_id);
	if (!query.exec()) qCritical() << "Error executing query: " << query.lastError().text();
	query.prepare("delete from notesText where docid = ?");
	query.addBindValue(_id);
	if (!query.exec()) qCritical() << "Error executing query: " << query.lastError().text();
}

void Entry::setTags(const QStringList &tags)
{
	QSqlQuery query;
	if (!query.exec(QString("delete from taggedEntries where type = %1 and id = %2").arg(type()).arg(id()))) qCritical() << "Error executing query: " << query.lastError().text();
	_tags.clear();
	addTags(tags);
}

void Entry::addTags(const QStringList &tags)
{
	QSqlQuery query;
	query.prepare(QString("insert into taggedEntries values(%1, %2, ?, %3)").arg(type()).arg(id()).arg(QDateTime::currentDateTime().toTime_t()));
	foreach(const QString &tag, tags) {
		Tag t = Tag::getOrCreateTag(tag);
		if (!t.isValid()) {
			qCritical() << "Could not get valid tag for" << t.name();
			continue;
		}
		// Do not add tags that we already have
		if (_tags.contains(t)) continue;
		query.addBindValue(t.id());
		if (!query.exec()) qCritical() << "Error executing query: " << query.lastError().text();
		_tags << t;
	}
	emit entryChanged(this);
}

bool Entry::Note::operator==(const Note &note)
{
	return _id == note._id;
}

QString Entry::shortVersion(VersionLength length) const {
	QString text;
	QStringList strList;

	strList = writings();
	bool hasWriting = !strList.isEmpty();
	if (hasWriting) {
		if (length == TinyVersion) text += strList[0];
		else text += strList.join(", ");
	}

	strList = readings();
	bool hasReading = !strList.isEmpty();
	if (hasWriting && hasReading) text += " (";
	if (hasReading) {
		if (length == TinyVersion) text += strList[0];
		else text += strList.join(", ");
	}
	if (hasWriting && hasReading) text += ")";

	// Senses
	strList = meanings();
	bool hasMeaning = !strList.isEmpty();
	if (hasMeaning) {
		text += ":";
		if (strList.size() == 1 || length == TinyVersion) text += " " + strList[0];
		else {
			int cpt = 1;
			foreach (const QString &str, strList)
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
