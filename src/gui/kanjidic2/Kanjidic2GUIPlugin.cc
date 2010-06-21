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

#include "core/TextTools.h"
#include "gui/TrainSettings.h"
#include "gui/kanjidic2/Kanjidic2EntryFormatter.h"
#include "gui/kanjidic2/KanjiPopup.h"
#include "gui/MainWindow.h"
#include "gui/EntryTypeFilterWidget.h"
#include "gui/kanjidic2/Kanjidic2Preferences.h"
#include "gui/kanjidic2/Kanjidic2GUIPlugin.h"
// TODO BAD - dependency against JMdict!
#include "gui/jmdict/JMdictGUIPlugin.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QCoreApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAction>
#include <QToolTip>
#include <QAbstractTextDocumentLayout>
#include <QScrollBar>

const QString Kanjidic2GUIPlugin::kanjiGrades[] = {
	QT_TRANSLATE_NOOP("Kanjidic2GUIPlugin", "Invalid"),
	QT_TRANSLATE_NOOP("Kanjidic2GUIPlugin", "Elementary 1st grade"),
	QT_TRANSLATE_NOOP("Kanjidic2GUIPlugin", "Elementary 2nd grade"),
	QT_TRANSLATE_NOOP("Kanjidic2GUIPlugin", "Elementary 3rd grade"),
	QT_TRANSLATE_NOOP("Kanjidic2GUIPlugin", "Elementary 4th grade"),
	QT_TRANSLATE_NOOP("Kanjidic2GUIPlugin", "Elementary 5th grade"),
	QT_TRANSLATE_NOOP("Kanjidic2GUIPlugin", "Elementary 6th grade"),
	QT_TRANSLATE_NOOP("Kanjidic2GUIPlugin", "Invalid"),
	QT_TRANSLATE_NOOP("Kanjidic2GUIPlugin", "Secondary school"),
	QT_TRANSLATE_NOOP("Kanjidic2GUIPlugin", "Used for names (1)"),
	QT_TRANSLATE_NOOP("Kanjidic2GUIPlugin", "Used for names (2)"),
};

PreferenceItem<bool> Kanjidic2GUIPlugin::kanjiTooltipEnabled("kanjidic", "kanjiTooltipEnabled", true);

Kanjidic2GUIPlugin::Kanjidic2GUIPlugin() : Plugin("kanjidic2GUI"), _flashKL(0), _flashKS(0), _flashML(0), _flashMS(0), _readingPractice(0), _showKanjiPopup(0), _linkHandler(0), _wordsLinkHandler(0), _componentsLinkHandler(0), _filter(0), _trainer(0), _readingTrainer(0), _cAction(0), _kAction(0), _dragStarted(false), _dragEntryRef(0)
{
}

Kanjidic2GUIPlugin::~Kanjidic2GUIPlugin()
{
}

