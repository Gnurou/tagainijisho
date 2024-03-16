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

#ifndef __GUI_BOOKLETPRINTENGINE_H_
#define __GUI_BOOKLETPRINTENGINE_H_

#include <QPrintEngine>

#include "gui/ProxyPaintEngine.h"

class BookletPrintEngine : public QPrintEngine, public ProxyPaintEngine {
  private:
    QPrinter *_printer;
    QPrintEngine *_engine;
    int _pageCpt;

    qreal _subPageWidth;
    qreal _subPageHeight;
    qreal _subPageScaleFactor;
    qreal _xDecay;
    qreal _yDecay;

  protected:
    void resetPainter();
    void setupPainterForPage(int pageNbr);
    void drawPageLayout();

  public:
    BookletPrintEngine(QPrinter *printer);
    virtual ~BookletPrintEngine();

    virtual bool abort();
    virtual int metric(QPaintDevice::PaintDeviceMetric id) const;
    virtual bool newPage();
    virtual QPrinter::PrinterState printerState() const;
    virtual QVariant property(PrintEnginePropertyKey key) const;
    virtual void setProperty(PrintEnginePropertyKey key, const QVariant &value);

    virtual bool begin(QPaintDevice *pdev);
    virtual bool end();
};

#endif
