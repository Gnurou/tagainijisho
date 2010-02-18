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


#ifndef __CORE_ASYNCENTRYLOADER_H_
#define __CORE_ASYNCENTRYLOADER_H_

#include <QtDebug>

#include "core/ASyncQuery.h"
#include "core/EntriesCache.h"

/**
 * An ASynchronous entry loader.
 *
 * Instances of this class work just like ASyncQuery in that they execute
 * SQL queries and emit signals when results are found. The difference
 * is that the result of queries passed to ASyncEntryLoader MUST be two
 * integers that represent the type and identifier of entries, to be passed
 * to EntriesCache::get(). In addition to the signals emitted by ASyncQuery,
 * this class also emits a result(EntryPointer<Entry>) signal,
 * that passes all the entries matching the SQL query.
 *
 * It is possible that the entries emitted are actually null pointers - it is
 * up to the receiver to check the validity of the entries.
 */
class ASyncEntryLoader : public ASyncQuery {
	Q_OBJECT
protected slots:
	void _loadEntry(const QSqlRecord &record);

public:
	ASyncEntryLoader(DatabaseThread *dbConn);
	virtual ~ASyncEntryLoader();

signals:
	void result(EntryPointer result);
};

#endif /* ASYNCENTRYLOADER_H_ */
