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

#include "gui/EntriesPrinter.h"

#include "gui/EntryFormatter.h"
#include "gui/BookletPrinter.h"

#include <QPrintPreviewDialog>
#include <QProgressDialog>

EntriesPrinter::EntriesPrinter(QWidget* parent) : QObject(parent)
{
}

#define PRINT_MINIMAL_SPACING 10

void EntriesPrinter::printPageOfEntries(const QList<QPicture> &entries, QPainter *painter, int height)
{
	// First adjust the distance between entries
	int dist;
	if (entries.size() == 1) dist = 0;
	else {
		int totalHeight = 0;
		foreach(const QPicture &pic, entries) totalHeight += pic.height();
		// Use uniform repartition only if the space taken by the entries is more than 3/4 of the page
		if (totalHeight > height / 1.5) dist = (height - totalHeight) / entries.size() - 1;
		// Otherwise use a default space
		else dist = PRINT_MINIMAL_SPACING;
	}

	// Then print all entries drawn so far
	int pos = 0;
	foreach(const QPicture &pic, entries) {
		painter->save();
		painter->drawPicture(QPoint(0, pos), pic);
		painter->restore();
		pos += pic.height() + dist;
	}
}

void EntriesPrinter::prepareAndPrintJob(QPrinter* printer)
{
	int fromPage = -1, toPage = -1;
	// Do we have a print range specified?
	if (printer->printRange() == QPrinter::PageRange) {
		fromPage = printer->fromPage();
		toPage = printer->toPage();
	}

	// Setup progress bar
	QProgressDialog progressDialog(tr("Preparing print job..."), tr("Abort"), 0, _entries.size(), qobject_cast<QWidget *>(parent()));
	progressDialog.setMinimumDuration(50);
	progressDialog.setWindowTitle(tr("Printing..."));
	progressDialog.setWindowModality(Qt::WindowModal);
	progressDialog.show();

	// Print entries page by page
	int pageNbr = 1;
	QList<QPicture> waitingEntries;
	QPainter painter(printer);
	QRectF pageRect = painter.window();
	QRectF remainingSpace = pageRect;
	for (int i = 0; i < _entries.size(); i++) {
		if (progressDialog.wasCanceled()) return;
		QRectF usedSpace;
		QPicture tPicture;
		QPainter picPainter(&tPicture);
		ConstEntryPointer entry = qVariantValue<EntryPointer>(_entries[i].data(Entry::EntryRole));
		// An entry, print it
		if (entry) {
			const EntryFormatter *formatter(EntryFormatter::getFormatter(entry));
			if (!formatter) continue;
			formatter->draw(entry, picPainter, pageRect, usedSpace, _baseFont);
			if (!pageRect.contains(usedSpace)) {
				qDebug() << "Warning: entry does not fit on whole page, giving up this one...";
				continue;
			}
			picPainter.end();
			tPicture.setBoundingRect(usedSpace.toRect());
		}
		// Not an entry, print the text role
		else {
		}
		// Do we need a new page here?
		if (remainingSpace.height() < usedSpace.height()) {
			// Print the current page
			if (fromPage == -1 || (pageNbr >= fromPage && pageNbr <= toPage)) {
				// If not on the first page, get a new page
				if (pageNbr > 1 && pageNbr > fromPage) printer->newPage();
				printPageOfEntries(waitingEntries, &painter, pageRect.height());
			}
			remainingSpace = pageRect;
			waitingEntries.clear();
			++pageNbr;
			// Optimize if we already reached the last page
			if (fromPage != -1 && pageNbr > toPage) return;
		}
		waitingEntries << tPicture;
		// Update remaining space, taking care to keep some white between entries
		remainingSpace.setTop(remainingSpace.top() + usedSpace.height() + PRINT_MINIMAL_SPACING);

		progressDialog.setValue(i);
	}

	if (fromPage == -1 || (pageNbr >= fromPage && pageNbr <= toPage)) {
		if (pageNbr > 1 && pageNbr > fromPage) printer->newPage();
		printPageOfEntries(waitingEntries, &painter, pageRect.height());
	}
}

void EntriesPrinter::prepareAndPrintBookletJob(QPrinter* printer)
{
	BookletPrinter bPrinter(printer);
	bPrinter.setPrintRange(printer->printRange());
	bPrinter.setFromTo(printer->fromPage() * 8 - 7, printer->toPage() * 8);
	_baseFont.setPointSize(15);
	prepareAndPrintJob(&bPrinter);
	_baseFont = QFont();
}

void EntriesPrinter::print(const QModelIndexList &entries, QPrinter* printer)
{
	_entries = entries;
	prepareAndPrintJob(printer);
}

void EntriesPrinter::printPreview(const QModelIndexList &entries, QPrinter* printer)
{
	_entries = entries;
	QPrintPreviewDialog dialog(printer, qobject_cast<QWidget *>(parent()));
	dialog.setWindowTitle(tr("Print preview"));
	connect(&dialog, SIGNAL(paintRequested(QPrinter *)), this, SLOT(prepareAndPrintJob(QPrinter *)));
	dialog.exec();
}

void EntriesPrinter::printBooklet(const QModelIndexList &entries, QPrinter* printer)
{
	_entries = entries;
	prepareAndPrintBookletJob(printer);
}

void EntriesPrinter::printBookletPreview(const QModelIndexList &entries, QPrinter* printer)
{
	_entries = entries;
	QPrintPreviewDialog dialog(printer, qobject_cast<QWidget *>(parent()));
	dialog.setWindowTitle(tr("Booklet print preview"));
	connect(&dialog, SIGNAL(paintRequested(QPrinter *)), this, SLOT(prepareAndPrintBookletJob(QPrinter *)));
	dialog.exec();
}
