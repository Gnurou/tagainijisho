/*
 *  Copyright (C) 2010  Alexandre Courbot
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

#ifndef __GUI_JMDICTFILTERWIDGET_H
#define __GUI_JMDICTFILTERWIDGET_H

#include "gui/SearchFilterWidget.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QMenu>
#include <QStringList>
#include <QAction>

class JMdictFilterWidget : public SearchFilterWidget
{
	Q_OBJECT
private:
	QLineEdit *_containedKanjis;
	QLineEdit *_containedComponents;
	QCheckBox *_studiedKanjisCheckBox;
	QCheckBox *_kanaOnlyCheckBox;

	QPushButton *_posButton;
	QStringList _posList;
	QPushButton *_dialButton;
	QStringList _dialList;
	QPushButton *_fieldButton;
	QStringList _fieldList;
	QPushButton *_miscButton;
	QStringList _miscList;

	void __onPropertyTriggered(QAction *action, QStringList &list, QPushButton *button);

protected:
	virtual void _reset();
	static QActionGroup *addCheckableProperties(const QMap<QString, QPair<QString, quint16> >&map, QMenu* menu);

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

	bool kanaOnlyWords() const { return _kanaOnlyCheckBox->isChecked(); }
	void setKanaOnlyWords(bool value) { _kanaOnlyCheckBox->setChecked(value); }
	Q_PROPERTY(bool kanaOnlyWords READ kanaOnlyWords WRITE setKanaOnlyWords)

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

#endif
