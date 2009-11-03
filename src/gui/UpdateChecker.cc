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

#include <QUrl>

#if defined(Q_WS_X11)
#define PLATFORM "Unix/X11"
#elif defined(Q_WS_WIN)
#define PLATFORM "Windows"
#elif defined(Q_WS_WIN)
#define PLATFORM "Mac"
#elif defined(Q_WS_WIN)
#define PLATFORM "Unix/Embedded"
#endif

UpdateChecker::UpdateChecker(QObject *parent) : QObject(parent)
{
	_http = new QHttp("www.tagaini.net", 80, this);
	_buffer = new QBuffer(this);

	connect(_http, SIGNAL(requestFinished(int, bool)),
			this, SLOT(requestFinished(int, bool)));
}

UpdateChecker::~UpdateChecker()
{
}

void UpdateChecker::checkForUpdates()
{
	QHttpRequestHeader request("GET", "/updates/latestversion.php");
	request.setValue("Host", "www.tagaini.net");
	request.setValue("User-Agent", QString("Tagaini Jisho %1 (%2)").arg(QUOTEMACRO(VERSION)).arg(PLATFORM));
	_http->request(request, 0, _buffer);
}

void UpdateChecker::requestFinished(int id, bool error)
{
	if (error) return;
	QString buffer(QString(_buffer->data()).trimmed());
	// If the first character is not a digit, this means we got another page
	if (!buffer[0].isDigit()) return;
	QString latestVersion(buffer.contains('\n') ? buffer.left(buffer.indexOf('\n')) : buffer);
	if (QString(QUOTEMACRO(VERSION)).compare(latestVersion) < 0) emit updateAvailable(latestVersion);
}

