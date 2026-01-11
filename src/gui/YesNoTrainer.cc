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

#include "gui/YesNoTrainer.h"
#include "core/Database.h"
#include "core/EntriesCache.h"
#include "gui/EntryFormatter.h"
#include "gui/TemplateFiller.h"

#include <QDate>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QtDebug>

PreferenceItem<QByteArray> YesNoTrainer::windowGeometry("trainWindow", "geometry", "");

YesNoTrainer::YesNoTrainer(QWidget *parent)
    : QWidget(parent), _trainingMode(Japanese), currentEntry(0), _query(Database::connection()) {
    frontParts << "front";
    backParts << "back";

    restoreGeometry(windowGeometry.value());

    setWindowTitle("Training");

    QHBoxLayout *hLayout = new QHBoxLayout();
    showAnswerButton = new QPushButton(tr("Answer (&1)"), this);
    // Replace shortcut to remove all controller keys
    showAnswerButton->setShortcut(QKeySequence(showAnswerButton->shortcut()[0].key()));
    connect(showAnswerButton, SIGNAL(clicked()), this, SLOT(showAnswer()));
    hLayout->addWidget(showAnswerButton);
    goodAnswerButton = new QPushButton(QIcon(":/images/icons/good.png"), tr("Correct! (&2)"), this);
    goodAnswerButton->setShortcut(QKeySequence(goodAnswerButton->shortcut()[0].key()));
    connect(goodAnswerButton, SIGNAL(clicked()), this, SLOT(goodAnswer()));
    hLayout->addWidget(goodAnswerButton);
    wrongAnswerButton =
        new QPushButton(QIcon(":/images/icons/wrong.png"), tr("Wrong... (&3)"), this);
    wrongAnswerButton->setShortcut(QKeySequence(wrongAnswerButton->shortcut()[0].key()));
    connect(wrongAnswerButton, SIGNAL(clicked()), this, SLOT(wrongAnswer()));
    hLayout->addWidget(wrongAnswerButton);
    skipButton = new QPushButton(tr("Skip (&4)"), this);
    skipButton->setShortcut(QKeySequence(skipButton->shortcut()[0].key()));
    connect(skipButton, SIGNAL(clicked()), this, SLOT(skip()));
    hLayout->addWidget(skipButton);

    showAnswerButton->setEnabled(false);
    goodAnswerButton->setEnabled(false);
    wrongAnswerButton->setEnabled(false);
    skipButton->setEnabled(false);

    _detailedView = new ToolBarDetailedView(this);
    _detailedView->detailedView()->setHistoryEnabled(false);

    _counterLabel = new QLabel(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(hLayout);
    layout->addWidget(_detailedView);
    layout->addWidget(_counterLabel);
}

YesNoTrainer::~YesNoTrainer() { windowGeometry.set(saveGeometry()); }

void YesNoTrainer::setQuery(QString queryString) {
    // Run the query
    _queryString = queryString;
    if (!_query.exec(queryString))
        qDebug() << "Error executing query:" << _query.lastError().message();
}

void YesNoTrainer::clear() {
    currentEntry = EntryPointer();
    _detailedView->detailedView()->clear();
    showAnswerButton->setEnabled(false);
    goodAnswerButton->setEnabled(false);
    wrongAnswerButton->setEnabled(false);
    skipButton->setEnabled(false);
}

void YesNoTrainer::_train() {
    if (!_query.next())
        hasResults(0);
    else {
        EntryPointer entry(EntryRef(_query.valueInt(0), _query.valueInt(1)).get());
        train(entry);
    }
}

void YesNoTrainer::train() {
    _goodCount = _wrongCount = _totalCount = 0;
    _train();
}

void YesNoTrainer::train(const EntryPointer &entry) {
    clear();
    _counterLabel->setText(
        tr("Correct: %1, Wrong: %2, Total: %3").arg(_goodCount).arg(_wrongCount).arg(_totalCount));
    _detailedView->detailedView()->setKanjiClickable(false);

    currentEntry = entry;

    showAnswerButton->setEnabled(true);
    goodAnswerButton->setEnabled(false);
    wrongAnswerButton->setEnabled(false);
    skipButton->setEnabled(true);

    QTextDocument *document(_detailedView->detailedView()->document());

    const EntryFormatter *formatter(EntryFormatter::getFormatter(entry));
    if (!formatter) {
        qWarning("%s %d: Warning: cannot find formatter for entry!", __FILE__, __LINE__);
        return;
    }

    QString css(formatter->CSS());
    // Add the font style CSS
    css += QString("\n%1 {\n%2}\n")
               .arg(".furigana")
               .arg(DetailedViewFonts::CSS(DetailedViewFonts::KanaHeader));
    css += QString("\n%1 {\n%2}\n")
               .arg(".mainwriting")
               .arg(DetailedViewFonts::CSS(DetailedViewFonts::KanjiHeader));
    css += QString("\n%1 {\n%2}\n")
               .arg(".kanji")
               .arg(DetailedViewFonts::CSS(DetailedViewFonts::Kanji));
    css +=
        QString("\n%1 {\n%2}\n").arg(".kana").arg(DetailedViewFonts::CSS(DetailedViewFonts::Kana));
    document->setDefaultStyleSheet(css);

    const QStringList &parts = trainingMode() == Japanese ? frontParts : backParts;

    TemplateFiller filler;
    QString html(filler.fill(filler.extract(formatter->htmlTemplate(), parts), formatter, entry));
    document->setHtml(html);
}

void YesNoTrainer::showAnswer() {
    _detailedView->detailedView()->setKanjiClickable(true);

    showAnswerButton->setEnabled(false);
    goodAnswerButton->setEnabled(true);
    wrongAnswerButton->setEnabled(true);
    _detailedView->detailedView()->display(currentEntry);
}

void YesNoTrainer::hasResults(unsigned int nbResults) {
    if (nbResults == 0) {
        if (_totalCount == 0)
            QMessageBox::information(this, tr("No entries to train"),
                                     tr("There are no entries to train as of now. Please add "
                                        "entries to "
                                        "your study list or check the train settings."));
        else
            QMessageBox::information(this, tr("No more entries to train"),
                                     tr("All the matching entries have been trained."));
        close();
    }
}

void YesNoTrainer::goodAnswer() {
    // Needed to avoid redrawing everything before clearing because of the
    // updated() signal of the entry and to avoid a database error in case the
    // detailed view is still fetching data (as we are going to acquire a write
    // lock)
    _detailedView->detailedView()->clear();
    currentEntry->train(true);
    _goodCount++;
    _totalCount++;
    getNextEntry();
}

void YesNoTrainer::wrongAnswer() {
    // Needed to avoid redrawing everything before clearing because of the
    // updated() signal of the entry
    _detailedView->detailedView()->setEntry(EntryPointer());
    currentEntry->train(false);
    _wrongCount++;
    _totalCount++;
    getNextEntry();
}

void YesNoTrainer::skip() {
    _totalCount++;
    getNextEntry();
}

void YesNoTrainer::getNextEntry() { _train(); }
