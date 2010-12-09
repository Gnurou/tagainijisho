/*
 *  Copyright (C) 2010  Alexandre Courbot
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

#include <QObject>
#include <QTest>

#include "sqlite/Connection.h"
#include "sqlite/Query.h"

#include <QTemporaryFile>

/**
 * Test class that checks the database interface.
 */
class SQLiteTests : public QObject
{
	Q_OBJECT
private:
	SQLite::Connection connection;
	SQLite::Query query;

	QTemporaryFile dbFile;
	QTemporaryFile attachedFile;

private slots:
	void initTestCase();
	void cleanupTestCase();

	void connectionConnect();
	void connectionAttach();
	void queryBlank();
	void queryCreate();
	void queryPrepare();
	void queryInsert_data();
	void queryInsert();
	void queryRetrieve_data();
	void queryRetrieve();
	void queryRetrieveAll();
	void transaction();
	void queryClean();
	void connectionDetach();
	void connectionClose();
};