bool Kanjidic2GUIPlugin::onRegister()
{
	// Check if the kanjidic plugin is loaded
	if (!Plugin::pluginExists("kanjidic2")) return false;
	// Register the formatter
	if (!EntryFormatter::registerFormatter(KANJIDIC2ENTRY_GLOBALID, &Kanjidic2EntryFormatter::instance())) return false;
	// Register the link handlers
	_linkHandler = new KanjiLinkHandler();
	if (!DetailedViewLinkManager::registerHandler(_linkHandler)) return false;
	_wordsLinkHandler = new KanjiAllWordsHandler();
	if (!DetailedViewLinkManager::registerHandler(_wordsLinkHandler)) return false;
	_componentsLinkHandler = new KanjiAllComponentsOfHandler();
	if (!DetailedViewLinkManager::registerHandler(_componentsLinkHandler)) return false;

	// Add the main window entries
	MainWindow *mainWindow(MainWindow::instance());
	QMenu *menu = mainWindow->trainMenu();
	QMenu *menu2 = menu->addMenu(tr("Kanji flashcards"));
	_flashKL = menu2->addAction(tr("From &kanji, whole study list"));
	_flashKS = menu2->addAction(tr("From &kanji, current set"));
	connect(_flashKL, SIGNAL(triggered()), this, SLOT(trainingKanjiList()));
	connect(_flashKS, SIGNAL(triggered()), this, SLOT(trainingKanjiSet()));
	menu2->addSeparator();
	_flashML = menu2->addAction(tr("From &meaning, whole study list"));
	_flashMS = menu2->addAction(tr("From &meaning, current set"));
	connect(_flashML, SIGNAL(triggered()), this, SLOT(trainingMeaningList()));
	connect(_flashMS, SIGNAL(triggered()), this, SLOT(trainingMeaningSet()));
	_readingPractice = menu->addAction(tr("&Reading practice, whole study list"));
	connect(_readingPractice, SIGNAL(triggered()), this, SLOT(readingPractice()));
	
	// Add the components searchers to the tool bar
	_kAction = new KanjiInputPopupAction(new KanjiInputter(new RadicalKanjiSelector(), false, mainWindow), tr("Radical search input"), mainWindow);
	_kAction->setShortcut(QKeySequence("Ctrl+k"));
	mainWindow->searchMenu()->addAction(_kAction);
	_cAction = new KanjiInputPopupAction(new KanjiInputter(new ComponentKanjiSelector(), true, mainWindow), tr("Component search input"), mainWindow);
	_cAction->setShortcut(QKeySequence("Ctrl+j"));
	mainWindow->searchMenu()->addAction(_cAction);

	_showKanjiPopup = new QAction(tr("Show stroke popup for currently displayed kanji"), this);
	_showKanjiPopup->setShortcut(QKeySequence("Ctrl+s"));
	_showKanjiPopup->setShortcutContext(Qt::ApplicationShortcut);
	connect(_showKanjiPopup, SIGNAL(triggered()), this, SLOT(popupDetailedViewKanjiEntry()));
	MainWindow::instance()->addAction(_showKanjiPopup);

	// Register the searchbar extender
	_filter = new Kanjidic2FilterWidget(0);
	mainWindow->searchWidget()->addSearchFilter(_filter);

	// Register the detailed view event filter
	DetailedView::registerEventFilter(this);

	// Register the preferences panel
	PreferencesWindow::addPanel(&Kanjidic2Preferences::staticMetaObject);

	return true;
}

bool Kanjidic2GUIPlugin::onUnregister()
{
	// Remove the preferences panel
	PreferencesWindow::removePanel(&Kanjidic2Preferences::staticMetaObject);

	// Remove the detailed view event filter
	DetailedView::removeEventFilter(this);

	MainWindow *mainWindow = MainWindow::instance();
	
	mainWindow->removeAction(_showKanjiPopup);
	delete _showKanjiPopup;
	_showKanjiPopup = 0;

	// Remove the search extender
	mainWindow->searchWidget()->removeSearchFilterWidget(_filter->name());
	delete _filter; _filter = 0;
	// Remove the components searchers
	delete _cAction; _cAction = 0;
	delete _kAction; _kAction = 0;
	// Remove the main window entries
	delete _flashKL; _flashKL = 0;
	delete _flashKS; _flashKS = 0;
	delete _flashML; _flashML = 0;
	delete _flashMS; _flashMS = 0;
	delete _readingPractice; _readingPractice = 0;
	// Remove the link handlers
	DetailedViewLinkManager::removeHandler(_linkHandler);
	delete _linkHandler; _linkHandler = 0;
	DetailedViewLinkManager::removeHandler(_wordsLinkHandler);
	delete _wordsLinkHandler; _wordsLinkHandler = 0;
	DetailedViewLinkManager::removeHandler(_componentsLinkHandler);
	delete _componentsLinkHandler; _componentsLinkHandler = 0;
	// Remove the formatter
	EntryFormatter::removeFormatter(KANJIDIC2ENTRY_GLOBALID);
	return true;
}

// TODO duplicate code from JMdictGUIPlugin
void Kanjidic2GUIPlugin::training(YesNoTrainer::TrainingMode mode, const QString &queryString)
{
	bool restart = false;
	// Trainer is automatically set to 0 by the destroyed() slot
	if (_trainer && (_trainer->trainingMode() != mode || _trainer->query() != queryString)) delete _trainer;
	if (!_trainer) {
		restart = true;
		_trainer = new YesNoTrainer(MainWindow::instance());
		_trainer->setAttribute(Qt::WA_DeleteOnClose);
		_trainer->setWindowFlags(Qt::Window);
		connect(_trainer, SIGNAL(destroyed()), this, SLOT(trainerDeleted()));
		_trainer->setTrainingMode(mode);
		_trainer->setQuery(queryString);
	}

	_trainer->show();
	_trainer->activateWindow();
	_trainer->raise();

	if (restart) _trainer->train();
}

