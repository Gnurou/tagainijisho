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
#include "gui/BetterSpinBox.h"
#include "gui/KanjiValidator.h"
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

Kanjidic2GUIPlugin::Kanjidic2GUIPlugin() : Plugin("kanjidic2GUI"), _flashKL(0), _flashKS(0), _flashML(0), _flashMS(0), _readingPractice(0), _linkHandler(0), _wordsLinkHandler(0), _componentsLinkHandler(0), _filter(0), _trainer(0), _readingTrainer(0), _cAction(0), _kAction(0), _dragStarted(false), _dragEntryRef(0)
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
		// No button pushed - see if we have to show the kanji popup
		// TODO this property should be moved out of view
		if (view->kanjisClickable()) {
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
						view->viewport()->setCursor(QCursor(Qt::PointingHandCursor));
						// Only show the tooltip if the entry exists in the database!
						if (kanjiTooltipEnabled.value() && entry) {
							const Kanjidic2EntryFormatter *formatter(static_cast<const Kanjidic2EntryFormatter *>(EntryFormatter::getFormatter(KANJIDIC2ENTRY_GLOBALID)));
							formatter->showToolTip(entry, QCursor::pos());
						}
					}
				}
				return false;
			}
			view->viewport()->unsetCursor();
			if (QToolTip::isVisible()) {
				QToolTip::hideText();
			}
		}
		return false;
	}
	case QEvent::MouseButtonRelease:
	{
		QMouseEvent *e(static_cast<QMouseEvent *>(_event));
		if (e->button() == Qt::LeftButton && _dragStarted) {
			// Released on a kanji? Let's show the popup if needed.
			_dragStarted = false;
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

KanjiLinkHandler::KanjiLinkHandler() : DetailedViewLinkHandler("drawkanji")
{
}

void KanjiLinkHandler::handleUrl(const QUrl &url, DetailedView *view)
{
	QString kanji = url.queryItemValue("kanji");
	if (kanji.isEmpty()) return;

	// Do it here so that the popup appears at the position we clicked, even if loading
	// takes time and we moved the mouse.
	QPoint pos = QCursor::pos();

//	QRect windowRect = QApplication::activeWindow()->frameGeometry();
	QRect windowRect = QApplication::desktop()->availableGeometry(view);

	Kanjidic2EntryPointer entry(KanjiEntryRef(TextTools::singleCharToUnicode(kanji)).get());
	if (!entry) return;

	KanjiPopup *popup = new KanjiPopup(QApplication::focusWidget());
	QObject::connect(popup, SIGNAL(requestDisplay(EntryPointer)), MainWindow::instance()->detailedView(), SLOT(display(EntryPointer)));

	popup->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	popup->setWindowModality(Qt::ApplicationModal);
	popup->setWindowTitle(tr("Tracing for %1").arg(kanji));
	popup->setAttribute(Qt::WA_DeleteOnClose);
	popup->setWindowFlags(Qt::Popup);
	popup->display(entry);

	QPoint lowerRight = pos + QPoint(popup->size().width(), popup->size().height());
	if (!windowRect.contains(QPoint(lowerRight.x(), pos.y()))) pos.setX(windowRect.right() - popup->width());
	if (!windowRect.contains(QPoint(pos.x(), lowerRight.y()))) pos.setY(windowRect.bottom() - popup->height());

	popup->move(pos);
	popup->show();
	// Need to display a second time once the window is visible to get the labels correctly formatted.
	popup->display(entry);

	pos = QCursor::pos();
	lowerRight = pos + QPoint(popup->size().width(), popup->size().height());
	if (!windowRect.contains(QPoint(lowerRight.x(), pos.y()))) pos.setX(windowRect.right() - popup->width());
	if (!windowRect.contains(QPoint(pos.x(), lowerRight.y()))) pos.setY(windowRect.bottom() - popup->height());

	// Do it again to ensure the geometry is correctly calculated after show()
	popup->move(pos);
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

static void prepareFourCornerComboBox(QComboBox *box)
{
	box->addItem("");
	box->addItem(QString::fromUtf8("亠"));
	box->addItem(QString::fromUtf8("一"));
	box->addItem(QString::fromUtf8("｜"));
	box->addItem(QString::fromUtf8("丶"));
	box->addItem(QString::fromUtf8("十"));
	box->addItem(QString::fromUtf8("キ"));
	box->addItem(QString::fromUtf8("口"));
	box->addItem(QString::fromUtf8("厂"));
	box->addItem(QString::fromUtf8("八"));
	box->addItem(QString::fromUtf8("小"));
}

Kanjidic2FilterWidget::Kanjidic2FilterWidget(QWidget *parent) : SearchFilterWidget(parent, "kanjidic")
{
	_propsToSave << "strokeCount" << "isStrokeRange" << "maxStrokeCount" << "radicals" << "components" << "unicode" << "skip" << "fourCorner" << "grades";

	QGroupBox *_strokeCountGroupBox = new QGroupBox(tr("Stroke count"), this);
	connect(_strokeCountGroupBox, SIGNAL(toggled(bool)), this, SLOT(commandUpdate()));
	{
		QVBoxLayout *vLayout = new QVBoxLayout(_strokeCountGroupBox);
		QHBoxLayout *hLayout = new QHBoxLayout();
		hLayout->setContentsMargins(0, 0, 0, 0);
		_strokeCountSpinBox = new BetterSpinBox(this);
		_strokeCountSpinBox->setSpecialValueText("");
		_strokeCountSpinBox->setRange(0, 34);
		hLayout->addWidget(_strokeCountSpinBox);
		connect(_strokeCountSpinBox, SIGNAL(valueChanged(QString)), this, SLOT(onStrokeRangeChanged()));
		_maxStrokeCountSpinBox = new BetterSpinBox(this);
		_maxStrokeCountSpinBox->setSpecialValueText("");
		_maxStrokeCountSpinBox->setRange(0, 34);
		_maxStrokeCountSpinBox->setVisible(false);
		_maxStrokeCountSpinBox->setEnabled(false);
		hLayout->addWidget(_maxStrokeCountSpinBox);
		connect(_maxStrokeCountSpinBox, SIGNAL(valueChanged(QString)), this, SLOT(onStrokeRangeChanged()));
		_rangeCheckBox = new QCheckBox(tr("Range"), _strokeCountGroupBox);
		vLayout->addLayout(hLayout);
		vLayout->addWidget(_rangeCheckBox);
		connect(_rangeCheckBox, SIGNAL(toggled(bool)), this, SLOT(onStrokeRangeToggled(bool)));
	}
	QGroupBox *componentsGroupBox = new QGroupBox(this);
	{
		QVBoxLayout *vLayout = new QVBoxLayout(componentsGroupBox);
		QLabel *label;
		label = new QLabel(tr("Radicals"), componentsGroupBox);
		label->setAlignment(Qt::AlignHCenter);
		vLayout->addWidget(label);
		_radicals = new QLineEdit(componentsGroupBox);
		connect(_radicals, SIGNAL(textChanged(const QString &)), this, SLOT(commandUpdate()));
		// _radKSelector will be allocated the first time it is used
		_radKSelector = 0;
		_radicals->installEventFilter(this);
		vLayout->addWidget(_radicals);
		label = new QLabel(tr("Components"), componentsGroupBox);
		label->setAlignment(Qt::AlignHCenter);
		vLayout->addWidget(label);
		_components = new QLineEdit(componentsGroupBox);
		KanjiValidator *kanjiValidator = new KanjiValidator(_components);
		_components->setValidator(kanjiValidator);
		connect(_components, SIGNAL(textChanged(const QString &)), this, SLOT(commandUpdate()));
		// _compKSelector will be allocated the first time it is used
		_compKSelector = 0;
		_components->installEventFilter(this);
		vLayout->addWidget(_components);
	}
	QGroupBox *unicodeGroupBox = new QGroupBox(tr("Unicode"), this);
	{
		QHBoxLayout *hLayout = new QHBoxLayout(unicodeGroupBox);
		_unicode = new HexSpinBox(unicodeGroupBox);
		_unicode->setRange(0, 0x2A6DF);
		_unicode->setPrefix("0x");
		connect(_unicode, SIGNAL(valueChanged(int)), this, SLOT(delayedCommandUpdate()));
		hLayout->addWidget(_unicode);
	}
	QGroupBox *skipGroupBox = new QGroupBox(tr("SKIP code"), this);
	{
		QHBoxLayout *hLayout = new QHBoxLayout(skipGroupBox);
		_skip1 = new QSpinBox(unicodeGroupBox);
		_skip1->setRange(0, 4);
		_skip1->setSpecialValueText(" ");
		hLayout->addWidget(_skip1);
		connect(_skip1, SIGNAL(valueChanged(int)), this, SLOT(delayedCommandUpdate()));
		_skip2 = new QSpinBox(unicodeGroupBox);
		_skip2->setRange(0, 30);
		_skip2->setSpecialValueText(" ");
		hLayout->addWidget(_skip2);
		connect(_skip2, SIGNAL(valueChanged(int)), this, SLOT(delayedCommandUpdate()));
		_skip3 = new QSpinBox(unicodeGroupBox);
		_skip3->setRange(0, 30);
		_skip3->setSpecialValueText(" ");
		hLayout->addWidget(_skip3);
		connect(_skip3, SIGNAL(valueChanged(int)), this, SLOT(delayedCommandUpdate()));
	}
	QGroupBox *fourCornerGroupBox = new QGroupBox(tr("Four corner"), this);
	{
		QHBoxLayout *hLayout = new QHBoxLayout(fourCornerGroupBox);
		QGridLayout *gLayout = new QGridLayout();
		hLayout->addLayout(gLayout);
		_fcTopLeft = new QComboBox(fourCornerGroupBox);
		prepareFourCornerComboBox(_fcTopLeft);
		connect(_fcTopLeft, SIGNAL(currentIndexChanged(int)), this, SLOT(delayedCommandUpdate()));
		gLayout->addWidget(_fcTopLeft, 0, 0);
		_fcTopRight = new QComboBox(fourCornerGroupBox);
		prepareFourCornerComboBox(_fcTopRight);
		connect(_fcTopRight, SIGNAL(currentIndexChanged(int)), this, SLOT(delayedCommandUpdate()));
		gLayout->addWidget(_fcTopRight, 0, 1);
		_fcBotLeft = new QComboBox(fourCornerGroupBox);
		prepareFourCornerComboBox(_fcBotLeft);
		connect(_fcBotLeft, SIGNAL(currentIndexChanged(int)), this, SLOT(delayedCommandUpdate()));
		gLayout->addWidget(_fcBotLeft, 1, 0);
		_fcBotRight = new QComboBox(fourCornerGroupBox);
		prepareFourCornerComboBox(_fcBotRight);
		connect(_fcBotRight, SIGNAL(currentIndexChanged(int)), this, SLOT(delayedCommandUpdate()));
		gLayout->addWidget(_fcBotRight, 1, 1);
		_fcExtra = new QComboBox(fourCornerGroupBox);
		prepareFourCornerComboBox(_fcExtra);
		connect(_fcExtra, SIGNAL(currentIndexChanged(int)), this, SLOT(delayedCommandUpdate()));
		hLayout->addWidget(_fcExtra);
	}
	QGroupBox *gradeGroupBox = new QGroupBox(tr("School grade"), this);
	{
		QHBoxLayout *hLayout = new QHBoxLayout(gradeGroupBox);
		_gradeButton = new QPushButton(tr("Grade"), gradeGroupBox);
		QMenu *menu = new QMenu(_gradeButton);
		QAction *action;
		actionGroup = new QActionGroup(menu);
		actionGroup->setExclusive(false);
		for (int i = 1 ; i <= 10; i++) {
			if (i == 7) continue;
			action = actionGroup->addAction(tr(Kanjidic2GUIPlugin::kanjiGrades[i].toLatin1()));
			action->setProperty("Agrade", i);
			action->setCheckable(true);
		}
		connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onGradeTriggered(QAction *)));
		menu->addActions(actionGroup->actions());
		menu->addSeparator();
		allKyouku = menu->addAction(tr("All &Kyouiku kanji"));
		allKyouku->setCheckable(true);
		connect(allKyouku, SIGNAL(toggled(bool)), this, SLOT(allKyoukuKanjis(bool)));
		allJouyou = menu->addAction(tr("All &Jouyou kanji"));
		allJouyou->setCheckable(true);
		connect(allJouyou, SIGNAL(toggled(bool)), this, SLOT(allJouyouKanjis(bool)));

		_gradeButton->setMenu(menu);
		hLayout->addWidget(_gradeButton);
	}
	
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(_strokeCountGroupBox);
	mainLayout->addWidget(componentsGroupBox);
	mainLayout->addWidget(unicodeGroupBox);
	mainLayout->addWidget(skipGroupBox);
	mainLayout->addWidget(fourCornerGroupBox);
	mainLayout->addWidget(gradeGroupBox);
	mainLayout->setContentsMargins(0, 0, 0, 0);
}

bool Kanjidic2FilterWidget::eventFilter(QObject *watched, QEvent *event)
{
	if ((watched == _radicals || watched == _components)) {
		QLineEdit *focusWidget = qobject_cast<QLineEdit *>(watched);
		switch (event->type()) {
			// Automatically popup the selector on focus
			case QEvent::FocusIn:
			{
				KanjiSelector *selector = 0;
				bool justCreated = false;
				// Create the selector if this is the first time we use it
				if (focusWidget == _radicals && !_radKSelector) {
					_radKSelector = new RadicalKanjiSelector(_radicals);
					KanjiSelectorValidator *selectorValidator = new KanjiSelectorValidator(_radKSelector, _radicals);
					_radicals->setValidator(selectorValidator);
					justCreated = true;
				} else if (focusWidget == _components && !_compKSelector) {
					_compKSelector = new ComponentKanjiSelector(_components);
					justCreated = true;
				}
				if (focusWidget == _radicals) selector = _radKSelector;
				else if (focusWidget == _components) selector = _compKSelector;
				// If the selector has been created, setup its properties
				if (justCreated) {
					selector->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
					selector->setAttribute(Qt::WA_ShowWithoutActivating);
					selector->setWindowModality(Qt::NonModal);
					selector->setFocusPolicy(Qt::NoFocus);
					selector->installEventFilter(this);
					selector->associateTo(focusWidget);
					selector->reset();
				}
				selector->move(focusWidget->mapToGlobal(QPoint(focusWidget->rect().left() + (focusWidget->rect().width() - selector->rect().width()) / 2, focusWidget->rect().bottom())));	
				selector->show();
				break;
			}
			// Automatically hide the selector on focus out
			case QEvent::FocusOut:
				if (focusWidget == _radicals && !(QApplication::widgetAt(QCursor::pos()) != _radKSelector)) _radKSelector->hide();
				else if (focusWidget == _components && !(QApplication::widgetAt(QCursor::pos()) != _compKSelector)) _compKSelector->hide();
				break;
			// Show the selector is the line edit has focus
			case QEvent::Enter:
				if (focusWidget == _radicals && _radicals->hasFocus()) _radKSelector->show();
				else if (focusWidget == _components && _components->hasFocus()) _compKSelector->show();
				break;
			// Hide the selector when leaving for something else than the selector
			case QEvent::Leave:
				if (focusWidget == _radicals && _radicals->hasFocus() && QApplication::widgetAt(QCursor::pos()) != _radKSelector) _radKSelector->hide();
				else if (focusWidget == _components && _components->hasFocus() && QApplication::widgetAt(QCursor::pos()) != _compKSelector) _compKSelector->hide();
				break;
			// If ESC is pressed while a radical/component line edit has focus, hide its selector
			case QEvent::KeyPress:
			{
				int key = static_cast<QKeyEvent *>(event)->key();
				if (key != Qt::Key_Escape) break;
				if (focusWidget == _radicals) _radKSelector->hide();
				else if (focusWidget == _components) _compKSelector->hide();
				break;
			}
			default:
				break;
		}
	}
	else if (watched == _radKSelector || watched == _compKSelector) {
		switch (event->type()) {
			// If the mouse leaves a selector for something else than its associated line edit, hide it
			case QEvent::Leave:
				if (watched == _radKSelector && QApplication::widgetAt(QCursor::pos()) != _radicals) _radKSelector->hide();
				else if (watched == _compKSelector && QApplication::widgetAt(QCursor::pos()) != _components) _compKSelector->hide();
				break;
			default:
				break;
		}
	}
	return SearchFilterWidget::eventFilter(watched, event);
}

void Kanjidic2FilterWidget::onStrokeRangeToggled(bool checked)
{
	// We will need to update the query if we uncheck the strokes checkbox and maxStrokes != minStrokes != 0.
	bool needUpdate = (!checked && _maxStrokeCountSpinBox->value() != 0 && _maxStrokeCountSpinBox->value() != _strokeCountSpinBox->value());
	_maxStrokeCountSpinBox->setVisible(checked);
	_maxStrokeCountSpinBox->setEnabled(checked);
	// Do not allow the command update to be called as the state did not really change
	_maxStrokeCountSpinBox->blockSignals(true);
	_maxStrokeCountSpinBox->setValue(checked ? _strokeCountSpinBox->value() : 0);
	_maxStrokeCountSpinBox->blockSignals(false);
	if (needUpdate) commandUpdate();
}

void Kanjidic2FilterWidget::onStrokeRangeChanged()
{
	if (_maxStrokeCountSpinBox->isEnabled()) {
		int minValue = _strokeCountSpinBox->value();
		int maxValue = _maxStrokeCountSpinBox->value();
		if (sender() == _strokeCountSpinBox && minValue > maxValue) _maxStrokeCountSpinBox->setValue(minValue);
		else if (sender() == _maxStrokeCountSpinBox && maxValue < minValue) _strokeCountSpinBox->setValue(maxValue);
	}
	delayedCommandUpdate();
}

void Kanjidic2FilterWidget::allKyoukuKanjis(bool checked)
{
	bool prevStatus = allJouyou->blockSignals(true);
	allJouyou->setChecked(false);
	allJouyou->blockSignals(prevStatus);
	_gradesList.clear();
	foreach(QAction *action, actionGroup->actions()) {
		int grade = action->property("Agrade").toInt();
		if (grade <= 6) {
			action->setChecked(checked);
			if (checked) _gradesList << QString::number(grade);
		}
		else action->setChecked(false);
	}
	actionGroup->setEnabled(!checked);
	commandUpdate();
}

void Kanjidic2FilterWidget::allJouyouKanjis(bool checked)
{
	bool prevStatus = allKyouku->blockSignals(true);
	allKyouku->setChecked(false);
	allKyouku->blockSignals(prevStatus);
	_gradesList.clear();
	foreach(QAction *action, actionGroup->actions()) {
		int grade = action->property("Agrade").toInt();
		if (grade <= 8) {
			action->setChecked(checked);
			if (checked) _gradesList << QString::number(grade);
		}
		else action->setChecked(false);
	}
	actionGroup->setEnabled(!checked);
	commandUpdate();
}

QString Kanjidic2FilterWidget::currentCommand() const
{
	QString ret;

	int minStrokes = _strokeCountSpinBox->value();
	int maxStrokes = _maxStrokeCountSpinBox->value();
	if ((!_maxStrokeCountSpinBox->isEnabled() || maxStrokes == 0 || maxStrokes == minStrokes) && minStrokes > 0) {
		ret += QString(" :stroke=%1").arg(minStrokes);
	}
	else if (minStrokes != 0 && maxStrokes != 0) {
		ret += QString(" :stroke=%1,%2").arg(minStrokes).arg(maxStrokes);
	}
	QString kanjis = _radicals->text();
	if (!kanjis.isEmpty()) {
		bool first = true;
		ret += " :radical=";
		foreach(QChar c, kanjis) {
			if (!first) ret +=",";
			else first = false;
			ret += QString("\"%1\"").arg(c);
		}
	}
	kanjis = _components->text();
	if (!kanjis.isEmpty()) {
		bool first = true;
		ret += " :component=";
		foreach(QChar c, kanjis) {
			if (!first) ret +=",";
			else first = false;
			ret += QString("\"%1\"").arg(c);
		}
	}
	if (_unicode->value()) ret += QString(" :unicode=%1").arg(_unicode->text());
	if (_skip1->value() || _skip2->value() || _skip3->value()) ret += QString(" :skip=%1").arg(skip());
	if (_fcTopLeft->currentIndex() > 0 || _fcTopRight->currentIndex() > 0 || _fcBotLeft->currentIndex() > 0 || _fcBotRight->currentIndex() > 0 || _fcExtra->currentIndex() > 0) ret += QString(" :fourcorner=%1").arg(fourCorner());
	if (!_gradesList.isEmpty()) ret += " :grade=" + _gradesList.join(",");
	return ret;
}

QString Kanjidic2FilterWidget::currentTitle() const
{
	QString ret;

	QString kanjis = _radicals->text() + _components->text();
	if (!kanjis.isEmpty()) {
		bool first = true;
		ret += "[";
		foreach(QChar c, kanjis) {
			if (!first) ret +=",";
			else first = false;
			ret += c;
		}
		ret += "]";
	}
	int minStrokes = _strokeCountSpinBox->value();
	int maxStrokes = _maxStrokeCountSpinBox->value();
	if ((!_maxStrokeCountSpinBox->isEnabled() || maxStrokes == 0 || maxStrokes == minStrokes) && minStrokes > 0) {
		ret += tr(", %1 strokes").arg(minStrokes);
	} else if (minStrokes != 0 && maxStrokes != 0) {
		if (minStrokes == 0) ret += tr(", strokes<=%1").arg(maxStrokes);
		else if (maxStrokes == 0) ret += tr(", strokes>=%1").arg(minStrokes);
		else ret += tr(", %1-%2 strokes").arg(minStrokes).arg(maxStrokes);
	}
	if (_unicode->value()) ret += tr(", unicode: %1").arg(_unicode->text());
	if (_skip1->value() || _skip2->value() || _skip3->value()) ret += tr(", skip: %1").arg(skip());
	if (_fcTopLeft->currentIndex() > 0 || _fcTopRight->currentIndex() > 0 || _fcBotLeft->currentIndex() > 0 || _fcBotRight->currentIndex() > 0 || _fcExtra->currentIndex() > 0) ret += tr(", 4c: %1").arg(fourCorner());
	if (!_gradesList.isEmpty()) ret += tr(", grade: %1").arg(_gradesList.join(","));
	if (!ret.isEmpty()) ret = tr("Kanji") + ret;
	else ret = tr("Kanji");
	return ret;
}

void Kanjidic2FilterWidget::onGradeTriggered(QAction *action)
{
	if (action->isChecked()) {
		int grade = action->property("Agrade").toInt();
		_gradesList << QString::number(grade);
	} else {
		int grade = action->property("Agrade").toInt();
		_gradesList.removeOne(QString::number(grade));
	}
	if (!_gradesList.isEmpty()) _gradeButton->setText(tr("Grade:") + _gradesList.join(","));
	else _gradeButton->setText(tr("Grade"));
	commandUpdate();
}

void Kanjidic2FilterWidget::updateFeatures()
{
	if (_strokeCountSpinBox->value() || _maxStrokeCountSpinBox->value() || !_radicals->text().isEmpty() || !_components->text().isEmpty() || !_gradesList.isEmpty() || _unicode->value() ||_skip1->value() || _skip2->value() || _skip3->value() || _fcTopLeft->currentIndex() != 0 || _fcTopRight->currentIndex() != 0 || _fcBotLeft->currentIndex() != 0 || _fcBotRight->currentIndex() != 0 || _fcExtra->currentIndex() != 0) emit disableFeature("wordsdic");
	else emit enableFeature("wordsdic");
}

void Kanjidic2FilterWidget::_reset()
{
	_strokeCountSpinBox->setValue(0);
	_maxStrokeCountSpinBox->setValue(0);
	_rangeCheckBox->setChecked(false);
	_unicode->setValue(0);
	_skip1->setValue(0);
	_skip2->setValue(0);
	_skip3->setValue(0);
	_fcTopLeft->setCurrentIndex(0);
	_fcTopRight->setCurrentIndex(0);
	_fcBotLeft->setCurrentIndex(0);
	_fcBotRight->setCurrentIndex(0);
	_fcExtra->setCurrentIndex(0);
	_gradesList.clear();
	foreach (QAction *action, _gradeButton->menu()->actions()) if (action->isChecked()) action->trigger();
	_radicals->clear();
	_components->clear();
}

void Kanjidic2FilterWidget::setGrades(const QStringList &list)
{
	_gradesList.clear();
	foreach(QAction *action, _gradeButton->menu()->actions()) {
		if (action->isChecked()) action->trigger();
		if (list.contains(action->property("Agrade").toString())) action->trigger();
	}
}

QString Kanjidic2FilterWidget::skip() const
{
	return QString("%1-%2-%3").arg(_skip1->value() ? QString::number(_skip1->value()) : "?").arg(_skip2->value() ? QString::number(_skip2->value()) : "?").arg(_skip3->value() ? QString::number(_skip3->value()) : "?");
}

void Kanjidic2FilterWidget::setSkip(const QString &value)
{
	QStringList skipList(value.split('-'));
	if (skipList.size() != 3) return;
	bool ok;
	int t, c1, c2;
	t = skipList[0].toInt(&ok); if (!ok && skipList[0] != "?") return;
	c1 = skipList[1].toInt(&ok); if (!ok && skipList[1] != "?") return;
	c2 = skipList[2].toInt(&ok); if (!ok && skipList[2] != "?") return;
	_skip1->setValue(t);
	_skip2->setValue(c1);
	_skip3->setValue(c2);
}

QString Kanjidic2FilterWidget::fourCorner() const
{
	return QString("%1%2%3%4.%5").arg(_fcTopLeft->currentIndex() > 0 ? QString::number(_fcTopLeft->currentIndex() - 1) : "?")
		.arg(_fcTopRight->currentIndex() > 0 ? QString::number(_fcTopRight->currentIndex() - 1) : "?")
		.arg(_fcBotLeft->currentIndex() > 0 ? QString::number(_fcBotLeft->currentIndex() - 1) : "?")
		.arg(_fcBotRight->currentIndex() > 0 ? QString::number(_fcBotRight->currentIndex() - 1) : "?")
		.arg(_fcExtra->currentIndex() > 0 ? QString::number(_fcExtra->currentIndex() - 1) : "?");
}

void Kanjidic2FilterWidget::setFourCorner(const QString &value)
{
	// Sanity check
	if (value.size() != 6 || value[4] != '.') return;
	
	int topLeft, topRight, botLeft, botRight, extra;
	topLeft = value[0].isDigit() ? value[0].toAscii() - '0' : -1;
	topRight = value[1].isDigit() ? value[1].toAscii() - '0' : -1;
	botLeft = value[2].isDigit() ? value[2].toAscii() - '0' : -1;
	botRight = value[3].isDigit() ? value[3].toAscii() - '0' : -1;
	extra = value[5].isDigit() ? value[5].toAscii() - '0' : -1;
	_fcTopLeft->setCurrentIndex(topLeft + 1);
	_fcTopRight->setCurrentIndex(topRight + 1);
	_fcBotLeft->setCurrentIndex(botLeft + 1);
	_fcBotRight->setCurrentIndex(botRight + 1);
	_fcExtra->setCurrentIndex(extra + 1);
}
