#ifndef AST_DEF_H
#define AST_DEF_H

#include <stdbool.h>
#include "symbol_tableDef.h"

typedef enum NodeLabels {
    program,
    otherfunctions,
    function,
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
    union,
} NodeLabels;

typedef struct ASTFunctionNode {
    SymbolTable* functionScope; // each function has its own scope and main function will have global scope
    char* functionName;
} ASTFunctionNode;

typedef struct AST_TK_RUID {
    char* ruidName;
} AST_TK_RUID;

typedef struct AST_TK_FIELDID {
    char* fieldName;
} AST_TK_FIELDID;

typedef union ASTNodeUnion {
    ASTFunctionNode astFunctionNode;
    AST_TK_RUID astRUIDNode;
    AST_TK_FIELDID astFieldIdNode;
} ASTNodeUnion;

typedef struct ASTNode {
    NodeLabels label;
    ASTNodeUnion* astNode;
    int numberOfChildren;
    struct ASTNode* parent;
    struct ASTNode** children;
    bool isLeaf;
} ASTNode;

typedef struct ASTRoot {
    ASTNode* root;
} ASTRoot;

#endif