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

#include "sqlite/Connection.h"

#include "core/Paths.h"
#include "core/Preferences.h"

#include <QThread>
#include <QString>
#include <QRegExp>
#include <QVector>
#include <QMap>
#include <QTemporaryFile>
#include <QDir>

struct sqlite3;

class Database : public QThread
{
	Q_OBJECT
private:
	static void dbWarning(const QString &message);
	/// Set to the name of the current user DB file
	static QString _userDBFile;
	/// Temporary file used to create the temporary user DB
	QTemporaryFile *_tFile;
	static QMap<QString, QString> _attachedDBs;
	static Database *_instance;

	SQLite::Connection _connection;
	sqlite3 *sqliteHandler;
	Database(const QString &userDBFile = QString(), bool temporary = false, QObject *parent = 0);
	~Database();

	bool createUserDB();
	bool updateUserDB(int currentVersion);
	bool connectUserDB(QString filename = "");
	bool checkUserDB();
	bool connectToTemporaryDatabase();
	void closeDB();

private slots:
	// Synchronous
	void quit();

protected:
	void run();

public:
	static void startThreaded(const QString &userDBFile = QString(), bool temporary = false);
	static void startUnthreaded(const QString &userDBFile = QString(), bool temporary = false);
	static SQLite::Connection *connection() { return &_instance->_connection; }

	static void stop();
	static const QString &userDBFile() { return _userDBFile; }
	static const QString defaultDBFile() { return QDir(userProfile()).absoluteFilePath("user.db"); }

	static bool attachDictionaryDB(const QString &file, const QString &alias, int expectedVersion);
	static bool detachDictionaryDB(const QString &alias);
	static const QMap<QString, QString> &attachedDBs() { return _attachedDBs; }
	static Database *instance() { return _instance; }
	static bool isThreaded();
	static Qt::ConnectionType aSyncConnection();

	static QVector<QRegExp> staticRegExps;

	// Begin immediate cannot work correctly here because it tries to get a lock on the attached databases, which are read-only
	//static bool transaction() { bool r = instance->database.exec("BEGIN IMMEDIATE TRANSACTION").isValid(); qDebug() << "T" << r; if (!r) qDebug() << instance->database.lastError().text(); return r; }
	static bool transaction() { return _instance->_connection.transaction(); }
	static bool rollback() { return _instance->_connection.rollback(); }
	static bool commit() { return _instance->_connection.commit(); }
	static const SQLite::Error &lastError() { return _instance->_connection.lastError(); }

	static void sqliteFix();
};

#endif
