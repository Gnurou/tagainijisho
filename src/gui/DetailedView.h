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

#ifndef __GUI_DETAILED_VIEW_H
#define __GUI_DETAILED_VIEW_H

#include "core/EntriesCache.h"
#include "core/ASyncEntryLoader.h"
#include "gui/SingleEntryView.h"
#include "gui/AbstractHistory.h"
#include "gui/ScrollBarSmoothScroller.h"

#include <QTextBrowser>
#include <QTextCursor>
#include <QItemSelection>
#include <QMap>
#include <QPair>
#include <QMouseEvent>
#include <QQueue>
#include <QSet>

class DetailedView;

/**
 * An asynchronous database job to be performed by the dedicated thread of this
 * detailed view. A query is executed and a slot is called when a result is found.
 *
 * The slot to be invoked MUST be of the form slot(EntryPointer<Entry>).
 */
class DetailedViewJob
{
private:
	void __init();

protected:
	QString _sql;
	QTextCursor _cursor;

public:
	DetailedViewJob(const QString &sql, const QTextCursor &cursor);
	DetailedViewJob(const QTextCursor &cursor);
	virtual ~DetailedViewJob() {}

	QTextCursor &cursor() { return _cursor; }
	const QString &sql() const { return _sql; }

	virtual void firstResult() { }
	virtual void result(EntryPointer entry) { }
	virtual void completed() { }
};

class DetailedViewJobRunner : public QObject
{
	Q_OBJECT
protected:
	DetailedView *_view;
	DatabaseThread *_dbThread;
	ASyncEntryLoader *_aQuery;

	QQueue<DetailedViewJob *> _jobs;
	DetailedViewJob *_currentJob;
	bool _ignoreJobs;

protected slots:
	void onFirstResult();
	void onResult(EntryPointer entry);
	void onCompleted();
	void onAborted();
	void onError(const QString &error);
	void runNextJob();

public:
	DetailedViewJobRunner(DetailedView *view, QObject *parent = 0);
	virtual ~DetailedViewJobRunner();

	void addJob(DetailedViewJob *job);
	void runAllJobs();
	void abortCurrentJob();
	void abortAllJobs();
};

class DetailedViewFonts : public QObject
{
	Q_OBJECT
public:
	typedef enum { DefaultText, Kana, Kanji, KanaHeader, KanjiHeader, MAX_FONTS } FontRole;
	static PreferenceItem<int> textFontSize;
	static PreferenceItem<int> kanjiFontSize;
	static PreferenceItem<int> kanaFontSize;
	static PreferenceItem<int> kanjiHeaderFontSize;
	static PreferenceItem<int> kanaHeaderFontSize;

private:
	static DetailedViewFonts *_instance;

	QFont _font[MAX_FONTS];
	QColor _color[MAX_FONTS];

	void _setFont(FontRole role, const QFont &font);
	void _fontsChanged();
	QColor _defaultColor(FontRole role) const;
	QTextCharFormat _charFormat(FontRole role) const;
	QString _CSS(FontRole role) const;
	QString _HTML(FontRole role) const;

public:
	DetailedViewFonts(QWidget *parent = 0);

	static void setFont(FontRole role, const QFont &font) { _instance->_setFont(role, font); }
	static void fontsChanged() { _instance->_fontsChanged(); }
	static const QFont &font(FontRole role) { return _instance->_font[role]; }
	static const QColor &color(FontRole role) { return _instance->_color[role]; }
	static QColor defaultColor(FontRole role) { return _instance->_defaultColor(role); }
	static QTextCharFormat charFormat(FontRole role) { return _instance->_charFormat(role); }
	static QString CSS(FontRole role) { return _instance->_CSS(role); }
	static QString HTML(FontRole role) { return _instance->_HTML(role); }

signals:
	void fontsHaveChanged();

friend class DetailedView;
};

/**
 * Handles links when clicked inside the detailed view.
 */
class DetailedViewLinkHandler
{
private:
	QString _scheme;

public:
	DetailedViewLinkHandler(const QString &scheme) : _scheme(scheme) {}
	virtual ~DetailedViewLinkHandler() {}

	const QString &scheme() const { return _scheme; }
	virtual void handleUrl(const QUrl &url, DetailedView *view) = 0;
};

