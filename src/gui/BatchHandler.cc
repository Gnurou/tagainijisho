/*
 *  Copyright (C) 2012  Alexandre Courbot
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
#include "gui/BatchHandler.h"

#include <QProgressDialog>
#include <QMessageBox>

/**
 * Apply a custom handler to a selection after beginning a database transaction.
 * This is a commonly-used pattern when marking lots of entries from an entry view.
 */
void BatchHandler::applyOnSelection(const BatchHandler &handler, const QModelIndexList &selection, QWidget *parent)
{
	QProgressDialog progressDialog(tr("Marking entries..."), tr("Abort"), 0, selection.size(), parent);
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	if (!Database::connection()->transaction()) {
		QMessageBox::warning(0, tr("Cannot start transaction"), QString(tr("Error while trying to start database transaction.")));
		return;
	}
	bool completed = true;
	foreach (const QModelIndex &index, selection) {
		EntryPointer entry = qVariantValue<EntryPointer>(index.data(Entry::EntryRole));
		if (progressDialog.wasCanceled()) {
			completed = false;
			break;
		}
		progressDialog.setValue(i++);
		if (!entry) continue;
		handler.apply(entry);
	}
	if (!completed) {
		Database::connection()->rollback();
	}
	else if (!Database::connection()->commit()) {
		Database::connection()->rollback();
		QMessageBox::warning(0, tr("Cannot commit transaction"), QString(tr("Error while trying to commit database transaction.")));
		return;
	}
}

void BatchHandler::applyOnEntries(const BatchHandler &handler, const QList<EntryPointer> &entries, QWidget *parent)
{
	QProgressDialog progressDialog(tr("Marking entries..."), tr("Abort"), 0, entries.size(), parent);
	progressDialog.setMinimumDuration(1000);
	progressDialog.setWindowTitle(tr("Operation in progress..."));
	progressDialog.setWindowModality(Qt::WindowModal);

	int i = 0;
	if (!Database::connection()->transaction()) {
		QMessageBox::warning(0, tr("Cannot start transaction"), QString(tr("Error while trying to start database transaction.")));
		return;
	}
	bool completed = true;
	foreach (const EntryPointer &entry, entries) {
		if (progressDialog.wasCanceled()) {
			completed = false;
			break;
		}
		progressDialog.setValue(i++);
		if (!entry) continue;
		handler.apply(entry);
	}
	if (!completed) {
		Database::connection()->rollback();
	}
	else if (!Database::connection()->commit()) {
		Database::connection()->rollback();
		QMessageBox::warning(0, tr("Cannot commit transaction"), QString(tr("Error while trying to commit database transaction.")));
		return;
	}
}
