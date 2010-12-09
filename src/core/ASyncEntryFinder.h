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


#ifndef __CORE_ASYNCENTRYFINDER_H_
#define __CORE_ASYNCENTRYFINDER_H_

#include <QtDebug>

#include "core/ASyncQuery.h"
#include "core/EntriesCache.h"

/**
 * An ASynchronous entry finder.
 *
 * Instances of this class work just like ASyncQuery in that they execute
 * SQL queries and emit signals when results are found. The difference
 * is that the result of queries passed to ASyncEntryLoader MUST be two
 * integers that represent the type and identifier of entries, used to
 * construct an EntryRef.
 *
 */
class ASyncEntryFinder : public ASyncQuery {
	Q_OBJECT
protected slots:
	void _loadEntry(const QList<QVariant> &record);

public:
	ASyncEntryFinder(DatabaseThread *dbConn);
	virtual ~ASyncEntryFinder();

signals:
	void result(const EntryRef &result);
};

#endif /* ASYNCENTRYLOADER_H_ */
