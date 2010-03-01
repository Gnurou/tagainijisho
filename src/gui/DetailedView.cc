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

#include "core/TextTools.h"
#include "core/Database.h"
#include "gui/EntryFormatter.h"
#include "gui/DetailedView.h"
// TODO Would be nice to get rid of this one...
#include "gui/MainWindow.h"
// TODO and this one too
#include "gui/TagsFilterWidget.h"

#include <QtDebug>

#include <QToolTip>
#include <QCursor>
#include <QAbstractTextDocumentLayout>
#include <QScrollBar>
#include <QPixmap>
#include <QMenu>
#include <QToolTip>
#include <QApplication>
#include <QPalette>
#include <QDir>
#include <QMap>

DetailedViewFonts *DetailedViewFonts::_instance = 0;
PreferenceItem<QString> DetailedViewFonts::textFont("mainWindow/detailedView", "textFont", "");
PreferenceItem<QString> DetailedViewFonts::kanjiFont("mainWindow/detailedView", "kanjiFont", "");
PreferenceItem<QString> DetailedViewFonts::kanaFont("mainWindow/detailedView", "kanaFont", "");
PreferenceItem<QString> DetailedViewFonts::kanjiHeaderFont("mainWindow/detailedView", "kanjiHeaderFont", QFont("Helvetica", 28).toString());
PreferenceItem<QString> DetailedViewFonts::kanaHeaderFont("mainWindow/detailedView", "kanaHeaderFont", "");

PreferenceItem<bool> DetailedView::smoothScrolling("mainWindow/detailedView", "smoothScrolling", true);
PreferenceItem<int> DetailedView::historySize("mainWindow/detailedView", "historySize", 1000);
EntryMenuHandler DetailedView::_entryHandler;
TagsLinkHandler DetailedView::_tagsLinkHandler;
QSet<QObject *> DetailedView::_eventFilters;
QSet<DetailedView *> DetailedView::_instances;

void DetailedView::registerEventFilter(QObject *obj)
{
	_eventFilters << obj;
	foreach (DetailedView *view, _instances) view->viewport()->installEventFilter(obj);
}

void DetailedView::removeEventFilter(QObject *obj)
{
	foreach (DetailedView *view, _instances) view->viewport()->removeEventFilter(obj);
	_eventFilters.remove(obj);
}

DetailedView::DetailedView(QWidget *parent) : QTextBrowser(parent), _kanjisClickable(true), _historyEnabled(true), _history(historySize.value()), entryView(0), _jobsRunner(this)
{
	// Add the default handlers if not already done (first instanciation)
	if (!DetailedViewLinkManager::getHandler(_entryHandler.scheme())) DetailedViewLinkManager::registerHandler(&_entryHandler);
	if (!DetailedViewLinkManager::getHandler(_tagsLinkHandler.scheme())) DetailedViewLinkManager::registerHandler(&_tagsLinkHandler);
	// Is the fonts manager instance already running?
	if (!DetailedViewFonts::_instance) DetailedViewFonts::_instance = new DetailedViewFonts();
	connect(DetailedViewFonts::_instance, SIGNAL(fontsHaveChanged()), this, SLOT(redisplay()));

	// Add the registered event filters
	foreach (QObject *obj, _eventFilters) viewport()->installEventFilter(obj);

	// Add ourselves to the list of instances
	_instances << this;

	// Let the navigation handler take care of our events related to navigation
	connect(this, SIGNAL(anchorClicked(const QUrl &)),
			DetailedViewLinkManager::instance(), SLOT(handleUrl(const QUrl &)));

	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setOpenLinks(false);
	setMouseTracking(true);
	connect(&entryView, SIGNAL(entryChanged(Entry*)), this, SLOT(redisplay()));

	_historyPrevAction = new QAction(QIcon(":/images/icons/go-previous.png"), tr("Previous entry"), this);
	_historyPrevAction->setShortcuts(QKeySequence::Back);
	connect(_historyPrevAction, SIGNAL(triggered()), this, SLOT(previous()));
	_historyPrevAction->setEnabled(false);
	addAction(_historyPrevAction);

	_historyNextAction = new QAction(QIcon(":/images/icons/go-next.png"), tr("Next entry"), this);
	_historyNextAction->setShortcuts(QKeySequence::Forward);
	connect(_historyNextAction, SIGNAL(triggered()), this, SLOT(next()));
	_historyNextAction->setEnabled(false);
	addAction(_historyNextAction);

	clear();
	setSmoothScrolling(smoothScrolling.value());
}

