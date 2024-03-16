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

#include <QtDebug>

#include "core/Paths.h"
#include "gui/UpdateChecker.h"
#include "tagaini_config.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#if defined(Q_OS_LINUX)
#define PLATFORM "Linux"
#elif defined(Q_OS_WIN)
#define PLATFORM "Windows"
#elif defined(Q_OS_OSX)
#define PLATFORM "Mac OS X"
#elif defined(Q_OS_ANDROID)
#define PLATFORM "Android"
#else
#define PLATFORM "Unknown"
#endif

UpdateChecker::UpdateChecker(const QString &versionURL, QObject *parent)
    : QObject(parent), _versionURL(versionURL) {
    _http = new QNetworkAccessManager(this);

    connect(_http, SIGNAL(finished(QNetworkReply *)), this, SLOT(finished(QNetworkReply *)));
}

UpdateChecker::~UpdateChecker() {}

void UpdateChecker::checkForUpdates(bool beta) {
    QNetworkRequest request(QUrl("http://www.tagaini.net" + _versionURL));
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      QString("Tagaini Jisho %1 (%2)").arg(VERSION).arg(PLATFORM));
    _http->get(request);
}

void UpdateChecker::finished(QNetworkReply *reply) {
    QString buffer(QString(reply->readAll()).trimmed());
    // If the first character is not a digit, this means we got another page
    if (!buffer[0].isDigit())
        return;
    QString latestVersion(buffer.contains('\n') ? buffer.left(buffer.indexOf('\n')) : buffer);
    if (QString(VERSION).compare(latestVersion) < 0)
        emit updateAvailable(latestVersion);
}
