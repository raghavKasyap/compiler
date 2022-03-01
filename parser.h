#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

#define INITIAL_MAX_PRODUCTIONS 3
#define ADDITIONAL_PRODUCTIONS_SIZE 3

FirstAndFollow* computeFirstAndFollowSets(Grammar* grammar);
bool* computeFirstSetNonTerminal(ProductionRules* currNonTerminal, Grammar* grammar, bool** firstSets, bool* isCalculated);
#endif