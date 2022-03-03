#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

#define INITIAL_MAX_PRODUCTIONS 3
#define ADDITIONAL_PRODUCTIONS_SIZE 3

// helper functions for core functions
bool* computeFirstSetNonTerminal(ProductionRules* currNonTerminal, Grammar* grammar, bool** firstSets, bool* isCalculated);
bool *computeFollowSetNonTerminal(int currNtId, RHSNonTerminalAppearance **rhsNtAppearance, Grammar *grammar, bool **firstSets, bool **followSets, bool *isCalculated, bool* prevCalledNts);

// core functions of parser.c file
FirstAndFollow *computeFirstAndFollowSets(Grammar *grammar);
ParseTable createParseTable (FirstAndFollow* faft, ParseTable table, Grammar* grammar);
ParseTreeRoot* parseInputSourceCode(char* tokenFile, ParseTable table, Grammar* grammar,FirstAndFollow* firstAndFollowSets);

#endif