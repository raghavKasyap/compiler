#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "symbol_tableDef.h"

SymbolTable* initializeSymbolTable(int scope);
int computeHash(char* name);
void addSymbolToTable(SymbolTable* symbolTable, Symbol* symbol);
void initializeSymbol(TokenInfo* token, int offset, SymbolTag symbolTag);

#endif