class DetailedViewLinkManager : public QObject
{
	Q_OBJECT
private:
	static DetailedViewLinkManager _instance;
	static QMap<QString, DetailedViewLinkHandler *> _handlers;
	DetailedViewLinkManager() : QObject(0) {}

public:
	static DetailedViewLinkManager *instance() { return &_instance; }

	static bool registerHandler(DetailedViewLinkHandler *handler);
	static DetailedViewLinkHandler *getHandler(const QString &scheme);
	static bool removeHandler(DetailedViewLinkHandler *handler);
	static bool removeHandler(const QString &scheme);

public slots:
	void handleUrl(const QUrl &url);
};

/**
 * Default handler to manage open partially displayed entries
 */
class EntryMenuHandler : public DetailedViewLinkHandler
{
public:
	EntryMenuHandler();
	void handleUrl(const QUrl &url, DetailedView *view);
};

class ListLinkHandler : public DetailedViewLinkHandler
{
public:
	ListLinkHandler();
	void handleUrl(const QUrl &url, DetailedView *view);
};

class SearchBar;
/**
 * Handles when tags are clicked
 */
class TagsLinkHandler : public DetailedViewLinkHandler
{
public:
	TagsLinkHandler();
	void handleUrl(const QUrl &url, DetailedView *view);
};

/**
 * A rich-text widget suitable for displaying details about entries
 * and doing basic manipulations on them.
 */
class DetailedView : public QTextBrowser
{
	Q_OBJECT
private:
	// Set of event filters used by all detailed views
	static QSet<QObject *> _eventFilters;
	// Set of all the instances of detailed views, so we can install the event filters
	// even after instanciation
	static QSet<DetailedView *> _instances;
	static EntryMenuHandler _entryHandler;
	static TagsLinkHandler _tagsLinkHandler;
	static ListLinkHandler _listLinkHandler;
	bool _kanjiClickable;
	bool _historyEnabled;
	ScrollBarSmoothScroller _scroller;
	/// Used to know whether we are about to drag the entry currently displayed
	EntryRef _dragEntryRef;
	QPoint _dragStartPos;
	bool _dragStarted;

protected:
	AbstractHistory<EntryRef, QList<EntryRef> > _history;
	SingleEntryView _entryView;
	DetailedViewJobRunner _jobsRunner;
	QList<ConstEntryPointer> _watchedEntries;
	QAction *_historyPrevAction;
	QAction *_historyNextAction;
	
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);

public:
	static void registerEventFilter(QObject *obj);
	static void removeEventFilter(QObject *obj);

	DetailedView(QWidget *parent = 0);
	virtual ~DetailedView();
	void setSmoothScrolling(bool value);

	bool kanjiClickable() const { return _kanjiClickable; }
	void setKanjiClickable(bool clickable);
	void setHistoryEnabled(bool enabled);
	const SingleEntryView *entryView() const { return &_entryView; }

	void addBackgroundJob(DetailedViewJob *job);
	static const QSet<DetailedView *> &instances() { return _instances; }

	/**
	 * Fake a click event on the provided URL.
	 */
	void fakeClick(const QUrl &url) { emit anchorClicked(url); }
	/**
	 * Add an entry to watch, i.e. the view will be
	 * redrawn if this entry changes.
	 */
	void addWatchEntry(const ConstEntryPointer &entry);

	static PreferenceItem<int> historySize;
	static PreferenceItem<bool> smoothScrolling;

	void populateToolBar(QToolBar *toolbar);
	
protected slots:
	/// Display previous item in history, if any.
	void previous();
	/// Display next item in history, if any.
	void next();
	/**
	 * Display an entry without updating history.
	 * If update is true, then the entry is redisplayed
	 * regardless of whether it is the same as before.
	 */
	virtual void _display(const EntryPointer &entry, bool update = false);

public slots:
	/// Display an entry, updating the history.
	void display(const EntryPointer& entry);
	/// Redraw the current entry, if any.
	void redisplay();
	/// Clear the display (keep history)
	void clear();

	/**
	 * Manually inform the view about which entry it is displaying. Calling
	 * display() is enough to do this, but there are times when this
	 * the entry is drawn manually using the QTextBrowser interface (for instance,
	 * when drawing only part of the entry for flashcards). In these cases, calling
	 * this function allows to use the right-click menu entries.
	 *
	 * Note that the history is not updated by this function.
	 */
	void setEntry(const EntryPointer &entry) { _entryView.setEntry(entry); }

signals:
	void entryDisplayed(const ConstEntryPointer &entry);
};

#endif
