/*
 *  Copyright (C) 2009-2011  Alexandre Courbot
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

#include "sqlite3.h"
#include "sqlite/fts3_tokenizer.h"
#include "core/TextTools.h"
#include "sqlite/SQLite.h"

#include <QSet>
#include <QtDebug>
#include <QRegExp>

static QSet<QString> ignoredWords;
static QByteArray kanasConverted;

static void regexpFunc(sqlite3_context *context, int argc, sqlite3_value **argv)
{
	QString text(TextTools::hiragana2Katakana(QString::fromUtf8((const char *)sqlite3_value_text(argv[1]))));
	// Get the regexp referenced by the request
	QRegExp regexp = QRegExp((const char *)sqlite3_value_text(argv[0]));
	regexp.setCaseSensitivity(Qt::CaseInsensitive);

	bool res = text.contains(regexp);
	sqlite3_result_int(context, res);
}

/**
 * Returns a pseudo-random value which is biaised by the parameter given (which must
 * be between 0 and 100). The bigger the parameter, the biggest chances the generated
 * has to be low.
 */
static void biaised_random(sqlite3_context *context, int nParams, sqlite3_value **values)
{
	if (nParams != 1) {
		sqlite3_result_error(context, "Invalid number of arguments!", -1);
		return;
	}
	int score = sqlite3_value_int(values[0]);
//	int minVal = score == 0 ? 0 : qrand() % score;
	int res = qrand() % (101 - score);
	sqlite3_result_int(context, res);
}

typedef struct {
	QSet<int>* _set;
} uniquecount_aggr;

static void uniquecount_aggr_step(sqlite3_context *context, int nParams, sqlite3_value **values)
{
	uniquecount_aggr *aggr_struct = static_cast<uniquecount_aggr *>(sqlite3_aggregate_context(context, sizeof(uniquecount_aggr)));
	if (!aggr_struct->_set) aggr_struct->_set = new QSet<int>();
	for (int i = 0; i < nParams; i++) {
		if (sqlite3_value_type(values[i]) == SQLITE_NULL) continue;
		aggr_struct->_set->insert(sqlite3_value_int(values[i]));
	}
}

static void uniquecount_aggr_finalize(sqlite3_context *context)
{
	uniquecount_aggr *aggr_struct = static_cast<uniquecount_aggr *>(sqlite3_aggregate_context(context, sizeof(uniquecount_aggr)));
	int res = aggr_struct->_set->size();
	delete aggr_struct->_set;
	sqlite3_result_int(context, res);
}

static void fts_compress(sqlite3_context *context, int argc, sqlite3_value **argv)
{
	QByteArray text(reinterpret_cast<const char *>(sqlite3_value_text(argv[0])));
	QByteArray compressed(qCompress(text, 9));
	sqlite3_result_blob(context, compressed.data(), compressed.length(), 0);
}

static void fts_uncompress(sqlite3_context *context, int argc, sqlite3_value **argv)
{
	QByteArray data(static_cast<const char *>(sqlite3_value_blob(argv[0])), sqlite3_value_bytes(argv[0]));
	QByteArray text(qUncompress(data));
	sqlite3_result_text(context, text.data(), text.size(), 0);
}

int isToIgnore(const char *token)
{
	if (!strcmp(token, "a")) return true;
	if (!strcmp(token, "an")) return true;
	if (!strcmp(token, "to")) return true;
	if (!strcmp(token, "of")) return true;
	return false;
	//return ignoredWords.contains(token);
}

const char *hiraganasToKatakanas(const char *src)
{
	QString s(QString::fromUtf8(src));
//	qDebug("%x", s[0].unicode());
//	return src;
//	printf("%x %x %x\n", (unsigned char)src[0], (unsigned char)src[1], (unsigned char)src[2]);
	kanasConverted = TextTools::hiragana2Katakana(s).toUtf8();
//	qDebug() << src << kanasConverted;
	return kanasConverted.constData();
}

// Function to register a tokenizer
int register_tokenizer(sqlite3 *db, const char *zName, const sqlite3_tokenizer_module *p)
{
	int rc;
	sqlite3_stmt *pStmt;
	const char *zSql = "SELECT fts3_tokenizer(?, ?)";

	rc = sqlite3_prepare_v2(db, zSql, -1, &pStmt, 0);
	if( rc!=SQLITE_OK ){
		return rc;
	}

	sqlite3_bind_text(pStmt, 1, zName, -1, SQLITE_STATIC);
	sqlite3_bind_blob(pStmt, 2, &p, sizeof(p), SQLITE_STATIC);
	sqlite3_step(pStmt);

	return sqlite3_finalize(pStmt);
}

