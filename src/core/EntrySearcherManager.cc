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

#include "core/Preferences.h"
#include "core/Database.h"
#include "core/EntrySearcherManager.h"

EntrySearcherManager *EntrySearcherManager::_instance = 0;
PreferenceItem<bool> EntrySearcherManager::studiedEntriesFirst("mainWindow/resultsView", "studiedEntriesFirst", true);

const int resultsPerPage = 50;

EntrySearcherManager::EntrySearcherManager() : quotedWordsMatch(SearchCommand::quotedWordsMatch().pattern()),
	validSearchCompoundMatch(SearchCommand::commandMatch().pattern() + "|" + SearchCommand::singleWordMatch().pattern() + "|" + SearchCommand::quotedWordsMatch().pattern()),
	validSearchMatch(" *((" + validSearchCompoundMatch.pattern() + ") *)* *")
{
	QueryBuilder::Order::orderingWay["jlpt"] = QueryBuilder::Order::DESC;
}

QStringList EntrySearcherManager::splitSearchString(const QString &searchString)
{
	if (!validSearchMatch.exactMatch(searchString)) return QStringList();
	int pos = 0;
	QStringList res;
	while ((pos = validSearchCompoundMatch.indexIn(searchString, pos)) != -1) {
		QString compound = validSearchCompoundMatch.cap();
		// Special handling for quoted words
		if (quotedWordsMatch.exactMatch(compound))
			compound.remove(compound.size() - 1, 1).remove(0, 1);
		res << compound;
		pos += validSearchCompoundMatch.matchedLength();
	}
	return res;
}

EntrySearcherManager &EntrySearcherManager::instance()
{
	if (!_instance) _instance = new EntrySearcherManager();
	return *_instance;
}

void EntrySearcherManager::addInstance(EntrySearcher *searcher)
{
	if (!_instances.contains(searcher)) _instances << searcher;
}

bool EntrySearcherManager::removeInstance(EntrySearcher *searcher)
{
	return _instances.removeOne(searcher);
}

static void replaceJapaneseWildCards(QString &str)
{
	str.replace(QString::fromUtf8("？"), "?");
	str.replace(QString::fromUtf8("＊"), "*");
	str.replace(QString::fromUtf8("："), ":");
	str.replace(QString::fromUtf8("’"), "'");
	str.replace(QString::fromUtf8("”"), "\"");
	str.replace(QString::fromUtf8("＝"), "=");
	str.replace(QString::fromUtf8("　"), " ");
}

bool EntrySearcherManager::buildQuery(const QString &search, QueryBuilder &query)
{
	// Clear previous static regexps - this is bad, but no better solution for now
	Database::staticRegExps.clear();
	QString searchString(search);
	replaceJapaneseWildCards(searchString);

	// Extract commands and words
	QStringList split = splitSearchString(searchString.trimmed());

	// Empty string or invalid format, nothing to do!
	if (split.size() == 0) return false;

	// First filter ordering commands
	QStringList orders;
	if (studiedEntriesFirst.value()) orders << "study" << "score";
	orders << "matchPos" << "jlpt" << "freq";

	// Transform words into commands, if applicable
	bool validQuery = false;
	foreach (EntrySearcher *searcher, _instances) {

		// Try to convert all parameters to commands
		QList<SearchCommand> commands;
		if (searcher->searchToCommands(split, commands)) {
			// Looks like we will need a statement for this query
			QueryBuilder::Statement statement;

			// Try to get every command into sql statements
			searcher->buildStatement(commands, statement);
			if (commands.isEmpty()) {
				int colpos = 0;
				foreach(const QString &order, orders) {
					colpos = statement.addColumn(searcher->canSort(order, statement));
				}

				searcher->setColumns(statement);

				query.addStatement(statement);
				validQuery = true;
			}
		}
	}

	if (!validQuery) return false;

	int orderColStart = 3;
	for (int i = 0; i < orders.size(); i++)
		query.addOrder(QueryBuilder::Order(QString("%1").arg(orderColStart + i), QueryBuilder::Order::orderingWay[orders[i]]));

	return true;
}

EntrySearcher *EntrySearcherManager::getEntrySearcher(EntryType entryType)
{
	foreach(EntrySearcher *searcher, _instances)
		if (searcher->entryType() == entryType) return searcher;
	return 0;
}
