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

#ifndef __CORE_KANJIDIC2_ENTRY
#define __CORE_KANJIDIC2_ENTRY

#include "core/Entry.h"

#include <QStack>

#define KANJIDIC2ENTRY_GLOBALID 2

class KanjiStroke;

class KanjiComponent
{
private:
	const KanjiComponent *_parent;
	QList<const KanjiComponent *> _childs;
	QList<const KanjiStroke *> _strokes;
	QString _element;
	QString _original;
	unsigned int _number;

public:
	KanjiComponent(const KanjiComponent *parent = 0, const QString &element = "", const QString &original = "", unsigned int number = 0);
	virtual ~KanjiComponent();
	
	const KanjiComponent *parent() const { return _parent; }
	const QList<const KanjiComponent *> &components() const { return _childs; }
	const QList<const KanjiStroke *> &strokes() const { return _strokes; }
	void addChild(KanjiComponent *child) { _childs << child; }
	void addStroke(KanjiStroke *stroke) { _strokes << stroke; }
	const QString &element() const { return _element; }
	const QString &original() const { return _original; }

	/**
	 * Returns the representation of this group (if any) according to whether we
	 * want its simplified representation or not.
	 */
	const QString &repr(bool simplified = false) const;
	/**
	 * Returns the unicode of this group (if any) according to whether we want
	 * its simplified representation or not.
	 */
	unsigned int unicode(bool simplified = false) const;
	unsigned int number() const { return _number; }
};

class KanjiStroke
{
private:
	const KanjiComponent *_parent;
	QChar _type;
	QString _path;

public:
	KanjiStroke(const KanjiComponent *parent, const QChar &type, const QString &path);
	virtual ~KanjiStroke();

	const KanjiComponent *parent() const { return _parent; }
	const QChar &type() const { return _type; }
	const QString &path() const { return _path; }
};

class Kanjidic2Entry : public Entry
{
	Q_OBJECT
public:
	class KanjiReading {
	private:
		QString _type;
		QString _reading;

	public:
		KanjiReading(const QString &type, const QString &reading) : _type(type), _reading(reading) {}
		const QString &type() const { return _type; }
		const QString &reading() const { return _reading; }
	};

	class KanjiMeaning {
	private:
		QString _lang;
		QString _meaning;

	public:
		KanjiMeaning(const QString &lang, const QString &meaning) : _lang(lang), _meaning(meaning) {}
		const QString &lang() const { return _lang; }
		const QString &meaning() const { return _meaning; }
	};

private:
	bool _inDB;
	QString _kanji;

	QList<KanjiReading> _readings;
	QList<KanjiMeaning> _meanings;
	QStringList _nanoris;
	QList<int> _variationOf;

	int _grade;
	int _strokeCount;
	int _kanjiFrequency;
	int _jlpt;
	QString _skip;

	/**
	 * Contains the components of the kanji, in their order of appearance.
	 * A kanji always contains at least one component, which is its root.
	 */
	QList<KanjiComponent> _components;

	/**
	 * Contains the strokes of the kanji, in their order of appearance.
	 */
	QList<KanjiStroke> _strokes;

protected:
	KanjiComponent *addComponent(const KanjiComponent *parent, const QString &element, const QString &original, unsigned int number);
	KanjiStroke *addStroke(const KanjiComponent *parent, const QChar &type, const QString &path);

public:
	// Needed to register type
	Kanjidic2Entry() : Entry() {}
	Kanjidic2Entry(const QString &kanji, bool inDB, int grade = -1, int strokeCount = -1, int kanjiFrequency = -1, int jlpt = -1);
	virtual ~Kanjidic2Entry() {}

	/**
	 * Returns whether this kanji was present in the database.
	 */
	bool inDB() const { return _inDB; }
	const QString &kanji() const { return _kanji; }
	int unicode() const { return id(); }
	const QString &skipCode() const { return _skip; }
	const QList<KanjiReading> &kanjiReadings() const { return _readings; }
	const QList<KanjiMeaning> &kanjiMeanings() const { return _meanings; }
	const QStringList &nanoris() const { return _nanoris; }

	const QList<KanjiComponent> &components() const { return _components; }
	const QList<KanjiStroke> &strokes() const { return _strokes; }
	const KanjiComponent *root() const { if (_components.isEmpty()) return 0; else return &_components[0]; }
	const QList<const KanjiComponent *> &rootComponents() const { return root()->components(); }

	/**
	 * Returns the grade of thi kanji. -1 means it does not have any.
	 */
	int grade() const { return _grade; }
	/**
	 * Returns the stroke count of thi kanji. -1 means the stroke count
	 * is unknown.
	 */
	int strokeCount() const { return _strokeCount; }
	/**
	 * Returns the frequency of this kanji. -1 means it has no frequency
	 * associated.
	 */
	int kanjiFrequency() const { return _kanjiFrequency; }
	/**
	 * Returns the jlpt level of this kanji. -1 means it is not part of
	 * any JLPT test.
	 */
	int jlpt() const { return _jlpt; }
	QString meaningsString() const;

	virtual QStringList writings() const;
	virtual QStringList readings() const;
	virtual QStringList meanings() const;
	virtual QString name() const { return kanji(); }

	QStringList onyomiReadings() const;
	QStringList kunyomiReadings() const;

	friend class Kanjidic2EntrySearcher;
};

#endif
