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

#ifndef __CORE_QUERYBUILDER_H_
#define __CORE_QUERYBUILDER_H_

#include <QString>
#include <QList>
#include <QHash>
#include <QStringList>

class QueryBuilder
{
public:
	/**
	 * Describes a database column by its fully-qualified name
	 */
	class Column
	{
	private:
		QString _table;
		QString _column;
		QString _function;

	public:
		Column(const QString &table, const QString &column, const QString &function = "") : _table(table), _column(column), _function(function) {}
		Column(const QString &value) : _table(), _column(value) {}
		const QString &table() const { return _table; }
		const QString &column() const { return _column; }
		const QString &function() const { return _function; }
		QString toString() const;

		bool operator==(const Column &c) const;
	};

	/**
	 * JOIN statement - joins a table to the current request.
	 */
	class Join
	{
	public:
		/**
		 * Type of join
		 */
		typedef enum { Cross, Left } Type;
	private:
		Type _type;
		Column _column1;
		Column _column2;
		QString _additionalCondition;
		static QHash<QString, int> _tablePriority;

	public:
		/**
		 * Constructor for two-column join. Such join objects ignore the with argument
		 * of the toString() method.
		 */
		Join(const Column &column1, const Column &column2, const QString &additionalCondition = QString(""), Type type = Cross) : _type(type), _column1(column1), _column2(column2), _additionalCondition(additionalCondition) {}
		/**
		 * Constructor for one-column join. The right part of the join is determined later,
		 * using the with argument of the toString() method.
		 */
		Join(const Column &column1, const QString &additionalCondition = QString(""), Type type = Cross) : _type(type), _column1(column1), _column2(Column("", "")), _additionalCondition(additionalCondition) {}
		/**
		 * Returns true if that Join has been constructed with both a left and right column.
		 */
		bool hasRightPart() const { return !(column2() == Column("", "")); }
		Type type() const { return _type; }
		const Column &column1() const { return _column1; }
		const Column &column2() const { return _column2; }
		const QString &table1() const { return column1().table(); }
		const QString &table2() const { return column2().table(); }
		bool hasAdditionalCondition() const { return !_additionalCondition.isEmpty(); }
		const QString &additionalCondition() const { return _additionalCondition; }

		/**
		 * Records a given priority for a database table. Tables priority
		 * determines the order in which tables joins occur within a request -
		 * a factor that is determining for performance. Higher priority
		 * means the table is merged ahead.
		 */
		static void addTablePriority(const QString &table, int priority);

		/**
		 * Returns the priority associated to the table, of -1 if no
		 * priority has been affected yet.
		 */
		static int tablePriority(const QString &table);

		/**
		 * Compares two joins according to their types and priorities.
		 * Left joins are superior to cross joins, and higher priorities
		 * are inferior to lower priorities.
		 */
		bool operator<(const Join &join) const;

		bool operator==(const Join &join) const;

		QString toString(const Column &with) const;
	};

	/**
	 * WHERE statement - restricts the selected set of database entries
	 * to those matching the given constraint.
	 *
	 * WHERE statements can be used recursively, using a OR or AND relationship.
	 */
	class Where
	{
	private:
		QString _constraint;
		QList<Where> _wheres;

	public:
		Where(const QString &constraint) : _constraint(constraint) {}
		const QString &constraint() const { return _constraint; }
		QString toString() const;
		void addWhere(const Where &where, int pos = -1);

		bool operator==(const Where &c) const;
	};

	/**
	 * ORDER statement - defines the order by which database entries are
	 * sorted.
	 */
	class Order
	{
	public:
		typedef enum { ASC = 0, DESC } Way;
	private:
		QString _factor;
		Way _way;
	public:
		Order(const QString &factor, Way way = ASC) : _factor(factor), _way(way) {}
		const QString &factor() const { return _factor; }
		Way way() const { return _way; }

		QString toString() const;
		bool operator==(const Order &c) const;

		static QHash<QString, Way> orderingWay;

	};

	class GroupBy
	{
	private:
		QString _group;
		QString _having;
	public:
		GroupBy(const QString &group = "", const QString &having = "") : _group(group), _having(having) {}

		bool active() const { return !group().isEmpty(); }
		const QString &group() const { return _group; }
		const QString &having() const {return _having; }

		QString toString() const;
	};

	class Limit
	{
	private:
		bool _active;
		unsigned int _start;
		unsigned int _nbResults;
	public:
		/// No limit - selects all results
		Limit() : _active(false), _start(0), _nbResults(0) {}
		Limit(unsigned int start, unsigned int nbResults) : _active(true), _start(start), _nbResults(nbResults) {}
		Limit(unsigned int nbResults) : _active(true), _start(0), _nbResults(nbResults) {}

		bool active() const { return _active; }
		unsigned int start() const { return _start; }
		unsigned int nbResults() const { return _nbResults; }
		unsigned int end() const { return _start + _nbResults; }

		QString toString() const;
	};

	class Statement
	{
	private:
		bool _distinct;
		/// Columns to select
		QList<Column> _columns;
		/// Tables that are joined during the request
		QList<Join> _joins;
		/// Selection restrictions
		QList<Where> _wheres;
		/// Optional group by statement
		GroupBy _groupBy;

		QString sqlStatementRightPart() const;
		QString sqlStatementGroupPart() const;

		/// Shortcuts the normal join sort system and
		/// put the table named here first - dirty hack,
		/// but useful to improve speed on some requests.
		QString _firstTable;

	public:
		Statement() : _distinct(false) {}

		bool distinct() const { return _distinct; }
		void setDistinct(bool distinct) { _distinct = distinct; }

		/**
		 * Add a column to select
		 * @return the position where the column has been inserted
		 *         This is useful for further reference in the query
		 *         (e.g. ordering...)
		 */
		int addColumn(const Column &column, int pos = -1);
		QList<Column> &columns() { return _columns; }
		const QList<Column> &columns() const { return _columns; }

		/// Add a jointure
		void addJoin(const Join &join);
		QList<Join> &joins() { return _joins; }
		const QList<Join> &joins() const { return _joins; }

		/// Add a data filter
		void addWhere(const Where &where, int pos = -1);
		QList<Where> &wheres() { return _wheres; }
		const QList<Where> &wheres() const { return _wheres; }

		QString buildSqlStatement() const;

		/// Return the left-most column in the query's join
		Column leftColumn() const;

		const GroupBy &groupBy() const { return _groupBy; }
		void setGroupBy(const GroupBy &groupBy) { _groupBy = groupBy; }


		/**
		* Analyze all the columns of this statement and
		* add the missing joins as required.
		*/
		void autoJoin();

		const QString &firstTable() const { return _firstTable; }
		void setFirstTable(const QString &table) { _firstTable = table; }
	};

private:
	/// Sub-queries to unify
	QList<Statement> _statements;
	/// Results orders
	QList<Order> _orders;
	Limit _limit;

public:
	QueryBuilder();

	void clear();

	/**
	 * Builds the SQL statement corresponding to the query.
	 */
	QString buildSqlStatement(bool order = true) const;

	/// Add an union
	void addStatement(const Statement &statement, int pos = -1);
	const QList<Statement> &statements() const { return _statements; }
	QList<Statement> &statements_noconst() { return _statements; }
	/// Add an ordering
	void addOrder(const Order &order, int pos = -1);
	QList<Order> &orders() { return _orders; }
	const QList<Order> &orders() const { return _orders; }

	const Limit &limit() const { return _limit; }
	void setLimit(const Limit &limit) { _limit = limit; }

	const Statement *getStatementForEntryType(int entryType) const;
};

#endif /* QUERYBUILDER_H_ */
