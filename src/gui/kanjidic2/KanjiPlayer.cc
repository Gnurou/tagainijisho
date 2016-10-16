/*
 *  Copyright (C) 2008/2009  Alexandre Courbot
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
#include "gui/kanjidic2/KanjiPlayer.h"

#include <QDebug>

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QToolButton>
#include <QPainterPathStroker>

#define TIMER_INTERVAL 20

#define STATE_STROKE 1
#define STATE_WAIT 2

#define CONTROL_ICON_SIZE 10

PreferenceItem<int> KanjiPlayer::animationSpeed("kanjidic", "animationSpeed", 30);
PreferenceItem<int> KanjiPlayer::delayBetweenStrokes("kanjidic", "delayBetweenStrokes", 10);
PreferenceItem<int> KanjiPlayer::animationLoopDelay("kanjidic", "animationLoopDelay", -1);
PreferenceItem<bool> KanjiPlayer::showGridPref("kanjidic", "showStrokesGrid", true);
PreferenceItem<bool> KanjiPlayer::showStrokesNumbersPref("kanjidic", "showStrokesNumbers", false);
PreferenceItem<int> KanjiPlayer::strokesNumbersSizePref("kanjidic", "strokesNumbersSize", 4);

static QList<QColor> colList(QList<QColor>() << Qt::black << QColor(0x0d, 0x5b, 0xa6) << QColor(0xce, 0x34, 0x34) << QColor(0x04, 0x9a,0x40) << QColor(0xe6, 0xa6, 0x00) << QColor(0xd2, 0x7d, 0x8e) << Qt::blue << Qt::red << Qt::green << Qt::cyan << Qt::magenta << Qt::yellow);

KanjiPlayer::KanjiPlayer(QWidget *parent) : QWidget(parent), _timer(), _kanji(0), renderer(), _picture(), _state(STATE_STROKE), _showGrid(showGridPref.value()), _showStrokesNumbers(showStrokesNumbersPref.value()), _strokesNumbersSize(strokesNumbersSizePref.value()), _highlightedComponent(0)
{
	setAnimationSpeed(animationSpeed.value());
	setDelayBetweenStrokes(delayBetweenStrokes.value());
	setAnimationLoopDelay(animationLoopDelay.value());
	
	_playAction = new QAction(QIcon(QPixmap(":/images/icons/control-play.png").scaledToHeight(CONTROL_ICON_SIZE, Qt::SmoothTransformation)), tr("Play"), this);
	_playAction->setShortcut(QKeySequence("Space"));
	connect(_playAction, SIGNAL(triggered()), this, SLOT(play()));
	_pauseAction = new QAction(QIcon(QPixmap(":/images/icons/control-pause.png").scaledToHeight(CONTROL_ICON_SIZE, Qt::SmoothTransformation)), tr("Pause"), this);
	_pauseAction->setShortcut(QKeySequence("Space"));
	connect(_pauseAction, SIGNAL(triggered()), this, SLOT(stop()));
	_gotoEndAction = new QAction(QIcon(QPixmap(":/images/icons/control-stop.png").scaledToHeight(CONTROL_ICON_SIZE, Qt::SmoothTransformation)), tr("Stop"), this);
	_gotoEndAction->setShortcut(QKeySequence("R"));
	connect(_gotoEndAction, SIGNAL(triggered()), this, SLOT(gotoEnd()));
	_nextStrokeAction = new QAction(QIcon(QPixmap(":/images/icons/control-ff.png").scaledToHeight(CONTROL_ICON_SIZE, Qt::SmoothTransformation)), tr("Next stroke"), this);
	_nextStrokeAction->setShortcut(QKeySequence("Right"));
	connect(_nextStrokeAction, SIGNAL(triggered()), this, SLOT(nextStroke()));
	_prevStrokeAction = new QAction(QIcon(QPixmap(":/images/icons/control-fr.png").scaledToHeight(CONTROL_ICON_SIZE, Qt::SmoothTransformation)), tr("Previous stroke"), this);
	_prevStrokeAction->setShortcut(QKeySequence("Left"));
	connect(_prevStrokeAction, SIGNAL(triggered()), this, SLOT(prevStroke()));
	
	_playAction->setEnabled(false);
	_pauseAction->setEnabled(false);
	_gotoEndAction->setEnabled(false);
	_prevStrokeAction->setEnabled(false);
	_nextStrokeAction->setEnabled(false);
	
	kanjiView = new QLabel(this);
	kanjiView->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	kanjiView->setFocusPolicy(Qt::NoFocus);
	kanjiView->setPicture(_picture);
	kanjiView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	kanjiView->setMouseTracking(true);
	kanjiView->installEventFilter(this);
	kanjiView->setAttribute(Qt::WA_Hover);

	strokeCountLabel = new QLabel(this);
	QFont font;
	font.setPointSize(font.pointSize() - 2);
	strokeCountLabel->setFont(font);
	strokeCountLabel->setAlignment(Qt::AlignHCenter);
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(kanjiView, 0, Qt::AlignCenter);
	QHBoxLayout *controlLayout = new QHBoxLayout();
	controlLayout->setContentsMargins(0, 0, 0, 0);
	controlLayout->setSpacing(0);
	
	playButton = new QToolButton(this);
	playButton->setMaximumSize(20, 20);
	playButton->setDefaultAction(playAction());
	controlLayout->addWidget(playButton);
	resetButton = new QToolButton(this);
	resetButton->setMaximumSize(20, 20);
	resetButton->setDefaultAction(resetAction());
	controlLayout->addWidget(resetButton);
	controlLayout->addWidget(strokeCountLabel);
	prevButton = new QToolButton(this);
	prevButton->setMaximumSize(20, 20);
	prevButton->setDefaultAction(prevStrokeAction());
	controlLayout->addWidget(prevButton);
	nextButton = new QToolButton(this);
	nextButton->setMaximumSize(20, 20);
	nextButton->setDefaultAction(nextStrokeAction());
	controlLayout->addWidget(nextButton);
	mainLayout->addLayout(controlLayout);

	/*connect(playButton, SIGNAL(clicked()), this, SLOT(onPlayButtonPushed()));
	connect(resetButton, SIGNAL(clicked()), this, SLOT(gotoEnd()));
	connect(prevButton, SIGNAL(clicked()), this, SLOT(prevStroke()));
	connect(nextButton, SIGNAL(clicked()), this, SLOT(nextStroke()));*/
	

	QFontMetrics metrics(font);
	strokeCountLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	// Determine the largest size that our label can take.
	int maxWidth(0);
	for (char c = '0'; c <= '9';c++) maxWidth = qMax(metrics.width(c), maxWidth);
	strokeCountLabel->setMinimumWidth(metrics.width("/") + maxWidth * 4 + 2);

	setPictureSize(100);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	_timer.setInterval(TIMER_INTERVAL);
	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateAnimationState()));
}

