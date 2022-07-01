// Group 21 
// Raghava Kasyap Kristipati - 2019A7PS0087P
// K.V.S Preetam             - 2019A7PS0030P
// Shanmukh Chandra Yama     - 2019A7PS0028P
// Uday Dheeraj Nulu         - 2019A7PS0083P
// Yadagiri Shiva Sai Sashank - 2019A7PS0068P

#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

#define INITIAL_MAX_PRODUCTIONS 3
#define ADDITIONAL_PRODUCTIONS_SIZE 3

// helper functions for core functions
Grammar * generateGrammarFromFile(char* fileName);
bool* computeFirstSetNonTerminal(ProductionRules* currNonTerminal, Grammar* grammar, bool** firstSets, bool* isCalculated);
bool *computeFollowSetNonTerminal(int currNtId, RHSNonTerminalAppearance **rhsNtAppearance, Grammar *grammar, bool **firstSets, bool **followSets, bool *isCalculated, bool* prevCalledNts);
void printTokenInFile(FILE* fptr1);

// core functions of parser.c file
FirstAndFollow *computeFirstAndFollowSets(Grammar *grammar);
ParseTable createParseTable (FirstAndFollow* faft, ParseTable table, Grammar* grammar);
ParseTreeRoot* parseInputSourceCode(char* tokenFile, ParseTable table, Grammar* grammar,FirstAndFollow* firstAndFollowSets);
void printParseTree(ParseTreeRoot *tree, char *outFile);
void printparsetreeconsole(ParseTreeRoot *tree);
void print_parsenodecount();
void print_parsememory();

#endif