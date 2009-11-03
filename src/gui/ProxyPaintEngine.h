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

#ifndef __GUI_PROXYPAINTENGINE_H_
#define __GUI_PROXYPAINTENGINE_H_

#include <QPaintEngine>
#include <QPainter>

/**
 * A proxy paint engine that applies some transformations before painting.
 *
 * It uses a local QPainter that applies transformations to drawing operations
 * before playing them on a QPaintDevice.
 */
class ProxyPaintEngine : public QPaintEngine
{
protected:
	QPaintDevice *_target;
	QPainter _painter;
public:
	ProxyPaintEngine(QPaintDevice *target);
	virtual ~ProxyPaintEngine();

	QPainter *proxyPainter() { return &_painter; }

	virtual bool begin(QPaintDevice *pdev);
	virtual bool end();

	virtual void drawImage(const QRectF &rectangle, const QImage &image, const QRectF &sr,
			Qt::ImageConversionFlags flags = Qt::AutoColor);
	virtual void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr );
	virtual void drawPath(const QPainterPath & path);
	virtual void drawPoints(const QPointF *points, int pointCount);
	virtual void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode);
	virtual void drawTextItem(const QPointF &p, const QTextItem &textItem);
	virtual void drawTiledPixmap(const QRectF &rect, const QPixmap &pixmap, const QPointF &p);
	virtual Type type() const;
	virtual void updateState(const QPaintEngineState &state );
};

#endif
