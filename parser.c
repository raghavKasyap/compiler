#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parserDef.h"
#include "lexerDef.h"
#include "parser.h"

// Function to fetch the Non-Terminal ID of non_terminal from NonTerminalIDs array
int fetchNonTerminalId(char* non_terminal) {
    for(int i = 0; i < NUM_NON_TERMINALS; i++) {
        if (strcmp(NonTerminalIDs[i], non_terminal) == 0) {
            return i;
        }
    }
    return -1; // returning -1 if non-terminal not found
};

// Function to fetch the TerminalID of terminal which is a string
int fetchTerminalId(char* terminal) {
    for(int i = 0; i < NUM_TERMINALS; i++) {
        if (strcmp(TerminalIDs[i], terminal) == 0) {
            return i;
        }
    }
    return -1; // returning -1 if non-terminal is not found
};

// Function for creating symbols for Non-terminals and Terminals;
Symbol createSymbol(char* symbol) {
    int type = (symbol[0] >= 'a' && symbol[1] <= 'z') ? 1 : 0;
    int value = type ? fetchNonTerminalId(symbol) : fetchTerminalId(symbol);

    Symbol newSymbol = {type, value};
    return newSymbol;
}

// This function is used to initalized grammar and set the productions of each non-terminal to defaults
Grammar* initGrammar() {
    Grammar* grammar = (Grammar*) malloc(sizeof(Grammar));
    grammar -> productionRules = (ProductionRules *) malloc(NUM_NON_TERMINALS * sizeof(ProductionRules));
    
    for(int i = 0; i < NUM_NON_TERMINALS; i++) {
        ProductionRules* currNonTerminalProds = &(grammar -> productionRules[i]);
        currNonTerminalProds -> nonTerminalId = i;
        currNonTerminalProds -> rules = (GrammarRule* ) malloc(INITIAL_MAX_PRODUCTIONS * sizeof(GrammarRule));
        currNonTerminalProds -> currSize = 0;
        currNonTerminalProds -> maxSize = INITIAL_MAX_PRODUCTIONS;
    }

    return grammar;
}

// This function is used for creating new Grammar rule 
GrammarRule* createNewRule(SymbolLinkedList* rightHandSide) {
    GrammarRule* newRule = (GrammarRule *) malloc(sizeof(GrammarRule));
    newRule -> rightHandSide = rightHandSide;
    newRule -> isEpsilon = rightHandSide -> symbol.type == 0 && rightHandSide -> symbol.symbolID == 57 ? 1 : 0;
    return newRule;
}

// This function is used to add the newRule of nonTerminalId into grammar
void addNewRuleToGrammar(Grammar* grammar, int nonTerminalId, GrammarRule* newRule) {
    ProductionRules* currNonTerminalProds = &(grammar -> productionRules[nonTerminalId]);

    if (currNonTerminalProds -> currSize < currNonTerminalProds -> maxSize) {
        currNonTerminalProds -> rules[currNonTerminalProds -> currSize] = *newRule;
        currNonTerminalProds -> currSize++;
    } else {
        currNonTerminalProds -> maxSize = currNonTerminalProds -> maxSize + ADDITIONAL_PRODUCTIONS_SIZE;
        currNonTerminalProds -> rules = (GrammarRule *) realloc(currNonTerminalProds -> rules, sizeof(GrammarRule) * currNonTerminalProds -> maxSize);
        currNonTerminalProds -> rules[currNonTerminalProds -> currSize] = *newRule;
        currNonTerminalProds -> currSize++;
    }
}

Grammar * generateGrammarFromFile(char* fileName) {
    FILE* fptr = fopen(fileName , "r");
    Grammar* grammar = initGrammar();

    while(!feof(fptr)) {
        char productionRule[200];
        fgets(productionRule, 200, fptr);
        char * symbol = strtok(productionRule, " ");
        int nonTerminalId = fetchNonTerminalId(symbol);
        printf("%d", nonTerminalId);
        printf(" ");
        SymbolLinkedList *rightHandSide = (SymbolLinkedList *) malloc(sizeof(SymbolLinkedList));
        SymbolLinkedList *currHead = rightHandSide;
        currHead -> next = NULL;

        while(symbol != NULL) {
            symbol = strtok(NULL, " ");
            if (symbol == NULL || strcmp(symbol, "\n") == 0 ) {
                break;
            }
            Symbol newSymbol = createSymbol(symbol);
            SymbolLinkedList* nextSymbolInRule = (SymbolLinkedList *) malloc(sizeof(SymbolLinkedList));
            nextSymbolInRule -> symbol = newSymbol;
            nextSymbolInRule -> next = NULL;
            currHead -> next = nextSymbolInRule;
            currHead = currHead -> next;
            printf("%d", newSymbol.symbolID);
            printf(" ");
        }
        
        // creating newRule;
        GrammarRule* newRule = createNewRule(rightHandSide -> next);
        
        // Adding newRule to grammar
        addNewRuleToGrammar(grammar, nonTerminalId, newRule);

        printf("\n");
    }

    fclose(fptr);
    return grammar;
}

