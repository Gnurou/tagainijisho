/*
 *  Copyright (C) 2008/2009/2010  Alexandre Courbot
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

#ifndef __GUI_TEXTFILTERWIDGET_H
#define __GUI_TEXTFILTERWIDGET_H

#include "core/Preferences.h"
#include "gui/SearchFilterWidget.h"

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>

class TextFilterWidget : public SearchFilterWidget {
    Q_OBJECT
  private:
    QComboBox *_searchField;
    QCheckBox *_romajiSearchAllowed;
    bool _reseted;

  private slots:
    void runSearch();
    void onItemSelected(int item);
    void resetSearchText();
    void onSearchTextChanged(const QString &text);
    void onRomajiChanged(bool state);

  protected:
    virtual void _reset();

  public:
    TextFilterWidget(QWidget *parent = 0);
    virtual QString name() const { return "searchtext"; }
    virtual QString currentTitle() const;
    virtual QString currentCommand() const { return text(); }
    QComboBox *searchField() { return _searchField; }

    QString text() const { return _searchField->lineEdit()->text(); }
    void setText(const QString &text);
    bool romajiSearchAllowed() const { return _romajiSearchAllowed->isChecked(); }
    void setRomajiSearchAllowed(bool value) { _romajiSearchAllowed->setChecked(value); }
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool allowRomajiSearch READ romajiSearchAllowed WRITE setRomajiSearchAllowed)

    static PreferenceItem<int> textSearchHistorySize;
};

#endif
