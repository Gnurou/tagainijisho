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

#include "SQLiteTests.h"

#include <QtDebug>
#include <QTemporaryFile>

void SQLiteTests::initTestCase()
{
	QVERIFY(dbFile.open());
	QVERIFY(attachedFile.open());
}

void SQLiteTests::cleanupTestCase()
{
}

void SQLiteTests::connectionConnect()
{
	QTemporaryFile bogusFile;

	// Initial state
	QVERIFY(connection.connected() == false);
	QVERIFY(connection.dbFileName() == "");
	QVERIFY(connection.lastError().isError() == false);
	// State after connection
	QVERIFY(connection.connect(dbFile.fileName()) == true);
	QVERIFY(connection.connected() == true);
	QVERIFY(connection.dbFileName() == dbFile.fileName());
	QVERIFY(connection.lastError().isError() == false);
	// Shall not be able to connect twice
	QVERIFY(connection.connect(bogusFile.fileName()) == false);
	QVERIFY(connection.connected() == true);
	QVERIFY(connection.dbFileName() == dbFile.fileName());
	QVERIFY(connection.lastError().isError() == true);
}

void SQLiteTests::connectionAttach()
{
	QVERIFY(connection.attach(attachedFile.fileName(), "attacheddb") == true);
}

void SQLiteTests::connectionDetach()
{
	QVERIFY(connection.detach("attacheddb") == true);
}

void SQLiteTests::connectionClose()
{
	QVERIFY(connection.close() == true);
	QVERIFY(connection.connected() == false);
	QVERIFY(connection.dbFileName().isEmpty());
	QVERIFY(connection.lastError().isError() == false);
	// Cannot close twice
	QVERIFY(connection.close() == false);
	QVERIFY(connection.lastError().isError() == true);
	// Can safely connect after closing
	QVERIFY(connection.connect(dbFile.fileName()) == true);
	QVERIFY(connection.connected() == true);
	QVERIFY(connection.dbFileName() == dbFile.fileName());
	QVERIFY(connection.lastError().isError() == false);
	// Close and verify again
	QVERIFY(connection.close() == true);
	QVERIFY(connection.connected() == false);
	QVERIFY(connection.dbFileName().isEmpty());
	QVERIFY(connection.lastError().isError() == false);
}

QTEST_MAIN(SQLiteTests)
