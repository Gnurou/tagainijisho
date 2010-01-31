/*
 *  Copyright (C) 2009/2010  Alexandre Courbot
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

#include "gui/TagsFilterWidget.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QMenu>
#include <QFocusEvent>

TagsFilterWidget::TagsFilterWidget(QWidget *parent) : SearchFilterWidget(parent)
{
	_propsToSave << "tags";

	QHBoxLayout *hLayout = new QHBoxLayout(this);
	lineInput = new TagsLineInput(this);
	hLayout->addWidget(lineInput);
	connect(lineInput, SIGNAL(allValidTags()), this, SLOT(commandUpdate()));

	tagsButton = new QPushButton(tr("..."), this);
	QMenu *menu = new QMenu(tagsButton);
	connect(menu, SIGNAL(aboutToShow()), this, SLOT(populateTagsMenu()));
	tagsButton->setMenu(menu);
	hLayout->addWidget(tagsButton);
	connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(tagMenuClicked(QAction*)));
}

void TagsFilterWidget::populateTagsMenu()
{
	QMenu *menu = tagsButton->menu();
	menu->clear();
	QStringList tags(lineInput->text().toLower().split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts));
	foreach(QString tag, Tag::knownTagsModel()->contents()) {
		QString tagStr = tag;
		QAction *action = menu->addAction(tag.replace(0, 1, tag[0].toUpper()));
		action->setProperty("TJtag", tagStr);
		action->setCheckable(true);
		if (tags.contains(tag.toLower())) action->setChecked(true);
	}
}

void TagsFilterWidget::focusInEvent(QFocusEvent *event)
{
	lineInput->setFocus(event->reason());
}

void TagsFilterWidget::tagMenuClicked(QAction *action)
{
	QStringList tags(lineInput->text().toLower().split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts));
	QString tag(action->property("TJtag").toString());
	if (!tags.contains(tag)) {
		QString str(lineInput->text());
		if (str.size() > 0 && !str.endsWith(' ')) str += " ";
		lineInput->setText(str + tag + " ");
	}
	else {
		QRegExp regexp(QString("\\b%1\\b").arg(tag));
		QString text(lineInput->text());
		text.remove(regexp);
		text.remove(QRegExp("^ *"));
		text.replace(QRegExp("  +"), " ");
		regexp.setCaseSensitivity(Qt::CaseInsensitive);
		lineInput->setText(text);
	}
}

void TagsFilterWidget::setTags(const QString &tags)
{
	QString rTags(tags);
	if (tags.size() && tags[tags.size() - 1] != ' ') rTags += " ";
	lineInput->setText(rTags);
}

QString TagsFilterWidget::currentTitle() const
{
	QStringList tags(lineInput->text().split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts));
	if (tags.isEmpty()) return tr("Tags");
	else return tr("Tagged %1").arg(tags.join(","));
}

QString TagsFilterWidget::currentCommand() const
{
	QStringList tags(lineInput->text().split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts));
	if (tags.isEmpty()) return "";
	return QString(":tag%1%2").arg(tags.isEmpty() ? "" : "=").arg(tags.join(","));
}

void TagsFilterWidget::_reset()
{
	lineInput->clear();
}
