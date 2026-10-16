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

#include "gui/LookupUrl.h"
#include "gui/LookupApplication.h"

#include <QFileOpenEvent>
#include <QSignalSpy>
#include <QtTest>

class LookupUrlTest : public QObject {
    Q_OBJECT
  private slots:
    void searchDecoding() {
        auto lookup = LookupUrl::parse(QUrl("tagaini://search?q=%E5%8B%89%E5%BC%B7"));
        QCOMPARE(lookup.type, LookupUrl::Search);
        QCOMPARE(lookup.text, QString::fromUtf8("勉強"));
        lookup = LookupUrl::parse(QUrl("tagaini://search?q=a%20b%26c%23d%2Be%2520"));
        QCOMPARE(lookup.text, QString("a b&c#d+e%20"));
        lookup = LookupUrl::parse(QUrl("tagaini://search?q=a+b"));
        QCOMPARE(lookup.text, QString("a+b"));
    }

    void word() {
        const auto lookup = LookupUrl::parse(QUrl("tagaini://word/1000000"));
        QCOMPARE(lookup.type, LookupUrl::Word);
        QCOMPARE(lookup.entryId, quint32(1000000));
    }

    void kanji_data() {
        QTest::addColumn<QString>("url");
        QTest::addColumn<QString>("character");
        QTest::addColumn<quint32>("codepoint");
        QTest::newRow("literal") << QString::fromUtf8("tagaini://kanji/食")
                                 << QString::fromUtf8("食") << quint32(0x98DF);
        QTest::newRow("encoded") << QString("tagaini://kanji/%E9%A3%9F") << QString::fromUtf8("食")
                                 << quint32(0x98DF);
        QTest::newRow("supplementary") << QString("tagaini://kanji/%F0%A0%AE%B7")
                                       << QString::fromUtf8("𠮷") << quint32(0x20BB7);
    }

    void kanji() {
        QFETCH(QString, url);
        QFETCH(QString, character);
        QFETCH(quint32, codepoint);
        const auto lookup = LookupUrl::parse(QUrl(url, QUrl::StrictMode));
        QCOMPARE(lookup.type, LookupUrl::Kanji);
        QCOMPARE(lookup.text, character);
        QCOMPARE(lookup.entryId, codepoint);
    }

    void invalid_data() {
        QTest::addColumn<QString>("url");
        const QStringList urls = {"https://search?q=test",
                                  "tagaini:search?q=test",
                                  "tagaini://settings",
                                  "tagaini://search",
                                  "tagaini://search?q=",
                                  "tagaini://search?q=%20",
                                  "tagaini://search?q=a&q=b",
                                  "tagaini://search?q=a&extra=b",
                                  "tagaini://search/path?q=test",
                                  "tagaini://user@search?q=test",
                                  "tagaini://search:12?q=test",
                                  "tagaini://search?q=test#fragment",
                                  "tagaini://word/0",
                                  "tagaini://word/-1",
                                  "tagaini://word/+1",
                                  "tagaini://word/4294967296",
                                  "tagaini://word/abc",
                                  "tagaini://word/1/2",
                                  "tagaini://word/1%0A",
                                  "tagaini://word/1%0D%0A",
                                  "tagaini://word/1%00",
                                  "tagaini://word/1?q=test",
                                  "tagaini://word/1#fragment",
                                  "tagaini://kanji",
                                  "tagaini://kanji/",
                                  "tagaini://kanji/%E9%A3%9F%E5%93%81",
                                  "tagaini://kanji/%E9%A3%9F/extra",
                                  "tagaini://kanji/%E9%A3%9F?q=test",
                                  "tagaini://kanji/%E9%A3%9F#fragment",
                                  "tagaini://kanji/%252F",
                                  "tagaini://kanji/%2F",
                                  "tagaini://kanji/%20",
                                  "tagaini://kanji/%0A",
                                  "tagaini://kanji/%00",
                                  "tagaini://search?q=%GG"};
        for (const auto &url : urls)
            QTest::newRow(qPrintable(url)) << url;
    }