void Kanjidic2GUIPlugin::trainingKanjiList()
{
	training(YesNoTrainer::Japanese, TrainSettings::buildQueryString(KANJIDIC2ENTRY_GLOBALID));
}

void Kanjidic2GUIPlugin::trainingKanjiSet()
{
	QueryBuilder *qBuilder(MainWindow::instance()->searchWidget()->queryBuilder());
	const QueryBuilder::Statement *stat(qBuilder->getStatementForEntryType(KANJIDIC2ENTRY_GLOBALID));
	if (!stat) {
		QMessageBox::information(MainWindow::instance(), tr("Nothing to train"), tr("There are no kanji entries in this set to train on."));
		return;
	}

	QString queryString(stat->buildSqlStatement());
	queryString += " " + TrainSettings::buildOrderString("score");
	training(YesNoTrainer::Japanese, queryString);
}

void Kanjidic2GUIPlugin::trainingMeaningList()
{
	training(YesNoTrainer::Translation, TrainSettings::buildQueryString(KANJIDIC2ENTRY_GLOBALID));}

void Kanjidic2GUIPlugin::trainingMeaningSet()
{
	QueryBuilder *qBuilder(MainWindow::instance()->searchWidget()->queryBuilder());
	const QueryBuilder::Statement *stat(qBuilder->getStatementForEntryType(KANJIDIC2ENTRY_GLOBALID));
	if (!stat) {
		QMessageBox::information(MainWindow::instance(), tr("Nothing to train"), tr("There are no kanji entries in this set to train on."));
		return;
	}

	QString queryString(stat->buildSqlStatement());
	queryString += " " + TrainSettings::buildOrderString("score");
	training(YesNoTrainer::Translation, queryString);
}

void Kanjidic2GUIPlugin::trainerDeleted()
{
	_trainer = 0;
}

void Kanjidic2GUIPlugin::readingPractice()
{
	bool restart = false;
	if (!_readingTrainer) {
		restart = true;
		_readingTrainer = new ReadingTrainer(MainWindow::instance());
		_readingTrainer->setAttribute(Qt::WA_DeleteOnClose);
		_readingTrainer->setWindowFlags(Qt::Window);
		connect(_readingTrainer, SIGNAL(destroyed()), this, SLOT(readingTrainerDeleted()));
		_readingTrainer->newSession();
	}

	_readingTrainer->show();
	_readingTrainer->activateWindow();
	_readingTrainer->raise();

	if (restart) _readingTrainer->train();
}

void Kanjidic2GUIPlugin::popupDetailedViewKanjiEntry()
{
	EntryPointer tEntry;
	QWidget *activeWindow = QApplication::activeWindow();
	if (activeWindow == MainWindow::instance())
		tEntry = MainWindow::instance()->detailedView()->entryView()->entry();
	else if (activeWindow->inherits("YesNoTrainer")) {
		YesNoTrainer *trainer = qobject_cast<YesNoTrainer *>(activeWindow);
		if (!trainer->answerShown()) return;
		tEntry = trainer->detailedView()->entryView()->entry();
	}
	if (tEntry && tEntry->type() == KANJIDIC2ENTRY_GLOBALID) {
		showPopup(tEntry.staticCast<Kanjidic2Entry>(), QCursor::pos());
	}
}

void Kanjidic2GUIPlugin::readingTrainerDeleted()
{
	_readingTrainer = 0;
}

