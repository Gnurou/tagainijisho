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

void SQLiteTests::queryBlank()
{
	QVERIFY(query.connection() == 0);
	QVERIFY(query.isValid() == false);
}

void SQLiteTests::queryCreate()
{
	query.useWith(&connection);
	QVERIFY(query.connection() == &connection);
	QVERIFY(query.isValid() == true);
	QVERIFY(query.next() == false);
	QVERIFY(query.seek(0, false) == false);
	QVERIFY(query.value(0).isValid() == false);
	QVERIFY(query.prepare("create table test(col1 int, col2 bigint, col3 float, col4 text, col5 blob, col6 tinyint)"));
	QVERIFY(query.exec());
	QVERIFY(!query.next());
}

void SQLiteTests::queryPrepare()
{
	QVERIFY(query.prepare("insert into test values(?, ?, ?, ?, ?, ?)"));
}

void SQLiteTests::queryBind()
{
	query.reset();
	QVERIFY(query.bindValue((quint32)0xffffffff));
	QVERIFY(query.bindValue((qlonglong)0xffffffffffffffff));
	QVERIFY(query.bindValue(3.14157));
	QVERIFY(query.bindValue(QString::fromUtf8("あいうえお")));
	QByteArray bArray;
	bArray.append("abcdefghifklmnop");
	QVERIFY(query.bindValue(bArray));
	QVERIFY(query.bindValue(QVariant(QVariant::String)));
}

void SQLiteTests::queryInsert()
{
	QVERIFY(query.exec());
	QVERIFY(!query.next());
	QCOMPARE(query.lastInsertId(), (qint64)1);
}

void SQLiteTests::queryRetrieve()
{
	QVERIFY(query.prepare("select * from test"));
	QVERIFY(query.exec());
	QVERIFY(query.next());
	QCOMPARE(query.value(0).type(), QVariant::LongLong);
	QCOMPARE(query.value(0).toULongLong(), (quint64)0xffffffff);
	QCOMPARE(query.value(1).type(), QVariant::LongLong);
	QCOMPARE(query.value(1).toULongLong(), (quint64)0xffffffffffffffff);
	QCOMPARE(query.value(2).type(), QVariant::Double);
	QCOMPARE(query.value(2).toDouble(), 3.14157);
	QCOMPARE(query.value(3).type(), QVariant::String);
	QCOMPARE(query.value(3).toString(), QString::fromUtf8("あいうえお"));
	QCOMPARE(query.value(4).type(), QVariant::ByteArray);
	QByteArray bArray;
	bArray.append("abcdefghifklmnop");
	QCOMPARE(query.value(4).toByteArray(), bArray);
	QVERIFY(query.value(5).isNull());
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
