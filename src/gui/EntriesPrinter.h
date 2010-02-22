/*
 *  Copyright (C) 2008/2009/2010  Alexandre Courbot
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

#ifndef __GUI_ENTRIES_PRINTER_H
#define __GUI_ENTRIES_PRINTER_H

#include <QAbstractListModel>
#include <QModelIndexList>
#include <QWidget>
#include <QFont>
#include <QPrinter>
#include <QPicture>

/**
 * Pretty-prints entries, either in regular of booklet form.
 */
class EntriesPrinter : public QObject
{
	Q_OBJECT
private:
	const QAbstractListModel *_model;
	const QModelIndexList &_selection;
	QFont _baseFont;
	QWidget *_parent;

	/**
	 * Present the user with a config dialog to setup printing. Returns true
	 * if the user confirmed the dialog, false if he cancelled it.
	 */
	bool askForPrintOptions(QPrinter &printer, const QString &title = tr("Print"));
	/**
	 * Prints all the given entries (pre-printed into QPictures) on one page. The
	 * entries must all fit within the range 0..height, and height must not be higher
	 * than the available height of the painter.
	 */
	void printPageOfEntries(const QList<QPicture> &entries, QPainter *painter, int height);

protected slots:
	/**
	 * Print the job on the given printer, using the default print font.
	 * An alternative font can be specified if font is not null.
	 */
	void prepareAndPrintJob(QPrinter* printer);
	void prepareAndPrintBookletJob(QPrinter* printer);

public:
	EntriesPrinter(const QAbstractListModel *model, const QModelIndexList &selection = QModelIndexList(), QWidget *parent = 0);
	
	void print();
	void printPreview();
	void printBooklet();
	void printBookletPreview();
};

#endif
