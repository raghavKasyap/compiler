#ifndef AST_DEF_H
#define AST_DEF_H

#include <stdbool.h>
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
    errorCondition,
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

typedef struct ASTFunctionNode {
    SymbolTable* functionScope; // each function has its own scope and main function will have global scope
    char* functionName;
} ASTFunctionNode;

typedef struct AST_TK_ID{
    char* IdName;
} AST_TK_ID;

typedef struct AST_TK_RUID {
    char* ruidName;
} AST_TK_RUID;

typedef struct AST_TK_FIELDID {
    char* fieldName;
} AST_TK_FIELDID;

typedef struct AST_TK_GLOBAL{
    bool global;
}AST_TK_GLOBAL;

typedef struct AST_TK_FUNID{
    SymbolTable* funDefinition;
}AST_TK_FUNID;

typedef union ASTNodeUnion {
    ASTFunctionNode astFunctionNode;
    AST_TK_RUID astRUIDNode;
    AST_TK_FIELDID astFieldIdNode;
    AST_TK_ID astIDNode;
    AST_TK_GLOBAL astglobalNode;
    AST_TK_FUNID astFunIdNode;
} ASTNodeUnion;

typedef struct ASTNode {
    NodeLabels label;
    ASTNodeUnion* astNode;
    int numberOfChildren;
    char op;
    struct ASTNode* parent;
    struct ASTNode** children;
    bool isLeaf;
} ASTNode;

typedef struct ASTRoot {
    ASTNode* root;
} ASTRoot;

#endif