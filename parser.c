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

// Function for initalizing grammar and seting the productions of each non-terminal to defaults
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

// Function for creating new Grammar rule 
GrammarRule* createNewRule(SymbolLinkedList* rightHandSide) {
    GrammarRule* newRule = (GrammarRule *) malloc(sizeof(GrammarRule));
    newRule -> rightHandSide = rightHandSide;
    newRule -> isEpsilon = rightHandSide -> symbol.type == 0 && rightHandSide -> symbol.symbolID == EPS ? 1 : 0;
    return newRule;
}

// Function to add the newRule of nonTerminalId into grammar
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

// Function reads grammar from grammar.txt and creates in memory grammar data structure.
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

// Helper function for initializing the first and follow set in FirstAndFollow datastructure
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

// Helper function to initialize FirstAndFollow data structure
FirstAndFollow* initializeFirstAndFollow() {
    FirstAndFollow* firstAndFollowSets = (FirstAndFollow *) malloc(sizeof(FirstAndFollow));
    firstAndFollowSets -> firstSets = initializeTable();
    firstAndFollowSets -> followSets = initializeTable();
    return firstAndFollowSets;
}

// utility function which is used to take union of 2 sets (bit vectors)
void unionSets(bool* set, bool* tempSet) {
    for (int i = 0; i < NUM_TERMINALS - 1; i++) {
        if (tempSet[i]) set[i] = true;
    }
}

bool* computeFirstSetTerminal(int terminalId) {
    bool* firstSetTerminal = (_Bool *)(malloc(NUM_TERMINALS * sizeof(_Bool)));

    for (int i = 0; i < NUM_TERMINALS; i++) 
        firstSetTerminal[i] = false;

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
        unionSets(firstSetRule, tempFirstSet);
        
        while(tempFirstSet[EPS] && currHead -> next != NULL) {
            currHead = currHead -> next;
            
            if (currHead -> symbol.type == 0) {
                tempFirstSet = computeFirstSetTerminal(currHead -> symbol.symbolID);
            } else {
                ProductionRules* nextNonTerminal = fetchNonTerminal(grammar, currHead -> symbol.symbolID);
                tempFirstSet = computeFirstSetNonTerminal(nextNonTerminal, grammar, firstSets, isCalculated);
            }
            
            unionSets(firstSetRule, tempFirstSet);
        }

        if(currHead -> next == NULL && currHead -> symbol.type == 1 && firstSets[currHead -> symbol.symbolID][EPS]) {
            firstSetRule[EPS] = true;
        }

        return firstSetRule;
    };

    ProductionRules* currNonTerminal = fetchNonTerminal(grammar, currSymbol.symbolID);
    bool* tempFirstSet = computeFirstSetNonTerminal(currNonTerminal, grammar, firstSets, isCalculated);
    unionSets(firstSetRule, tempFirstSet);

    while(tempFirstSet[EPS] && currHead -> next != NULL) {
        currHead = currHead -> next;
        
        if (currHead -> symbol.type == 0) {
            tempFirstSet = computeFirstSetTerminal(currHead -> symbol.symbolID);
        } else {
            ProductionRules* nextNonTerminal = fetchNonTerminal(grammar, currHead -> symbol.symbolID);
            tempFirstSet = computeFirstSetNonTerminal(nextNonTerminal, grammar, firstSets, isCalculated);
        }
        
        unionSets(firstSetRule, tempFirstSet);
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
        unionSets(firstSet, tempFirstSet);
        
        if (tempFirstSet[EPS]) derivesEps = true;
    }

    if (derivesEps) firstSet[EPS] = true;
    firstSets[currNonTerminal -> nonTerminalId] = firstSet;
    isCalculated[currNonTerminal -> nonTerminalId] = true;
    return firstSet;
}

//utility functions for debugging & printing to console.

// Function for printing the RHS location of each non_terminal
void printRHS(RHSNonTerminalAppearance** rhs) {
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        RHSNonTerminalAppearance* curr = rhs[i];
        printf("%d -> ", i);
        curr = curr -> next;
        while(curr != NULL) {
            printf("[%d, %d] ", curr -> location[0], curr -> location[1]);
            curr = curr -> next;
        }
        printf("\n");
    }
}

// Function for printing first sets and follow sets
void printSets(_Bool* sets) {
    for(int i = 0; i < NUM_TERMINALS; i++) {
        if(sets[i]) {
            printf(TerminalIDs[i]);
            printf(" ");
        }
    }
}

// Function to print firstSets and followSets

void printAllSets(FirstAndFollow* firstAndFollow) {
    for(int i = 0; i < NUM_NON_TERMINALS; i++) {
        printf("%s -> ", NonTerminalIDs[i]);
        printSets(firstAndFollow -> firstSets[i]);
        printf("                    ");
        printSets(firstAndFollow -> followSets[i]);
        printf("\n");
    }
}

