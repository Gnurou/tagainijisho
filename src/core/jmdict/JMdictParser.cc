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

#include <QtDebug>

#include "core/jmdict/JMdictParser.h"

QRegularExpression
    JMdictParser::versionRegExp(" JMdict created: (\\d\\d\\d\\d\\-\\d\\d\\-\\d\\d) ");

JMdictParser::JMdictParser(const QStringList &langs)
    : languages(langs), gotVersion(false), posBitFieldsCount(0), fieldBitFieldsCount(0),
      miscBitFieldsCount(0), dialectBitFieldsCount(0) {}

static uint getFreqScore(const QString &code) {
    if (code == "news1")
        return 50;
    else if (code == "news2")
        return 10;
    else if (code == "ichi1")
        return 50;
    else if (code == "ichi2")
        return 10;
    else if (code == "spec1")
        return 50;
    else if (code == "spec2")
        return 10;
    else if (code == "gai1")
        return 50;
    else if (code == "gai2")
        return 10;
    else if (code.mid(0, 2) == "nf")
        return 51 - code.mid(2, 4).toInt();
    else {
        qDebug() << "Unknown frequency code" << code;
        return 0;
    }
}

bool JMdictParser::parse(QXmlStreamReader &reader) {
    DOCUMENT_BEGIN(reader)
    if (reader.tokenType() == QXmlStreamReader::DTD) {
        foreach (const QXmlStreamEntityDeclaration &decl, reader.entityDeclarations()) {
            const QString vString = decl.value().toString();
            const QString nString = decl.name().toString();
            entities[nString] = vString;
            reversedEntities[vString] = nString;
        }
    }
    TAG(JMdict)
    TAG_PRE(entry)
    JMdictItem entry;
    TAG_BEGIN(entry)
    TAG(ent_seq)
    CHARACTERS
    entry.id = TEXT.toInt();
    DONE ENDTAG TAG_PRE(k_ele) entry.kanji << JMdictKanjiWritingItem();
    JMdictKanjiWritingItem &kWriting = entry.kanji.last();
    TAG_BEGIN(k_ele)
    TAG(keb)
    CHARACTERS
    kWriting.writing = TEXT;
    DONE ENDTAG TAG(ke_pri) kWriting.frequency = getFreqScore(TEXT);
    entry.frequency += kWriting.frequency;
    ENDTAG
    ENDTAG
    TAG_PRE(r_ele)
    entry.kana << JMdictKanaReadingItem();
    JMdictKanaReadingItem &kReading = entry.kana.last();
    TAG_BEGIN(r_ele)
    TAG(reb)
    CHARACTERS
    kReading.reading = TEXT;
    DONE ENDTAG TAG(re_pri) kReading.frequency = getFreqScore(TEXT);
    entry.frequency += kReading.frequency;
    ENDTAG
    TAG_PRE(re_nokanji)
    kReading.noKanji = true;
    TAG_BEGIN(re_nokanji)
    ENDTAG
    TAG(re_restr)
    CHARACTERS
    QString writing(TEXT);
    // Find the index of the writing that matches the given parameter
    int idx = 0;
    foreach (const JMdictKanjiWritingItem &kWriting, entry.kanji) {
        if (kWriting.writing == writing)
            break;
        ++idx;
    }
    if (idx == entry.kanji.size()) {
        qDebug("Entry %d: re_restr without a matching keb", entry.id);
    } else
        kReading.restrictedTo << idx;
    DONE ENDTAG ENDTAG TAG_PRE(sense) entry.senses << JMdictSenseItem();
    JMdictSenseItem &sense = entry.senses.last();
    TAG_BEGIN(sense)
    TAG_PRE(gloss)
    QString lang;
    if (HAS_ATTR("xml:lang")) {
        lang = ATTR("xml:lang").mid(0, 2);
        // Turn the 3-letters convention into a 2-letters one for the
        // sake of consistency
        if (lang == "sp")
            lang = "es";
        else if (lang == "ge")
            lang = "de";
    } else
        lang = "en";
    bool shallInsert = languages.contains(lang);
    TAG_BEGIN(gloss)
    CHARACTERS
    if (shallInsert)
        sense.gloss[lang] << TEXT;
    DONE ENDTAG TAG(pos) QString key(reversedEntities[TEXT]);
    sense.pos << key;
    // If not met yet, calculate the bit field for this entity
    if (!posBitFields.contains(key))
        posBitFields[key] = posBitFieldsCount++;
    ENDTAG
    TAG(field)
    QString key(reversedEntities[TEXT]);
    sense.field << key;
    // If not met yet, calculate the bit field for this entity
    if (!fieldBitFields.contains(key))
        fieldBitFields[key] = fieldBitFieldsCount++;
    ENDTAG
    TAG(misc)
    QString key(reversedEntities[TEXT]);
    sense.misc << key;
    // If not met yet, calculate the bit field for this entity
    if (!miscBitFields.contains(key))
        miscBitFields[key] = miscBitFieldsCount++;
    ENDTAG
    TAG(dial)
    QString key(reversedEntities[TEXT]);
    sense.dialect << key;
    // If not met yet, calculate the bit field for this entity
    if (!dialBitFields.contains(key))
        dialBitFields[key] = dialectBitFieldsCount++;
    ENDTAG
    TAG(stagk)
    CHARACTERS
    QString writing(TEXT);
    // Find the index of the writing that matches the given parameter
    int idx = 0;
    foreach (const JMdictKanjiWritingItem &kWriting, entry.kanji) {
        if (kWriting.writing == writing)
            break;
        ++idx;
    }
    if (idx == entry.kanji.size()) {
        qDebug("Entry %d: stagk without a matching keb", entry.id);
    } else
        sense.restrictedToKanji << idx;
    DONE ENDTAG TAG(stagr) CHARACTERS QString reading(TEXT);
    // Find the index of the writing that matches the given parameter
    int idx = 0;
    foreach (const JMdictKanaReadingItem &kReading, entry.kana) {
        if (kReading.reading == reading)
            break;
        ++idx;
    }
    if (idx == entry.kana.size()) {
        qDebug("Entry %d: stagr without a matching reb", entry.id);
    } else
        sense.restrictedToKana << idx;
    DONE ENDTAG TAG(lsource) ENDTAG TAG_POST
        // We are not interested in senses for which there is no gloss!
        if (sense.gloss.isEmpty()) entry.senses.removeLast();
    DONE TAG_POST if (!onItemParsed(entry)) return false;
    DONE ENDTAG COMMENT if (!gotVersion) {
        QRegularExpressionMatch match = versionRegExp.match(TEXT);
        if (match.hasMatch()) {
            _dictVersion = match.captured(1);
            gotVersion = true;
        }
    }
    DONE DOCUMENT_END
}
