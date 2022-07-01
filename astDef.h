// Group 21 
// Raghava Kasyap Kristipati - 2019A7PS0087P
// K.V.S Preetam             - 2019A7PS0030P
// Shanmukh Chandra Yama     - 2019A7PS0028P
// Uday Dheeraj Nulu         - 2019A7PS0083P
// Yadagiri Shiva Sai Sashank - 2019A7PS0068P
#ifndef AST_DEF_H
#define AST_DEF_H

#include <stdbool.h>
#include "parserDef.h"
#include "symbol_tableDef.h"

typedef enum NodeLabels {
    program,
    otherfunctions,
    function,
    inputpar,
    parameterlist,
    mainFunction,
    stmts,
    recordDefinition,
    unionDefinition,
    tkRuid,
    fieldDefinitions,
    moreFields,
    fieldDefinition,
    tkFieldId,
    tkint,
    tkreal,
    definetypestmt,
    record,
    unionStruct,  // should change this
    declarations,
    declaration,
    otherStmts,
    tkid,
    tkglobal,
    singleOrRecordId,
    returnstmt,
    anlonalreturn,
    idlist,
    null,
    constructedVariable,
    funcallstmt,
    tkfunid,
    moreExpansions,
    assignmentStmt,
    outputparameters,
    inputparameters,
    ioStmt,
    readFunc,
    writeFunc,
    iterativeStmt,
    tknum,
    tkrnum,
    booleanExpression,
    conditionalStmt,
    tkand,
    tkor,
    outputpar,
    remainingList,
    tkrecordruid,
    tkunionruid,
    type_def_ruid,
    tklt,
    tkle,
    tkeq,
    tkgt,
    tkge,
    tkne,
    tknot,
    elseCondition,
    arithmeticExpression,
    term,
    expprime,
    tkplus,
    tkminus,
    tkmul,
    tkdiv,
    termPrime,
    typeDefinitions,
    optionalReturn,
} NodeLabels;

typedef enum Operator{
    noOperator,
    plus,
    minus,
    division,
    multiplication,
}Operator;

typedef struct ASTFunctionNode {
    SymbolTable* functionScope; // each function has its own scope and main function will have global scope
    char* functionName;
} ASTFunctionNode;

typedef struct AST_TK_ID{
    char* IdName;
} AST_TK_ID;

typedef struct AST_TK_RUID_DEF {
    char* ruidName;
} AST_TK_RUID_DEF;

typedef struct AST_TK_FIELDID_DEF {
    char* fieldName;
} AST_TK_FIELDID_DEF;

typedef struct AST_TK_GLOBAL{
    bool global;
}AST_TK_GLOBAL;

typedef struct AST_TK_FUNID{
    char* name;
    SymbolTable* funDefinition;
}AST_TK_FUNID;

typedef union ASTNodeUnion {
    ASTFunctionNode* astFunctionNode;
    AST_TK_RUID_DEF* astRUIDNode;
    AST_TK_FIELDID_DEF* astFieldIdNode;
    AST_TK_ID* astIDNode;
    AST_TK_GLOBAL* astglobalNode;
    AST_TK_FUNID* astFunIdNode;
    
} ASTNodeUnion;

typedef struct ASTNode {
    int lineNumber;
    bool isError;
    NodeLabels label;
    ASTNodeUnion* astNode;
    int numberOfChildren;
    Operator op;
    struct ASTNode* parent;
    struct ASTNode** children;
    bool isLeaf;
    SymbolTable* lookUpTable;
} ASTNode;

typedef struct ASTRoot {
    ASTNode* root;
    int ast_memory;
    int ast_nodes;
} ASTRoot;

#endif