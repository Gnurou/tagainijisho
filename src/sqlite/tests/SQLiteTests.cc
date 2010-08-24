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
	QVERIFY(!connection.connected());
	QVERIFY(connection.dbFileName().isEmpty());
	QVERIFY(!connection.lastError().isError());
	// State after connection
	QVERIFY(connection.connect(dbFile.fileName()));
	QVERIFY(connection.connected());
	QCOMPARE(connection.dbFileName(), dbFile.fileName());
	QVERIFY(!connection.lastError().isError());
	// Shall not be able to connect twice
	QVERIFY(!connection.connect(bogusFile.fileName()));
	QVERIFY(connection.connected());
	QCOMPARE(connection.dbFileName(), dbFile.fileName());
	QVERIFY(connection.lastError().isError());
}

void SQLiteTests::connectionAttach()
{
	QVERIFY(connection.attach(attachedFile.fileName(), "attacheddb"));
}

void SQLiteTests::queryBlank()
{
	QVERIFY(query.connection() == 0);
	QVERIFY(!query.isValid());
	QCOMPARE(query.columnsCount(), 0);
}

void SQLiteTests::queryCreate()
{
	query.useWith(&connection);
	QVERIFY(query.connection() == &connection);
	QVERIFY(query.isValid());
	QVERIFY(!connection.lastError().isError());
	QCOMPARE(query.columnsCount(), 0);
	QVERIFY(!query.next());
	QVERIFY(!query.seek(0, false));
	QVERIFY(!query.valueAvailable(0));
	QVERIFY(query.prepare("create table test(col1 int, col2 bigint, col3 float, col4 text, col5 blob, col6 tinyint)"));
	QVERIFY(!connection.lastError().isError());
	QVERIFY(query.exec());
	QVERIFY(!connection.lastError().isError());
	QVERIFY(!query.next());
	QVERIFY(!connection.lastError().isError());
}

void SQLiteTests::queryPrepare()
{
	QVERIFY(query.prepare("insert into test values(?, ?, ?, ?, ?, ?)"));
}

void SQLiteTests::queryInsert_data()
{
	QTest::addColumn<quint32>("col1");
	QTest::addColumn<qlonglong>("col2");
	QTest::addColumn<double>("col3");
	QTest::addColumn<QString>("col4");
	QTest::addColumn<QByteArray>("col5");
	QTest::addColumn<qint64>("rowid");

	QByteArray bArray;
	bArray.append("abcdefghifklmnop");
	QTest::newRow("first line")
		<< (quint32)0xffffffff
		<< (qlonglong)0xffffffffffffffff
		<< 3.14157
		<< QString::fromUtf8("あいうえお")
		<< bArray
		<< (qint64)1;
	bArray = QByteArray();
	QTest::newRow("second line")
		<< (quint32)0
		<< (qlonglong)0
		<< 0.0
		<< QString::fromUtf8("")
		<< bArray
		<< (qint64)2;
}

void SQLiteTests::queryInsert()
{
	QFETCH(quint32, col1);
	QFETCH(qlonglong, col2);
	QFETCH(double, col3);
	QFETCH(QString, col4);
	QFETCH(QByteArray, col5);
	QFETCH(qint64, rowid);

	query.reset();
	QVERIFY(query.bindValue(col1));
	QVERIFY(query.bindValue(col2));
	QVERIFY(query.bindValue(col3));
	QVERIFY(query.bindValue(col4));
	QVERIFY(query.bindValue(col5));
	QVERIFY(query.bindNullValue());
	QVERIFY(query.exec());
	QVERIFY(!query.next());
	QCOMPARE(query.columnsCount(), 0);
	QCOMPARE(query.lastInsertId(), rowid);
}

void SQLiteTests::queryRetrieve_data()
{
	queryInsert_data();
}

void SQLiteTests::queryRetrieve()
{
	QFETCH(quint32, col1);
	QFETCH(qlonglong, col2);
	QFETCH(double, col3);
	QFETCH(QString, col4);
	QFETCH(QByteArray, col5);
	QFETCH(qint64, rowid);

	QVERIFY(query.prepare("select * from test where rowid = ?"));
	QVERIFY(query.bindValue(rowid));
	QVERIFY(query.exec());
	QCOMPARE(query.columnsCount(), 6);
	QVERIFY(query.next());
	QCOMPARE(query.columnsCount(), 6);

	QVERIFY(query.valueAvailable(0));
	QCOMPARE(query.valueType(0), SQLite::Integer);
	QCOMPARE(query.valueUInt(0), col1);

	QVERIFY(query.valueAvailable(1));
	QCOMPARE(query.valueType(1), SQLite::Integer);
	QCOMPARE(query.valueInt64(1), col2);

	QVERIFY(query.valueAvailable(2));
	QCOMPARE(query.valueType(2), SQLite::Float);
	QCOMPARE(query.valueDouble(2), col3);

	QVERIFY(query.valueAvailable(3));
	QCOMPARE(query.valueType(3), SQLite::String);
	QCOMPARE(query.valueString(3), col4);

	QVERIFY(query.valueAvailable(4));
	QCOMPARE(query.valueType(4), SQLite::Blob);
	QCOMPARE(query.valueBlob(4), col5);

	QVERIFY(query.valueAvailable(5));
	QCOMPARE(query.valueType(5), SQLite::Null);
	QVERIFY(query.valueIsNull(5));

	// No data in 7th column
	QVERIFY(!query.valueAvailable(6));
	QCOMPARE(query.valueType(6), SQLite::None);
	// Only one line in the results set
	QVERIFY(!query.next());
}

void SQLiteTests::queryRetrieveAll()
{
	QVERIFY(query.exec("select * from test"));
	QVERIFY(query.next());
	QVERIFY(query.next());
	QVERIFY(!query.next());
}

void SQLiteTests::transaction()
{
}

void SQLiteTests::queryClean()
{
	// Otherwise the connection will refuse to close
	query.useWith(0);
	QVERIFY(!query.isValid());
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
