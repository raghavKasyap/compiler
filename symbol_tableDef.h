#ifndef SYMBOL_TABLE_DEF_H
#define SYMBOL_TABLE_DEF_H

#include <stdbool.h>
#include "lexerDef.h"

#define SYMBOL_TABLE_SIZE 50;

typedef struct VariableIdentifier VariableIdentifier;
typedef struct SymbolTable SymbolTable;

typedef enum SymbolTag {
    variableIdentifier,
    functionIdentifier,
    recordTypeDefinition,
    unionTypeDefinition,
    functionParameters
} SymbolTag;

typedef enum Type {
    basic,
    recordOrUnion
} Type;

typedef enum PrimitiveType {
    INT,
    real
} PrimitiveType;

typedef struct BasicTypeExpression {
    PrimitiveType currType;
} BasicTypeExpression;

typedef struct ConstructedTypeInfoNodes {
    VariableIdentifier* variableInfo;
    char* name;
    struct ConstructedTypeInfoNodes* next;
} ConstructedTypeInfoNodes;

typedef struct CostructedTypeExpression {
    char* recordName;  // while comparing two variables this is used
    ConstructedTypeInfoNodes* typeExpression;  // while checking the fields of a record this is used
} CostructedTypeExpression;

typedef union TypeExpression {
    BasicTypeExpression typeExpressionBasic;
    CostructedTypeExpression typeExpressionConstructed;
} TypeExpression;

// Can be primitive and constructed
typedef struct VariableIdentifier {
    Type type;
    TypeExpression typeExpression;
} VariableIdentifier;

typedef struct FunctionIdentifier {
    SymbolTable* scopeOfTable; // when we have a function we must create a new symbol table whose address is stored here.
    ConstructedTypeInfoNodes* inputParameters;
    ConstructedTypeInfoNodes* outputParameters;
} FunctionIdentifier;

typedef struct RecordTypeDefinition {
    ConstructedTypeInfoNodes* typeExpression; // this will be inherited by the declared variables
} RecordTypeDefinition;

typedef struct UnionTypeDefinition {
    ConstructedTypeInfoNodes* typeExpression; // this will be inherited by the declared variables
} UnionTypeDefinition;

// Not implementing this as of now.
// typedef struct FunctionParameters {

// } FunctionParameters;

typedef union SymbolRecord {
    VariableIdentifier variableIdentifier;
    FunctionIdentifier functionIdentifier;
    RecordTypeDefinition recordTypeDefinition;
    UnionTypeDefinition unionTypeDefinition;
    // FunctionParameters functionParameters;
} SymbolRecord;

typedef struct Symbol {
    TokenInfo* token;
    int offset; 
    SymbolTag symbolTag;
    SymbolRecord symbolRecord;
    struct Symbol* next;
} Symbol;

typedef struct SymbolTable {
    Symbol** symbolsHashTable;
    int currentOffset;
    int scope; // for making the hashtable dynamic
    struct SymbolTable* globalTable; // storing the address of global symobl table to acces global attributes (variables and records and typedefinitions).
} SymbolTable;

#endif
