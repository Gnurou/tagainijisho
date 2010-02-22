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
#include "gui/jmdict/JMdictYesNoTrainer.h"
#include "gui/SearchFilterWidget.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QMenu>
#include <QStringList>
#include <QAction>

class JMdictLinkHandler;
class JMdictFilterWidget;

class JMdictGUIPlugin : public QObject, public Plugin
{
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

class JMdictLinkHandler : public DetailedViewLinkHandler
{
public:
	JMdictLinkHandler();
	void handleUrl(const QUrl &url, DetailedView *view);
};

class JMdictFilterWidget : public SearchFilterWidget
{
	Q_OBJECT
private:
	QLineEdit *_containedKanjis;
	QLineEdit *_containedComponents;
	QCheckBox *_studiedKanjisCheckBox;

	QPushButton *_posButton;
	QStringList _posList;
	QPushButton *_dialButton;
	QStringList _dialList;
	QPushButton *_fieldButton;
	QStringList _fieldList;
	QPushButton *_miscButton;
	QStringList _miscList;

protected:
	virtual void _reset();
	static QActionGroup *addCheckableProperties(const QVector< QPair< QString, QString > >& defs, QMenu* menu);

protected slots:
	void updateMiscFilteredProperties();

public:
	JMdictFilterWidget(QWidget *parent = 0);
	virtual QString name() const { return "jmdictoptions"; }
	virtual QString currentTitle() const;
	virtual QString currentCommand() const;
	virtual void updateFeatures();

	QString containedKanjis() const { return _containedKanjis->text(); }
	void setContainedKanjis(const QString &kanjis) { _containedKanjis->setText(kanjis); }
	Q_PROPERTY(QString containedKanjis READ containedKanjis WRITE setContainedKanjis)

	 QString containedComponents() const { return _containedComponents->text(); }
	 void setContainedComponents(const QString &components) { _containedComponents->setText(components); }
	 Q_PROPERTY(QString containedComponents READ containedComponents WRITE setContainedComponents)

	bool studiedKanjisOnly() const { return _studiedKanjisCheckBox->isChecked(); }
	void setStudiedKanjisOnly(bool value) { _studiedKanjisCheckBox->setChecked(value); }
	Q_PROPERTY(bool studiedKanjisOnly READ studiedKanjisOnly WRITE setStudiedKanjisOnly)

	QStringList pos() const { return _posList; }
	void setPos(const QStringList &list);
	Q_PROPERTY(QStringList pos READ pos WRITE setPos)

	QStringList dial() const { return _dialList; }
	void setDial(const QStringList &list);
	Q_PROPERTY(QStringList dial READ dial WRITE setDial)

	QStringList field() const { return _fieldList; }
	void setField(const QStringList &list);
	Q_PROPERTY(QStringList field READ field WRITE setField)

	QStringList misc() const { return _miscList; }
	void setMisc(const QStringList &list);
	Q_PROPERTY(QStringList misc READ misc WRITE setMisc)

protected slots:
	void onPosTriggered(QAction *action);
	void onDialTriggered(QAction *action);
	void onFieldTriggered(QAction *action);
	void onMiscTriggered(QAction *action);
};

#endif // JMDICTGUIPLUGIN_H
