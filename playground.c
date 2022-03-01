#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
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
        currNonTerminalProds -> maxSize = currNonTerminalProds -> maxSize + 3;
        currNonTerminalProds -> rules = (GrammarRule *) realloc(currNonTerminalProds -> rules, sizeof(GrammarRule) * currNonTerminalProds -> maxSize);
        currNonTerminalProds -> rules[currNonTerminalProds -> currSize] = *newRule;
        currNonTerminalProds -> currSize++;
    }
}

int main(){
    FILE* fptr = fopen("Complete Grammar.txt" , "r");
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
        GrammarRule* newRule = createNewRule(rightHandSide);
        
        // Adding newRule to grammar
        addNewRuleToGrammar(grammar, nonTerminalId, newRule);

        printf("\n");
    }

    fclose(fptr);
}