_Bool** initializeTable() {
    _Bool** table = (_Bool **) malloc(NUM_NON_TERMINALS * sizeof(_Bool *));
    for(int i = 0; i < NUM_NON_TERMINALS; i++) {
        table[i] = (_Bool*) malloc(NUM_TERMINALS * sizeof(_Bool));
    }

    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        for (int j = 0; j < NUM_TERMINALS; j++) {
            table[i][j] = false;
        }
    }
    return table;
}

FirstAndFollow* initializeFirstAndFollow() {
    FirstAndFollow* firstAndFollowSets = (FirstAndFollow *) malloc(sizeof(FirstAndFollow));
    firstAndFollowSets -> firstSets = initializeTable();
    firstAndFollowSets -> followSets = initializeTable();
    return firstAndFollowSets;
}

void unionFirstSets(bool* firstSet, bool* tempFirstSet) {
    for (int i = 0; i < NUM_TERMINALS - 2; i++) {
        if (tempFirstSet[i]) firstSet[i] = true;
    }
}

bool* computeFirstSetTerminal(int terminalId) {
    bool* firstSetTerminal = (_Bool*)(calloc(NUM_TERMINALS, sizeof(int)));
    firstSetTerminal[terminalId] = true;
    return firstSetTerminal;
}

ProductionRules* fetchNonTerminal(Grammar* grammar, int nonTerminalId) {
    return &(grammar -> productionRules[nonTerminalId]);
}

bool* computeFirstSetRule(GrammarRule* rule, Grammar* grammar, bool** firstSets, bool* isCalculated) {
    bool* firstSetRule = (_Bool*)(malloc(NUM_TERMINALS * sizeof(_Bool)));
    
    for (int i = 0; i < NUM_TERMINALS; i++) 
        firstSetRule[i] = false;

    if(rule -> isEpsilon) {
        firstSetRule[EPS] = true;
        return firstSetRule;
    }

    
    SymbolLinkedList* currHead = rule -> rightHandSide;
    Symbol currSymbol = currHead -> symbol;

    
    if (currSymbol.type == 0) {
        return computeFirstSetTerminal(currSymbol.symbolID);
    }

    if (isCalculated[currSymbol.symbolID] == true) {
        bool* tempFirstSet = firstSets[currSymbol.symbolID];
        unionFirstSets(firstSetRule, tempFirstSet);
        
        while(tempFirstSet[EPS] && currHead -> next != NULL) {
            currHead = currHead -> next;
            
            if (currHead -> symbol.type == 0) {
                tempFirstSet = computeFirstSetTerminal(currHead -> symbol.symbolID);
            } else {
                ProductionRules* nextNonTerminal = fetchNonTerminal(grammar, currHead -> symbol.symbolID);
                tempFirstSet = computeFirstSetNonTerminal(nextNonTerminal, grammar, firstSets, isCalculated);
            }
            
            unionFirstSets(firstSetRule, tempFirstSet);
        }

        if(currHead -> next == NULL && currHead -> symbol.type == 1 && firstSets[currHead -> symbol.symbolID][EPS]) {
            firstSetRule[EPS] = true;
        }

        return firstSetRule;
    };

    ProductionRules* currNonTerminal = fetchNonTerminal(grammar, currSymbol.symbolID);
    bool* tempFirstSet = computeFirstSetNonTerminal(currNonTerminal, grammar, firstSets, isCalculated);
    unionFirstSets(firstSetRule, tempFirstSet);

    while(tempFirstSet[EPS] && currHead -> next != NULL) {
        currHead = currHead -> next;
        
        if (currHead -> symbol.type == 0) {
            tempFirstSet = computeFirstSetTerminal(currHead -> symbol.symbolID);
        } else {
            ProductionRules* nextNonTerminal = fetchNonTerminal(grammar, currHead -> symbol.symbolID);
            tempFirstSet = computeFirstSetNonTerminal(nextNonTerminal, grammar, firstSets, isCalculated);
        }
        
        unionFirstSets(firstSetRule, tempFirstSet);
    }

    if(currHead -> next == NULL && currHead -> symbol.type == 1 && firstSets[currHead -> symbol.symbolID][EPS]) {
        firstSetRule[EPS] = true;
    }

    return firstSetRule;
}

