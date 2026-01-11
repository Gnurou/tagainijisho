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

#ifndef __GUI_KANJIRENDERING_H
#define __GUI_KANJIRENDERING_H

#include <QList>
#include <QMap>
#include <QPainter>
#include <QPainterPath>
#include <QPicture>
#include <QWidget>

#include "core/EntriesCache.h"
#include "core/kanjidic2/Kanjidic2Entry.h"

static const float KANJI_AREA_WIDTH = 109.0;
static const float KANJI_AREA_HEIGHT = 109.0;

/**
 * A class that is able to render a kanji of part of if using a QPainter.
 * TODO either remove and only keep the QPainterPath, or make it inherit
 * QPainterPath and add methods to partially render the stroke.
 */
class KanjiRenderer {
  public:
    class Stroke {
      private:
        const KanjiStroke *_stroke;
        QPainterPath _painterPath;

        QPainterPath pathFromSVG(QString svgPath);

      public:
        Stroke();
        Stroke(const KanjiStroke *const stroke);
        const KanjiStroke *stroke() const { return _stroke; }
        const QPainterPath &painterPath() const { return _painterPath; }
        qreal length() const { return _painterPath.length(); }

        /**
         * Render a part of the stroke (if length >= 0) or the complete
         * stroke (if length < 0).
         */
        void render(QPainter *painter, qreal length = -1.0, qreal startFrom = 0.0) const;

        bool operator==(const Stroke &s) const { return this == &s; }

        // KanjiRenderer needs to center the strokes with respect to the grid
        // (for kana characters)
        friend class KanjiRenderer;
    };

  private:
    ConstKanjidic2EntryPointer _kanji;
    // Keep all the strokes in order
    QList<Stroke> _strokes;
    // Associates the kanji strokes with their path
    QMap<const KanjiStroke *, Stroke *> _strokesMap;

  public:
    KanjiRenderer();
    KanjiRenderer(ConstKanjidic2EntryPointer kanji);
    void setKanji(ConstKanjidic2EntryPointer kanji);

    const ConstKanjidic2EntryPointer &kanji() { return _kanji; }

    /**
     * Render all the strokes of the kanji.
     */
    void renderStrokes(QPainter *painter);
    /**
     * Render all the strokes of a given component of the kanji.
     */
    void renderComponentStrokes(const KanjiComponent &component, QPainter *painter);
    /**
     * Render a single stroke of the kanji.
     */
    void renderStroke(const KanjiStroke &stroke, QPainter *painter);
    /**
     * Render the stroke number.
     */
    void renderStrokeNumber(const KanjiStroke &stroke, QPainter *painter, qreal baseSize = 4);
    /**
     * Render the kanji grid.
     */
    void renderGrid(QPainter *painter);
    /**
     * Returns the total length of a stroke, useful for partial drawing. 0 is
     * returned if the path is unknown to this renderer.
     */
    qreal strokeLength(const KanjiStroke &stroke) const;

    const QList<Stroke> &strokes() const { return _strokes; }
    const Stroke *strokeFor(const KanjiStroke &stroke) const;
};

#endif
