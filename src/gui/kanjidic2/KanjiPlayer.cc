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
#include "gui/kanjidic2/KanjiPlayer.h"

#include <QDebug>

#include <QVBoxLayout>
#include <QMouseEvent>

/*
#ifdef __MINGW32__
#define PAUSE_BETWEEN_STROKES 30
#define ANIMATION_SPEED 1
#define TIMER_INTERVAL 10
#else
#define PAUSE_BETWEEN_STROKES 15
#define ANIMATION_SPEED 2
*/
#define TIMER_INTERVAL 20
/*#endif*/

#define STATE_STROKE 0
#define STATE_WAIT 1

PreferenceItem<int> KanjiPlayer::animationSpeed("kanjidic", "animationSpeed", 30);
PreferenceItem<int> KanjiPlayer::delayBetweenStrokes("kanjidic", "delayBetweenStrokes", 10);

KanjiPlayer::KanjiPlayer(int size, QWidget *parent) : QWidget(parent), _timer(), _kanji(0), renderer(), _picture(), _state(STATE_STROKE), _highlightedComponent(0)
{
	setAnimationSpeed(animationSpeed.value());
	setDelayBetweenStrokes(delayBetweenStrokes.value());

	QFrame *kanjiFrame = new QFrame(this);
	{
		kanjiFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
		QVBoxLayout *frameLayout = new QVBoxLayout(kanjiFrame);
		kanjiView = new QLabel(kanjiFrame);
		kanjiView->setPicture(_picture);
		kanjiView->setMouseTracking(true);
		kanjiView->installEventFilter(this);
		kanjiView->setAttribute(Qt::WA_Hover);
		frameLayout->addWidget(kanjiView);
	}

	strokeCountLabel = new QLabel(this);
	QFont font;
	font.setPointSize(font.pointSize() - 2);
	strokeCountLabel->setFont(font);
	strokeCountLabel->setAlignment(Qt::AlignHCenter);
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(kanjiFrame);
	mainLayout->addWidget(strokeCountLabel);

	setPictureSize(size);

	_timer.setInterval(20);
	connect(&_timer, SIGNAL(timeout()), this, SLOT(updateAnimation()));
}

void KanjiPlayer::setKanji(const Kanjidic2Entry *kanji)
{
	_entry = kanji;
	_kanji = kanji;
	_highlightedComponent = 0;
	strokeCountLabel->setText("");
	renderer.setKanji(kanji);
	resetAnimation();
	renderCurrentState();
	update();
}

void KanjiPlayer::setPictureSize(int newSize)
{
	_pictureSize = newSize;
	_picture.setBoundingRect(QRect(0, 0, newSize, newSize));
	kanjiView->clear();
	kanjiView->setPicture(_picture);
}

void KanjiPlayer::updateAnimation()
{
	if (_state == STATE_WAIT) {
		if (++_lengthCpt >= _delayBetweenStrokes) {
			_lengthCpt = 0;
			_state = STATE_STROKE;
		}
		return;
	}
	// If we arrive here, we are in STATE_STROKE
	// Is the animation over already?
	if (_strokesCpt >= renderer.strokes().size()) {
		stopAnimation();
		return;
	}

	if (_lengthCpt == 0.0) strokeCountLabel->setText(tr("stroke %1/%2").arg(_strokesCpt + 1).arg(renderer.strokes().size()));

	const KanjiRenderer::Stroke &currentStroke(renderer.strokes()[_strokesCpt]);
	// First update the state of the animation
	_lengthCpt += _animationSpeed;
	if (_lengthCpt > currentStroke.length()) {
		_strokesCpt++;
		_lengthCpt = 0.0;
		_state = STATE_WAIT;
	}
	update();
}

void KanjiPlayer::highlightComponent(const KanjiComponent *component)
{
	if (component == _highlightedComponent) return;
	_highlightedComponent = component;
	// Only schedule a redraw if the animation is stopped
	if (!_timer.isActive()) update();
}

void KanjiPlayer::unHighlightComponent()
{
	highlightComponent(0);
}