//
// Words ignore tokenizer.
// A replica of SQLite's simple tokenizer that ignores the words given as
// arguments. Useful to avoid indexing short words like "a", "the" and the
// like.
//
typedef struct ignore_tokenizer {
	sqlite3_tokenizer base;
	char delim[128];             /* flag ASCII delimiters */
} ignore_tokenizer;

typedef struct ignore_tokenizer_cursor {
  sqlite3_tokenizer_cursor base;
  const char *pInput;          /* input we are tokenizing */
  int nBytes;                  /* size of the input */
  int iOffset;                 /* current position in pInput */
  int iToken;                  /* index of next token to be returned */
  char *pToken;                /* storage for current token */
  int nTokenAllocated;         /* space allocated to zToken buffer */
} ignore_tokenizer_cursor;

static int ignoreDelim(ignore_tokenizer *t, unsigned char c){
  return c<0x80 && t->delim[c];
}

/*
** Create a new tokenizer instance.
*/
static int ignoreCreate(
  int argc, const char * const *argv,
  sqlite3_tokenizer **ppTokenizer
){
  ignore_tokenizer *t;

  t = (ignore_tokenizer *) sqlite3_malloc(sizeof(*t));
  if( t==NULL ) return SQLITE_NOMEM;
  memset(t, 0, sizeof(*t));

  /* TODO(shess) Delimiters need to remain the same from run to run,
  ** else we need to reindex.  One solution would be a meta-table to
  ** track such information in the database, then we'd only want this
  ** information on the initial create.
  */
  if( argc>1 ){
    int i, n = strlen(argv[1]);
    for(i=0; i<n; i++){
      unsigned char ch = argv[1][i];
      /* We explicitly don't support UTF-8 delimiters for now. */
      if( ch>=0x80 ){
        sqlite3_free(t);
        return SQLITE_ERROR;
      }
      t->delim[ch] = 1;
    }
  } else {
    /* Mark non-alphanumeric ASCII characters as delimiters */
    int i;
    for(i=1; i<0x80; i++){
      t->delim[i] = !isalnum(i);
    }
  }

  *ppTokenizer = &t->base;
  return SQLITE_OK;
}

/*
** Destroy a tokenizer
*/
static int ignoreDestroy(sqlite3_tokenizer *pTokenizer){
  sqlite3_free(pTokenizer);
  return SQLITE_OK;
}

/*
** Prepare to begin tokenizing a particular string.  The input
** string to be tokenized is pInput[0..nBytes-1].  A cursor
** used to incrementally tokenize this string is returned in 
** *ppCursor.
*/
static int ignoreOpen(
  sqlite3_tokenizer *pTokenizer,         /* The tokenizer */
  const char *pInput, int nBytes,        /* String to be tokenized */
  sqlite3_tokenizer_cursor **ppCursor    /* OUT: Tokenization cursor */
){
  ignore_tokenizer_cursor *c;

  c = (ignore_tokenizer_cursor *) sqlite3_malloc(sizeof(*c));
  if( c==NULL ) return SQLITE_NOMEM;

  c->pInput = pInput;
  if( pInput==0 ){
    c->nBytes = 0;
  }else if( nBytes<0 ){
    c->nBytes = (int)strlen(pInput);
  }else{
    c->nBytes = nBytes;
  }
  c->iOffset = 0;                 /* start tokenizing at the beginning */
  c->iToken = 0;
  c->pToken = NULL;               /* no space allocated, yet. */
  c->nTokenAllocated = 0;

  *ppCursor = &c->base;
  return SQLITE_OK;
}

/*
** Close a tokenization cursor previously opened by a call to
** simpleOpen() above.
*/
static int ignoreClose(sqlite3_tokenizer_cursor *pCursor){
  ignore_tokenizer_cursor *c = (ignore_tokenizer_cursor *) pCursor;
  sqlite3_free(c->pToken);
  sqlite3_free(c);
  return SQLITE_OK;
}

