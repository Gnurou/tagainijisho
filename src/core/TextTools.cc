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

#include <QtDebug>

#include "TextTools.h"

#define UNICODE_HIRAGANA_BEGIN 0x3040
#define UNICODE_HIRAGANA_END 0x309F

#define UNICODE_KATAKANA_BEGIN 0x30A0
#define UNICODE_KATAKANA_END 0x30FF

#define UNICODE_KATAKANA_EXT_BEGIN 0x31F0
#define UNICODE_KATAKANA_EXT_END 0x31FF

#define UNICODE_CJK_EXTA_BEGIN 0x3400
#define UNICODE_CJK_EXTA_END 0x4DBF
#define UNICODE_CJK_BEGIN 0x4E00
#define UNICODE_CJK_END 0x9FC3
#define UNICODE_CJK_COMPAT_BEGIN 0xF900
#define UNICODE_CJK_COMPAT_END 0xFAD9
#define UNICODE_CJK_EXTB_BEGIN 0x20000
#define UNICODE_CJK_EXTB_END 0x2A6DF
#define UNICODE_CJK_RADICALS_BEGIN 0x2E80
#define UNICODE_CJK_RADICALS_END 0x2EFF

#define UNICODE_PUNCTUATION_BEGIN 0x3000
#define UNICODE_PUNCTUATION_END 0x303f

