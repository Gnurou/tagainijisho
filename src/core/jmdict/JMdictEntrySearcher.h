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

#ifndef __CORE_JMDICT_ENTRY_SEARCHER_H_
#define __CORE_JMDICT_ENTRY_SEARCHER_H_

#include <QMap>
#include <QSet>
#include <QString>
#include <QVector>

#include "core/EntrySearcher.h"
#include "core/Preferences.h"

#include <QObject>

class JMdictEntrySearcher : public QObject, public EntrySearcher {
    Q_OBJECT
  private:
    static QSet<QString> _miscFilterMask;
    static QSet<QString> _explicitlyRequestedMiscs;

  protected slots:
    void updateMiscFilterMask();

  public:
    static QSet<QString> miscFilterEntities() { return _miscFilterMask; }
    static QVector<quint64> miscFilterMask();

    static QSet<QString> explicitlyRequestedMiscs() { return _explicitlyRequestedMiscs; }

    JMdictEntrySearcher();
    virtual ~JMdictEntrySearcher() {}

    virtual QueryBuilder::Column entryId() const {
        return QueryBuilder::Column("jmdict.entries", "id");
    }

    virtual SearchCommand commandFromWord(const QString &word) const;

    virtual void buildStatement(QList<SearchCommand> &commands, QueryBuilder::Statement &statement);
    virtual QueryBuilder::Column canSort(const QString &sort,
                                         const QueryBuilder::Statement &statement);

    static PreferenceItem<QString> miscPropertiesFilter;
};

#endif
