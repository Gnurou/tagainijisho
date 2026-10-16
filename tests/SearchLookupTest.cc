/*
 *  Copyright (C) 2026  Alex Ehlke
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

#include "core/Database.h"
#include "core/EntriesCache.h"
#include "core/EntryListCache.h"
#include "core/Tag.h"
#include "gui/EntryTypeFilterWidget.h"
#include "gui/SearchWidget.h"
#include "gui/TextFilterWidget.h"

#include <QSignalSpy>
#include <QtTest>

class SearchLookupTest : public QObject {
    Q_OBJECT
  private slots:
    void initTestCase() {
        EntriesCache::init();
        QStringList errors;
        QVERIFY2(Database::init(QString(), true, errors), qPrintable(errors.join("\n")));
        Tag::init();
    }

    void repeatedLookupHistory() {
        SearchWidget search;
        search.show();
        QCoreApplication::processEvents();
        QSignalSpy queries(search.searchBuilder(), &SearchBuilder::queryRequested);
        const QString first = QString::fromUtf8("勉強");
        const QString second = QString::fromUtf8("食");
        search.searchText(first);
        search.searchText(first);
        search.searchText(second);
        // Each lookup is one search, with no intermediate empty history item.
        QCOMPARE(queries.size(), 3);
        auto *text = qobject_cast<TextFilterWidget *>(search.getSearchFilter("searchtext"));
        QVERIFY(text);
        search.goPrev();
        QCOMPARE(text->text(), first);
        search.goNext();
        QCOMPARE(text->text(), second);
    }

    void clearsPreviousFilters() {
        SearchWidget search;
        search.show();
        QCoreApplication::processEvents();
        auto *type =
            qobject_cast<EntryTypeFilterWidget *>(search.getSearchFilter("entrytypeselector"));
        QVERIFY(type);
        type->setType(EntryTypeFilterWidget::Kanjis);
        QSignalSpy queries(search.searchBuilder(), &SearchBuilder::queryRequested);
        const QString query = QString::fromUtf8("勉強");
        search.searchText(query);
        QCOMPARE(type->type(), int(EntryTypeFilterWidget::All));
        QCOMPARE(queries.size(), 1);
        QCOMPARE(queries.first().first().toString(), query);
    }

    void invalidLookupClearsPreviousResults() {
        SearchWidget search;
        search.show();
        QCoreApplication::processEvents();
        // Seed an unrendered result: this check concerns list replacement,
        // so no dictionary entry needs to be loaded.
        search.resultsList()->addResult(EntryRef());
        QCOMPARE(search.resultsList()->nbResults(), 1);
        search.searchText(":unsupported-command");
        QCOMPARE(search.resultsList()->nbResults(), 0);
    }

    void cleanupTestCase() {
        Tag::cleanup();
        EntryListCache::cleanup();
        Database::stop();
        EntriesCache::cleanup();
    }
};

QTEST_MAIN(SearchLookupTest)
#include "SearchLookupTest.moc"
