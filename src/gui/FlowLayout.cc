/****************************************************************************
 **
 ** Copyright (C) 2004-2007 Trolltech ASA. All rights reserved.
 **
 ** This file is part of the example classes of the Qt Toolkit.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License version 2.0 as published by the Free Software Foundation
 ** and appearing in the file LICENSE.GPL included in the packaging of
 ** this file.  Please review the following information to ensure GNU
 ** General Public Licensing requirements will be met:
 ** http://www.trolltech.com/products/qt/opensource.html
 **
 ** If you are unsure which license is appropriate for your use, please
 ** review the following information:
 ** http://www.trolltech.com/products/qt/licensing.html or contact the
 ** sales department at sales@trolltech.com.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

#include <QtGui>

#include "gui/FlowLayout.h"

FlowLayout::FlowLayout(QWidget *parent, int margin, int spacing) : QLayout(parent) {
    setContentsMargins(margin, margin, margin, margin);
    setSpacing(spacing);
}

FlowLayout::FlowLayout(int spacing) { setSpacing(spacing); }

FlowLayout::~FlowLayout() {
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}

void FlowLayout::addItem(QLayoutItem *item) { itemList.append(item); }

int FlowLayout::count() const { return itemList.size(); }

QLayoutItem *FlowLayout::itemAt(int index) const { return itemList.value(index); }

QLayoutItem *FlowLayout::takeAt(int index) {
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    else
        return 0;
}

Qt::Orientations FlowLayout::expandingDirections() const { return Qt::Orientations(); }

bool FlowLayout::hasHeightForWidth() const { return true; }

int FlowLayout::heightForWidth(int width) const {
    int height = doLayout(QRect(0, 0, width, 0), true);
    return height;
}

void FlowLayout::setGeometry(const QRect &rect) {
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize FlowLayout::sizeHint() const { return minimumSize(); }

QSize FlowLayout::minimumSize() const {
    QSize size;
    QLayoutItem *item;
    foreach (item, itemList)
        size = size.expandedTo(item->minimumSize());

    QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    return size;
}

int FlowLayout::doLayout(const QRect &rect, bool testOnly) const {
    int x = rect.x();
    int y = rect.y();
    int lineHeight = 0;

    QLayoutItem *item;
    foreach (item, itemList) {
        int nextX = x + item->sizeHint().width() + spacing();
        if (nextX - spacing() > rect.right() && lineHeight > 0) {
            x = rect.x();
            y = y + lineHeight + spacing();
            nextX = x + item->sizeHint().width() + spacing();
            lineHeight = 0;
        }

        if (!testOnly)
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y();
}
