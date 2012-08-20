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

#include <QString>
#include <QVector>
#include <QMap>
#include <QTemporaryFile>
#include <QDir>
#include <QCoreApplication>

struct sqlite3;

class Database
{
Q_DECLARE_TR_FUNCTIONS(Database)
private:
	/// Set to the name of the current user DB file
	static TString _userDBFile;
	/// Temporary file used to create the temporary user DB
	QTemporaryFile *_tFile;
	static QMap<TString, TString> _attachedDBs;
	static Database *_instance;

	SQLite::Connection _connection;
	Database(const QString &userDBFile = QString());
	~Database();

	bool createUserDB();
	bool updateUserDB(int currentVersion);
	bool connectUserDB(QString filename, QStringList &errors);
	bool checkUserDB(QStringList &errors);
	bool connectToTemporaryDatabase(QStringList &errors);
	void closeDB();

public:
	static bool init(const QString &userDBFile, bool temporary, QStringList &errors);
	static void stop();
	static Database *instance() { return _instance; }
	static SQLite::Connection *connection() { return &_instance->_connection; }

	static const TString &userDBFile() { return _userDBFile; }
	static const QString defaultDBFile() { return QDir(userProfile()).absoluteFilePath("user.db"); }

	static bool attachDictionaryDB(const TString &file, const TString &alias, int expectedVersion);
	static bool detachDictionaryDB(const TString &alias);
	static const QMap<TString, TString> &attachedDBs() { return _attachedDBs; }

	static const SQLite::Error &lastError() { return _instance->_connection.lastError(); }
};

#endif
