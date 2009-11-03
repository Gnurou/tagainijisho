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

#include "gui/ProxyPaintEngine.h"

#include <QtDebug>

ProxyPaintEngine::ProxyPaintEngine(QPaintDevice *target) : QPaintEngine(QPaintEngine::AllFeatures), _target(target)
{
}

ProxyPaintEngine::~ProxyPaintEngine()
{
}

bool ProxyPaintEngine::begin(QPaintDevice *pdev)
{
	bool ret = _painter.begin(_target);
	if (ret) setActive(true);
	return ret;
}

bool ProxyPaintEngine::end()
{
	bool ret = _painter.end();
	if (ret) setActive(false);
	return ret;
}

void ProxyPaintEngine::drawImage(const QRectF &rectangle, const QImage &image, const QRectF &sr, Qt::ImageConversionFlags flags)
{
	_painter.drawImage(rectangle, image, sr, flags);
}

void ProxyPaintEngine::drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr)
{
	_painter.drawPixmap(r, pm, sr);
}

void ProxyPaintEngine::drawPath(const QPainterPath & path)
{
	_painter.drawPath(path);
}

void ProxyPaintEngine::drawPoints(const QPointF *points, int pointCount)
{
	_painter.drawPoints(points, pointCount);
}

void ProxyPaintEngine::drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode)
{
	Qt::FillRule drawRule = (mode == QPaintEngine::OddEvenMode ? Qt::OddEvenFill : Qt::WindingFill);
	_painter.drawPolygon(points, pointCount, drawRule);
}

void ProxyPaintEngine::drawTextItem(const QPointF &p, const QTextItem &textItem)
{
	_painter.drawTextItem(p, textItem);
}

void ProxyPaintEngine::drawTiledPixmap(const QRectF &rect, const QPixmap &pixmap, const QPointF &p)
{
	_painter.drawTiledPixmap(rect, pixmap, p);
}

QPaintEngine::Type ProxyPaintEngine::type() const
{
	return _painter.paintEngine()->type();
}

void ProxyPaintEngine::updateState(const QPaintEngineState &state)
{
	_painter.paintEngine()->updateState(state);
}
