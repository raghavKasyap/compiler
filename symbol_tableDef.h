// Group 21 
// Raghava Kasyap Kristipati - 2019A7PS0087P
// K.V.S Preetam             - 2019A7PS0030P
// Shanmukh Chandra Yama     - 2019A7PS0028P
// Uday Dheeraj Nulu         - 2019A7PS0083P
// Yadagiri Shiva Sai Sashank - 2019A7PS0068P

#ifndef SYMBOL_TABLE_DEF_H
#define SYMBOL_TABLE_DEF_H

#include <stdbool.h>
#include "lexerDef.h"

#define SYMBOL_TABLE_SIZE 50

typedef struct VariableIdentifier VariableIdentifier;
typedef struct SymbolTable SymbolTable;
typedef struct SymbolInstance SymbolInstance;

typedef enum SymbolTag {
    variableIdentifier,
    functionIdentifier,
    recordTypeDefinition,
    unionTypeDefinition,
    functionParameters,
    typedef_Definition,
} SymbolTag;

// typedef enum NumOrRNumType{
//     Num,
//     RNum,
// } NumOrRNumType;

// typedef enum ArithmeticTypeTag{
//     VariableIdtag,
//     NumOrRNumtag,
// } ArithmeticTypeTag;

// typedef struct NumOrRNum{
//     NumOrRNumType type;
// } NumOrRNum;

typedef enum Type {
    basic,
    recordOrUnion
} Type;

typedef enum PrimitiveType {
    INT,
    real
} PrimitiveType;

typedef enum ConstructedType{
    UNION,
    RECORD,
    TAGGED_RECORD,
    TYPE_DEF,
}ConstructedType;

typedef struct BasicTypeExpression {
    PrimitiveType currType;
} BasicTypeExpression;


typedef struct ConstructedTypeInfoNodes {
    VariableIdentifier* variableInfo;
    char* name;
    struct ConstructedTypeInfoNodes* next;
} ConstructedTypeInfoNodes;

typedef struct ConstructedTypeExpression {
    ConstructedType currType;
    char* recordName;  // while comparing two variables this is used
    int width; // can be calculated by underlying expression.
    ConstructedTypeInfoNodes* typeExpression;  // while checking the fields of a record this is used
} ConstructedTypeExpression;

typedef union TypeExpression {
    BasicTypeExpression* typeExpressionBasic;
    ConstructedTypeExpression* typeExpressionConstructed;
} TypeExpression;

// typedef union ArithmeticExpression{
//     VariableIdentifier* variableIdentifier;
//      NumOrRNum* numOrRNum;
// } ArithmeticExpression;

typedef struct ArithmeticTypeExpression{
    VariableIdentifier* variableIdentifier;
}ArithmeticTypeExpression;

// Can be primitive and constructed
typedef struct VariableIdentifier {
    bool isGlobal;
    Type type;
    TypeExpression* typeExpression;
} VariableIdentifier;

typedef struct FunctionIdentifier {
    SymbolTable* scopeOfTable; // when we have a function we must create a new symbol table whose address is stored here.
    SymbolInstance* inputParameters;
    SymbolInstance* outputParameters;
} FunctionIdentifier;

typedef struct AliasList {
    SymbolInstance* symbolEntry;
    struct AliasList* next;
} AliasList;

typedef struct RecordTypeDefinition {
    ConstructedTypeInfoNodes* typeExpression; // this will be inherited by the declared variables
    AliasList* alias;
    bool isTaggedRecord;
} RecordTypeDefinition;

typedef struct TypedefDefinition{
    ConstructedTypeInfoNodes* typeExpression; // this will be inherited by the declared variables
    char* originalname;
    bool isUnion;
}TypedefDefinition;

typedef struct UnionTypeDefinition {
    ConstructedTypeInfoNodes* typeExpression; // this will be inherited by the declared variables
    AliasList* alias;
} UnionTypeDefinition;

// Not implementing this as of now.
typedef struct FunctionParameters {
    SymbolInstance** parameterList;
    int type; // 0 -> input parameters, 1 -> output parameters;
} FunctionParameters;

typedef union SymbolRecord {
    VariableIdentifier* variableIdentifier;
    FunctionIdentifier* functionIdentifier;
    RecordTypeDefinition* recordTypeDefinition;
    UnionTypeDefinition* unionTypeDefinition;
    FunctionParameters* functionParameters;
    TypedefDefinition* typedefdefinition;
} SymbolRecord;

typedef struct SymbolInstance {
    char* name;
    int offset; 
    int width;
    SymbolTag symbolTag;
    SymbolRecord* symbolRecord;
    struct SymbolInstance* next;
} SymbolInstance;

typedef struct SymbolTable {
    SymbolInstance** symbolsHashTable;
    int currentOffset;
    char* scope; // as we need to print the module name here
    struct SymbolTable* globalTable; // storing the address of global symobl table to acces global attributes (variables and records and typedefinitions).
} SymbolTable;

#endif
