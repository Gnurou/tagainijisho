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

#include "tagaini_config.h"
#include "core/TextTools.h"
#include "core/Database.h"
#include "gui/Font.h"
#include "gui/EntryFormatter.h"
#include "gui/TemplateFiller.h"
#include "gui/DetailedView.h"
// TODO Would be nice to get rid of this one...
#include "gui/MainWindow.h"
// TODO and this one too
#include "gui/TagsFilterWidget.h"
// TODO and this one too!
#include "gui/EntryListWidget.h"

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
#include <QDesktopServices>
#include <QDrag>
#include <QUrl>
#include <QUrlQuery>

DetailedViewFonts *DetailedViewFonts::_instance = 0;

PreferenceItem<int> DetailedViewFonts::textFontSize("mainWindow/detailedView", "textFontSize", -1);
PreferenceItem<int> DetailedViewFonts::kanjiFontSize("mainWindow/detailedView", "kanjiFontSize", -1);
PreferenceItem<int> DetailedViewFonts::kanaFontSize("mainWindow/detailedView", "kanaFontSize", -1);
PreferenceItem<int> DetailedViewFonts::kanjiHeaderFontSize("mainWindow/detailedView", "kanjiHeaderFontSize", 28);
PreferenceItem<int> DetailedViewFonts::kanaHeaderFontSize("mainWindow/detailedView", "kanaHeaderFontSize", -1);

PreferenceItem<bool> DetailedView::smoothScrolling("mainWindow/detailedView", "smoothScrolling", true);
PreferenceItem<int> DetailedView::historySize("mainWindow/detailedView", "historySize", 1000);
EntryMenuHandler DetailedView::_entryHandler;
TagsLinkHandler DetailedView::_tagsLinkHandler;
ListLinkHandler DetailedView::_listLinkHandler;
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

DetailedView::DetailedView(QWidget *parent) : QTextBrowser(parent), _kanjiClickable(true), _historyEnabled(true), _dragEntryRef(0, 0), _dragStarted(false), _history(historySize.value()), _entryView(0), _jobsRunner(this)
{
	// Add the default handlers if not already done (first instanciation)
	if (!DetailedViewLinkManager::getHandler(_entryHandler.scheme())) DetailedViewLinkManager::registerHandler(&_entryHandler);
	if (!DetailedViewLinkManager::getHandler(_tagsLinkHandler.scheme())) DetailedViewLinkManager::registerHandler(&_tagsLinkHandler);
	if (!DetailedViewLinkManager::getHandler(_listLinkHandler.scheme())) DetailedViewLinkManager::registerHandler(&_listLinkHandler);
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
	connect(&_entryView, SIGNAL(entryChanged(Entry*)), this, SLOT(redisplay()));

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
	clear();
	_entryView.setEntry(entry);
	if (_historyEnabled) {
		_historyPrevAction->setEnabled(_history.hasPrevious());
		_historyNextAction->setEnabled(_history.hasNext());
	}
	const EntryFormatter *formatter(EntryFormatter::getFormatter(entry));
	TemplateFiller filler;
	
	if (!formatter) qWarning("%s %d: %s", __FILE__, __LINE__, "No formatter found for entry!");
	else {
		// Apply the default font
		setFont(DetailedViewFonts::font(DetailedViewFonts::DefaultText));
		QString css(formatter->CSS());
		// Add the font style CSS
		css += QString("\n%1 {\n%2}\n").arg(".furigana").arg(DetailedViewFonts::CSS(DetailedViewFonts::KanaHeader));
		css += QString("\n%1 {\n%2}\n").arg(".mainwriting").arg(DetailedViewFonts::CSS(DetailedViewFonts::KanjiHeader));
		css += QString("\n%1 {\n%2}\n").arg(".kanji").arg(DetailedViewFonts::CSS(DetailedViewFonts::Kanji));
		css += QString("\n%1 {\n%2}\n").arg(".kana").arg(DetailedViewFonts::CSS(DetailedViewFonts::Kana));
		// Fill the HTML template with the immediate information
		QString html(filler.fill(formatter->htmlTemplate(), formatter, entry));
#ifdef DEBUG_DETAILED_VIEW
		qDebug() << css;
		qDebug() << html;
#endif
		document()->setDefaultStyleSheet(css);
		document()->setHtml(html);
		// Now find the jobs that need to be run from the document
		QRegExp funcMatch("\\$\\!\\$(\\w+)");
		QTextCursor pos(document()), matchPos;
		while (!(matchPos = document()->find(funcMatch, pos)).isNull()) {
			funcMatch.exactMatch(matchPos.selectedText());
			QString jobClass(funcMatch.cap(1));
			// Remove the matched text and update the current position
			matchPos.removeSelectedText();
			pos = matchPos;
			// Get the list of jobs to run by invoking the jobs method
			QList<DetailedViewJob *> jobs;
			QMetaObject::invokeMethod(const_cast<EntryFormatter *>(formatter), QString("job" + jobClass).toLatin1().constData(), Qt::DirectConnection, Q_RETURN_ARG(QList<DetailedViewJob *>, jobs), Q_ARG(ConstEntryPointer, entry), Q_ARG(QTextCursor, matchPos));
			// Add the jobs added by the keyword to the list of jobs to run
			foreach (DetailedViewJob *job, jobs) {
				addBackgroundJob(job);
			}
		}
		// Start running background jobs
		_jobsRunner.runAllJobs();
		
		emit entryDisplayed(entry);
	}
}