DetailedView::~DetailedView()
{
	// Remove ourselves from the list of instances
	_instances.remove(this);
}

void DetailedView::_display(const EntryPointer &entry, bool update)
{
//	if (!update && entry == entryView.entry()) return;
	clear();
	entryView.setEntry(entry);
	if (_historyEnabled) {
		_historyPrevAction->setEnabled(_history.hasPrevious());
		_historyNextAction->setEnabled(_history.hasNext());
	}
	QTextCursor cursor(document());
	const EntryFormatter *formatter(EntryFormatter::getFormatter(entry));
	if (formatter) formatter->detailedVersion(entry, cursor, this);
	else qWarning("%s %d: %s", __FILE__, __LINE__, "No formatter found for entry!");
	_jobsRunner.runAllJobs();
}

void DetailedView::setSmoothScrolling(bool value)
{
	if (value) {
		_charm.activateOn(this);
	}
	else {
		_charm.deactivate();
	}
}

void DetailedView::display(const EntryPointer &entry)
{
	if (_historyEnabled) {
		_history.add(QPair<int, int>(entry->type(), entry->id()));
	}
	_display(entry);
}

void DetailedView::redisplay()
{
	if (entryView.entry()) {
		// We need this pointer because display starts by clearing
		// the entryView - which means the current entry reference
		// counter could potentially reach zero and the entry be
		// deleted.
		EntryPointer tentry(entryView.entry());
		_display(tentry, true);
	}
}

void DetailedView::clear()
{
	foreach (const EntryPointer &entry, _watchedEntries) {
		disconnect(entry.data(), SIGNAL(entryChanged(Entry *)), this, SLOT(redisplay()));
	}
	_watchedEntries.clear();

	_jobsRunner.abortAllJobs();

	QTextBrowser::clear();
	entryView.setEntry(EntryPointer());

	QDir dir(":/images/flags");
	QStringList fileNames = dir.entryList(QStringList("*.png"), QDir::Files, QDir::Name);
	QMutableStringListIterator i(fileNames);
	while (i.hasNext()) {
		i.next();
		i.setValue(dir.filePath(i.value()));
	}
	foreach (const QString &imageFile, fileNames) {
		QStringList split = imageFile.split("/");
		QString url = "flag:" + split[split.size() - 1].left(2);
		document()->addResource(QTextDocument::ImageResource, QUrl(url), QPixmap(imageFile));
	}
	document()->addResource(QTextDocument::ImageResource, QUrl("tagicon"), QPixmap(":/images/icons/tags.png"));
	document()->addResource(QTextDocument::ImageResource, QUrl("moreicon"), QPixmap(":/images/icons/zoom-in.png"));
}

void DetailedView::setKanjisClickable(bool clickable)
{
	_kanjisClickable = clickable;
//	if (!clickable) viewport()->unsetCursor();
}

void DetailedView::setHistoryEnabled(bool enabled)
{
	_historyEnabled = enabled;
	_historyPrevAction->setEnabled(enabled ? _history.hasPrevious() : false);
	_historyNextAction->setEnabled(enabled ? _history.hasNext() : false);
}

void DetailedView::previous()
{
	QPair<int, int> prev;
	bool ok = _history.previous(prev);
	if (!ok) return;
	EntryPointer entry(EntriesCache::get(prev.first, prev.second));
	_display(entry);
}

void DetailedView::next()
{
	QPair<int, int> next;
	bool ok = _history.next(next);
	if (!ok) return;
	EntryPointer entry(EntriesCache::get(next.first, next.second));
	_display(entry);
}

void DetailedView::addBackgroundJob(DetailedViewJob *job)
{
	_jobsRunner.addJob(job);
}

void DetailedView::addWatchEntry(const EntryPointer &entry)
{
	_watchedEntries << entry;
	connect(entry.data(), SIGNAL(entryChanged(Entry *)), this, SLOT(redisplay()));
}

void DetailedView::populateToolBar(QToolBar *toolBar)
{
	toolBar->addAction(_historyPrevAction);
	toolBar->addAction(_historyNextAction);
	toolBar->addSeparator();
	entryView.populateToolBar(toolBar);
}