void KanjiPlayer::setKanji(const ConstKanjidic2EntryPointer &entry)
{
	_kanji = entry;
	_highlightedComponent = 0;
	strokeCountLabel->setText("");
	playButton->setEnabled(true);
	renderer.setKanji(_kanji);
	reset();
	renderCurrentState();
	updateStrokesCountLabel();
	update();
}

void KanjiPlayer::setPictureSize(int newSize)
{
	_pictureSize = newSize;
	_picture.setBoundingRect(QRect(0, 0, newSize, newSize));
	kanjiView->clear();
	kanjiView->setPicture(_picture);
	kanjiView->setMinimumSize(newSize, newSize);
}

void KanjiPlayer::updateActionsState()
{
	// Update play button action
	if (_timer.isActive()) { playButton->removeAction(playAction()); playButton->setDefaultAction(pauseAction()); }
	else { playButton->removeAction(pauseAction()); playButton->setDefaultAction(playAction()); }
	_playAction->setEnabled(!_timer.isActive());
	_pauseAction->setEnabled(_timer.isActive());
	_gotoEndAction->setEnabled(_strokesCpt < renderer.strokes().size());
	_prevStrokeAction->setEnabled(_strokesCpt > 0);
	_nextStrokeAction->setEnabled(_strokesCpt < renderer.strokes().size());
}

