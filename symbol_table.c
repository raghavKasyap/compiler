#include "symbol_table.h"

SymbolTable* initializeSymbolTable(int scope,SymbolTable* global) {
    SymbolTable* symbolTable = (SymbolTable)malloc(sizeof(SymbolTable));
    symbolTable -> scope = scope;
    symbolTable->symbolsHashTable = (Symbol**)malloc(sizeof(Symbol*) * SYMBOL_TABLE_SIZE);
    symbolTable->globalTable = global;
    symbolTable->currentOffset = 0;
    return symbolTable;
}

int computeHash(char* name){
    int n = SYMBOL_TABLE_SIZE;
    unsigned long hash = 0;
    int c;

    while (c = *name++) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash % n;
}

void addSymbolToTable(SymbolTable* symbolTable, Symbol* symbol){
    Symbol** hash_Table = symbolTable -> symbolsHashTable;
    TokenInfo* tk_info = symbol -> token;
    char* name = tk_info -> lexeme;
    int hash_val = computeHash(name);
    Symbol* symbolList = hash_Table[hash_val];
    while(symbolList->next){
        symbolList = symbolList->next;
    }
    symbolList->next = symbol;
}

void initializeSymbol(TokenInfo* token, int offset, SymbolTag symbolTag){
    Symbol* symbol = (Symbol*)malloc(sizeof(Symbol));
    symbol->token = token;
    symbol->offset = offset;
    symbol->symbolTag = symbol->symbolTag;
    switch ((symbol->symbolTag))
    {
    case variableIdentifier:
        VariableIdentifier* varIdentifier = (VariableIdentifier*)malloc(sizeof(VariableIdentifier));
        //variableIdentifier->
        symbol->symbolRecord.variableIdentifier=
        break;
    
    default:
        break;
    }
    symbol->next = NULL;
}
