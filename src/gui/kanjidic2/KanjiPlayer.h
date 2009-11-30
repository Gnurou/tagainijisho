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

#ifndef __GUI_KANJI_PLAYER_H
#define __GUI_KANJI_PLAYER_H

#include <QWidget>
#include <QLabel>
#include <QPainterPath>
#include <QPainter>
#include <QTimer>
#include <QPixmap>
#include <QList>
#include <QPushButton>

#include "core/kanjidic2/Kanjidic2Entry.h"
#include "gui/kanjidic2/KanjiRenderer.h"

class KanjiPlayer : public QWidget {
	Q_OBJECT
private:
	QTimer _timer;
	// To keep the kanji we display in memory...
	EntryPointer<const Entry> _entry;
	// ... and avoid casting it all the time.
	const Kanjidic2Entry *_kanji;

	QLabel *strokeCountLabel;
	KanjiRenderer renderer;
	// Off-screen rendering of the kanji
	QPicture _picture;
	// Actual display of the kanji
	QLabel *kanjiView;
	int _pictureSize;
	qreal _animationSpeed;
	int _delayBetweenStrokes;
	int _strokesCpt;
	int _state;
	qreal _lengthCpt;
	const KanjiComponent *_highlightedComponent;
	QPushButton *playButton;
	QPushButton *resetButton;
	QPushButton *nextButton;
	QPushButton *prevButton;

protected:
	/**
	 * Render the state of the animation into _picture
	 */
	void renderCurrentState();
	virtual void paintEvent(QPaintEvent * event);
	virtual bool eventFilter(QObject *obj, QEvent *event);
	void updateStrokesCountLabel();
	void updateButtonsState();

protected slots:
	virtual void updateAnimationState();
	void playButtonClicked();
	void resetButtonClicked();

public:
	KanjiPlayer(QWidget *parent = 0);
	const KanjiComponent *highlightedComponent() const { return _highlightedComponent; }
	int pictureSize() const { return _pictureSize; }
	void setPictureSize(int newSize);

	static PreferenceItem<int> animationSpeed;
	static PreferenceItem<int> delayBetweenStrokes;

public slots:
	void setKanji(const Kanjidic2Entry *entry);
	void setPosition(int strokeNbr);

	void setAnimationSpeed(int speed) { _animationSpeed = speed / 10.0; }
	void setDelayBetweenStrokes(int delay) { _delayBetweenStrokes = delay; }

	void highlightComponent(const KanjiComponent *component);
	void unHighlightComponent();

	void play();
	void stop();
	void reset();
	void nextStroke();
	void prevStroke();

signals:
	void animationStarted();
	void animationStopped();
	void animationReset();
	void componentHighlighted(const KanjiComponent *component);
	void componentUnHighlighted(const KanjiComponent *component);
	void componentClicked(const KanjiComponent *component);

	void strokeStarted(int number);
	void strokeCompleted(int number);
};

#endif
