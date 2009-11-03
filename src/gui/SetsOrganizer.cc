/*
 *  Copyright (C) 2009  Alexandre Courbot
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

#include "gui/SetsOrganizer.h"

#include <QtSql>
#include <QMenu>
#include <QPushButton>
#include <QMessageBox>

SetTreeItem::SetTreeItem(int setId, int position, bool isFolder, const QString &label) : QTreeWidgetItem(isFolder ? FolderType : SetType), _setId(setId), _position(position), _parentCopy(0)
{
	setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);
	if (type() == FolderType) {
		setFlags(flags() | Qt::ItemIsDropEnabled);
		setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
	}
	else setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
	setText(0, label);
}

SetTreeItem::SetTreeItem(const SetTreeItem &other) : QTreeWidgetItem(other.type())
{
	*this = other;
	_parentCopy = other.parent();
}

SetTreeItem::~SetTreeItem()
{
}

void SetTreeItem::setData(int column, int role, const QVariant & value)
{
	if (column == 0 && role == Qt::EditRole) {
		QSqlQuery query;
		query.prepare("UPDATE sets SET label = ? WHERE rowid = ?");
		query.addBindValue(value);
		query.addBindValue(setId());
		if (!query.exec()) {
			qDebug() << "Error executing query:" << query.lastError();
			return;
		}
	}
	QTreeWidgetItem::setData(column, role, value);
}

SetsTreeWidget::SetsTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
	_mimeTypes << "tagaini/settreeitem";
}

void SetsTreeWidget::dropEvent(QDropEvent *event)
{
	QAbstractItemView::dropEvent(event);
}

QMimeData *SetsTreeWidget::mimeData(const QList<QTreeWidgetItem *> items) const
{
	QByteArray ba;
	QDataStream ds(&ba, QIODevice::WriteOnly);
	foreach (QTreeWidgetItem *item, items) {
		SetTreeItem *clone = new SetTreeItem(*static_cast<SetTreeItem *>(item));
		populateFolder(clone);
		// This is sooo bad... >_<;
		ds << (quint64)clone;
	}
	QMimeData *md = new QMimeData();
	md->setData("tagaini/settreeitem", ba);
	return md;
}

bool SetsTreeWidget::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action)
{
	SetTreeItem *movedItem;
	SetTreeItem *prevParent, *newParent(static_cast<SetTreeItem *>(parent));
	int prevParentId, newParentId(newParent ? newParent->setId() : 0);
	QByteArray ba = data->data("tagaini/settreeitem");
	QDataStream ds(&ba, QIODevice::ReadOnly);
	while (!ds.atEnd()) {
		int prevPosition, newPosition(index);
		// This is bad... >_<;
		quint64 tmp;
		ds >> tmp; movedItem = (SetTreeItem *)tmp;
		prevPosition = movedItem->position();
		prevParent = static_cast<SetTreeItem *>(movedItem->parentCopy());
		prevParentId = prevParent ? prevParent->setId() : 0;
		// Fix the new position with respect to the real database
		if (index > 0 && (prevParent == newParent && index > prevPosition)) newPosition--;

		// Update the position of the item
		movedItem->setPosition(newPosition);

		// Only update the layout if there is any change
		if (!(prevParent == newParent && prevPosition == newPosition)) {
			QSqlQuery query;
#define QUERY(q) { if (!query.exec(q)) qDebug() << "Query failed" << query.lastError(); }
			// Update the positions of the items after the one we removed
			QUERY(QString("UPDATE sets SET position = position - 1 where parent %1 and position > %2").arg(!prevParentId ? "is null" : QString("= %1").arg(prevParentId)).arg(prevPosition));
			QList<SetTreeItem *> siblings(childsOf(prevParent));
			foreach (SetTreeItem *sibling, siblings) {
				int sibPos = sibling->position();
				if (sibPos > prevPosition) sibling->setPosition(sibPos - 1);
			}
			// Update the positions of the items after the one we add
			QUERY(QString("UPDATE sets SET position = position + 1 where parent %1 and position >= %2").arg(!newParentId ? "is null" : QString("= %1").arg(newParentId)).arg(newPosition));
			siblings = childsOf(newParent);
			foreach (SetTreeItem *sibling, siblings) {
				// Do not update the item
				if (sibling == movedItem) continue;
				int sibPos = sibling->position();
				if (sibPos >= newPosition) sibling->setPosition(sibPos + 1);
			}
			// Move our item
			QUERY(QString("UPDATE sets SET position = %1, parent = %2 where rowid = %3").arg(newPosition).arg(!newParentId ? "null" : QString("%1").arg(newParentId)).arg(movedItem->setId()));
		}
		// Now put our item to its new position
		if (!newParent) insertTopLevelItem(index, movedItem);
		else newParent->insertChild(index, movedItem);
#undef QUERY
	}
	return true;
}

void SetsTreeWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	menu.addAction(QIcon(":/images/icons/delete.png"), tr("Delete"), this, SLOT(deleteSelection()));
	menu.exec(event->globalPos());
}

void SetsTreeWidget::deleteSet(SetTreeItem *item)
{
	SetTreeItem *parent = static_cast<SetTreeItem *>(item->parent());
	// If the row is a folder, delete recursively
	if (item->isFolder()) {
		while (item->childCount()) {
			SetTreeItem *child(static_cast<SetTreeItem *>(item->child(0)));
			deleteSet(child);
		}
	}
	QSqlQuery query;
	// Delete the row
	query.prepare("DELETE FROM sets where rowid = ?");
	query.addBindValue(item->setId());
	if (!query.exec()) {
		qDebug() << "Error executing query:" << query.lastError();
		return;
	}
	// Decrease the position of sets after the deleted one
	query.prepare(QString("UPDATE sets SET position = position - 1 where parent %1 and position > %2").arg(!parent ? "is null" : QString("= %1").arg(parent->setId())).arg(item->position()));
	if (!query.exec()) {
		qDebug() << "Error executing query:" << query.lastError();
		return;
	}
	// Don't forget to update our model
	QList<SetTreeItem *> siblings(childsOf(parent));
	foreach (SetTreeItem *sibling, siblings) if (sibling->position() > item->position()) sibling->setPosition(sibling->position() - 1);
	delete item;
}

void SetsTreeWidget::deleteSelection()
{
	if (QMessageBox::question(this, tr("Confirm deletion"), tr("Are you sure you want to delete the selected sets/folders?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) return;

	QList<QTreeWidgetItem *> selected = selectedItems();
	foreach (QTreeWidgetItem *rItem, selected) {
		SetTreeItem *item(static_cast<SetTreeItem *>(rItem));
		deleteSet(item);
	}
}

QList<SetTreeItem *> SetsTreeWidget::childsOf(SetTreeItem *parent)
{
		QList<SetTreeItem *> siblings;
		if (!parent) for (int i = 0; i < topLevelItemCount(); i++) siblings << static_cast<SetTreeItem *>(topLevelItem(i));
		else for (int i = 0; i < parent->childCount(); i++) siblings << static_cast<SetTreeItem *>(parent->child(i));
		return siblings;
}

void SetsTreeWidget::populateRoot()
{
	QSqlQuery query;

	query.exec("SELECT rowid, position, state IS NULL, label FROM sets WHERE parent IS NULL ORDER BY position");
	while (query.next()) {
		SetTreeItem *item = new SetTreeItem(query.value(0).toInt(), query.value(1).toInt(), query.value(2).toBool(), query.value(3).toString());
		if (item->isFolder()) {
			item->setIcon(0, QIcon(":/images/icons/folder.png"));
			populateFolder(item);
		}
		addTopLevelItem(item);
	}
}

void SetsTreeWidget::populateFolder(SetTreeItem *parent) const
{
	QSqlQuery query;

	query.prepare("SELECT rowid, position, state IS NULL, label FROM sets WHERE parent = ? ORDER BY position");
	query.addBindValue(parent->setId());
	query.exec();
	while (query.next()) {
		SetTreeItem *item = new SetTreeItem(query.value(0).toInt(), query.value(1).toInt(), query.value(2).toBool(), query.value(3).toString());
		if (item->isFolder()) {
			item->setIcon(0, QIcon(":/images/icons/folder.png"));
			populateFolder(item);
		}
		parent->addChild(item);
	}
}

SetsOrganizer::SetsOrganizer(QWidget *parent) : QDialog(parent)
{
	setupUi(this);
	treeWidget->populateRoot();
}