DetailedViewJobRunner::DetailedViewJobRunner(DetailedView * view, QObject *parent) : QObject(parent), _view(view), _currentJob(0), _ignoreJobs(false)
{
	_dbThread = new DatabaseThread();

	_aQuery = new ASyncEntryLoader(_dbThread);

	connect(_aQuery, SIGNAL(firstResult()), this, SLOT(onFirstResult()));
	connect(_aQuery, SIGNAL(result(EntryPointer)),
	        this, SLOT(onResult(EntryPointer)));
	connect(_aQuery, SIGNAL(completed()), this, SLOT(onCompleted()));
	connect(_aQuery, SIGNAL(aborted()), this, SLOT(onAborted()));
	connect(_aQuery, SIGNAL(error(const QString &)),
	        this, SLOT(onError(const QString &)));
}

DetailedViewJobRunner::~DetailedViewJobRunner()
{
	abortAllJobs();
	delete _aQuery;
	delete _dbThread;
}

void DetailedViewJobRunner::addJob(DetailedViewJob *job)
{
	_jobs << job;
}

void DetailedViewJobRunner::runNextJob()
{
	// Delete the current job, if any
	if (_currentJob) delete _currentJob;
	// No more job to run?
	if (_jobs.isEmpty()) {
		_currentJob = 0;
		return;
	}
	_currentJob = _jobs.dequeue();
	// Since we are only using the Query from here, it should always be available!
	if (_currentJob->sql().isEmpty()) runNextJob();
	else {
		if (!_aQuery->exec(_currentJob->sql()))
			qWarning("%s %d: %s", __FILE__, __LINE__, "Unable to start background job");
	}
}

void DetailedViewJobRunner::runAllJobs()
{
	// Are jobs already running?
	if (_currentJob) return;
	runNextJob();
}

void DetailedViewJobRunner::abortCurrentJob()
{
	// Do we have a job running?
	if (_currentJob) {
		// Then stop its execution
		_aQuery->abort();
		// Flush events that may have been posted by the DB thread
		_ignoreJobs = true;
		QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
		_ignoreJobs = false;

		// This would be a more elegant solution than processEvents,
		// but unfortunately is not acceptable.
		// Shared pointers are held in the message queue - if only pointer to
		// and entry is in the queue, the entry is deleted and the QObject
		// destructor attempts to lock the events queue which results in a
		// dead lock. Setting the entries cache size to zero clearly exposes
		// this problem.
//		QCoreApplication::removePostedEvents(this);
		// Delete the job
		delete _currentJob;
		_currentJob = 0;
	}
}

void DetailedViewJobRunner::abortAllJobs()
{
	// Clean the jobs queue
	foreach (DetailedViewJob *job, _jobs) delete job;
	_jobs.clear();
	// And abort the running job
	abortCurrentJob();
}

void DetailedViewJobRunner::onFirstResult()
{
	// If we are just flushing the job results queue, don't lose time here
	if (_ignoreJobs) return;

	Q_ASSERT(_currentJob != 0);
	// Fix the position of the cursor - we moved it one step to the left to ensure it does not follow
	// the flow of text
	QTextCursor &tCursor = _currentJob->cursor();
	tCursor.movePosition(QTextCursor::Right);
	_currentJob->firstResult();
	tCursor.movePosition(QTextCursor::Left);
}

void DetailedViewJobRunner::onResult(EntryPointer entry)
{
	// If we are just flushing the job results queue, don't lose time here
	if (_ignoreJobs) return;

	Q_ASSERT(_currentJob != 0);
	// Fix the position of the cursor - we moved it one step to the left to ensure it does not follow
	// the flow of text
	QTextCursor &tCursor = _currentJob->cursor();
	tCursor.movePosition(QTextCursor::Right);
	_currentJob->result(entry);
	_view->addWatchEntry(entry);
	tCursor.movePosition(QTextCursor::Left);
}

void DetailedViewJobRunner::onCompleted()
{
	// If we are just flushing the job results queue, don't lose time here
	if (_ignoreJobs) return;

	Q_ASSERT(_currentJob != 0);
	// Fix the position of the cursor - we moved it one step to the left to ensure it does not follow
	// the flow of text
	QTextCursor &tCursor = _currentJob->cursor();
	tCursor.movePosition(QTextCursor::Right);
	_currentJob->completed();
	tCursor.movePosition(QTextCursor::Left);

	runNextJob();
}

void DetailedViewJobRunner::onAborted()
{
	Q_ASSERT(_currentJob != 0);
	runNextJob();
}

void DetailedViewJobRunner::onError(const QString &error)
{
	qDebug() << "An error occured while processing job" << _currentJob->sql() << ":" << error;
	abortCurrentJob();
	runNextJob();
}

