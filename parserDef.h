// define data structures for grammar, first and follow table, parse table, stack ADT, parse tree ADT.
#ifndef PARSER_DEF_H
#define PARSER_DEF_H

#include "lexerDef.h"

#define NUM_NON_TERMINALS 52

// Defining Non-Terminals using enum as number of non-terminals used in the grammar are fixed.
typedef enum {
    program,
    otherFunctions,
    mainFunction,
    stmts,
    function,
    input_par,
    parameter_list,
    output_par,
    dataType,
    remaining_list,
    primitiveDatatype,
    constructiveDatatype,
    typeDefinitions,
    declarations,
    otherStmts,
    returnStmt,
    actualOrRedefined,
    typeDefinition,
    definetypestmt,
    fieldDefinitions,
    fieldDefinition,
    moreFields,
    fieldType,
    global_or_not,
    stmt,
    assignmentStmt,
    iterativeStmt,
    conditionalStmt,
    ioStmt,
    funCallStmt,
    singleOrRecId,
    arithmeticExpression,
    constructedVariable,
    oneExpansion,
    moreExpansions,
    outputParameters,
    inputParameters,
    idList,
    booleanExpression,
    elsecondition,
    var,
    term,
    expPrime,
    lowPrecedenceOperators,
    factor,
    termPrime,
    highPrecedenceOperators,
    logicalOp,
    relationalOp,
    optionalReturn,
    more_ids,
    A
} NonTerminal;

// Structure that defines a symbol either terminal or non-terminal.
typedef struct Symbol {
    int type; // Used as tag for the finding the type in SymbolValueInfor Type = 0 represents terminal, Type = 1 represents nonTerminal.
    int TokenID; // Stores the TokendID of terminal or non-terminal which is decided by the type field.
} Symbol;

// LinkedList of symbols which is used for many purposes.
typedef struct SymbolLinkedList {
    Symbol symbol; 
    struct LinkedList* next; 
} SymbolLinkedList;

// Structure to represent the right hand side of grammar rule.
typedef struct GrammarRule {
    int isEpsilon; // For determing wether the RHS of the grammar rule is epsilon or not.
    SymbolLinkedList* RightHandSide; // stores the right hand side of the rule which can be a mixture of both terminals and non-Terminals
} GrammarRule;

// Structure which encapsulates all the prodcution rules for a non-terminal
typedef struct ProductionRules {
    NonTerminal non_terminal;
    GrammarRule* rules;
    int currSize;
    int maxSize;
} ProductionRules;

// Structure which encapsualtes all the rules of each non-termial 
typedef struct Grammar {
    ProductionRules* productionRules;
    int numOfNonterminals;
} Grammar;

// This structure stores the first and follow sets for each non-terminal
typedef struct FirstAndFollowSets {
    unsigned int** firstSets; // 2D array which contains the first sets of each non-terminal
    unsigned int** followSets; // 2D array which contains the follow sets of each non-terminal
} FirstAndFollowSets;

typedef struct

#endif
