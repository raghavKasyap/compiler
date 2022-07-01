// Group 21 
// Raghava Kasyap Kristipati - 2019A7PS0087P
// K.V.S Preetam             - 2019A7PS0030P
// Shanmukh Chandra Yama     - 2019A7PS0028P
// Uday Dheeraj Nulu         - 2019A7PS0083P
// Yadagiri Shiva Sai Sashank - 2019A7PS0068P

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "symbol_tableDef.h"
#include "astDef.h"

SymbolTable* initializeSymbolTable(char* scope, SymbolTable* global);
int computeHash(char* name);
void addSymbolToTable(SymbolTable* symbolTable, SymbolInstance* symbol);
SymbolInstance* initializeSymbol(char* name, int offset, SymbolTag symbolTag);
int populateRecordTypeExpression(SymbolInstance* recordSymbol, ASTNode* astNode, SymbolTable* st);
SymbolInstance* createVariableSymbol(ASTNode* currParam, ASTNode* currParamDataType, SymbolTable* globalSymbolTable, SymbolTable* currSymbolTable);
SymbolTable* constructSymbolTableRecursive(ASTNode* astNode, SymbolTable* globalSymbolTable, SymbolTable* currSymbolTable);
SymbolTable* constructSymbolTable(ASTRoot* ast);
void printSymbolTable(SymbolTable* currSymbolTable);
void printTypeInfoFunction(SymbolInstance* symbol);
void printTypeInfoRecord(SymbolInstance* symbol);
void printSymbolTable(SymbolTable* currSymbolTable);
void printGlobalVariables(SymbolTable* currSymbolTable);
void printRecordVariables(SymbolTable* currSymbolTable);
#endif