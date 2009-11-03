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

#include "gui/BookletPrintEngine.h"

#include <QtDebug>

BookletPrintEngine::BookletPrintEngine(QPrinter *printer) : QPrintEngine(), ProxyPaintEngine(printer), _printer(printer), _engine(printer->printEngine()), _pageCpt(0)
{
}

BookletPrintEngine::~BookletPrintEngine()
{
}

bool BookletPrintEngine::abort()
{
	bool res = _engine->abort();
	if (res) _pageCpt = 0;
	return res;
}

int BookletPrintEngine::metric(QPaintDevice::PaintDeviceMetric id) const
{
	return _engine->metric(id);
}

void BookletPrintEngine::resetPainter()
{
	QPainter *painter = ProxyPaintEngine::painter();
	painter->resetMatrix();

	painter->setViewport(proxyPainter()->viewport());
	painter->setWindow(proxyPainter()->window());
}

void BookletPrintEngine::setupPainterForPage(int pageNbr)
{
	resetPainter();
	QPainter *painter = ProxyPaintEngine::painter();

	if (pageNbr >=1 && pageNbr <= 4) {
		painter->rotate(-90);
		painter->translate(-_subPageWidth * (4 - pageNbr + 1) + _xDecay, _yDecay);
		painter->scale(_subPageScaleFactor, _subPageScaleFactor);
	}
	else {
		if (pageNbr == 0) pageNbr = 8;
		painter->rotate(90);
		painter->translate(_subPageWidth * (pageNbr - 5) + _xDecay, -_subPageHeight * 2 + _yDecay);
		painter->scale(_subPageScaleFactor, _subPageScaleFactor);
//		painter->translate(_subPageWidth / 6.0, 0);
	}
}

void BookletPrintEngine::drawPageLayout()
{
	QPainter *painter = ProxyPaintEngine::painter();

	QRectF window = painter->window();
	// Draw the page rectangle
	painter->drawRect(window);
	painter->drawLine(QPoint(window.center().x(), window.top()), QPoint(window.center().x(), window.bottom()));
	painter->drawLine(QPoint(window.left(), window.top() + _subPageWidth), QPoint(window.right(), window.top() + _subPageWidth));
	painter->drawLine(QPoint(window.left(), window.top() + _subPageWidth * 2), QPoint(window.right(), window.top() + _subPageWidth * 2));
	painter->drawLine(QPoint(window.left(), window.top() + _subPageWidth * 3), QPoint(window.right(), window.top() + _subPageWidth * 3));
}

bool BookletPrintEngine::newPage()
{
	bool res = true;
	if (++_pageCpt == 8) {
		_pageCpt = 0;
		res = _engine->newPage();
		if (res) {
			resetPainter();
			drawPageLayout();
		}
	}
	if (res) {
		setupPainterForPage(_pageCpt);
	}
	return true;
}

QPrinter::PrinterState BookletPrintEngine::printerState() const
{
	return _engine->printerState();
}

QVariant BookletPrintEngine::property(PrintEnginePropertyKey key) const
{
	return _engine->property(key);
}

void BookletPrintEngine::setProperty(PrintEnginePropertyKey key, const QVariant & value)
{
	_engine->setProperty(key, value);
}

bool BookletPrintEngine::begin(QPaintDevice *pdev)
{
	bool ret = ProxyPaintEngine::begin(pdev);
	if (ret) {
		// Setup our transformation properties
		QRect window = _printer->pageRect();
		QRect paperRect = _printer->paperRect();

		// Authorized printing size for every page
		_subPageWidth = window.height() / 4.0;
		_subPageHeight = window.width() / 2.0;

		// Scale factor for the two dimensions
		qreal widthFactor = _subPageWidth / paperRect.width();
		qreal heightFactor = _subPageHeight / paperRect.height();
		// Have to multiply the dimensions by this in order to have
		// a page that fits into the printable area
		_subPageScaleFactor = qMax(widthFactor, heightFactor);
		// Values to add to the beginning of each sub page printable area
		// in order to have a well-centered page.
		_xDecay = (_subPageWidth - window.width() * _subPageScaleFactor) / 2.0;
		_yDecay = (_subPageHeight - window.height() * _subPageScaleFactor) / 2.0;

		resetPainter();
		drawPageLayout();
		setupPainterForPage(0);
	}
	return ret;
}

bool BookletPrintEngine::end()
{
	return ProxyPaintEngine::end();
}
