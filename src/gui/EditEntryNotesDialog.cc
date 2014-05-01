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

#include "EditEntryNotesDialog.h"

#include <QtDebug>

#include <QVBoxLayout>
#include <QPushButton>

PreferenceItem<QByteArray> EditEntryNotesDialog::windowGeometry("notesWindow", "geometry", "");

QVariant EntryNotesModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) return QVariant();

	if (index.row() >= rowCount()) return QVariant();

	if (role == NoteRole) return QVariant::fromValue(&_entry.notes()[index.row()]);
	else if (role == Qt::DisplayRole) return _entry.notes()[index.row()].note();

	return QVariant();
}

QVariant EntryNotesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole) return QVariant();

	return tr("Notes");
}

bool EntryNotesModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	entry().updateNote(_entry.notes()[index.row()], value.toString());
	emit dataChanged(index, index);
	return true;
}

bool EntryNotesModel::insertRows(int row, int count, const QModelIndex & parent)
{
	// Here we cannot control where notes are inserted - this is not a problem,
	// as the GUI only makes it possible to insert one note at the end.
	beginInsertRows(QModelIndex(), row, row + count);
	for (int i = 0; i < count; i++)
		entry().addNote(tr("<New note>"));
	endInsertRows();
	return true;
}

bool EntryNotesModel::removeRows(int row, int count, const QModelIndex & parent)
{
	beginRemoveRows(QModelIndex(), row, row + count);
	for (int i = 0; i < count; i++)
		entry().deleteNote(entry().notes()[row + i]);
	endRemoveRows();
	return true;
}

QString EditEntryNotesDialogEntryDelegate::displayText(const QVariant & value, const QLocale & locale) const {
	return QStyledItemDelegate::displayText(value, locale).section(QChar::LineSeparator, 0, 0);
}

QSize EditEntryNotesDialogEntryDelegate::sizeHint (const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QSize(300, QStyledItemDelegate::sizeHint(option, index).height());
}

EditEntryNotesDialog::EditEntryNotesDialog(Entry &entry, QWidget *parent) : QDialog(parent), currentNote(0)
{
	restoreGeometry(windowGeometry.value());

	setWindowTitle(tr("Notes for %1").arg(entry.name()));
	setModal(true);

	QLabel *existingLabel = new QLabel(tr("Existing notes:"), this);
	model = new EntryNotesModel(entry, this);
	view = new QListView(this);
	view->setItemDelegate(new EditEntryNotesDialogEntryDelegate(this));
	view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
	view->setModel(model);
	QItemSelectionModel *selModel = view->selectionModel();
	connect(selModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
	this, SLOT(displayNote(const QItemSelection &, const QItemSelection &)));

	textEdit = new QTextEdit(this);
	textEdit->setAcceptRichText(false);
	textEdit->setEnabled(false);

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(close()));
	QPushButton *button = buttonBox->addButton(tr("&New note"), QDialogButtonBox::ActionRole);
	connect(button, SIGNAL(clicked()), this, SLOT(newNote()));
	deleteButton = buttonBox->addButton(tr("&Delete note"), QDialogButtonBox::ActionRole);
	deleteButton->setEnabled(false);
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteNote()));

	QVBoxLayout *layout = new QVBoxLayout(this);	
	layout->addWidget(existingLabel);
	layout->addWidget(view, 1);
	layout->addWidget(textEdit, 2);
	layout->addWidget(buttonBox);

	if (model->rowCount() >= 1) view->setCurrentIndex(model->index(0, 0));
}

EditEntryNotesDialog::~EditEntryNotesDialog()
{
	windowGeometry.set(saveGeometry());
}

void EditEntryNotesDialog::displayNote(const QItemSelection &selected, const QItemSelection &deselected)
{
	// Update current note if changed
	if (currentNote != 0 && currentNote->note() != textEdit->toPlainText())
		model->setData(deselected.indexes()[0], textEdit->toPlainText());

	currentNote = 0;
	textEdit->clear();
	textEdit->setEnabled(false);
	deleteButton->setEnabled(false);
	if (selected.indexes().size() != 1) return;

	currentNote = selected.indexes()[0].data(EntryNotesModel::NoteRole).value<Entry::Note *>();
	deleteButton->setEnabled(true);
	textEdit->setEnabled(true);
	textEdit->setPlainText(currentNote->note());
	textEdit->moveCursor(QTextCursor::End);
	textEdit->setFocus();
}

void EditEntryNotesDialog::newNote()
{
	int row = model->rowCount();
	model->insertRow(row);
	view->selectionModel()->select(model->index(row), QItemSelectionModel::ClearAndSelect);
	textEdit->selectAll();
}

void EditEntryNotesDialog::deleteNote()
{
	textEdit->clear();
	currentNote = 0;
	deleteButton->setEnabled(false);
	textEdit->setEnabled(false);
	model->removeRow(view->selectionModel()->selectedIndexes()[0].row());
}

void EditEntryNotesDialog::closeEvent(QCloseEvent *event)
{
	// Update current note if changed
	if (currentNote != 0 && currentNote->note() != textEdit->toPlainText())
		model->setData(view->selectionModel()->selectedIndexes()[0], textEdit->toPlainText());
}
