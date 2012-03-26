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
#include "core/jmdict/JMdictPlugin.h"
#include "core/kanjidic2/Kanjidic2Plugin.h"
#include "gui/SearchWidget.h"
#include "gui/MainWindow.h"
#include "gui/jmdict/JMdictGUIPlugin.h"
#include "gui/kanjidic2/Kanjidic2GUIPlugin.h"
#include "gui/kanjidic2/KanaSelector.h"

#include <QtDebug>

MainWindow *_mainWindow;

void MakeScreenshots::makeScreenshot(QWidget *widget, const QString &saveTo)
{
	// This sequence forces a layout update
	widget->show();
	widget->update();
	widget->hide();

	// Write to the image
	QImage image(widget->size(), QImage::Format_ARGB32);
	widget->render(&image);
	Q_ASSERT(image.save(saveTo));
}

void MakeScreenshots::initTestCase()
{
	QStringList dbErr;
	Database::init("", true, dbErr);
	// Initialize tags
	Tag::init();

	EntriesCache::init();
	// Register core plugins
	Plugin *kanjidic2Plugin = new Kanjidic2Plugin();
	Plugin *jmdictPlugin = new JMdictPlugin();
	//Plugin *tatoebaPlugin = new TatoebaPlugin();
	if (!Plugin::registerPlugin(kanjidic2Plugin))
		qFatal("Error registering kanjidic2 plugin!");
	if (!Plugin::registerPlugin(jmdictPlugin))
		qFatal("Error registering JMdict plugin!");

	_mainWindow = new MainWindow();

	Plugin *kanjidic2GUIPlugin = new Kanjidic2GUIPlugin();
	Plugin *jmdictGUIPlugin = new JMdictGUIPlugin();
	if (!Plugin::registerPlugin(jmdictGUIPlugin))
		qFatal("Error registering JMdict GUI plugin!");
	if (!Plugin::registerPlugin(kanjidic2GUIPlugin))
		qFatal("Error registering kanjidic2 GUI plugin!");
}

void MakeScreenshots::cleanupTestCase()
{
	//_mainWindow->close();
	delete _mainWindow;
	Database::stop();
}

void MakeScreenshots::mainWindow()
{
	_mainWindow->listDockWidget()->setVisible(true);
	Kanjidic2GUIPlugin::instance()->kanaDockWidget()->setVisible(true);
	makeScreenshot(_mainWindow, "mainwindow.png");
	_mainWindow->listDockWidget()->setVisible(false);
	Kanjidic2GUIPlugin::instance()->kanaDockWidget()->setVisible(false);
	// Recalculate main window layout
	_mainWindow->show();
	_mainWindow->update();
	_mainWindow->hide();
}

void MakeScreenshots::trainingWindow()
{
}

void MakeScreenshots::searchWidget()
{
	QWidget *widget = _mainWindow->searchDockWidget();
	makeScreenshot(widget, "searchwidget.png");
}

QTEST_MAIN(MakeScreenshots)