// Function to check validity of first sets and follow sets
void checkValiditySets(FirstAndFollow* firstAndFollow) {
    bool** firstSet = firstAndFollow -> firstSets;
    bool** followSet = firstAndFollow -> followSets;
    bool* commonTerminals = (_Bool *) malloc(NUM_TERMINALS * sizeof(_Bool));

    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        bool derivesEps = firstSet[i][EPS];
        printf("%d -> ", NonTerminalIDs[i]);

        if (derivesEps) {
            for(int j = 0; j < NUM_TERMINALS; j++) {
                if (firstSet[i] == followSet[i]) {
                    commonTerminals[i] = true;
                }
            }
        }
        
    }
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
    newNode -> tail = NULL;
    return newNode;
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

bool checkAllFalse(bool* followSet) {
    bool allFalse = true;
    for (int i = 0; i < NUM_TERMINALS; i++)  {
        if (followSet[i]) allFalse = false;
    }
    return allFalse;
}

bool* createNewBaseFollowSet() {
    bool* tempFollowSet = (_Bool*)(malloc(NUM_TERMINALS * sizeof(_Bool)));

    for (int i = 0; i < NUM_TERMINALS; i++) 
        tempFollowSet[i] = false;

    tempFollowSet[DOLLAR] = true;
    return tempFollowSet;
}

bool* computeFollowSetRule(int currNtId, int currRuleNt, GrammarRule* rule, Grammar* grammar, bool** firstSets, bool** followSets, bool* isCalculated, RHSNonTerminalAppearance** rhsNtAppearance, bool* prevCalledNts) {
    // search for the currNonTerminalLocation
    SymbolLinkedList* rhsSymbolsHead = rule -> rightHandSide;

    while(rhsSymbolsHead -> symbol.type != 1 || rhsSymbolsHead -> symbol.symbolID != currNtId) {
        rhsSymbolsHead = rhsSymbolsHead -> next;
    }

    bool* followSetRule = (_Bool*)(malloc(NUM_TERMINALS * sizeof(_Bool)));
    
    for (int i = 0; i < NUM_TERMINALS; i++) 
        followSetRule[i] = false;

    SymbolLinkedList* followSymbol = rhsSymbolsHead -> next;

    // Handle different cases based on the location
    // When the symbol is at the end of the production
    if (followSymbol == NULL) {
        bool* tempFollowSet = (_Bool*)(malloc(NUM_TERMINALS * sizeof(_Bool)));

        for (int i = 0; i < NUM_TERMINALS; i++) 
            tempFollowSet[i] = false;

        if (currNtId != currRuleNt) {
            tempFollowSet = computeFollowSetNonTerminal(currRuleNt, rhsNtAppearance, grammar, firstSets, followSets, isCalculated, prevCalledNts);
        }

        return tempFollowSet;
    }

    // when the symbol is followed by a terminal
    if (followSymbol -> symbol.type == 0) {
        return computeFirstSetTerminal(followSymbol -> symbol.symbolID);
    }

    bool* tempFollowSet = firstSets[followSymbol -> symbol.symbolID];
    unionSets(followSetRule, tempFollowSet);

    while(tempFollowSet[EPS] && followSymbol -> next != NULL) {
        followSymbol = followSymbol -> next;

        if(followSymbol -> symbol.type == 0) {
            tempFollowSet = computeFirstSetTerminal(followSymbol -> symbol.symbolID);
        } else {
            tempFollowSet = firstSets[followSymbol -> symbol.symbolID];
        }

        unionSets(followSetRule, tempFollowSet);
    }

    if (tempFollowSet[EPS] && followSymbol -> next == NULL && currNtId != currRuleNt) {
        tempFollowSet = computeFollowSetNonTerminal(currRuleNt, rhsNtAppearance, grammar, firstSets, followSets, isCalculated, prevCalledNts);
        unionSets(followSetRule, tempFollowSet);
    }

    return followSetRule;
}

bool* computeFollowSetNonTerminal(int currNtId, RHSNonTerminalAppearance** rhsNtAppearance, Grammar* grammar, bool** firstSets, bool** followSets, bool* isCalculated, bool* prevCalledNts) {
    if(isCalculated[currNtId] == true && !checkAllFalse(followSets[currNtId])) {
        return followSets[currNtId];
    }

    bool* followSet = (_Bool*)(malloc(NUM_TERMINALS * sizeof(_Bool)));
    RHSNonTerminalAppearance* rhsList = rhsNtAppearance[currNtId];

    for (int i = 0; i < NUM_TERMINALS; i++) 
        followSet[i] = false;
    
    if(prevCalledNts[currNtId]) {
        return followSet;
    } else {
        prevCalledNts[currNtId] = true;
    }

    // base case for start symbol
    if (currNtId == 0) {
        bool* tempFollowSet = createNewBaseFollowSet();
        unionSets(followSet, tempFollowSet);
    }

    // go to production rule
    rhsList =  rhsList -> next;  // Take us to the first occurence
    
    while(rhsList != NULL) {
        int* location = rhsList -> location;
        GrammarRule* rule = &(grammar -> productionRules[location[0]].rules[location[1]]);
        bool* tempFollowSet = computeFollowSetRule(currNtId, location[0], rule, grammar, firstSets, followSets, isCalculated, rhsNtAppearance, prevCalledNts);
        unionSets(followSet, tempFollowSet);
        rhsList = rhsList -> next;
    }

    followSets[currNtId] = followSet;
    isCalculated[currNtId] = true;
    return followSet;
}