bool* computeFirstSetNonTerminal(ProductionRules* currNonTerminal, Grammar* grammar, bool** firstSets, bool* isCalculated) {
    bool* firstSet = (_Bool*)(malloc(NUM_TERMINALS * sizeof(_Bool)));
    
    for (int i = 0; i < NUM_TERMINALS; i++) 
        firstSet[i] = false;
    // initialize everything in firstSet; todo

    bool derivesEps = false;
    
    for (int i = 0; i < currNonTerminal -> currSize; i++) {
        GrammarRule* rule = &(currNonTerminal -> rules[i]);
        
        bool* tempFirstSet = computeFirstSetRule(rule, grammar, firstSets, isCalculated);
        unionFirstSets(firstSet, tempFirstSet);
        
        if (tempFirstSet[EPS]) derivesEps = true;
    }

    if (derivesEps) firstSet[EPS] = true;
    firstSets[currNonTerminal -> nonTerminalId] = firstSet;
    isCalculated[currNonTerminal -> nonTerminalId] = true;
    return firstSet;
}

RHSNonTerminalAppearance** initialize_RHS_NT_Appearance() {
    RHSNonTerminalAppearance** rhsNonTerminalAppearance = (RHSNonTerminalAppearance **) malloc (NUM_NON_TERMINALS * sizeof(RHSNonTerminalAppearance *));
    
    for(int i = 0; i < NUM_NON_TERMINALS; i++) {
        rhsNonTerminalAppearance[i] = (RHSNonTerminalAppearance *) malloc (sizeof(RHSNonTerminalAppearance));
        rhsNonTerminalAppearance[i] -> next = NULL;
        rhsNonTerminalAppearance[i] -> tail = rhsNonTerminalAppearance[i];
    }

    return rhsNonTerminalAppearance;
}

RHSNonTerminalAppearance* createNewNode(int nonTerminalId, int productionNum) {
    RHSNonTerminalAppearance* newNode = (RHSNonTerminalAppearance *) malloc (sizeof(RHSNonTerminalAppearance));
    int* location = (int *) malloc (2 * sizeof(int));
    location[0] = nonTerminalId;
    location[1] = productionNum;
    newNode -> location = location;
    newNode -> next = NULL;
}

void populateRHSAppearance(RHSNonTerminalAppearance** rhsNonTerminalAppearance, Grammar* grammar) {
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        ProductionRules* currNonTerminal = &(grammar -> productionRules[i]);
        for (int j = 0; j < currNonTerminal -> currSize; j++) {
            GrammarRule* rules = &(currNonTerminal -> rules[j]); 
            SymbolLinkedList* list = rules -> rightHandSide;
            
            while(list != NULL) {
                if (list -> symbol.type == 1) {
                    rhsNonTerminalAppearance[list -> symbol.symbolID] -> tail -> next = createNewNode(currNonTerminal -> nonTerminalId, j);
                    rhsNonTerminalAppearance[list -> symbol.symbolID] -> tail = rhsNonTerminalAppearance[list -> symbol.symbolID] -> tail -> next;
                }
                list = list -> next;
            }
        }
    }
}

FirstAndFollow* computeFirstAndFollowSets(Grammar* grammar) {
    FirstAndFollow* firstAndFollowSets = initializeFirstAndFollow();
    bool* isCalculated = (_Bool *) malloc(NUM_NON_TERMINALS * sizeof(_Bool));
    RHSNonTerminalAppearance** rhsNonTerminalAppearance = initialize_RHS_NT_Appearance();
    populateRHSAppearance(rhsNonTerminalAppearance, grammar);
    
    for (int i = 0; i < NUM_NON_TERMINALS; i++) 
        isCalculated[i] = false;
    
    
    for(int i = 0; i < NUM_NON_TERMINALS; i++) {
        ProductionRules* currNonTerminal = &(grammar -> productionRules[i]);
        computeFirstSetNonTerminal(currNonTerminal, grammar, firstAndFollowSets -> firstSets, isCalculated);
    }


    
    return firstAndFollowSets;
}

int main() {
    Grammar* grammar;
    FirstAndFollow* firstAndFollowSets;
    
    // storing the grammar of the langugae into the grammar data structure
    grammar = generateGrammarFromFile("Complete Grammar.txt");

    // calculating first and follow sets and storing them in firstAndFollowSets data strcture
    firstAndFollowSets = computeFirstAndFollowSets(grammar);

    for(int i = 0; i < NUM_TERMINALS; i++) {
        printf("%d ", firstAndFollowSets ->firstSets[15][i]);
    }
}