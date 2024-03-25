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

#include "gui/NotesFilterWidget.h"
#include "gui/TJLineEdit.h"

#include <QFocusEvent>
#include <QGroupBox>
#include <QHBoxLayout>

NotesFilterWidget::NotesFilterWidget(QWidget *parent) : SearchFilterWidget(parent) {
    _propsToSave << "notes";

    words = new TJLineEdit(this);
    connect(words, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(words, SIGNAL(returnPressed()), this, SLOT(commandUpdate()));

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(5);
    hLayout->addWidget(words);

    setFocusProxy(words);
}

void NotesFilterWidget::setNotes(const QString &notes) {
    QString rNotes(notes);
    if (notes.size() && notes[notes.size() - 1] != ' ')
        rNotes += " ";
    words->setText(rNotes);
}

QString NotesFilterWidget::currentTitle() const {
    if (words->text().isEmpty())
        return tr("Notes");
    QStringList args(words->text().split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts));
    if (args.size() == 1 && args[0] == "*")
        return tr("Has note");
    return tr("Note contains %1").arg(words->text());
}

QString NotesFilterWidget::currentCommand() const {
    if (words->text().isEmpty())
        return "";
    QStringList args(words->text().split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts));
    if (args.size() == 1 && args[0] == "*")
        return ":note";
    return QString(":note=%1").arg(args.join(","));
}

void NotesFilterWidget::onTextChanged(const QString &newText) {
    if (newText.isEmpty() || QRegExp("^.*[ ,\\.\\*]$").exactMatch(newText))
        commandUpdate();
}

void NotesFilterWidget::_reset() { words->clear(); }
