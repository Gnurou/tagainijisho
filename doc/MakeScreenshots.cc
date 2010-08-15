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

#include "MakeScreenshots.h"
#include "core/Database.h"
#include "gui/SearchWidget.h"
#include "gui/MainWindow.h"

#include <QtDebug>

#include <QLabel>

void MakeScreenshots::makeScreenshot(QWidget *widget, const QString &saveTo)
{
	widget->show();
	widget->update();
	QImage image(widget->size(), QImage::Format_ARGB32);
	widget->render(&image);
	Q_ASSERT(image.save(saveTo));
}

void MakeScreenshots::initTestCase()
{
	Database::startThreaded();
}

void MakeScreenshots::cleanupTestCase()
{
	Database::stop();
}

void MakeScreenshots::mainWindow()
{
	MainWindow *widget = new MainWindow();
	makeScreenshot(widget, "mainwindow.png");
	delete widget;
}

void MakeScreenshots::trainingWindow()
{
}

void MakeScreenshots::searchWidget()
{
	SearchWidget *widget = new SearchWidget();
	makeScreenshot(widget, "searchwidget.png");
	delete widget;
}

QTEST_MAIN(MakeScreenshots)
