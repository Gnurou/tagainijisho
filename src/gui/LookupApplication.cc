/*
 *  Copyright (C) 2026  Alex Ehlke
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

#include "gui/LookupApplication.h"
#include "gui/LookupUrl.h"

#include <QFileOpenEvent>

bool LookupApplication::event(QEvent *event) {
    if (event->type() == QEvent::FileOpen) {
        const QUrl url = static_cast<QFileOpenEvent *>(event)->url();
        if (LookupUrl::parse(url).type != LookupUrl::Invalid) {
            // Keep the most recent request during startup or a modal lookup dialog.
            if (_lookupState != LookupState::Stopped) {
                _pendingLookup = url;
                deliverPendingLookup();
            }
            event->accept();
            return true;
        }
    }
    return QApplication::event(event);
}

void LookupApplication::setLookupReady() {
    if (_lookupState == LookupState::Stopped)
        return;
    _lookupState = LookupState::Ready;
    deliverPendingLookup();
}

void LookupApplication::stopLookups() {
    _lookupState = LookupState::Stopped;
    _pendingLookup.clear();
}

void LookupApplication::deliverPendingLookup() {
    if (_lookupState != LookupState::Ready || _handlingLookup)
        return;
    // A missing-entry dialog runs a nested event loop. Defer URLs received there
    // until the current lookup returns, rather than nesting dialogs/handlers.
    _handlingLookup = true;
    while (!_pendingLookup.isEmpty()) {
        const QUrl pending = _pendingLookup;
        _pendingLookup.clear();
        emit lookupRequested(pending);
    }
    _handlingLookup = false;
}
