/*
 *  Copyright (C) 2009  Alexandre Courbot
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

// C exported functions defined in sqlite3ext.cc
int isToIgnore(const char *token);
const char *hiraganasToKatakanas(const char *src);

/// This is to fix what seems to be a bug in SQLite (?) 
SQLITE_API void tagaini_sqlite3_fix_activevdbecnt(sqlite3 *db)
{
    db->activeVdbeCnt = 0;
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
        c->pToken = sqlite3_realloc(c->pToken, c->nTokenAllocated);
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

/*
** Allocate a new simple tokenizer.  Return a pointer to the new
** tokenizer in *ppModule
*/
void sqlite3Fts3IgnoreTokenizerModule(
  sqlite3_tokenizer_module const**ppModule
){
  *ppModule = &ignoreTokenizerModule;
}

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
        c->pToken = sqlite3_realloc(c->pToken, c->nTokenAllocated);
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

/*
** Allocate a new simple tokenizer.  Return a pointer to the new
** tokenizer in *ppModule
*/
void sqlite3Fts3KatakanaTokenizerModule(
  sqlite3_tokenizer_module const**ppModule
){
  *ppModule = &katakanaTokenizerModule;
}