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

#include "gui/SearchWidget.h"
#include "core/EntrySearcherManager.h"
#include "gui/EntryTypeFilterWidget.h"
#include "gui/JLPTFilterWidget.h"
#include "gui/NotesFilterWidget.h"
#include "gui/StudyFilterWidget.h"
#include "gui/TagsFilterWidget.h"
#include "gui/TextFilterWidget.h"

#include <QMessageBox>
#include <QResizeEvent>
#include <QScrollBar>

PreferenceItem<int> SearchWidget::historySize("mainWindow/resultsView", "historySize", 100);

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent), _history(historySize.value()) {
    setupUi(this);

    // Setup our event listener
    // filtersScrollArea->viewport()->installEventFilter(this);
    _searchFilters->installEventFilter(this);

    // Add the prev/next search actions to the results view bar
    QToolButton *prevSearchButton = new QToolButton(this);
    prevSearchButton->setDefaultAction(actionPreviousSearch);
    _resultsView->buttonsLayout()->insertWidget(0, prevSearchButton);
    QToolButton *nextSearchButton = new QToolButton(this);
    nextSearchButton->setDefaultAction(actionNextSearch);
    _resultsView->buttonsLayout()->insertWidget(1, nextSearchButton);

    // Setup the results model and view
    _results = new ResultsList(this);
    _resultsView->setModel(_results);

    // Search builder
    connect(&_searchBuilder, SIGNAL(queryRequested(QString)), this, SLOT(search(QString)));

    _filtersToolBar = resultsView()->helper()->defaultToolBar(this);
    _filtersToolBar->addAction(actionResetSearch);

    // Search filters
    EntryTypeFilterWidget *typeFilter = new EntryTypeFilterWidget(this);
    _searchFilterWidgets[typeFilter->name()] = typeFilter;
    _filtersToolBar->addWidget(typeFilter);
    connect(typeFilter, SIGNAL(updateTitle(const QString &)), _searchFilters,
            SLOT(onTitleChanged(const QString &)));
    _searchBuilder.addSearchFilter(typeFilter);
    TextFilterWidget *textFilter = new TextFilterWidget(this);
    addSearchFilter(textFilter);
    addSearchFilter(new StudyFilterWidget(this));
    addSearchFilter(new JLPTFilterWidget(this));
    addSearchFilter(new TagsFilterWidget(this));
    addSearchFilter(new NotesFilterWidget(this));

    // Show the text search by default
    _searchFilters->showWidget(textFilter);

    // Insert the tool bar
    static_cast<QBoxLayout *>(layout())->insertWidget(0, _filtersToolBar);
}

void SearchWidget::search(const QString &commands) {
    QString localCommands(commands.trimmed());
    if (!(localCommands.isEmpty() || localCommands == ":jmdict" || localCommands == ":kanjidic")) {
        _history.add(_searchBuilder.getState());
        _search(localCommands);
    } else {
        // Empty state
        _history.add(_searchBuilder.getState(), true);
        _results->abortSearch();
        _results->clear();
        actionPreviousSearch->setEnabled(_history.hasPrevious());
        actionNextSearch->setEnabled(_history.hasNext());
    }
}

void SearchWidget::_search(const QString &commands) {
    _queryBuilder.clear();

    actionPreviousSearch->setEnabled(_history.hasPrevious());
    actionNextSearch->setEnabled(_history.hasNext());

    // If we cannot build a valid query, no need to continue
    if (!EntrySearcherManager::instance().buildQuery(commands, _queryBuilder))
        return;

    _results->search(_queryBuilder);
}

void SearchWidget::goPrev() {
    QMap<QString, QVariant> q;
    bool ok = _history.previous(q);
    if (ok) {
        _searchBuilder.restoreState(q);
        _search(_searchBuilder.commands());
    }
}

void SearchWidget::goNext() {
    QMap<QString, QVariant> q;
    bool ok = _history.next(q);
    if (ok) {
        _searchBuilder.restoreState(q);
        _search(_searchBuilder.commands());
    }
}

void SearchWidget::resetSearch() { _searchBuilder.reset(); }

void SearchWidget::addSearchFilter(SearchFilterWidget *sWidget) {
    if (_searchBuilder.contains(sWidget->name()))
        return;
    _searchFilterWidgets[sWidget->name()] = sWidget;
    _filtersToolBar->addAction(new ToolBarWidget(
        _searchFilters->addWidget(sWidget->currentTitle(), sWidget), _filtersToolBar));
    connect(sWidget, SIGNAL(updateTitle(const QString &)), _searchFilters,
            SLOT(onTitleChanged(const QString &)));
    _searchBuilder.addSearchFilter(sWidget);
}

SearchFilterWidget *SearchWidget::getSearchFilter(const QString &name) {
    if (!_searchFilterWidgets.contains(name))
        return 0;
    return _searchFilterWidgets[name];
}

void SearchWidget::removeSearchFilterWidget(const QString &name) {
    if (!_searchFilterWidgets.contains(name))
        return;

    _searchFilterWidgets.remove(name);
    _searchBuilder.removeSearchFilter(name);
}

bool SearchWidget::eventFilter(QObject *obj, QEvent *event) {
    if (obj == _searchFilters) {
        if (event->type() == QEvent::Resize) {
            QResizeEvent *revt(static_cast<QResizeEvent *>(event));
            // The new height of the scroll area will depend on whether we need
            // to display the horizontal scrollbar or not
            int newHeight = revt->size().height();
            if (revt->size().width() > filtersScrollArea->viewport()->width())
                newHeight += filtersScrollArea->horizontalScrollBar()->height() + 2;
            filtersScrollArea->setMinimumHeight(newHeight);
            filtersScrollArea->setMaximumHeight(newHeight);
        }
    }
    return QWidget::eventFilter(obj, event);
}
