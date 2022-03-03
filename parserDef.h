// define data structures for grammar, first and follow table, parse table, stack ADT, parse tree ADT.
#ifndef PARSER_DEF_H
#define PARSER_DEF_H

#include <stdbool.h>
#include "lexerDef.h"

#define NUM_NON_TERMINALS 53

const char *TerminalIDs[] = {
    "TK_ASSIGNOP",
    "TK_COMMENT",
    "TK_FIELDID",
    "TK_ID",
    "TK_NUM",
    "TK_RNUM",
    "TK_FUNID",
    "TK_RUID",
    "TK_WITH",
    "TK_PARAMETERS",
    "TK_END",
    "TK_WHILE",
    "TK_UNION",
    "TK_ENDUNION",
    "TK_DEFINETYPE",
    "TK_AS",
    "TK_TYPE",
    "TK_MAIN",
    "TK_GLOBAL",
    "TK_PARAMETER",
    "TK_LIST",
    "TK_SQL",
    "TK_SQR",
    "TK_INPUT",
    "TK_OUTPUT",
    "TK_INT",
    "TK_REAL",
    "TK_COMMA",
    "TK_SEM",
    "TK_COLON",
    "TK_DOT",
    "TK_ENDWHILE",
    "TK_OP",
    "TK_CL",
    "TK_IF",
    "TK_THEN",
    "TK_ENDIF",
    "TK_READ",
    "TK_WRITE",
    "TK_RETURN",
    "TK_PLUS",
    "TK_MINUS",
    "TK_MUL",
    "TK_DIV",
    "TK_CALL",
    "TK_RECORD",
    "TK_ENDRECORD",
    "TK_ELSE",
    "TK_AND",
    "TK_OR",
    "TK_NOT",
    "TK_LT",
    "TK_LE",
    "TK_EQ",
    "TK_GT",
    "TK_GE",
    "TK_NE",
    "DOLLAR",
    "EPS"
};

const char* NonTerminalIDs[] = {
    "program",
    "otherFunctions",
    "mainFunction",
    "stmts",
    "function",
    "input_par",
    "output_par",
    "parameter_list",
    "dataType",
    "remaining_list",
    "primitiveDatatype",
    "constructiveDatatype",
    "typeDefinitions",
    "declarations",
    "declaration",
    "otherStmts",
    "returnStmt",
    "actualOrRedefined",
    "typeDefinition",
    "definetypestmt",
    "fieldDefinitions",
    "fieldDefinition",
    "moreFields",
    "fieldType",
    "global_or_not",
    "stmt",
    "assignmentStmt",
    "iterativeStmt",
    "conditionalStmt",
    "ioStmt",
    "funCallStmt",
    "singleOrRecId",
    "arithmeticExpression",
    "constructedVariable",
    "oneExpansion",
    "moreExpansions",
    "outputParameters",
    "inputParameters",
    "idList",
    "booleanExpression",
    "elsecondition",
    "var",
    "term",
    "expPrime",
    "lowPrecedenceOperators",
    "factor",
    "termPrime",
    "highPrecedenceOperators",
    "logicalOp",
    "relationalOp",
    "optionalReturn",
    "more_ids",
    "a"
};


// Structure that defines a symbol either terminal or non-terminal.
typedef struct Symbol {
    int type; // Used as tag for the finding the type in SymbolValueInfor Type = 0 represents terminal, Type = 1 represents nonTerminal.
    int symbolID; // Stores the TokendID of terminal or non-terminal which is decided by the type field.
} Symbol;

// LinkedList of symbols which is used for many purposes.
typedef struct SymbolLinkedList {
    Symbol symbol; 
    struct SymbolLinkedList* next; 
} SymbolLinkedList;

// Structure to represent the right hand side of grammar rule.
// does not have head
typedef struct GrammarRule {
    int isEpsilon; // For determing wether the RHS of the grammar rule is epsilon or not.
    int size;
    SymbolLinkedList* rightHandSide; // stores the right hand side of the rule which can be a mixture of both terminals and non-Terminals
} GrammarRule;

// Structure which encapsulates all the prodcution rules for a non-terminal
typedef struct ProductionRules {
    int nonTerminalId;
    GrammarRule* rules;
    int currSize;
    int maxSize;
} ProductionRules;

// Structure which encapsualtes all the rules of every non-termial 
typedef struct Grammar {
    ProductionRules* productionRules;
    int numOfNonterminals;
} Grammar;

// This structure stores the first and follow sets for each non-terminal
typedef struct FirstAndFollow {
    bool** firstSets; // 2D array which contains the first sets of each non-terminal
    bool** followSets; // 2D array which contains the follow sets of each non-terminal
} FirstAndFollow;

// it has head
typedef struct RHSNonTerminalAppearance {
    int* location;
    struct RHSNonTerminalAppearance* next;
    struct RHSNonTerminalAppearance* tail;
} RHSNonTerminalAppearance;

typedef int** ParseTable;

typedef struct ParseTreeNode {
    bool type;
    int symbolId;
    int numberOfChildren;
    int parentSymbolId; // -1 for root node;
    int lineNumber;
    char* lexeme;
    Value* value;
    struct ParseTreeNode** children;
} ParseTreeNode;

typedef struct Stack {
    bool type;
    int symbolId;
    ParseTreeNode* refToParseTree;
    struct Stack* next;
} Stack;

typedef struct ParseTreeRoot {
    ParseTreeNode* root;
} ParseTreeRoot;

#endif
