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

#include "KanjiRenderer.h"

#include <QDebug>

#include <QPainter>
#include <QPainterPathStroker>

KanjiRenderer::Stroke::Stroke()
{
}

KanjiRenderer::Stroke::Stroke(const KanjiStroke *const stroke) : _stroke(stroke), _painterPath(pathFromSVG(stroke->path()))
{
}

QPainterPath KanjiRenderer::Stroke::pathFromSVG(QString svgPath)
{
	enum SVGPathCommand { None = 0, Movepath, movepath, Closepath, Lineto, lineto, HLineto, Hlineto, VLineto, vLineto, Curveto, curveto, sCurveto, scurveto, Ellipse };
	QPainterPath retPath;

	// Process the string - add spaces between unseparated tokens
	int idx;
	while ((idx = svgPath.indexOf(QRegExp("[a-zA-Z]\\d|\\d[a-zA-Z]|\\w-"))) != -1) svgPath.insert(idx + 1, ' ');

	QStringList tokens = svgPath.split(QRegExp(" +|,"), QString::SkipEmptyParts);

	SVGPathCommand curAction = None;
	QPointF dest;
	QPointF p1;
	QPointF p2;
	QPointF prevP2;
	for (int tokenPos = 0; tokenPos < tokens.count(); tokenPos++) {
		// Check if a new action is available
		bool gotCommand = true;
		do {
			QString token = tokens[tokenPos];
			if (token == "M") curAction = Movepath;
			else if (token == "m") curAction = movepath;
			else if (token == "L") curAction = Lineto;
			else if (token == "l") curAction = lineto;
			else if (token == "C") curAction = Curveto;
			else if (token == "c") curAction = curveto;
			else if (token == "S") curAction = sCurveto;
			else if (token == "s") curAction = scurveto;
			else if (token == "z" || token == "Z") curAction = Closepath;
			else gotCommand = false;
			if (gotCommand) { tokenPos++; if (tokenPos >= tokens.count()) break; }
		} while (gotCommand);
		// Now execute the action
		switch (curAction) {
			case None:
				qWarning("Invalid kanji drawing path!");
				break;
			case Movepath:
			case movepath:
				dest = QPointF(tokens[tokenPos].toFloat(), tokens[tokenPos + 1].toFloat());
				if (curAction == movepath) dest += retPath.currentPosition();
				retPath.moveTo(dest);
				tokenPos++;
				break;
			case Lineto:
			case lineto:
				dest = QPointF(tokens[tokenPos].toFloat(), tokens[tokenPos + 1].toFloat());
				if (curAction == lineto) dest += retPath.currentPosition();
				retPath.lineTo(dest);
				tokenPos++;
				break;
			case Curveto:
			case curveto:
				p1 = QPointF(tokens[tokenPos].toFloat(), tokens[tokenPos + 1].toFloat());
				p2 = QPointF(tokens[tokenPos + 2].toFloat(), tokens[tokenPos + 3].toFloat());
				dest = QPointF(tokens[tokenPos + 4].toFloat(), tokens[tokenPos + 5].toFloat());
				if (curAction == curveto) {
					p1 += retPath.currentPosition();
					p2 += retPath.currentPosition();
					dest += retPath.currentPosition();
				}
				prevP2 = p2;
				retPath.cubicTo(p1, p2, dest);
				tokenPos += 5;
				break;
			case sCurveto:
			case scurveto:
				p1 = retPath.currentPosition() + retPath.currentPosition() - prevP2;
				p2 = QPointF(tokens[tokenPos].toFloat(), tokens[tokenPos + 1].toFloat());
				dest = QPointF(tokens[tokenPos + 2].toFloat(), tokens[tokenPos + 3].toFloat());
				if (curAction == scurveto) {
					p2 += retPath.currentPosition();
					dest += retPath.currentPosition();
				}
				retPath.cubicTo(p1, p2, dest);
				tokenPos += 3;
				break;
			case Closepath:
				retPath.closeSubpath();
				break;
			default:
				qWarning("Unknown command while drawing kanji path!");
				break;
		}
	}
	return retPath;
}

void KanjiRenderer::Stroke::render(QPainter *painter, qreal dLength) const
{
	if (dLength < 0.0 or dLength >= length()) painter->drawPath(painterPath());
	else {
		painter->save();
		// We must divide by the width of the pen because
		// the dash painting works with pen units.
		QPen pen(painter->pen());
		QVector<qreal> dashes;
		dashes << dLength / pen.width() << length();
		pen.setDashPattern(dashes);
		painter->setPen(pen);
		painter->drawPath(painterPath());
		painter->restore();
	}
}

KanjiRenderer::KanjiRenderer() : _kanji(0)
{
}

KanjiRenderer::KanjiRenderer(ConstKanjidic2EntryPointer kanji)
{
	setKanji(kanji);
}

void KanjiRenderer::setKanji(ConstKanjidic2EntryPointer kanji)
{
	_kanji = kanji;
	_strokes.clear();
	const QList<KanjiStroke> &strokes(kanji->strokes());
	foreach (const KanjiStroke &stroke, strokes) {
		_strokes << Stroke(&stroke);
		_strokesMap.insert(&stroke, &_strokes.last());
	}
}

void KanjiRenderer::renderStrokes(QPainter *painter)
{
	foreach (const KanjiStroke &stroke, _kanji->strokes())
		renderStroke(stroke, painter);
}

void KanjiRenderer::renderComponentStrokes(const KanjiComponent &component, QPainter *painter)
{
	// Recursively render all the childs of the component
	foreach (const KanjiStroke *stroke, component.strokes())
		renderStroke(*stroke, painter);
}

void KanjiRenderer::renderStroke(const KanjiStroke &stroke, QPainter *painter)
{
	const Stroke *str = _strokesMap[&stroke];
	str->render(painter);
}

const KanjiRenderer::Stroke *KanjiRenderer::strokeFor(const KanjiStroke &stroke) const
{
	if (!_strokesMap.contains(&stroke)) return 0;
	else return _strokesMap[&stroke];
}