void DetailedView::setSmoothScrolling(bool value)
{
	if (value) {
		_scroller.setScrollBar(this->verticalScrollBar());
	}
	else {
		_scroller.setScrollBar(0);
	}
}

void DetailedView::display(const EntryPointer &entry)
{
	if (entry == _entryView.entry()) return;
	if (_historyEnabled) {
		_history.add(EntryRef(entry));
	}
	_display(entry);
}

void DetailedView::redisplay()
{
	if (_entryView.entry()) {
		// We need this pointer because display starts by clearing
		// the entryView - which means the current entry reference
		// counter could potentially reach zero and the entry be
		// deleted.
		EntryPointer tentry(_entryView.entry());
		_display(tentry, true);
	}
}

void DetailedView::clear()
{
	foreach (const ConstEntryPointer &entry, _watchedEntries) {
		disconnect(entry.data(), SIGNAL(entryChanged(Entry *)), this, SLOT(redisplay()));
	}
	_watchedEntries.clear();

	_jobsRunner.abortAllJobs();

	QTextBrowser::clear();
	_entryView.setEntry(EntryPointer());

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
	document()->addResource(QTextDocument::ImageResource, QUrl("listicon"), QPixmap(":/images/icons/list.png"));
	document()->addResource(QTextDocument::ImageResource, QUrl("moreicon"), QPixmap(":/images/icons/zoom-in.png"));
}

void DetailedView::setKanjiClickable(bool clickable)
{
	_kanjiClickable = clickable;
}

void DetailedView::setHistoryEnabled(bool enabled)
{
	_historyEnabled = enabled;
	_historyPrevAction->setEnabled(enabled ? _history.hasPrevious() : false);
	_historyNextAction->setEnabled(enabled ? _history.hasNext() : false);
}

void DetailedView::previous()
{
	EntryRef prev;
	bool ok = _history.previous(prev);
	if (!ok) return;
	_display(prev.get());
}

void DetailedView::next()
{
	EntryRef next;
	bool ok = _history.next(next);
	if (!ok) return;
	_display(next.get());
}

void DetailedView::addBackgroundJob(DetailedViewJob *job)
{
	_jobsRunner.addJob(job);
}

void DetailedView::addWatchEntry(const ConstEntryPointer &entry)
{
	_watchedEntries << entry;
	connect(entry.data(), SIGNAL(entryChanged(Entry *)), this, SLOT(redisplay()));
}

void DetailedView::populateToolBar(QToolBar *toolBar)
{
	toolBar->addAction(_historyPrevAction);
	toolBar->addAction(_historyNextAction);
	toolBar->addSeparator();
	_entryView.populateToolBar(toolBar);
}

void DetailedView::mousePressEvent(QMouseEvent *e)
{
	QString anchor(anchorAt(e->pos()));
	if (e->button() == Qt::LeftButton && anchor.startsWith("entry://")) {
		QUrl url(anchor);
		_dragStarted = true;
		_dragStartPos = e->pos();
		_dragEntryRef = EntryRef(QUrlQuery(url).queryItemValue("type").toInt(), QUrlQuery(url).queryItemValue("id").toInt());
		e->accept();
	} else {
		_dragStarted = false;
		QTextBrowser::mousePressEvent(e);
	}
}

void DetailedView::mouseMoveEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::LeftButton) && _dragStarted) {
		if ((e->pos() - _dragStartPos).manhattanLength() >= QApplication::startDragDistance()) {
			_dragStarted = false;
			QDrag *drag = new QDrag(this);
			QMimeData *data = new QMimeData();
			QByteArray encodedData;
			QDataStream stream(&encodedData, QIODevice::WriteOnly);
			stream << _dragEntryRef;
			data->setData("tagainijisho/entry", encodedData);
			drag->setMimeData(data);
			drag->exec(Qt::CopyAction, Qt::CopyAction);
		}
		e->accept();
		return;
	}
	QTextBrowser::mouseMoveEvent(e);
}

void DetailedView::mouseReleaseEvent(QMouseEvent *e)
{
	_dragStarted = false;
	QTextBrowser::mouseReleaseEvent(e);
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
	_currentJob->firstResult();
}