namespace TextTools {

QString escapeForRegexp(const QString &string)
{
	QString ret = string;

	ret.replace('?', "\\w");
	ret.replace('*', "\\w*");
	return "\\b" + ret + "\\b";
}

bool isHiraganaChar(const QChar c) {
	return c >= UNICODE_HIRAGANA_BEGIN && c <= UNICODE_HIRAGANA_END;
}

bool isKatakanaChar(const QChar c) {
	return ( (c >= UNICODE_KATAKANA_BEGIN && c <= UNICODE_KATAKANA_END) ||
			 (c >= UNICODE_KATAKANA_EXT_BEGIN && c <= UNICODE_KATAKANA_EXT_END));
}

bool isPunctuationChar(const QChar c) {
	return c >= UNICODE_PUNCTUATION_BEGIN && c <= UNICODE_PUNCTUATION_END;
}

bool isKanaChar(const QChar c) {
	return (isHiraganaChar(c) || isKatakanaChar(c));
}
bool isKanjiChar(const uint c) {
		return ( (c >= UNICODE_CJK_BEGIN && c <= UNICODE_CJK_END) ||
				 (c >= UNICODE_CJK_COMPAT_BEGIN && c <= UNICODE_CJK_COMPAT_END) ||
				 (c >= UNICODE_CJK_EXTA_BEGIN && c <= UNICODE_CJK_EXTA_END) ||
				 (c >= UNICODE_CJK_RADICALS_BEGIN && c <= UNICODE_CJK_RADICALS_END) ||
				 (c >= UNICODE_CJK_EXTB_BEGIN && c <= UNICODE_CJK_EXTB_END) );
}

bool isKanjiChar(const QChar c, const QChar c2) {
	// Do we have a surrogate character here?
	if (!c.isHighSurrogate()) return isKanjiChar(c.unicode());
	// If we do, the second argument must be filled!
	else return isKanjiChar(QChar::surrogateToUcs4(c, c2));
}

bool isKanjiChar(const QString &s, int pos)
{
	QChar c;
	QChar c2;
	if (s.isEmpty() || pos >= s.size())
		return false;
	c = s[pos];
	if (c.isHighSurrogate() && pos < s.size() - 1) c2 = s[pos + 1];
	return isKanjiChar(c, c2);
}

bool isJapaneseChar(const QChar c, const QChar c2) {
	return isKanaChar(c) || isKanjiChar(c, c2) || isPunctuationChar(c);
}

bool isJapaneseChar(const QString &s, int pos) {
	if (s.isEmpty() || pos >= s.size())
		return false;
	return isKanaChar(s[pos]) || isKanjiChar(s, pos) || isPunctuationChar(s[pos]);
}

bool isRomajiChar(const QChar c) {
	return c <= 255;
}

bool isHiragana(const QString & string)
{
	for (int i = 0; i < string.size(); i++)
		if (!(isHiraganaChar(string[i]) || string[i].isPunct())) return false;
	return true;
}

bool isKatakana(const QString & string)
{
	for (int i = 0; i < string.size(); i++)
		if (!isKatakanaChar(string[i]) || string[i].isPunct()) return false;
	return true;
}

bool isKana(const QString & string)
{
	for (int i = 0; i < string.size(); i++)
		if (!isKanaChar(string[i]) || string[i].isPunct()) return false;
	return true;
}

bool isKanji(const QString & string)
{
	for (int i = 0; i < string.size(); i++) {
		const QChar &c = string[i];
		if (!isKanjiChar(string, i) || c.isPunct()) return false;
		if (c.isHighSurrogate()) i++;
	}
	return true;
}

bool isJapanese(const QString & string)
{
	for (int i = 0; i < string.size(); i++) {
		const QChar &c = string[i];
		if (!isJapaneseChar(string, i) || string[i].isPunct()) return false;
		if (c.isHighSurrogate()) i++;
	}
	return true;
}

bool isRomaji(const QString & string)
{
	for (int i = 0; i < string.size(); i++) {
		if (!isRomajiChar(string[i]) || string[i].isPunct()) return false;
	}
	return true;
}

KanaTable hiraganaTable = {
	{ QChar(0x3042), QChar(0x3044), QChar(0x3046), QChar(0x3048), QChar(0x304a) }, // あ
	{ QChar(0x304b), QChar(0x304d), QChar(0x304f), QChar(0x3051), QChar(0x3053) }, // か
	{ QChar(0x304c), QChar(0x304e), QChar(0x3050), QChar(0x3052), QChar(0x3054) }, // が
	{ QChar(0x3055), QChar(0x3057), QChar(0x3059), QChar(0x305b), QChar(0x305d) }, // さ
	{ QChar(0x3056), QChar(0x3058), QChar(0x305a), QChar(0x305c), QChar(0x305e) }, // ざ
	{ QChar(0x305f), QChar(0x3061), QChar(0x3064), QChar(0x3066), QChar(0x3068) }, // た
	{ QChar(0x3060), QChar(0x3062), QChar(0x3065), QChar(0x3067), QChar(0x3069) }, // だ
	{ QChar(0x306a), QChar(0x306b), QChar(0x306c), QChar(0x306d), QChar(0x306e) }, // な
	{ QChar(0x306f), QChar(0x3072), QChar(0x3075), QChar(0x3078), QChar(0x307b) }, // は
	{ QChar(0x3070), QChar(0x3073), QChar(0x3076), QChar(0x3079), QChar(0x307c) }, // ば
	{ QChar(0x3071), QChar(0x3074), QChar(0x3077), QChar(0x307a), QChar(0x307d) }, // ぱ
	{ QChar(0x307e), QChar(0x307f), QChar(0x3080), QChar(0x3081), QChar(0x3082) }, // ま
	{ QChar(0x3084), QChar(0x0000), QChar(0x3086), QChar(0x0000), QChar(0x3088) }, // や
	{ QChar(0x3089), QChar(0x308a), QChar(0x308b), QChar(0x308c), QChar(0x308d) }, // ら
	{ QChar(0x308f), QChar(0x3090), QChar(0x0000), QChar(0x3091), QChar(0x3092) }, // わ
	{ QChar(0x0000), QChar(0x0000), QChar(0x3094), QChar(0x0000), QChar(0x0000) }, // う゛
	{ QChar(0x3093), QChar(0x0000), QChar(0x0000), QChar(0x0000), QChar(0x0000) }, // ん
};

KanaTable katakanaTable = {
	{ QChar(0x30a2), QChar(0x30a4), QChar(0x30a6), QChar(0x30a8), QChar(0x30aa) }, // ア
	{ QChar(0x30ab), QChar(0x30ad), QChar(0x30af), QChar(0x30b1), QChar(0x30b3) }, // カ
	{ QChar(0x30ac), QChar(0x30ae), QChar(0x30b0), QChar(0x30b2), QChar(0x30b4) }, // ガ
	{ QChar(0x30b5), QChar(0x30b7), QChar(0x30b9), QChar(0x30bb), QChar(0x30bd) }, // サ
	{ QChar(0x30b6), QChar(0x30b8), QChar(0x30ba), QChar(0x30bc), QChar(0x30be) }, // ザ
	{ QChar(0x30bf), QChar(0x30c1), QChar(0x30c4), QChar(0x30c6), QChar(0x30c8) }, // タ
	{ QChar(0x30c0), QChar(0x30c2), QChar(0x30c5), QChar(0x30c7), QChar(0x30c9) }, // ダ
	{ QChar(0x30ca), QChar(0x30cb), QChar(0x30cc), QChar(0x30cd), QChar(0x30ce) }, // ナ
	{ QChar(0x30cf), QChar(0x30d2), QChar(0x30d5), QChar(0x30d8), QChar(0x30db) }, // ハ
	{ QChar(0x30d0), QChar(0x30d3), QChar(0x30d6), QChar(0x30d9), QChar(0x30dc) }, // バ
	{ QChar(0x30d1), QChar(0x30d4), QChar(0x30d7), QChar(0x30da), QChar(0x30dd) }, // パ
	{ QChar(0x30de), QChar(0x30df), QChar(0x30e0), QChar(0x30e1), QChar(0x30e2) }, // マ
	{ QChar(0x30e4), QChar(0x0000), QChar(0x30e6), QChar(0x0000), QChar(0x30e8) }, // ヤ
	{ QChar(0x30e9), QChar(0x30ea), QChar(0x30eb), QChar(0x30ec), QChar(0x30ed) }, // ラ
	{ QChar(0x30ef), QChar(0x30f0), QChar(0x0000), QChar(0x30f1), QChar(0x30f2) }, // ワ
	{ QChar(0x30f7), QChar(0x30f8), QChar(0x30f4), QChar(0x30f9), QChar(0x30fa) }, // ヴ
	{ QChar(0x30f3), QChar(0x0000), QChar(0x0000), QChar(0x0000), QChar(0x0000) }, // ン
};

static QMap<QChar, KanaInfo> _kanaInfos;

static void initKanaInfos()
{
	// Hiragana
	_kanaInfos[QChar(0x3041)] = KanaInfo("a", KanaInfo::Small);
	_kanaInfos[QChar(0x3043)] = KanaInfo("i", KanaInfo::Small);
	_kanaInfos[QChar(0x3045)] = KanaInfo("u", KanaInfo::Small);
	_kanaInfos[QChar(0x3047)] = KanaInfo("e", KanaInfo::Small);
	_kanaInfos[QChar(0x3049)] = KanaInfo("o", KanaInfo::Small);

	_kanaInfos[QChar(0x3042)] = KanaInfo("a");
	_kanaInfos[QChar(0x3044)] = KanaInfo("i");
	_kanaInfos[QChar(0x3046)] = KanaInfo("u");
	_kanaInfos[QChar(0x3048)] = KanaInfo("e");
	_kanaInfos[QChar(0x304a)] = KanaInfo("o");

	_kanaInfos[QChar(0x3095)] = KanaInfo("ka", KanaInfo::Small);
	_kanaInfos[QChar(0x304b)] = KanaInfo("ka");
	_kanaInfos[QChar(0x304d)] = KanaInfo("ki");
	_kanaInfos[QChar(0x304f)] = KanaInfo("ku");
	_kanaInfos[QChar(0x3096)] = KanaInfo("ke", KanaInfo::Small);
	_kanaInfos[QChar(0x3051)] = KanaInfo("ke");
	_kanaInfos[QChar(0x3053)] = KanaInfo("ko");

	_kanaInfos[QChar(0x304c)] = KanaInfo("ga");
	_kanaInfos[QChar(0x304e)] = KanaInfo("gi");
	_kanaInfos[QChar(0x3050)] = KanaInfo("gu");
	_kanaInfos[QChar(0x3052)] = KanaInfo("ge");
	_kanaInfos[QChar(0x3054)] = KanaInfo("go");

	_kanaInfos[QChar(0x3055)] = KanaInfo("sa");
	_kanaInfos[QChar(0x3057)] = KanaInfo("shi");
	_kanaInfos[QChar(0x3059)] = KanaInfo("su");
	_kanaInfos[QChar(0x305b)] = KanaInfo("se");
	_kanaInfos[QChar(0x305d)] = KanaInfo("so");

	_kanaInfos[QChar(0x3056)] = KanaInfo("za");
	_kanaInfos[QChar(0x3058)] = KanaInfo("ji");
	_kanaInfos[QChar(0x305a)] = KanaInfo("zu");
	_kanaInfos[QChar(0x305c)] = KanaInfo("ze");
	_kanaInfos[QChar(0x305e)] = KanaInfo("zo");

	_kanaInfos[QChar(0x305f)] = KanaInfo("ta");
	_kanaInfos[QChar(0x3061)] = KanaInfo("chi");
	_kanaInfos[QChar(0x3063)] = KanaInfo("tsu", KanaInfo::Small);
	_kanaInfos[QChar(0x3064)] = KanaInfo("tsu");
	_kanaInfos[QChar(0x3066)] = KanaInfo("te");
	_kanaInfos[QChar(0x3068)] = KanaInfo("to");

	_kanaInfos[QChar(0x3060)] = KanaInfo("da");
	_kanaInfos[QChar(0x3062)] = KanaInfo("dji");
	_kanaInfos[QChar(0x3065)] = KanaInfo("dzu");
	_kanaInfos[QChar(0x3067)] = KanaInfo("de");
	_kanaInfos[QChar(0x3069)] = KanaInfo("do");

	_kanaInfos[QChar(0x306a)] = KanaInfo("na");
	_kanaInfos[QChar(0x306b)] = KanaInfo("ni");
	_kanaInfos[QChar(0x306c)] = KanaInfo("nu");
	_kanaInfos[QChar(0x306d)] = KanaInfo("ne");
	_kanaInfos[QChar(0x306e)] = KanaInfo("no");

	_kanaInfos[QChar(0x306f)] = KanaInfo("ha");
	_kanaInfos[QChar(0x3072)] = KanaInfo("hi");
	_kanaInfos[QChar(0x3075)] = KanaInfo("fu");
	_kanaInfos[QChar(0x3078)] = KanaInfo("he");
	_kanaInfos[QChar(0x307b)] = KanaInfo("ho");

	_kanaInfos[QChar(0x3070)] = KanaInfo("ba");
	_kanaInfos[QChar(0x3073)] = KanaInfo("bi");
	_kanaInfos[QChar(0x3076)] = KanaInfo("bu");
	_kanaInfos[QChar(0x3079)] = KanaInfo("be");
	_kanaInfos[QChar(0x307c)] = KanaInfo("bo");

	_kanaInfos[QChar(0x3071)] = KanaInfo("pa");
	_kanaInfos[QChar(0x3074)] = KanaInfo("pi");
	_kanaInfos[QChar(0x3077)] = KanaInfo("pu");
	_kanaInfos[QChar(0x307a)] = KanaInfo("pe");
	_kanaInfos[QChar(0x307d)] = KanaInfo("po");

	_kanaInfos[QChar(0x307e)] = KanaInfo("ma");
	_kanaInfos[QChar(0x307f)] = KanaInfo("mi");
	_kanaInfos[QChar(0x3080)] = KanaInfo("mu");
	_kanaInfos[QChar(0x3081)] = KanaInfo("me");
	_kanaInfos[QChar(0x3082)] = KanaInfo("mo");

	_kanaInfos[QChar(0x3083)] = KanaInfo("ya", KanaInfo::Small);
	_kanaInfos[QChar(0x3084)] = KanaInfo("ya");
	_kanaInfos[QChar(0x3085)] = KanaInfo("yu", KanaInfo::Small);
	_kanaInfos[QChar(0x3086)] = KanaInfo("yu");
	_kanaInfos[QChar(0x3087)] = KanaInfo("yo", KanaInfo::Small);
	_kanaInfos[QChar(0x3088)] = KanaInfo("yo");

	_kanaInfos[QChar(0x3089)] = KanaInfo("ra");
	_kanaInfos[QChar(0x308a)] = KanaInfo("ri");
	_kanaInfos[QChar(0x308b)] = KanaInfo("ru");
	_kanaInfos[QChar(0x308c)] = KanaInfo("re");
	_kanaInfos[QChar(0x308d)] = KanaInfo("ro");

	_kanaInfos[QChar(0x308e)] = KanaInfo("wa", KanaInfo::Small);
	_kanaInfos[QChar(0x308f)] = KanaInfo("wa");
	_kanaInfos[QChar(0x3090)] = KanaInfo("wi", KanaInfo::Normal, KanaInfo::Rare);
	_kanaInfos[QChar(0x3091)] = KanaInfo("we", KanaInfo::Normal, KanaInfo::Rare);
	_kanaInfos[QChar(0x3092)] = KanaInfo("wo");

	_kanaInfos[QChar(0x3094)] = KanaInfo("vu", KanaInfo::Normal, KanaInfo::Rare);

	_kanaInfos[QChar(0x3093)] = KanaInfo("n");

	// Katakana
	// Lazy loop for katakana that have a hiragana equivalent
	foreach (const QChar hira, _kanaInfos.keys()) {
		_kanaInfos[hiraganaChar2Katakana(hira)] = _kanaInfos[hira];
	}
	// Other kanatana
	_kanaInfos[QChar(0x30f7)] = KanaInfo("va", KanaInfo::Normal, KanaInfo::Rare);
	_kanaInfos[QChar(0x30f8)] = KanaInfo("vi", KanaInfo::Normal, KanaInfo::Rare);
	_kanaInfos[QChar(0x30f9)] = KanaInfo("ve", KanaInfo::Normal, KanaInfo::Rare);
	_kanaInfos[QChar(0x30fa)] = KanaInfo("vo", KanaInfo::Normal, KanaInfo::Rare);
	_kanaInfos[QChar(0x30f0)] = KanaInfo("wi", KanaInfo::Normal, KanaInfo::Rare);
	_kanaInfos[QChar(0x30f1)] = KanaInfo("we", KanaInfo::Normal, KanaInfo::Rare);
	_kanaInfos[QChar(0x30f2)] = KanaInfo("wo", KanaInfo::Normal, KanaInfo::Rare);
}

int kanasTableRow(const QChar c)
{
	for (unsigned int i = 0; i < KANASTABLE_NBROWS; i++) {
		if (c < hiraganaTable[i][0] || c > hiraganaTable[i][4]) continue;
		for (int j = 0; j < 5; j++) {
			if (c == hiraganaTable[i][j]) return i;
		}
	}
	return -1;
}

const KanaInfo &kanaInfo(const QChar c)
{
	if (_kanaInfos.isEmpty()) initKanaInfos();
	return _kanaInfos[c];
}

QChar hiraganaChar2Katakana(const QChar hira)
{
	if (!isHiraganaChar(hira)) return hira;
	ushort code(hira.unicode());
	// These special characters should not be converted
	if (code >= 0x3099 && code <= 0x309c) return hira;
	return QChar(code + 0x60);
}

QString hiragana2Katakana(const QString &hira)
{
	QString kata(hira.size());
	for (int i = 0; i < hira.size(); i++) {
		if (isHiraganaChar(hira[i])) kata[i] = hiraganaChar2Katakana(hira[i]);
		else kata[i] = hira[i];
	}
	return kata;
}

QString unicodeToSingleChar(unsigned int unicode)
{
	QString ret;
	if (unicode) {
		if (unicode > 0x10000) {
			ret = QChar(QChar::highSurrogate(unicode));
			ret += QChar(QChar::lowSurrogate(unicode));
		}
		else ret = QChar(unicode);
	}
	return ret;
}

unsigned int singleCharToUnicode(const QString &chr, int pos)
{
	if (chr.size() <= pos) return 0;
	if (!chr[pos].isHighSurrogate()) return chr[pos].unicode();
	else if (chr.size() >= pos + 2 && chr[pos + 1].isLowSurrogate()) return QChar::surrogateToUcs4(chr[pos], chr[pos + 1]);
	else return 0;
}

static QMap<QString, QString> __kanaTranscribe()
{
	QMap<QString, QString> ret;
#define T(a, b) ret.insert(a, QString::fromUtf8(b));
	T("a", "ア");
	T("i", "イ");
	T("u", "ウ");
	T("e", "エ");
	T("o", "オ");
	T("ka", "カ");
	T("ki", "キ");
	T("ku", "ク");
	T("ke", "ケ");
	T("ko", "コ");
	T("ga", "ガ");
	T("gi", "ギ");
	T("gu", "グ");
	T("ge", "ゲ");
	T("go", "ゴ");
	T("sa", "サ");
	T("si", "シ");
	T("shi", "シ");
	T("su", "ス");
	T("se", "セ");
	T("so", "ソ");
	T("za", "ザ");
	T("zi", "ジ");
	T("ji", "ジ");
	T("zu", "ズ");
	T("ze", "ゼ");
	T("zo", "ゾ");
	T("ta", "タ");
	T("ti", "チ");
	T("chi", "チ");
	T("tu", "ツ");
	T("tsu", "ツ");
	T("te", "テ");
	T("to", "ト");
	T("da", "ダ");
	T("di", "ヂ");
	T("du", "ヅ");
	T("de", "デ");
	T("do", "ド");
	T("na", "ナ");
	T("ni", "ニ");
	T("nu", "ヌ");
	T("ne", "ネ");
	T("no", "ノ");
	T("ha", "ハ");
	T("hi", "ヒ");
	T("hu", "フ");
	T("fu", "フ");
	T("he", "ヘ");
	T("ho", "ホ");
	T("ba", "バ");
	T("bi", "ビ");
	T("bu", "ブ");
	T("be", "ベ");
	T("bo", "ボ");
	T("pa", "パ");
	T("pi", "ピ");
	T("pu", "プ");
	T("pe", "ペ");
	T("po", "ポ");
	T("ma", "マ");
	T("mi", "ミ");
	T("mu", "ム");
	T("me", "メ");
	T("mo", "モ");
	T("ya", "ヤ");
	T("yu", "ユ");
	T("yo", "ヨ");
	T("ra", "ラ");
	T("ri", "リ");
	T("ru", "ル");
	T("re", "レ");
	T("ro", "ロ");
	T("wa", "ワ");
	T("wi", "ウィ");
	T("wu", "ウ");
	T("we", "ウェ");
	T("wo", "ヲ");
	T("-", "ー");
	T("kya", "キャ");
	T("kyu", "キュ");
	T("kyo", "キョ");
	T("gya", "ギャ");
	T("gyu", "ギュ");
	T("gyo", "ギョ");
	T("sha", "シャ");
	T("shu", "シュ");
	T("sho", "ショ");
	T("ja", "ジャ");
	T("ju", "ジュ");
	T("jo", "ジョ");
	T("cha", "チャ");
	T("chu", "チュ");
	T("cho", "チョ");
	T("dja", "ヂャ");
	T("dju", "ヂュ");
	T("djo", "ヂョ");
	T("nya", "ニャ");
	T("nyu", "ニュ");
	T("nyo", "ニョ");
	T("hya", "ヒャ");
	T("hyu", "ヒュ");
	T("hyo", "ヒョ");
	T("bya", "ビャ");
	T("byu", "ビュ");
	T("byo", "ビョ");
	T("pya", "ピャ");
	T("pyu", "ピュ");
	T("pyo", "ピョ");
	T("mya", "ミャ");
	T("myu", "ミュ");
	T("myo", "ミョ");
	T("rya", "リャ");
	T("ryu", "リュ");
	T("ryo", "リョ");
	T("la", "ァ");
	T("li", "ィ");
	T("lu", "ゥ");
	T("le", "ェ");
	T("lo", "ォ");
#undef TR

	return ret;
}

static QSet<QChar> __nodouble()
{
	QSet<QChar> ret;

	ret << 'a' << 'i' << 'u' << 'e' << 'o' << 'y' << 'n';

	return ret;
}

static const QMap<QString, QString> kanaTranscribe(__kanaTranscribe());
static const QSet<QChar> nodouble(__nodouble());

QString romajiToKana(const QString &src)
{
	static QString nn = QString::fromUtf8("ン");
	static QString tt = QString::fromUtf8("ッ");
	QString ret;
	int i;

	for (i = 0; i < src.size();) {
		QString part = src.mid(i);
		int p = i;

		if (part.size() > 1 && part[0] == part[1] && !nodouble.contains(part[0])) {
			ret += tt;
			i += 1;
		}
		if (p != i) continue;
		foreach (const QString &roma, kanaTranscribe.keys()) {
			if (part.startsWith(roma)) {
				ret += kanaTranscribe[roma];
				i += roma.size();
				break;
			}
		}
		if (p != i) continue;
		if (part.startsWith("n")) {
			ret += nn;
			i += 1;
			if (part.size() > 1 && part[1] == 'n') i += 1;
		}
		if (p != i) continue;
		if (isPunctuationChar(part[0]) || part[0] == '*') {
			ret += part[0];
			i += 1;
			continue;
		}
		if (p != i) continue;
		// Did not match, return empty string
		return "";
	}
	return ret;
}

}
