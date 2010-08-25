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

#include "core/QueryBuilder.h"

#include <QtDebug>
#include <QStringList>

QHash<QString, int> QueryBuilder::Join::_tablePriority;
QHash<QString, QueryBuilder::Order::Way> QueryBuilder::Order::orderingWay;

bool QueryBuilder::Column::operator==(const Column &c) const
{
	return table() == c.table() && column() == c.column();
}

QString QueryBuilder::Column::toString() const
{
	QString s;
	if (!table().isEmpty()) s += table() + ".";
	s += column();
	if (!function().isEmpty()) s = function() + ("(") + s + (")");
 	return s;
}

void QueryBuilder::Join::addTablePriority(const QString &table, int priority)
{
	_tablePriority[table] = priority;
}

int QueryBuilder::Join::tablePriority(const QString &table)
{
	return _tablePriority[table];
}

bool QueryBuilder::Join::operator<(const Join &join) const
{
	return (tablePriority(table1()) > tablePriority(join.table1()));
}

bool QueryBuilder::Join::operator==(const Join &j) const
{
	return column1() == j.column1() && column2() == j.column2() && additionalCondition() == j.additionalCondition();
}

QString QueryBuilder::Join::toString(const Column &with) const
{
	QString res;
	switch (type()) {
	case Join::Cross:
		res += "join ";
		break;
	case Join::Left:
		res += "left join ";
		break;
	}
	res += table1() + " on " + column1().toString() + " = ";
	if (!hasRightPart()) res += with.toString();
	else res += column2().toString();
	if (additionalCondition() != "") res += " and " + additionalCondition();
	return res;
}

bool QueryBuilder::Where::operator==(const Where &w) const
{
	return constraint() == w.constraint();
}

QString QueryBuilder::Order::toString() const
{
	return factor() + (way() == ASC ? " ASC" : " DESC");
}

bool QueryBuilder::Order::operator==(const Order &o) const
{
	return factor() == o.factor() && way() == o.way();
}

QString QueryBuilder::GroupBy::toString() const
{
	QString ret;

	if (active()) {
		ret += "GROUP BY " + group();
		if (!having().isEmpty()) ret += " HAVING " + having();
	}

	return ret;
}

QString QueryBuilder::Limit::toString() const
{
	if (!active()) return "";
	else return QString("limit %1,%2").arg(start()).arg(nbResults());
}

int QueryBuilder::Statement::addColumn(const Column &column, int pos)
{
	if (pos == -1) pos = _columns.size();
	_columns.insert(pos, column);
	return pos;
}

void QueryBuilder::Statement::addJoin(const Join &join)
{
	if (_joins.contains(join)) return;
	_joins << join;
}

void QueryBuilder::Statement::addWhere(const Where &where, int pos)
{
	if (_wheres.contains(where)) return;

	if (pos == -1) pos = _wheres.size();
	_wheres.insert(pos, where);
}

QueryBuilder::Column QueryBuilder::Statement::leftColumn() const
{
	QList<Join> jList(joins());
	int previousTablePriority = 0;
	if (!firstTable().isEmpty()) {
		previousTablePriority = QueryBuilder::Join::tablePriority(firstTable());
		QueryBuilder::Join::addTablePriority(firstTable(), 65536);
	}
	qSort(jList.begin(), jList.end());
	if (!firstTable().isEmpty()) {
		QueryBuilder::Join::addTablePriority(firstTable(), previousTablePriority);
	}
	return jList[0].column1();
}

QString QueryBuilder::Statement::sqlStatementRightPart() const
{
	QString res;

	const Join *leftJoin = 0;
	QList<Join> jList(joins());
	int previousTablePriority = 0;
	if (!firstTable().isEmpty()) {
		previousTablePriority = QueryBuilder::Join::tablePriority(firstTable());
		QueryBuilder::Join::addTablePriority(firstTable(), 65536);
	}
	qSort(jList.begin(), jList.end());
	if (!firstTable().isEmpty()) {
		QueryBuilder::Join::addTablePriority(firstTable(), previousTablePriority);
	}

	if (!_joins.isEmpty()) {
		res += " from ";

		leftJoin = &jList[0];
		res += leftJoin->column1().table();

		for (int i = 1; i < jList.size(); i++) {
			const Join &j = jList[i];
			res += " " + j.toString(leftJoin->column1());
		}
	}

	if (!_wheres.isEmpty() || (leftJoin && leftJoin->hasAdditionalCondition())) {
		res += " where ";

		QStringList whereStrs;
		if (leftJoin->hasAdditionalCondition()) whereStrs << leftJoin->additionalCondition();

		foreach (const Where &where, wheres()) {
			whereStrs << where.toString();
		}
		res += whereStrs.join(" and ");
	}

	return res;
}

QString QueryBuilder::Statement::sqlStatementGroupPart() const
{
	if (_groupBy.active()) return " " + _groupBy.toString();

	return "";
}


QString QueryBuilder::Statement::buildSqlStatement() const
{
	QString res = "select ";

	if (distinct()) res += "distinct ";

	for (int i = 0; i < _columns.size(); i++) {
		if (i > 0) res += ", ";
		res += _columns[i].toString();
	}

	res += sqlStatementRightPart() + sqlStatementGroupPart();

	QString lC = leftColumn().toString();
	res.replace("{{leftcolumn}}", lC);
	return res;
}

void QueryBuilder::Statement::autoJoin()
{
	foreach (const Column &column, columns()) {
		const QString &table = column.table();
		bool found = false;
		foreach(const Join &join, joins()) {
			if (join.column1().table() == table || join.column2().table() == table) {
				found = true;
				break;
			}
		}
		if (found) continue;
		addJoin(QueryBuilder::Join(QueryBuilder::Column(table, "id"), "", QueryBuilder::Join::Left));
	}

	// Now do the same for where statements
	// TODO would be nice to analyze where statements as well!
/*	foreach (const Where &where, wheres()) {
		QRegExp regexp("[a-zA-Z][\w.]*\w* *[^\\(]");
	}*/
}

QueryBuilder::QueryBuilder()
{
}

void QueryBuilder::clear()
{
	_statements.clear();
	_orders.clear();
	_limit = Limit();
}

QString QueryBuilder::buildSqlStatement(bool order) const
{
	if (statements().size() == 0) return "";
	QStringList statementsList;
	foreach(const Statement &statement, statements())
		statementsList << statement.buildSqlStatement();

	QString res = statementsList.join(" UNION ALL ");

	if (!_orders.isEmpty() && order) {
		res += " ORDER BY ";
		for (int i = 0; i < _orders.size(); i++) {
			if (i > 0) res += ", ";
			res += _orders[i].toString();
		}
	}

	if (_limit.active()) res += " " + _limit.toString();

	return res;
}

void QueryBuilder::addStatement(const Statement &statement, int pos)
{
	if (pos == -1) pos = _statements.size();
	_statements.insert(pos, statement);
}

void QueryBuilder::addOrder(const Order &order, int pos)
{
	if (_orders.contains(order)) return;

	if (pos == -1) pos = _orders.size();
	_orders.insert(pos, order);
}

const QueryBuilder::Statement *QueryBuilder::getStatementForEntryType(int entryType) const
{
	foreach (const Statement &statement, _statements) {
		if (statement.columns()[0].toString().toInt() == entryType) return &statement;
	}
	return 0;
}
