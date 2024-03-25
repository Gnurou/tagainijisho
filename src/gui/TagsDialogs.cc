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

#include "gui/TagsDialogs.h"
#include "core/Database.h"
#include "core/Entry.h"
#include "core/Tag.h"
#include "gui/BatchHandler.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QProgressDialog>
#include <QVBoxLayout>
#include <QtAlgorithms>

QQueue<QStringList> TagsDialogs::lastAddedTags;

TagsLineInput::TagsLineInput(QWidget *parent) : TJLineEdit(parent) {
    tagsCompleter = new QCompleter(this);
    tagsCompleter->setModel(Tag::knownTagsModel());
    tagsCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    //	tagsCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    tagsCompleter->setWidget(this);
    connect(this, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(checkCompletion()));
    connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(onTextChanged(const QString &)));
    connect(tagsCompleter, SIGNAL(activated(const QString &)), this,
            SLOT(completeText(const QString &)));
}

void TagsLineInput::onTextChanged(const QString &newText) {
    QStringList tags = newText.split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts);
    foreach (const QString &tag, tags) {
        if (tag.endsWith('*')) {
            if (!Tag::knownTagsModel()->containsMatch(tag))
                return;
        } else if (!Tag::knownTagsModel()->contains(tag))
            return;
    }
    emit allValidTags();
}

void TagsLineInput::checkCompletion() {
    QString curText = text();

    // Do not display anything if there is no text or if we are in the middle of
    // a word
    if (curText.isEmpty() || (curText.size() > cursorPosition() &&
                              !(curText[cursorPosition()].category() &
                                (QChar::Separator_Space | QChar::Mark_NonSpacing)))) {
        tagsCompleter->popup()->hide();
        return;
    }

    // Shred the part at the right of the cursor - it's not needed anymore
    curText = curText.left(cursorPosition());

    // We have a potential completion to perform, let's isolate the interesting
    // portion
    int pos = curText.lastIndexOf(QRegExp("[ ,.!?]")) + 1;

    curText = curText.right(curText.size() - pos);

    // Same thing as before...
    if (curText.isEmpty()) {
        tagsCompleter->popup()->hide();
        return;
    }

    tagsCompleter->setCompletionPrefix(curText);
    tagsCompleter->complete();
}

void TagsLineInput::completeText(const QString &completion) {
    QString curText = text();

    curText.insert(cursorPosition(),
                   completion.right(completion.size() - tagsCompleter->completionPrefix().size()) +
                       " ");
    setText(curText);
}

TagsInputDialog::TagsInputDialog(const QString &title, const QString &message,
                                 const QStringList &initialTags, bool selectAll, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle(title);
    QLabel *label = new QLabel(message);

    lineEdit = new TagsLineInput(this);
    lineEdit->setText(initialTags.join(" "));
    if (selectAll)
        lineEdit->selectAll();

    QDialogButtonBox *buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addStretch();
    layout->addWidget(lineEdit);
    layout->addStretch();
    layout->addWidget(buttons);
}

bool TagsDialogs::splitTagsString(const QString &string, QStringList &tagsList,
                                  QStringList &invalidTags) {
    tagsList = string.split(QRegExp("[ ,\\.]"), QString::SkipEmptyParts);
    for (int j = 0; j < tagsList.size(); j++) {
        tagsList[j] = tagsList[j].trimmed();
        if (tagsList[j].isEmpty())
            continue;
        if (!Tag::isValidTag(tagsList[j])) {
            invalidTags << tagsList[j];
        }
    }
    tagsList.removeAll("");

    return (invalidTags.isEmpty());
}

struct SetTagsHandler : BatchHandler {
    const QStringList &_tags;
    SetTagsHandler(const QStringList &tags) : _tags(tags) {}

    void apply(const EntryPointer &e) const { e->setTags(_tags); }
};

bool TagsDialogs::setTagsDialog(const QList<EntryPointer> &entries, QWidget *parent) {
    QStringList split;
    if (entries.isEmpty())
        return false;

    foreach (const Tag &tag, entries[0]->tags())
        split << tag.name();
    bool identical = true;
    for (int i = 1; i < entries.size(); i++) {
        EntryPointer entry = entries[i];
        if (!identical)
            break;
        const QSet<Tag> &tags = entry->tags();
        if (tags.size() != split.size()) {
            identical = false;
            continue;
        }
        foreach (const Tag &tag, tags)
            if (!split.contains(tag.name())) {
                identical = false;
                continue;
            }
    }
    if (!identical)
        split.clear();

    QStringList invalidTags;
    do {
        invalidTags.clear();
        QString tags;
        QString winTitle;
        if (entries.size() == 1)
            winTitle = tr("Set tags for %1").arg(entries[0]->name());
        else
            winTitle = tr("Set tags");
        TagsInputDialog dialog(winTitle,
                               tr("Enter a space-separated list of tags for ") +
                                   (entries.size() > 1 ? tr("these entries:") : tr("this entry:")),
                               split, true, parent);
        if (dialog.exec() == QDialog::Accepted)
            tags = dialog.getText();
        else
            return false;

        if (!splitTagsString(tags, split, invalidTags))
            QMessageBox::warning(0, tr("Invalid tags"),
                                 tr("<p>These tags have invalid characters: ") +
                                     invalidTags.join(", ") +
                                     tr("</p><p>Accepted characters are letters, "
                                        "digits and non-quoting punctuation.</p>"));

    } while (!invalidTags.isEmpty());

    BatchHandler::applyOnEntries(SetTagsHandler(split), entries, parent);

    return true;
}

struct AddTagsHandler : BatchHandler {
    const QStringList &_tags;
    AddTagsHandler(const QStringList &tags) : _tags(tags) {}

    void apply(const EntryPointer &e) const { e->addTags(_tags); }
};

bool TagsDialogs::addTagsDialog(const QList<EntryPointer> &entries, QWidget *parent) {
    QStringList invalidTags;
    QStringList split;
    do {
        invalidTags.clear();
        QString tags;
        QString winTitle;
        if (entries.size() == 1)
            winTitle = tr("Add tags to %1").arg(entries[0]->name());
        else
            winTitle = tr("Add tags");
        TagsInputDialog dialog(winTitle,
                               tr("Enter a space-separated list of tags to add to ") +
                                   (entries.size() > 1 ? tr("these entries:") : tr("this entry:")),
                               QStringList(), false, parent);
        if (dialog.exec() == QDialog::Accepted)
            tags = dialog.getText();
        else
            return false;

        if (!splitTagsString(tags, split, invalidTags))
            QMessageBox::warning(0, tr("Invalid tags"),
                                 tr("<p>These tags have invalid characters: ") +
                                     invalidTags.join(", ") +
                                     tr("</p><p>Accepted characters are letters, "
                                        "digits and non-quoting punctuation.</p>"));

    } while (!invalidTags.isEmpty());

    BatchHandler::applyOnEntries(AddTagsHandler(split), entries, parent);

    lastAddedTags.enqueue(split);
    while (lastAddedTags.size() > 5)
        lastAddedTags.dequeue();

    return true;
}
