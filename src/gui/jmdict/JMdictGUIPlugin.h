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

#ifndef __GUI_JMDICTGUIPLUGIN_H
#define __GUI_JMDICTGUIPLUGIN_H

#include "core/Plugin.h"
#include "gui/jmdict/JMdictFilterWidget.h"
#include "gui/jmdict/JMdictYesNoTrainer.h"

class JMdictLinkHandler;
class JMdictFilterWidget;

class JMdictGUIPlugin : public QObject, public Plugin {
    Q_OBJECT
  private:
    QAction *_flashJL, *_flashJS, *_flashTL, *_flashTS;
    JMdictLinkHandler *_linkhandler;
    JMdictFilterWidget *_filter;
    JMdictYesNoTrainer *_trainer;

    void training(YesNoTrainer::TrainingMode mode, const QString &queryString);

  private slots:
    void trainerDeleted();

  protected slots:
    void trainingJapaneseList();
    void trainingJapaneseSet();
    void trainingTranslationList();
    void trainingTranslationSet();

  public:
    static PreferenceItem<bool> furiganasForTraining;

    JMdictGUIPlugin();
    virtual ~JMdictGUIPlugin();
    bool onRegister();
    bool onUnregister();
};

class JMdictLinkHandler : public DetailedViewLinkHandler {
  public:
    JMdictLinkHandler();
    void handleUrl(const QUrl &url, DetailedView *view);
};

#endif // JMDICTGUIPLUGIN_H
