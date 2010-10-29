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

#ifndef ORDEREDRBTREEDBTESTS_H
#define ORDEREDRBTREEDBTESTS_H

#include <QObject>
#include <QTest>

#include "core/EntryListDB.h"
#include "sqlite/Connection.h"
#include "sqlite/Query.h"
#include "core/OrderedRBDBNode.h"

#include <QTemporaryFile>

class OrderedRBTreeDBTests : public QObject
{
Q_OBJECT
private:
	SQLite::Connection connection;
	QTemporaryFile dbFile;
	// For entry-related tests
	EntryList listDB;

	// For tree-related tests
	DBList<QString> stringListDB;
        OrderedRBTree<OrderedRBDBTree<QString> > tree;

private slots:
	void initTestCase();
	void cleanupTestCase();

	void createTableTest();
	void insertDataTest();
	void retrieveDataTest();
	void updateDataTest();
	void removeDataTest();

        void createTreeTest();
        void massInsertTest();
        void massRemoveTest();

public:
	OrderedRBTreeDBTests() : listDB("testList"), stringListDB("stringList"), tree() {}
};

#endif // ORDEREDRBTREEDBTESTS_H

