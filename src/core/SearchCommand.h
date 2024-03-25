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

#ifndef __CORE_SEARCHCOMMAND_H
#define __CORE_SEARCHCOMMAND_H

#include <QRegExp>
#include <QStringList>

/**
 * Describes a command, as recognized by instances of the
 * EntrySearcher class.
 */
class SearchCommand {
  private:
    QString _command;
    QStringList _args;
    static SearchCommand _invalid;

    static QRegExp _singleWordMatch;
    static QRegExp _quotedWordsMatch;
    static QRegExp _argMatch;
    static QRegExp _commandMatch;

  public:
    static const QRegExp singleWordMatch() { return _singleWordMatch; }
    static const QRegExp quotedWordsMatch() { return _quotedWordsMatch; }
    static const QRegExp argMatch() { return _argMatch; }
    static const QRegExp commandMatch() { return _commandMatch; }

  public:
    SearchCommand() {}
    SearchCommand(const QString &command) : _command(command) {}

    void setCommand(const QString &command) { _command = command; }
    void addArgument(const QString &arg) { _args << arg; }
    const QString &command() const { return _command; }
    const QStringList &args() const { return _args; }

    bool isValid() const { return command() != "invalid"; }

    /**
     * Returns the unique instance that is used to represent
     * invalid commands.
     */
    static const SearchCommand &invalid() { return _invalid; }

    /**
     * Builds the command corresponding to the given string
     * (that must be in command format). Returns an invalid
     * entry if the string is not parseable as a command.
     */
    static SearchCommand fromString(const QString &string);

    bool operator==(const SearchCommand &c) const;
};

#endif