    void invalid() {
        QFETCH(QString, url);
        QCOMPARE(LookupUrl::parse(QUrl(url, QUrl::StrictMode)).type, LookupUrl::Invalid);
    }

    void eventDelivery() {
        auto *app = static_cast<LookupApplication *>(qApp);
        QSignalSpy spy(app, &LookupApplication::lookupRequested);
        QFileOpenEvent first(QUrl("tagaini://search?q=first"));
        QCoreApplication::sendEvent(app, &first);
        QCOMPARE(spy.size(), 0);
        QFileOpenEvent second(QUrl("tagaini://word/1000000"));
        QCoreApplication::sendEvent(app, &second);
        QFileOpenEvent invalid(QUrl("tagaini://word/0"));
        QCoreApplication::sendEvent(app, &invalid);
        app->setLookupReady();
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(0).toUrl(), second.url());
        app->setLookupReady();
        QCOMPARE(spy.size(), 1);
        QFileOpenEvent running(QUrl("tagaini://search?q=running"));
        QCoreApplication::sendEvent(app, &running);
        QCOMPARE(spy.size(), 2);
        QCOMPARE(spy.at(1).at(0).toUrl(), running.url());
        QFileOpenEvent kanji(QUrl("tagaini://kanji/%E9%A3%9F"));
        QCoreApplication::sendEvent(app, &kanji);
        QCOMPARE(spy.size(), 3);
        QCOMPARE(spy.at(2).at(0).toUrl(), kanji.url());
        QFileOpenEvent file(QString("/tmp/lookup-test.txt"));
        QCoreApplication::sendEvent(app, &file);
        QCOMPARE(spy.size(), 3);
        QCoreApplication::sendEvent(app, &invalid);
        QCOMPARE(spy.size(), 3);
    }

    void nestedEventDelivery() {
        auto *app = static_cast<LookupApplication *>(qApp);
        app->setLookupReady();
        QList<QUrl> delivered;
        bool handling = false;
        bool reentered = false;
        const auto connection =
            connect(app, &LookupApplication::lookupRequested, this, [&](const QUrl &url) {
                if (handling) {
                    reentered = true;
                    return;
                }
                handling = true;
                delivered << url;
                if (delivered.size() == 1) {
                    // Model URL delivery in a QMessageBox's nested event loop.
                    QCoreApplication::postEvent(app, new QFileOpenEvent(QUrl("tagaini://word/2")));
                    QCoreApplication::postEvent(app, new QFileOpenEvent(QUrl("tagaini://word/3")));
                    QCoreApplication::processEvents();
                }
                handling = false;
            });
        QFileOpenEvent first(QUrl("tagaini://word/1"));
        QCoreApplication::sendEvent(app, &first);
        disconnect(connection);
        QVERIFY(!reentered);
        QCOMPARE(delivered, QList<QUrl>({first.url(), QUrl("tagaini://word/3")}));
    }
    void stopsDuringShutdown() {
        auto *app = static_cast<LookupApplication *>(qApp);
        app->setLookupReady();
        QSignalSpy spy(app, &LookupApplication::lookupRequested);
        const auto connection =
            connect(app, &LookupApplication::lookupRequested, this, [app](const QUrl &) {
                QFileOpenEvent queued(QUrl("tagaini://word/2"));
                QCoreApplication::sendEvent(app, &queued);
                app->stopLookups();
            });
        QFileOpenEvent first(QUrl("tagaini://word/1"));
        QCoreApplication::sendEvent(app, &first);
        disconnect(connection);
        // Shutdown drops the request queued inside the active handler.
        QCOMPARE(spy.size(), 1);
        QFileOpenEvent late(QUrl("tagaini://word/3"));
        QCoreApplication::sendEvent(app, &late);
        QCOMPARE(spy.size(), 1);
        app->setLookupReady();
        QCoreApplication::sendEvent(app, &late);
        QCOMPARE(spy.size(), 1);
    }
};

int main(int argc, char **argv) {
    LookupApplication app(argc, argv);
    LookupUrlTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "LookupUrlTest.moc"