void KanjiPlayer::updateAnimationState()
{
	if (_state == STATE_WAIT) {
		if (++_lengthCpt >= _delayBetweenStrokes) {
			_lengthCpt = 0;
			_state = STATE_STROKE;
		}
	}
	else {
		// End of animation
		if (_strokesCpt >= renderer.strokes().size()) {
			stop();
			// Trigger a loop if needed
			if (_animationLoopDelay != -1) {
				QTimer::singleShot(_animationLoopDelay * 1000, this, SLOT(play()));
			}
			return;
		}
		const KanjiRenderer::Stroke &currentStroke(renderer.strokes()[_strokesCpt]);
		bool updateLabel = _lengthCpt == 0;
		_lengthCpt += _animationSpeed;
		if (updateLabel) updateStrokesCountLabel();
		if (_lengthCpt > currentStroke.length()) {
			nextStroke();
			_state = STATE_WAIT;
		}
		update();
	}
}

void KanjiPlayer::highlightComponent(const KanjiComponent *component)
{
	if (component == _highlightedComponent) return;
	_highlightedComponent = component;
	update();
}

void KanjiPlayer::unHighlightComponent()
{
	highlightComponent(0);
}

void KanjiPlayer::renderCurrentState()
{
	static const int strokes_size = 4;
	static const int outline_size = strokes_size + 2;
	static const Qt::PenCapStyle strokeCapStyle = Qt::RoundCap;

	QPainter painter(&_picture);
	painter.scale(pictureSize() / KANJI_AREA_WIDTH, pictureSize() / KANJI_AREA_HEIGHT);
	painter.setRenderHint(QPainter::Antialiasing);

	// Render the grid, if relevant
	if (showGrid()) {
		QPen gridPen;
		gridPen.setWidth(strokes_size / 2);
		gridPen.setColor(palette().color(QPalette::Mid));
		painter.setPen(gridPen);

		renderer.renderGrid(&painter);
	}
	
	// Render the outline
	QPen outLinePen;
	outLinePen.setColor(palette().color(QPalette::WindowText));
	outLinePen.setCapStyle(strokeCapStyle);
	outLinePen.setWidth(outline_size);
	painter.setPen(outLinePen);
	renderer.renderStrokes(&painter);
	if (highlightedComponent()) {
		outLinePen.setColor(palette().color(QPalette::Highlight));
		outLinePen.setWidth(outline_size + 1);
		painter.setPen(outLinePen);
		renderer.renderComponentStrokes(*highlightedComponent(), &painter);
	}
	outLinePen.setWidth(strokes_size);
	outLinePen.setColor(palette().color(QPalette::Window));
	painter.setPen(outLinePen);
	renderer.renderStrokes(&painter);

	// Render the strokes
	QPen strokesPen;
	strokesPen.setWidth(strokes_size);
	strokesPen.setCapStyle(strokeCapStyle);
	painter.setPen(strokesPen);

	const QList<const KanjiComponent *> &kComponents(_kanji->rootComponents());
	const QList<KanjiStroke> &kStrokes(_kanji->strokes());

	if (renderer.strokes().isEmpty()) return;

	static const uchar HIGHLIGHT_RATIO = 135;
	// Render full strokes
	for (int i = 0; i < _strokesCpt; i++) {
		const KanjiComponent *parent(0);
		if (highlightedComponent() && highlightedComponent()->strokes().contains(&kStrokes[i])) { parent = highlightedComponent(); }
		else foreach (const KanjiComponent *comp, kComponents) if (comp->strokes().contains(&kStrokes[i])) { parent = comp; break; }
		if (!parent) strokesPen.setColor(colList[0]);
		else strokesPen.setColor(colList[kComponents.indexOf(parent) + 1]);
		if (highlightedComponent() && parent == highlightedComponent()) strokesPen.setColor(strokesPen.color().lighter(HIGHLIGHT_RATIO));
		painter.setPen(strokesPen);
		renderer.strokes()[i].render(&painter);
	}
	// Render partial stroke
	if (_state == STATE_STROKE && _strokesCpt < renderer.strokes().size()) {
		const KanjiRenderer::Stroke &currentStroke(renderer.strokes()[_strokesCpt]);
		const KanjiComponent *parent(0);
		if (highlightedComponent() && highlightedComponent()->strokes().contains(currentStroke.stroke())) { parent = highlightedComponent(); }
		else foreach (const KanjiComponent *comp, kComponents) if (comp->strokes().contains(currentStroke.stroke())) { parent = comp; break; }
		if (!parent) strokesPen.setColor(colList[0]);
		else strokesPen.setColor(colList[kComponents.indexOf(parent) + 1]);
		if (highlightedComponent() && parent == highlightedComponent()) strokesPen.setColor(strokesPen.color().lighter(HIGHLIGHT_RATIO));
		painter.setPen(strokesPen);
		currentStroke.render(&painter, _lengthCpt);
	}

	// Render stroke numbers
	if (showStrokesNumbers()) {
		int strokesMax = _strokesCpt + (_state == STATE_STROKE && _strokesCpt < renderer.strokes().size() ? 1 : 0);
		for (int i = 0; i < strokesMax; i++) {
			renderer.renderStrokeNumber(kStrokes[i], &painter, strokesNumbersSize());
		}
	}

	painter.end();
}

