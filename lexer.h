#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "lexerDef.h"

int getStream(FILE *fp);
char getnextchar(FILE *fp);
void read_new_token();
void retract();
void initialize_Symbol_Table();

TokenInfo *insertEntry(Symbol_Table_Record **Symbol_Table, char *lexeme, int linenum, char *value, int token_id);
Symbol_Table_Record *lookup(Symbol_Table_Record **Symbol_Table, char *lexeme);
int hash_function(char *string);

TokenInfo *createToken(char *lexeme, int linenum, char *val, int token_id, bool isError);
char *getsubstring(char *start, char *end);
TokenInfo *getNextToken(FILE *fp);

void removeComments(FILE *fp, FILE *clearnFp);
void appendlastchar(char* fileName);

#endif