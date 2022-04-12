#include "symbol_table.h"
#include "ast.h"

SymbolTable* initializeSymbolTable(int scope,SymbolTable* global) {
    SymbolTable* symbolTable = (SymbolTable) malloc(sizeof(SymbolTable));
    symbolTable -> scope = scope;
    symbolTable -> symbolsHashTable = (SymbolInstance**) malloc(sizeof(SymbolInstance*) * SYMBOL_TABLE_SIZE);
    symbolTable -> globalTable = global;
    symbolTable -> currentOffset = 0;
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

void addSymbolToTable(SymbolTable* symbolTable, SymbolInstance* symbol){
    SymbolInstance** hash_Table = symbolTable -> symbolsHashTable;
    TokenInfo* tk_info = symbol -> token;
    char* name = tk_info -> lexeme;
    int hash_val = computeHash(name);
    SymbolInstance* symbolList = hash_Table[hash_val];
    while(symbolList->next){
        symbolList = symbolList->next;
    }
    symbolList->next = symbol;
}

void initializeSymbol(TokenInfo* token, int offset, SymbolTag symbolTag){
    SymbolInstance* symbol = (SymbolInstance*)malloc(sizeof(SymbolInstance));
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


SymbolTable* constructSymbolTable(ASTRoot* ast) {
    // Plan
    // For each function we have 
    // we are initializing the global symbol table
    SymbolTable* globalSymbolTable = initializeSymbolTable(0, NULL);

    return globalSymbolTable;
}