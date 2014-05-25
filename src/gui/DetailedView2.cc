/*
 *  Copyright (C) 2014  Alexandre Courbot
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

#include "gui/DetailedView2.h"
#include "core/EntriesCache.h"

#include <QWebView>
#include <QWebFrame>
#include <QDir>
#include <QUrl>

#include <QtDebug>

DetailedView2::DetailedView2(QWidget *parent) : QWebView(parent)
{
	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
	frame = page()->mainFrame();

	connect(frame, &QWebFrame::javaScriptWindowObjectCleared, [this] {
		frame->addToJavaScriptWindowObject("entryLoader", new EntryLoader2());
	});

	connect(this, &QWebView::loadFinished, [this](bool ok) {
		if (ok)
			frame->evaluateJavaScript(QString("showEntry(%1, %2)").arg(1).arg(1573150));
	});

	QString path(QString("file://") + QDir(QDir::currentPath()).filePath("detailedview.html"));
	load(QUrl(path));

	qDebug() << path;
}

EntryLoader2::EntryLoader2()
{
}

QString EntryLoader2::loadEntry(unsigned int type, unsigned int code)
{
	EntryPointer entry(EntryRef(type, code).get());
	return QString("{ \"id\": \"%1\" }").arg(entry->writings()[0]);
}