DetailedViewJob::DetailedViewJob(const QString &sql, const QTextCursor &cursor) : _sql(sql), _cursor(cursor)
{
	__init();
}

DetailedViewJob::DetailedViewJob(const QTextCursor &cursor) : _cursor(cursor)
{
	__init();
}

void DetailedViewJob::__init()
{
	// We *must* move the cursor one step to the left, otherwise it will move as
	// the user continues to insert text. The position will be fixed before
	// the cursor is passed to the callback function.
	_cursor.insertText(" ");
	_cursor.movePosition(QTextCursor::Left);
}

DetailedViewFonts::DetailedViewFonts(QWidget *parent) : QObject(parent)
{
	// Init fonts and colors with the default values
	_font[DefaultText].fromString(textFont.value());
	_font[Kanji].fromString(kanjiFont.value());
	_font[Kana].fromString(kanaFont.value());
	_font[KanjiHeader].fromString(kanjiHeaderFont.value());
	_font[KanaHeader].fromString(kanaHeaderFont.value());

	_color[DefaultText] = defaultColor(DefaultText);
	_color[Kanji] = defaultColor(Kanji);
	_color[Kana] = defaultColor(Kana);
	_color[KanjiHeader] = defaultColor(KanjiHeader);
	_color[KanaHeader] = defaultColor(KanaHeader);
}

void DetailedViewFonts::_setFont(FontRole role, const QFont &font)
{
	_font[role] = font;
}

void DetailedViewFonts::_fontsChanged()
{
	emit fontsHaveChanged();
}

QColor DetailedViewFonts::_defaultColor(FontRole role) const
{
	switch (role) {
	case KanaHeader:
		return Qt::red;
	default:
		return QApplication::palette().color(QPalette::Text);
	}
}

QTextCharFormat DetailedViewFonts::_charFormat(FontRole role) const
{
	QTextCharFormat res;
	res.setFont(font(role));
	res.setForeground(color(role));
	return res;
}

EntryMenuHandler::EntryMenuHandler() : DetailedViewLinkHandler("entry")
{
}

EntryMenuHandler::~EntryMenuHandler()
{
}

void EntryMenuHandler::handleUrl(const QUrl &url, DetailedView *view)
{
	EntryPointer entry(EntriesCache::get(url.queryItemValue("type").toInt(), url.queryItemValue("id").toInt()));
	if (entry) MainWindow::instance()->detailedView()->display(entry);
}

TagsLinkHandler::TagsLinkHandler() : DetailedViewLinkHandler("tag")
{
}

TagsLinkHandler::~TagsLinkHandler()
{
}

void TagsLinkHandler::handleUrl(const QUrl &url, DetailedView *view)
{
	MainWindow *mainWindow = MainWindow::instance();
	TagsFilterWidget *extender = qobject_cast<TagsFilterWidget *>(mainWindow->searchWidget()->getSearchFilter("tagssearch"));
	if (!extender) return;

	if (url.hasQueryItem("reset")) mainWindow->searchWidget()->searchBuilder()->reset();
	extender->setAutoUpdateQuery(false);
	extender->setTags(url.authority() + " ");
	extender->setAutoUpdateQuery(true);
	mainWindow->searchWidget()->searchBuilder()->runSearch();
}

QMap<QString, DetailedViewLinkHandler *> DetailedViewLinkManager::_handlers;
DetailedViewLinkManager DetailedViewLinkManager::_instance;

void DetailedViewLinkManager::handleUrl(const QUrl &url)
{
	DetailedView *view(qobject_cast<DetailedView *>(sender()));
	if (!view) return;
	DetailedViewLinkHandler *handler = getHandler(url.scheme());
	if (handler) handler->handleUrl(url, view);
}

bool DetailedViewLinkManager::registerHandler(DetailedViewLinkHandler *handler)
{
	if (_handlers.contains(handler->scheme())) return false;
	_handlers[handler->scheme()] = handler;
	return true;
}

DetailedViewLinkHandler *DetailedViewLinkManager::getHandler(const QString &scheme)
{
	if (!_handlers.contains(scheme)) return 0;
	return _handlers[scheme];
}

bool DetailedViewLinkManager::removeHandler(DetailedViewLinkHandler *handler)
{
	return removeHandler(handler->scheme());
}

bool DetailedViewLinkManager::removeHandler(const QString &scheme)
{
	if (!_handlers.contains(scheme)) return false;
	_handlers.remove(scheme);
	return true;
}