FirstAndFollow* computeFirstAndFollowSets(Grammar* grammar) {
    FirstAndFollow* firstAndFollowSets = initializeFirstAndFollow();
    bool* isCalculated = (_Bool *) malloc(NUM_NON_TERMINALS * sizeof(_Bool));
    
    // First Set computation
    for (int i = 0; i < NUM_NON_TERMINALS; i++) 
        isCalculated[i] = false;

    for(int i = 0; i < NUM_NON_TERMINALS; i++) {
        ProductionRules* currNonTerminal = &(grammar -> productionRules[i]);
        computeFirstSetNonTerminal(currNonTerminal, grammar, firstAndFollowSets -> firstSets, isCalculated);
    }

    // Follow Set computation
    RHSNonTerminalAppearance** rhsNonTerminalAppearance = initialize_RHS_NT_Appearance();
    populateRHSAppearance(rhsNonTerminalAppearance, grammar);
    
    printRHS(rhsNonTerminalAppearance);

    for (int i = 0; i < NUM_NON_TERMINALS; i++) 
        isCalculated[i] = false;
    
    bool* prevCalledNts = (_Bool *) malloc(NUM_NON_TERMINALS * sizeof(_Bool));
    
    // First Set computation
    for (int i = 0; i < NUM_NON_TERMINALS; i++) 
        prevCalledNts[i] = false;
    
    for(int i = 0; i < NUM_NON_TERMINALS; i++) {
        computeFollowSetNonTerminal(i, rhsNonTerminalAppearance, grammar, firstAndFollowSets -> firstSets, firstAndFollowSets -> followSets, isCalculated, prevCalledNts);
        
        for (int i = 0; i < NUM_NON_TERMINALS; i++) 
            prevCalledNts[i] = false;
    }

    return firstAndFollowSets;
}

ParseTable initializeParseTable() {
    ParseTable table = (ParseTable) malloc (NUM_NON_TERMINALS * sizeof(int *));
    for(int i = 0; i < NUM_NON_TERMINALS; i++) {
        table[i] = (int *) malloc (NUM_TERMINALS * sizeof(int));
        
        for(int j = 0; j < NUM_TERMINALS; j++) {
            table[i][j] = -1;
        }
    }

    return table;
}

ParseTable populateParseTable(FirstAndFollow* faft, ParseTable table, GrammarRule* alpha, int nonTerminalId, int prodNum) {
    // derives epsilon directly
    if (alpha -> isEpsilon) {
        for(int i = 0; i < NUM_TERMINALS - 1; i++) {
            if (faft -> followSets[nonTerminalId][i]) {
                table[nonTerminalId][i] = prodNum; 
            }
        }
    }
    else {
        SymbolLinkedList* symbolList = alpha -> rightHandSide;
        bool derivesEpsilon = false;
        while(symbolList != NULL) {
            if (symbolList -> symbol.type == 1) {
                int currFirstNonTerminal = symbolList -> symbol.symbolID;
                bool* firstSet = faft -> firstSets[currFirstNonTerminal];
                
                for (int i = 0; i < NUM_TERMINALS - 1; i++) {
                    if (firstSet[i]) {
                        table[nonTerminalId][i] = prodNum;
                    }
                }

                if (firstSet[EPS]) { // first (alpha) constains eplison
                    symbolList = symbolList -> next;
                    derivesEpsilon = true;
                } else { // first (alpha) but does not contain eplison;
                    symbolList = NULL;
                    derivesEpsilon = false;
                }
            }
            else {
                table[nonTerminalId][symbolList -> symbol.symbolID] = prodNum;
                symbolList = NULL;
                derivesEpsilon = false;
            }
        }

        if (derivesEpsilon) {
            for(int i = 0; i < NUM_TERMINALS - 1; i++) {
                if (faft -> followSets[nonTerminalId][i]) {
                    table[nonTerminalId][i] = prodNum; 
                }
            }
        }
    }
    return table;
}

ParseTable createParseTable(FirstAndFollow* faft, ParseTable table, Grammar* grammar) {
    table = initializeParseTable();

    for(int i = 15; i < 16; i++) {
        ProductionRules* currNonTerminal = &(grammar -> productionRules[i]);
        int nonTerminalId = currNonTerminal -> nonTerminalId;
        int numProds = currNonTerminal -> currSize;

        for(int j = 0; j < numProds; j++) {
            GrammarRule* alpha = &(currNonTerminal -> rules[j]);

            table = populateParseTable(faft, table, alpha, nonTerminalId, j);
        } 
    }

    return table;
}

int main() {
    Grammar* grammar;
    FirstAndFollow* firstAndFollowSets;
    
    // storing the grammar of the langugae into the grammar data structure
    grammar = generateGrammarFromFile("Complete Grammar.txt");

    // calculating first and follow sets and storing them in firstAndFollowSets data strcture
    firstAndFollowSets = computeFirstAndFollowSets(grammar);
    ParseTable table;
    table = createParseTable(firstAndFollowSets, table, grammar);
    printf("%d", table[15][31]);
}