/*
** Extract the next token from a tokenization cursor.  The cursor must
** have been opened by a prior call to simpleOpen().
*/
static int ignoreNext(
  sqlite3_tokenizer_cursor *pCursor,  /* Cursor returned by simpleOpen */
  const char **ppToken,               /* OUT: *ppToken is the token text */
  int *pnBytes,                       /* OUT: Number of bytes in token */
  int *piStartOffset,                 /* OUT: Starting offset of token */
  int *piEndOffset,                   /* OUT: Ending offset of token */
  int *piPosition                     /* OUT: Position integer of token */
){
  ignore_tokenizer_cursor *c = (ignore_tokenizer_cursor *) pCursor;
  ignore_tokenizer *t = (ignore_tokenizer *) pCursor->pTokenizer;
  unsigned char *p = (unsigned char *)c->pInput;

  while( c->iOffset<c->nBytes ){
    int iStartOffset;

beginParse:
    /* Scan past delimiter characters */
    while( c->iOffset<c->nBytes && ignoreDelim(t, p[c->iOffset]) ){
      c->iOffset++;
    }

    /* Count non-delimiter characters. */
    iStartOffset = c->iOffset;
    while( c->iOffset<c->nBytes && !ignoreDelim(t, p[c->iOffset]) ){
      c->iOffset++;
    }

    if( c->iOffset>iStartOffset ){
      int i, n = c->iOffset-iStartOffset;
      if( n+1>c->nTokenAllocated ){
        c->nTokenAllocated = n+21;
        c->pToken = (char *)sqlite3_realloc(c->pToken, c->nTokenAllocated);
        if( c->pToken==NULL ) return SQLITE_NOMEM;
      }
      for(i=0; i<n; i++){
        /* TODO(shess) This needs expansion to handle UTF-8
        ** case-insensitivity.
        */
        unsigned char ch = p[iStartOffset+i];
        c->pToken[i] = ch<0x80 ? tolower(ch) : ch;
      }
      c->pToken[i] = 0;
	if (isToIgnore(c->pToken)) goto beginParse;
      *ppToken = c->pToken;
      *pnBytes = n;
      *piStartOffset = iStartOffset;
      *piEndOffset = c->iOffset;
      *piPosition = c->iToken++;

      return SQLITE_OK;
    }
  }
  return SQLITE_DONE;
}

/*
** The set of routines that implement the simple tokenizer
*/
static const sqlite3_tokenizer_module ignoreTokenizerModule = {
  0,
  ignoreCreate,
  ignoreDestroy,
  ignoreOpen,
  ignoreClose,
  ignoreNext,
};

//
// Hiranaga to katakana tokenizer
// The following provides a special tokenizer that converts all hiraganas into
// katakanas before returning a token. This is helpful to index Japanese
// pronunciations in a unique way.
//
typedef struct katakana_tokenizer {
	sqlite3_tokenizer base;
	char delim[128];             /* flag ASCII delimiters */
} katakana_tokenizer;

typedef struct katakana_tokenizer_cursor {
  sqlite3_tokenizer_cursor base;
  const char *pInput;          /* input we are tokenizing */
  int nBytes;                  /* size of the input */
  int iOffset;                 /* current position in pInput */
  int iToken;                  /* index of next token to be returned */
  char *pToken;                /* storage for current token */
  int nTokenAllocated;         /* space allocated to zToken buffer */
} katakana_tokenizer_cursor;

static int katakanaDelim(katakana_tokenizer *t, unsigned char c){
  return c<0x80 && t->delim[c];
}

/*
** Create a new tokenizer instance.
*/
static int katakanaCreate(
  int argc, const char * const *argv,
  sqlite3_tokenizer **ppTokenizer
){
  katakana_tokenizer *t;

  t = (katakana_tokenizer *) sqlite3_malloc(sizeof(*t));
  if( t==NULL ) return SQLITE_NOMEM;
  memset(t, 0, sizeof(*t));

  /* TODO(shess) Delimiters need to remain the same from run to run,
  ** else we need to reindex.  One solution would be a meta-table to
  ** track such information in the database, then we'd only want this
  ** information on the initial create.
  */
  if( argc>1 ){
    int i, n = strlen(argv[1]);
    for(i=0; i<n; i++){
      unsigned char ch = argv[1][i];
      /* We explicitly don't support UTF-8 delimiters for now. */
      if( ch>=0x80 ){
        sqlite3_free(t);
        return SQLITE_ERROR;
      }
      t->delim[ch] = 1;
    }
  } else {
    /* Mark non-alphanumeric ASCII characters as delimiters */
    int i;
    for(i=1; i<0x80; i++){
      t->delim[i] = !isalnum(i);
    }
  }

  *ppTokenizer = &t->base;
  return SQLITE_OK;
}

/*
** Destroy a tokenizer
*/
static int katakanaDestroy(sqlite3_tokenizer *pTokenizer){
  sqlite3_free(pTokenizer);
  return SQLITE_OK;
}

/*
** Prepare to begin tokenizing a particular string.  The input
** string to be tokenized is pInput[0..nBytes-1].  A cursor
** used to incrementally tokenize this string is returned in 
** *ppCursor.
*/
static int katakanaOpen(
  sqlite3_tokenizer *pTokenizer,         /* The tokenizer */
  const char *pInput, int nBytes,        /* String to be tokenized */
  sqlite3_tokenizer_cursor **ppCursor    /* OUT: Tokenization cursor */
){
  katakana_tokenizer_cursor *c;

  c = (katakana_tokenizer_cursor *) sqlite3_malloc(sizeof(*c));
  if( c==NULL ) return SQLITE_NOMEM;

  c->pInput = pInput;
  if( pInput==0 ){
    c->nBytes = 0;
  }else if( nBytes<0 ){
    c->nBytes = (int)strlen(pInput);
  }else{
    c->nBytes = nBytes;
  }
  c->iOffset = 0;                 /* start tokenizing at the beginning */
  c->iToken = 0;
  c->pToken = NULL;               /* no space allocated, yet. */
  c->nTokenAllocated = 0;

  *ppCursor = &c->base;
  return SQLITE_OK;
}