bool Kanjidic2GUIPlugin::eventFilter(QObject *obj, QEvent *_event)
{
	DetailedView *view(qobject_cast<DetailedView *>(obj->parent()));
	if (!view) return false;
	int pos = view->document()->documentLayout()->hitTest(view->viewport()->mapFromGlobal(QCursor::pos() + QPoint(0, view->verticalScrollBar()->value())), Qt::ExactHit);
	switch(_event->type()) {
	case QEvent::MouseButtonPress:
	{
		QMouseEvent *e(static_cast<QMouseEvent *>(_event));
		// Left click on a kanji?
		if (e->button() == Qt::LeftButton && pos != -1) {
			// Either prepare for a drag or a click to display the kanji popup
			// if we are on a kanji
			QTextCursor cursor(view->document());
			cursor.setPosition(pos);
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
			if (cursor.charFormat().isAnchor()) return false;
			QChar t(cursor.selectedText()[0]);
			QString c(t);
			if (t.isHighSurrogate()) {
				cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
				c += cursor.selectedText()[1];
			}
			if (TextTools::isKanjiChar(c)) {
				// Yes, it is a kanji - prepare for a drag or a click
				// and consume the event.
				_dragStarted = true;
				_dragPos = e->pos();
				_dragEntryRef = KanjiEntryRef(TextTools::singleCharToUnicode(c));
				return true;
			}
		}
		return false;
	}
	case QEvent::MouseMove:
	{
		QMouseEvent *e(static_cast<QMouseEvent *>(_event));
		// Drag in progress -> see if we shall start a drag!
		if (_dragStarted) {
			if ((e->pos() - _dragPos).manhattanLength() >= QApplication::startDragDistance()) {
				_dragStarted = false;
				QDrag *drag = new QDrag(view);
				QMimeData *data = new QMimeData();
				QByteArray encodedData;
				QDataStream stream(&encodedData, QIODevice::WriteOnly);
				stream << _dragEntryRef;
				data->setData("tagainijisho/entry", encodedData);
				drag->setMimeData(data);
				drag->exec(Qt::CopyAction, Qt::CopyAction);
				_dragStarted = false;
			}
			// Always filter the event if the button is pushed
			return true;
		}
		// See if we have to show the kanji tooltip
		if (pos != -1) {
			QTextCursor cursor(view->document());
			cursor.setPosition(pos);
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
			if (!cursor.charFormat().isAnchor()) {
				QChar t(cursor.selectedText()[0]);
				QString c(t);
				if (t.isHighSurrogate()) {
					cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
					c += cursor.selectedText()[1];
				}
				if (TextTools::isKanjiChar(c)) {
					ConstKanjidic2EntryPointer entry(KanjiEntryRef(TextTools::singleCharToUnicode(c)).get());
					// If kanji are clickable, change the cursor
					if (view->kanjisClickable()) {
						view->viewport()->setCursor(QCursor(Qt::PointingHandCursor));
						// Only show the tooltip if the entry exists in the database!
						if (kanjiTooltipEnabled.value() && entry) {
							const Kanjidic2EntryFormatter *formatter(static_cast<const Kanjidic2EntryFormatter *>(EntryFormatter::getFormatter(KANJIDIC2ENTRY_GLOBALID)));
							formatter->showToolTip(entry, QCursor::pos());
						}
					}
				} else {
					if (view->kanjisClickable()) view->viewport()->setCursor(QCursor(Qt::ArrowCursor));
					QToolTip::showText(QCursor::pos(), "");
				}
			}
			return false;
		}
		view->viewport()->unsetCursor();
		if (QToolTip::isVisible()) {
			QToolTip::hideText();
		}
		return false;
	}
	case QEvent::MouseButtonRelease:
	{
		QMouseEvent *e(static_cast<QMouseEvent *>(_event));
		if (e->button() == Qt::LeftButton && _dragStarted) {
			// Released on a kanji? Let's show the popup if needed.
			_dragStarted = false;
			// TODO this property should be moved out of view
			if (view->kanjisClickable()) view->fakeClick(QUrl(QString("drawkanji:?kanji=%1").arg(_dragEntryRef.kanji())));
			return true;
		}
		return false;
	}
	case QEvent::ContextMenu:
	{
		QContextMenuEvent *event(static_cast<QContextMenuEvent *>(_event));
		QMenu *menu = 0;
		SingleEntryView tview;

		// Are we on an entry link or a kanji character?
		if (pos != -1) {
			QTextCursor cursor(view->document());
			cursor.setPosition(pos);
			cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
			if (cursor.charFormat().isAnchor()) {
				QUrl url(cursor.charFormat().anchorHref());
				if (url.scheme() == "entry") {
					EntryPointer entry(EntryRef(url.queryItemValue("type").toInt(), url.queryItemValue("id").toInt()).get());
					if (entry) tview.setEntry(entry);
				}
			}
			else {
				QChar c(cursor.selectedText()[0]);
				if (TextTools::isKanjiChar(c)) {
					EntryPointer entry(KanjiEntryRef(c.unicode()).get());
					if (entry) tview.setEntry(entry);
				}
			}
		}

		QAction *openAction(0);
		if (tview.entry()) {
			menu = new QMenu();
			openAction = menu->addAction(QIcon("images/icons/zoom-in.png"), tr("Open in detailed view..."));
			menu->addSeparator();
			tview.populateMenu(menu);
		}
		else return false;

		QAction *selected = menu->exec(event->globalPos());
		if (selected && selected == openAction) MainWindow::instance()->detailedView()->display(tview.entry());
		delete menu;
		return true;
	}
	default:
		break;
	}
	return false;
}