void DetailedViewJobRunner::onResult(EntryPointer entry)
{
	// If we are just flushing the job results queue, don't lose time here
	if (_ignoreJobs) return;

	Q_ASSERT(_currentJob != 0);
	_currentJob->result(entry);
	_view->addWatchEntry(entry);
	// Workaround for the fact QTextEdit does not seem to like when we change the
	// displayed text document so often - without this scheduled repaint graphical
	// glitches tend to appear on the first time a given entry is displayed.
	_view->viewport()->update();
}

void DetailedViewJobRunner::onCompleted()
{
	// If we are just flushing the job results queue, don't lose time here
	if (_ignoreJobs) return;

	Q_ASSERT(_currentJob != 0);
	_currentJob->completed();

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
	//_cursor.insertText(" ");
	//_cursor.movePosition(QTextCursor::Left);
}

DetailedViewFonts::DetailedViewFonts(QWidget *parent) : QObject(parent)
{
	int pointSize;

	// Init fonts and colors with the preferences values
	_font[DefaultText] = QFont();

	pointSize = DetailedViewFonts::textFontSize.value();
	if (pointSize > 0)
		_font[DefaultText].setPointSize(pointSize);
	_font[Kanji] = QFont(Font::jpFontFamily(), DetailedViewFonts::kanjiFontSize.value());
	_font[Kana] = QFont(Font::jpFontFamily(), DetailedViewFonts::kanaFontSize.value());
	_font[KanjiHeader] = QFont(Font::jpFontFamily(), DetailedViewFonts::kanjiHeaderFontSize.value());
	_font[KanaHeader] = QFont(Font::jpFontFamily(), DetailedViewFonts::kanaHeaderFontSize.value());

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

QString DetailedViewFonts::_CSS(FontRole role) const
{
	const QFont &font = _font[role];
	QString ret;
	if (!font.family().isEmpty()) ret += QString("\tfont-family: %1;\n").arg(font.family());
	if (color(role) != QApplication::palette().color(QPalette::Text))
		ret += QString("\tcolor: %1;\n").arg(EntryFormatter::colorTriplet(color(role)));
	ret += QString("\tfont-style: %1;\n").arg(font.italic() ? "italic" : "normal");
	ret += QString("\tfont-weight: %1;\n").arg(font.bold() ? "bold" : "normal");
	ret += QString("\tfont-size: %1pt;\n").arg(font.pointSize());

	return ret;
}

QString DetailedViewFonts::_HTML(FontRole role) const
{
	const QFont &font = _font[role];
	QString ret("<span style=\"");
	if (!font.family().isEmpty()) ret += QString(" font-family: %1;").arg(font.family());
	if (color(role) != QApplication::palette().color(QPalette::Text))
		ret += QString(" color: %1;").arg(EntryFormatter::colorTriplet(color(role)));
	ret += QString(" font-style: %1;").arg(font.italic() ? "italic" : "normal");
	ret += QString(" font-weight: %1;").arg(font.bold() ? "bold" : "normal");
	ret += QString(" font-size: %1pt;").arg(font.pointSize());

	return ret + "\">";
}

EntryMenuHandler::EntryMenuHandler() : DetailedViewLinkHandler("entry")
{
}

void EntryMenuHandler::handleUrl(const QUrl &url, DetailedView *view)
{
	EntryPointer entry(EntryRef(QUrlQuery(url).queryItemValue("type").toInt(), QUrlQuery(url).queryItemValue("id").toInt()).get());
	if (entry) MainWindow::instance()->detailedView()->display(entry);
}

ListLinkHandler::ListLinkHandler() : DetailedViewLinkHandler("list")
{
}

void ListLinkHandler::handleUrl(const QUrl &url, DetailedView *view)
{
	int rowId = QUrlQuery(url).queryItemValue("rowid").toInt();

	QAbstractItemView *aView = MainWindow::instance()->entryListWidget()->entryListView();
	EntryListModel *model = qobject_cast<EntryListModel *>(aView->model());
	if (!model) return;
	QModelIndex idx(model->index(rowId));
	MainWindow::instance()->listDockWidget()->setVisible(true);
	aView->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect);
	aView->scrollTo(idx);
}

TagsLinkHandler::TagsLinkHandler() : DetailedViewLinkHandler("tag")
{
}

void TagsLinkHandler::handleUrl(const QUrl &url, DetailedView *view)
{
	MainWindow *mainWindow = MainWindow::instance();
	TagsFilterWidget *extender = qobject_cast<TagsFilterWidget *>(mainWindow->searchWidget()->getSearchFilter("tagssearch"));
	if (!extender) return;

	if (QUrlQuery(url).hasQueryItem("reset")) mainWindow->searchWidget()->searchBuilder()->reset();
	extender->setAutoUpdateQuery(false);
	extender->setTags(QUrlQuery(url).queryItemValue("tag") + " ");
	extender->setAutoUpdateQuery(true);
	MainWindow::instance()->searchDockWidget()->setVisible(true);
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
	else if (url.scheme() == "http") QDesktopServices::openUrl(url);
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
