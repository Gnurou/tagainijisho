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
	retPath.setFillRule(Qt::WindingFill);

	// Process the string - add spaces between unseparated tokens
	int idx;
	while ((idx = svgPath.indexOf(QRegExp("[a-zA-Z]\\d|\\d[a-zA-Z]|\\w-"))) != -1) svgPath.insert(idx + 1, ' ');

	QStringList tokens = svgPath.split(QRegExp(" +|,"), QString::SkipEmptyParts);

	SVGPathCommand curAction = None;
	QPointF dest;
	QPointF p1;
	QPointF p2;
	QPointF lastControl;
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
				lastControl = retPath.currentPosition();
				break;
			case Lineto:
			case lineto:
				dest = QPointF(tokens[tokenPos].toFloat(), tokens[tokenPos + 1].toFloat());
				if (curAction == lineto) dest += retPath.currentPosition();
				retPath.lineTo(dest);
				tokenPos++;
				lastControl = retPath.currentPosition();
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
				retPath.cubicTo(p1, p2, dest);
				tokenPos += 5;
				lastControl = p2;
				break;
			case sCurveto:
			case scurveto:
				p1 = retPath.currentPosition() * 2 - lastControl;
				p2 = QPointF(tokens[tokenPos].toFloat(), tokens[tokenPos + 1].toFloat());
				dest = QPointF(tokens[tokenPos + 2].toFloat(), tokens[tokenPos + 3].toFloat());
				if (curAction == scurveto) {
					p2 += retPath.currentPosition();
					dest += retPath.currentPosition();
				}
				retPath.cubicTo(p1, p2, dest);
				tokenPos += 3;
				lastControl = p2;
				break;
			case Closepath:
				retPath.closeSubpath();
				lastControl = retPath.currentPosition();
				break;
			default:
				qWarning("Unknown command while drawing kanji path!");
				lastControl = retPath.currentPosition();
				break;
		}
	}
	return retPath;
}

void KanjiRenderer::Stroke::render(QPainter *painter, qreal dLength, qreal startFrom) const
{
	if (dLength < 0.0 or dLength >= length()) painter->drawPath(painterPath());
	else {
		painter->save();
		// We must divide by the width of the pen because
		// the dash painting works with pen units.
		QPen pen(painter->pen());
		QVector<qreal> dashes;
		dashes << 0.0 << startFrom << dLength / pen.width() << length();
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
	// Center the character horizontally if we are treating a kana
#if QT_VERSION >= 0x040600
	if (TextTools::isKana(kanji->kanji())) {
		QRectF bbox;
		foreach (const Stroke &stroke, _strokes) {
			bbox = bbox.united(stroke.painterPath().boundingRect());
		}
		QPointF translatePoint(QPointF(KANJI_AREA_WIDTH, KANJI_AREA_HEIGHT) / 2.0 - bbox.center());

		for (QList<Stroke>::iterator stroke = _strokes.begin(); stroke != _strokes.end(); ++stroke) {
			stroke->_painterPath.translate(translatePoint.x(), 0);
		}
	}
#endif
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

void KanjiRenderer::renderStrokeNumber(const KanjiStroke &stroke, QPainter *painter, qreal baseSize)
{
	painter->save();
	painter->setBrush(QBrush(Qt::white));
	QPen pen(painter->pen());
	pen.setWidth(1);
	painter->setPen(Qt::NoPen);
	// Find the position where the number is to be rendered
	const QPainterPath &path = _strokesMap[&stroke]->painterPath();
	QLineF line(QLineF(path.pointAtPercent(0.0), path.pointAtPercent(0.1)));
	//line = line.normalVector();
	line.setLength(-baseSize * 1.5);
	//line.setAngle(path.angleAtPercent(0.05));
	QRectF elipseRect(line.p2() - QPointF(baseSize, baseSize), QSize( (int) (baseSize * 2), (int) (baseSize * 2)));
	painter->drawEllipse(elipseRect);
	pen.setColor(Qt::black);
	painter->setPen(pen);
	painter->setBrush(Qt::NoBrush);
	//painter->drawLine(line);
	// Find the index of the stroke
	int pos = _strokes.indexOf(*(_strokesMap[&stroke])) + 1;
	QFont font;
	font.setPixelSize((int) (baseSize * 2 - 2));
	painter->setFont(font);
	painter->drawText(elipseRect, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(pos));
	painter->restore();
}

void KanjiRenderer::renderGrid(QPainter *painter)
{
	painter->save();
	painter->drawLine(QLineF(0, 109 / 2.0, 109, 109 / 2.0));
	painter->drawLine(QLineF(109 / 2.0, 0, 109 / 2.0, 109));
	QPen pen(painter->pen());
	pen.setWidth((int) (pen.width() / 3.0));
	pen.setStyle(Qt::DotLine);
	painter->setPen(pen);
	painter->drawLine(QLineF(0, 109 / 4.0, 109, 109 / 4.0));
	painter->drawLine(QLineF(0, 109 / 2.0 + 109 / 4.0, 109, 109 / 2.0 + 109 / 4.0));
	painter->drawLine(QLineF(109 / 4.0, 0, 109 / 4.0, 109));
	painter->drawLine(QLineF(109 / 2.0 + 109 / 4.0, 0, 109 / 2.0 + 109 / 4.0, 109));
	painter->restore();
}

const KanjiRenderer::Stroke *KanjiRenderer::strokeFor(const KanjiStroke &stroke) const
{
	if (!_strokesMap.contains(&stroke)) return 0;
	else return _strokesMap[&stroke];
}