void KanjiPlayer::renderCurrentState()
{
	QPen pen;
	pen.setWidth(2);
	pen.setColor(palette().color(QPalette::Dark));
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	QPen pen2;
	pen2.setWidth(4);
	pen2.setCapStyle(Qt::RoundCap);
	pen2.setJoinStyle(Qt::RoundJoin);

	QPainter painter(&_picture);
	painter.scale(pictureSize() / 109.0, pictureSize() / 109.0);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(pen);
	renderer.renderOutline(&painter);
	painter.setPen(pen2);

	const QList<const KanjiComponent *> &kComponents(_kanji->rootComponents());
	const QList<KanjiStroke> &kStrokes(_kanji->strokes());
	QList<QColor> colList;
	colList << Qt::black << Qt::darkBlue << Qt::darkRed << Qt::darkGreen << Qt::darkCyan << Qt::darkMagenta << Qt::darkYellow << Qt::blue << Qt::red << Qt::green << Qt::cyan << Qt::magenta << Qt::yellow;

	// No highlighted component, render the animation
	if (!highlightedComponent()) {
		// Render full strokes
		for (int i = 0; i < _strokesCpt; i++) {
			const KanjiComponent *parent(kStrokes[i].parent());
			while (parent && !kComponents.contains(parent)) parent = parent->parent();
			if (!parent) pen2.setColor(colList[0]);
			else pen2.setColor(colList[kComponents.indexOf(parent) + 1]);
			painter.setPen(pen2);
			renderer.strokes()[i].render(&painter);
		}
		// Render partial stroke
		if (_strokesCpt < renderer.strokes().size()) {
			const KanjiRenderer::Stroke &currentStroke(renderer.strokes()[_strokesCpt]);
			const KanjiComponent *parent(currentStroke.stroke()->parent());
			while (parent && !kComponents.contains(parent)) parent = parent->parent();
			if (!parent) pen2.setColor(colList[0]);
			else pen2.setColor(colList[kComponents.indexOf(parent) + 1]);
			painter.setPen(pen2);
			currentStroke.render(&painter, _lengthCpt);
		}
	}
	// Render the highlighted component only
	else {
		pen2.setColor(colList[kComponents.indexOf(highlightedComponent()) + 1].lighter(200));
		pen2.setWidth(pen2.width() + 1);
		painter.setPen(pen2);
		renderer.renderComponentStrokes(*highlightedComponent(), &painter);
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
		QPointF pos(mEvent->posF());
		QPointF fPos(pos.x() * (109.0 / pictureSize()), pos.y() * (109.0 / pictureSize()));
		const QList<KanjiRenderer::Stroke> &strokes(renderer.strokes());
		QPainterPathStroker stroker;
		stroker.setWidth(20);
		const KanjiComponent *comp(0);
		foreach (const KanjiRenderer::Stroke &stroke, strokes) {
			QPainterPath nPath(stroker.createStroke(stroke.painterPath()));
			if (nPath.contains(fPos)) {
				const KanjiComponent *parent(stroke.stroke()->parent());
				// Do not consider the root component
				if (!parent->parent()) continue;
				while (parent->parent() != _kanji->root()) parent = parent->parent();
				comp = parent;
				break;
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

void KanjiPlayer::startAnimation()
{
	if (_timer.isActive()) return;
	_timer.start();
	emit animationStarted();
}

void KanjiPlayer::stopAnimation()
{
	if (!_timer.isActive()) return;
	_timer.stop();
	emit animationStopped();
}

void KanjiPlayer::resetAnimation()
{
	stopAnimation();
	_strokesCpt = 0;
	_lengthCpt = 0.0;
	_state = STATE_STROKE;
	// Reset the picture
	QPainter painter;
	painter.begin(&_picture);
	painter.end();
	emit animationReset();
}

void KanjiPlayer::setPosition(int strokeNbr)
{
	_strokesCpt = strokeNbr;
	_lengthCpt = 0.0;
	_state = STATE_STROKE;
}
