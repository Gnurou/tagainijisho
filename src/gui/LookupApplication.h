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

#ifndef __GUI_LOOKUP_APPLICATION_H
#define __GUI_LOOKUP_APPLICATION_H

#include <QApplication>
#include <QUrl>

/// Receives macOS URL events even while startup dialogs are running.
class LookupApplication : public QApplication {
    Q_OBJECT
  private:
    enum class LookupState { Starting, Ready, Stopped };
    LookupState _lookupState = LookupState::Starting;
    bool _handlingLookup = false;
    QUrl _pendingLookup;

    void deliverPendingLookup();

  protected:
    bool event(QEvent *event) override;

  public:
    LookupApplication(int &argc, char **argv) : QApplication(argc, argv) {}
    void setLookupReady();
    /// Permanently stop delivery before destroying the window and plugins.
    void stopLookups();

  signals:
    void lookupRequested(const QUrl &url);
};

#endif