void Kanjidic2GUIPlugin::showPopup(const Kanjidic2EntryPointer &entry, const QPoint &pos)
{
	if (!entry) return;

	KanjiPopup *popup = new KanjiPopup();
	QObject::connect(popup, SIGNAL(requestDisplay(EntryPointer)), MainWindow::instance()->detailedView(), SLOT(display(EntryPointer)));

	popup->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	popup->setWindowTitle(tr("Tracing for %1").arg(entry->kanji()));
	popup->setAttribute(Qt::WA_DeleteOnClose);
	popup->setWindowFlags(Qt::Popup);
	popup->display(entry);

	popup->move(pos);
	MainWindow::fitToScreen(popup);
	popup->show();
	// Need to display a second time once the window is visible to get the labels correctly formatted.
	popup->display(entry);

	MainWindow::fitToScreen(popup);
}

KanjiLinkHandler::KanjiLinkHandler() : DetailedViewLinkHandler("drawkanji")
{
}

void KanjiLinkHandler::handleUrl(const QUrl &url, DetailedView *view)
{
	QString kanji = url.queryItemValue("kanji");
	if (kanji.isEmpty()) return;

	Kanjidic2EntryPointer entry(KanjiEntryRef(TextTools::singleCharToUnicode(kanji)).get());
	Kanjidic2GUIPlugin::showPopup(entry, QCursor::pos());
}

KanjiAllWordsHandler::KanjiAllWordsHandler() : DetailedViewLinkHandler("allwords")
{
}

void KanjiAllWordsHandler::handleUrl(const QUrl &url, DetailedView *view)
{
	MainWindow *mainWindow = MainWindow::instance();
	EntryTypeFilterWidget *entryType = qobject_cast<EntryTypeFilterWidget *>(mainWindow->searchWidget()->getSearchFilter("entrytypeselector"));
	JMdictFilterWidget *extender = qobject_cast<JMdictFilterWidget *>(mainWindow->searchWidget()->getSearchFilter("jmdictoptions"));
	if (!entryType || !extender) return;

	if (url.hasQueryItem("reset")) mainWindow->searchWidget()->searchBuilder()->reset();
	entryType->setAutoUpdateQuery(false);
	entryType->setType(EntryTypeFilterWidget::Vocabulary);
	entryType->setAutoUpdateQuery(true);
	extender->setAutoUpdateQuery(false);
	extender->setContainedKanjis(url.queryItemValue("kanji"));
	extender->setAutoUpdateQuery(true);
	mainWindow->searchWidget()->searchBuilder()->runSearch();
}

KanjiAllComponentsOfHandler::KanjiAllComponentsOfHandler() : DetailedViewLinkHandler("component")
{
}

void KanjiAllComponentsOfHandler::handleUrl(const QUrl &url, DetailedView *view)
{
	MainWindow *mainWindow = MainWindow::instance();
	EntryTypeFilterWidget *entryType = qobject_cast<EntryTypeFilterWidget *>(mainWindow->searchWidget()->getSearchFilter("entrytypeselector"));
	Kanjidic2FilterWidget *extender = qobject_cast<Kanjidic2FilterWidget *>(mainWindow->searchWidget()->getSearchFilter("kanjidicoptions"));
	if (!entryType || !extender) return;

	if (url.hasQueryItem("reset")) mainWindow->searchWidget()->searchBuilder()->reset();
	entryType->setAutoUpdateQuery(false);
	entryType->setType(EntryTypeFilterWidget::Kanjis);
	entryType->setAutoUpdateQuery(true);
	extender->setAutoUpdateQuery(false);
	extender->setComponents(url.queryItemValue("kanji"));
	extender->setAutoUpdateQuery(true);
	mainWindow->searchWidget()->searchBuilder()->runSearch();
}
