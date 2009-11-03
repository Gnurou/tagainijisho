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

#ifndef __CORE_DATABASE_H_
#define __CORE_DATABASE_H_

#include "core/Preferences.h"

#include <QThread>
#include <QtSql>

#include <QString>
#include <QRegExp>
#include <QVector>
#include <QMap>

struct sqlite3;

class Database : public QThread
{
	Q_OBJECT
private:
	static QMap<QString, QString> _attachedDBs;
	static Database *instance;

	QSqlDatabase database;
	sqlite3 *sqliteHandler;
	Database(QObject *parent = 0);

	bool createUserDB();
	bool updateUserDB(int currentVersion);
	void connectUserDB();
	void checkUserDB();
	void closeDB();

private slots:
	// Synchronous
	void quit();

protected:
	void run();

public:
	static void startThreaded();
	static void startUnthreaded();
	static void stop();

	static bool attachDictionaryDB(const QString &file, const QString &alias, int expectedVersion);
	static bool detachDictionaryDB(const QString &alias);
	static const QMap<QString, QString> &attachedDBs() { return _attachedDBs; }
	static Database *getInstance() { return instance; }
	static bool isThreaded();
	static Qt::ConnectionType aSyncConnection();

	static QVector<QRegExp> staticRegExps;

	static bool transaction() { return instance->database.transaction(); }
	static bool rollback() { return instance->database.rollback(); }
	static bool commit() { return instance->database.commit(); }
	static QSqlError lastError() { return instance->database.lastError(); }

	static PreferenceItem<QString> userProfile;

	/**
	 * Interrupt the running query in the database thread. When this function returns,
	 * the query is completely stopped.
	 */
	static void abortQuery();
};

#endif