/*
** Close a tokenization cursor previously opened by a call to
** simpleOpen() above.
*/
static int katakanaClose(sqlite3_tokenizer_cursor *pCursor){
  katakana_tokenizer_cursor *c = (katakana_tokenizer_cursor *) pCursor;
  sqlite3_free(c->pToken);
  sqlite3_free(c);
  return SQLITE_OK;
}

/*
** Extract the next token from a tokenization cursor.  The cursor must
** have been opened by a prior call to simpleOpen().
*/
static int katakanaNext(
  sqlite3_tokenizer_cursor *pCursor,  /* Cursor returned by simpleOpen */
  const char **ppToken,               /* OUT: *ppToken is the token text */
  int *pnBytes,                       /* OUT: Number of bytes in token */
  int *piStartOffset,                 /* OUT: Starting offset of token */
  int *piEndOffset,                   /* OUT: Ending offset of token */
  int *piPosition                     /* OUT: Position integer of token */
){
  katakana_tokenizer_cursor *c = (katakana_tokenizer_cursor *) pCursor;
  katakana_tokenizer *t = (katakana_tokenizer *) pCursor->pTokenizer;
  unsigned char *p = (unsigned char *)c->pInput;

  while( c->iOffset<c->nBytes ){
    int iStartOffset;

    /* Scan past delimiter characters */
    while( c->iOffset<c->nBytes && katakanaDelim(t, p[c->iOffset]) ){
      c->iOffset++;
    }

    /* Count non-delimiter characters. */
    iStartOffset = c->iOffset;
    while( c->iOffset<c->nBytes && !katakanaDelim(t, p[c->iOffset]) ){
      c->iOffset++;
    }

    if( c->iOffset>iStartOffset ){
      int i, n = c->iOffset-iStartOffset;
      if( n+1>c->nTokenAllocated ){
        c->nTokenAllocated = n+21;
        c->pToken = (char *)sqlite3_realloc(c->pToken, c->nTokenAllocated);
        if( c->pToken==NULL ) return SQLITE_NOMEM;
      }
      for(i=0; i<n; i++){
        /* TODO(shess) This needs expansion to handle UTF-8
        ** case-insensitivity.
        */
        unsigned char ch = p[iStartOffset+i];
        c->pToken[i] = ch<0x80 ? tolower(ch) : ch;
      }
      c->pToken[i] = 0;
      *ppToken = hiraganasToKatakanas(c->pToken);
      *pnBytes = n;
      *piStartOffset = iStartOffset;
      *piEndOffset = c->iOffset;
      *piPosition = c->iToken++;

      return SQLITE_OK;
    }
  }
  return SQLITE_DONE;
}

/*
** The set of routines that implement the simple tokenizer
*/
static const sqlite3_tokenizer_module katakanaTokenizerModule = {
  0,
  katakanaCreate,
  katakanaDestroy,
  katakanaOpen,
  katakanaClose,
  katakanaNext,
};

static int load_extensions(sqlite3 *handler, const char **pzErrMsg,
	const struct sqlite3_api_routines *pThunk)
{
	sqlite3ext_register_functions(handler);

	return SQLITE_OK;
}

extern "C"
{

void sqlite3ext_init()
{
	sqlite3_auto_extension((void (*)())load_extensions);
}

int sqlite3ext_register_functions(sqlite3 *handler)
{
	// Attach custom functions
	sqlite3_create_function(handler, "regexp", 2, SQLITE_UTF8, 0, regexpFunc, 0, 0);
	sqlite3_create_function(handler, "biaised_random", 1, SQLITE_UTF8, 0, biaised_random, 0, 0);
	sqlite3_create_function(handler, "uniquecount", -1, SQLITE_UTF8, 0, 0, uniquecount_aggr_step, uniquecount_aggr_finalize);
	sqlite3_create_function(handler, "ftscompress", 1, SQLITE_UTF8, 0, fts_compress, 0, 0);
	sqlite3_create_function(handler, "ftsuncompress", 1, SQLITE_UTF8, 0, fts_uncompress, 0, 0);

	return SQLITE_OK;
}

int sqlite3ext_register_tokenizers(sqlite3 *handler)
{
	int err = register_tokenizer(handler, "katakana", &katakanaTokenizerModule);
	if (err) {
		qDebug("Could not register katakana tokenizer!");
		return err;
	}

	return SQLITE_OK;
}

}