void KanjiPlayer::paintEvent(QPaintEvent * event)
{
	renderCurrentState();
	QWidget::paintEvent(event);
}

bool KanjiPlayer::eventFilter(QObject *obj, QEvent *event)
{
	if (obj != kanjiView) return false;
	if (event->type() == QEvent::MouseMove) {
		QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
		// First translate the coordinates to the strokes coordinates system
		QPointF pos(mEvent->localPos());
		QPointF fPos(pos.x() * (KANJI_AREA_WIDTH / pictureSize()), pos.y() * (KANJI_AREA_HEIGHT / pictureSize()));
		const QList<KanjiRenderer::Stroke> &strokes(renderer.strokes());
		QPainterPathStroker stroker;
		stroker.setWidth(20);
		
		const KanjiComponent *comp(0);
		const QList<const KanjiComponent *> &kComponents(_kanji->rootComponents());
		foreach (const KanjiRenderer::Stroke &stroke, strokes) {
			QPainterPath nPath(stroker.createStroke(stroke.painterPath()));
			if (nPath.contains(fPos)) {
				foreach (const KanjiComponent *component, kComponents) if (component->strokes().contains(stroke.stroke())) { comp = component; break; }
				if (comp) break;
			}
		}
		if (highlightedComponent() != comp) {
			emit componentUnHighlighted(highlightedComponent());
			qobject_cast<QWidget *>(obj)->unsetCursor();
		}
		if (comp) {
			highlightComponent(comp);
			emit componentHighlighted(comp);
			qobject_cast<QWidget *>(obj)->setCursor(QCursor(Qt::PointingHandCursor));
		}
	}
	if (event->type() == QEvent::MouseButtonPress && _highlightedComponent) {
		QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
		if (mEvent->button() == Qt::LeftButton) {
			emit componentClicked(_highlightedComponent);
			qobject_cast<QWidget *>(obj)->unsetCursor();
		}
	}
	else if (event->type() == QEvent::HoverLeave) {
		if (highlightedComponent()) emit componentUnHighlighted(highlightedComponent());
		unHighlightComponent();
		qobject_cast<QWidget *>(obj)->unsetCursor();
	}
	return false;
}

void KanjiPlayer::setPosition(int strokeNbr)
{
	if (strokeNbr > renderer.strokes().size()) strokeNbr = renderer.strokes().size();
	_strokesCpt = strokeNbr;
	_lengthCpt = 0.0;
	_state = STATE_STROKE;
	updateActionsState();
	updateStrokesCountLabel();
	update();
}

void KanjiPlayer::play()
{
	if (_timer.isActive()) return;
	if (_strokesCpt >= renderer.strokes().size()) reset();
	_timer.start();
	updateActionsState();
	emit animationStarted();
}

void KanjiPlayer::stop()
{
	if (!_timer.isActive()) return;
	_timer.stop();
	updateActionsState();
	emit animationStopped();
}

void KanjiPlayer::reset()
{
	stop();
	setPosition(0);
	// Reset the picture
	QPainter painter;
	painter.begin(&_picture);
	painter.end();
	emit animationReset();
}

void KanjiPlayer::nextStroke()
{
	if (_strokesCpt >= renderer.strokes().size()) return;
	setPosition(_strokesCpt + 1);
}

void KanjiPlayer::prevStroke()
{
	if (_strokesCpt <= 0) return;
	setPosition(_strokesCpt - 1);
}

void KanjiPlayer::onPlayButtonPushed()
{
	if (_timer.isActive()) stop();
	else play();
}

void KanjiPlayer::gotoEnd()
{
	stop();
	setPosition(renderer.strokes().size());
}

void KanjiPlayer::updateStrokesCountLabel()
{
	strokeCountLabel->setText(QString("%1/%2").arg(_strokesCpt + (_lengthCpt ? 1 : 0)).arg(renderer.strokes().size()));
}
