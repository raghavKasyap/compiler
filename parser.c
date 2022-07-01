// Group 21 
// Raghava Kasyap Kristipati - 2019A7PS0087P
// K.V.S Preetam             - 2019A7PS0030P
// Shanmukh Chandra Yama     - 2019A7PS0028P
// Uday Dheeraj Nulu         - 2019A7PS0083P
// Yadagiri Shiva Sai Sashank - 2019A7PS0068P

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include "lexer.h"
int parser_node_count = 0;
int parse_tree_memory = 0;
// array of terminal strings
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
    "EPS"};

// array of non-terminal strings
const char *NonTerminalIDs[] = {
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
    "a"};

// print tokens in source code file
void printTokenInFile(FILE* fptr1)
{
    while (1)
    {
        TokenInfo *tk = getNextToken(fptr1);
        printf("%d ", tk->linenum);
        printf("%s ", tk->lexeme);
        if(tk->tokenId == -1) printf("TK_ERROR ");
        else printf("%s \n", TerminalIDs[tk->tokenId]);
        if (tk->tokenId == DOLLAR){
            printf("\n**************\nTokenization has been successful\n\n**************\n");
            break;
        }
            
    }
    
}

// Function to fetch the Non-Terminal ID of non_terminal from NonTerminalIDs array
int fetchNonTerminalId(char *non_terminal)
{
    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        if (strcmp(NonTerminalIDs[i], non_terminal) == 0)
        {
            return i;
        }
    }
    return -1; // returning -1 if non-terminal not found
};

// Function to fetch the TerminalID of terminal which is a string
int fetchTerminalId(char *terminal)
{
    for (int i = 0; i < NUM_TERMINALS; i++)
    {
        if (strcmp(TerminalIDs[i], terminal) == 0)
        {
            return i;
        }
    }
    return -1; // returning -1 if non-terminal is not found
};

// Function for creating symbols for Non-terminals and Terminals;
Symbol createSymbol(char *symbol)
{
    int type = (symbol[0] >= 'a' && symbol[1] <= 'z') ? 1 : 0;
    int value = type ? fetchNonTerminalId(symbol) : fetchTerminalId(symbol);

    Symbol newSymbol = {type, value};
    return newSymbol;
}

// Function for initalizing grammar and seting the productions of each non-terminal to defaults
Grammar *initGrammar()
{
    Grammar *grammar = (Grammar *)malloc(sizeof(Grammar));
    grammar->productionRules = (ProductionRules *)malloc(NUM_NON_TERMINALS * sizeof(ProductionRules));

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        ProductionRules *currNonTerminalProds = &(grammar->productionRules[i]);
        currNonTerminalProds -> nonTerminalId = i;
        currNonTerminalProds -> rules = (GrammarRule *)malloc(INITIAL_MAX_PRODUCTIONS * sizeof(GrammarRule));
        currNonTerminalProds -> currSize = 0;
        currNonTerminalProds -> maxSize = INITIAL_MAX_PRODUCTIONS;
    }

    return grammar;
}

// Function for creating new Grammar rule
GrammarRule *createNewRule(SymbolLinkedList *rightHandSide, int size)
{
    GrammarRule *newRule = (GrammarRule *)malloc(sizeof(GrammarRule));
    newRule -> rightHandSide = rightHandSide;
    newRule -> isEpsilon = rightHandSide -> symbol.type == 0 && rightHandSide -> symbol.symbolID == EPS ? 1 : 0;
    newRule -> size = size;
    return newRule;
}

// Function to add the newRule of nonTerminalId into grammar
void addNewRuleToGrammar(Grammar *grammar, int nonTerminalId, GrammarRule *newRule)
{
    ProductionRules *currNonTerminalProds = &(grammar->productionRules[nonTerminalId]);

    if (currNonTerminalProds -> currSize < currNonTerminalProds -> maxSize)
    {
        currNonTerminalProds -> rules[currNonTerminalProds -> currSize] = *newRule;
        currNonTerminalProds -> currSize++;
    }
    else
    {
        currNonTerminalProds -> maxSize = currNonTerminalProds -> maxSize + ADDITIONAL_PRODUCTIONS_SIZE;
        currNonTerminalProds -> rules = (GrammarRule *)realloc(currNonTerminalProds -> rules, sizeof(GrammarRule) * currNonTerminalProds->maxSize);
        currNonTerminalProds -> rules[currNonTerminalProds -> currSize] = *newRule;
        currNonTerminalProds -> currSize++;
    }
}

// Function reads grammar from grammar.txt and creates in memory grammar data structure.
Grammar *generateGrammarFromFile(char *fileName)
{
    FILE *fptr = fopen(fileName, "r");
    Grammar *grammar = initGrammar();

    while (!feof(fptr))
    {
        char productionRule[200];
        fgets(productionRule, 200, fptr); // gets entire grammar line

        char *symbol = strtok(productionRule, " "); // tokenizes at space delimeter
        int nonTerminalId = fetchNonTerminalId(symbol);
    
        // declaring the right hand side of the grammar rule
        SymbolLinkedList *rightHandSide = (SymbolLinkedList *)malloc(sizeof(SymbolLinkedList));
        SymbolLinkedList *currHead = rightHandSide;

        int size = 0;
        currHead->next = NULL;

        // populating the RHS of the grammar rule into rightHandSide datastructure
        while (symbol != NULL)
        {
            symbol = strtok(NULL, " ");
            if (symbol == NULL || strcmp(symbol, "\n") == 0)
            {
                break;
            }

            if (symbol[0] >= 'a' && symbol[0] <= 'z' || symbol[0] >= 'A' && symbol[0] <= 'Z') {
                Symbol newSymbol = createSymbol(symbol);
                SymbolLinkedList *nextSymbolInRule = (SymbolLinkedList *)malloc(sizeof(SymbolLinkedList));

                size++;

                nextSymbolInRule -> symbol = newSymbol;
                nextSymbolInRule -> next = NULL;
                currHead -> next = nextSymbolInRule;
                currHead = currHead -> next;
            }
        }

        // creating newRule;
        GrammarRule *newRule = createNewRule(rightHandSide -> next, size);

        // Adding newRule to grammar
        addNewRuleToGrammar(grammar, nonTerminalId, newRule);
    }

    fclose(fptr);
    return grammar;
}

// Helper function for initializing the first and follow set in FirstAndFollow datastructure
_Bool **initializeTable()
{
    _Bool **table = (_Bool **)malloc(NUM_NON_TERMINALS * sizeof(_Bool *));
    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        table[i] = (_Bool *)malloc(NUM_TERMINALS * sizeof(_Bool));
    }

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        for (int j = 0; j < NUM_TERMINALS; j++)
        {
            table[i][j] = false;
        }
    }
    return table;
}

// Helper function to initialize FirstAndFollow data structure
FirstAndFollow *initializeFirstAndFollow()
{
    FirstAndFollow *firstAndFollowSets = (FirstAndFollow *)malloc(sizeof(FirstAndFollow)); 

    firstAndFollowSets -> firstSets = initializeTable();
    firstAndFollowSets -> followSets = initializeTable();

    return firstAndFollowSets;
}

// utility function which is used to take union of 2 sets (bit vectors)
void unionSets(bool *set, bool *tempSet)
{
    for (int i = 0; i < NUM_TERMINALS - 1; i++)
    {
        if (tempSet[i])
            set[i] = true;
    }
}

// function for computing the first set of a terminal
bool *computeFirstSetTerminal(int terminalId)
{
    bool *firstSetTerminal = (_Bool *)(malloc(NUM_TERMINALS * sizeof(_Bool)));

    for (int i = 0; i < NUM_TERMINALS; i++)
        firstSetTerminal[i] = false;

    firstSetTerminal[terminalId] = true;
    return firstSetTerminal;
}

// helper function for getting the production rules of a non-terminal
ProductionRules *fetchNonTerminal(Grammar *grammar, int nonTerminalId)
{
    return &(grammar->productionRules[nonTerminalId]);
}

// function for computing the first set of a rule (alpha) where alpha is A -> alpha
bool *computeFirstSetRule(GrammarRule *rule, Grammar *grammar, bool **firstSets, bool *isCalculated)
{
    bool *firstSetRule = (_Bool *)(malloc(NUM_TERMINALS * sizeof(_Bool)));

    // initializing first set of a rule
    for (int i = 0; i < NUM_TERMINALS; i++)
        firstSetRule[i] = false;

    // handles when rule is epsilon
    if (rule->isEpsilon)
    {
        firstSetRule[EPS] = true;
        return firstSetRule;
    }

    // fetching the alpha
    SymbolLinkedList *currHead = rule->rightHandSide;
    Symbol currSymbol = currHead->symbol;

    // handles the case where current symbol is terminal
    if (currSymbol.type == 0)
    {
        return computeFirstSetTerminal(currSymbol.symbolID);
    }

    // memoization-returning the first set of a symbol if it is already calculated 
    if (isCalculated[currSymbol.symbolID] == true)
    {
        bool *tempFirstSet = firstSets[currSymbol.symbolID];
        unionSets(firstSetRule, tempFirstSet);

        // A -> y1 y2 y3   where y1 -> eps
        while (tempFirstSet[EPS] && currHead->next != NULL)
        {
            currHead = currHead->next;

            if (currHead->symbol.type == 0)
            {
                tempFirstSet = computeFirstSetTerminal(currHead->symbol.symbolID);
            }
            else
            {
                ProductionRules *nextNonTerminal = fetchNonTerminal(grammar, currHead->symbol.symbolID);
                tempFirstSet = computeFirstSetNonTerminal(nextNonTerminal, grammar, firstSets, isCalculated);
            }

            unionSets(firstSetRule, tempFirstSet);
        }

        // A -> y1 y2 y3   where  y1,y2,y3 -> eps   then A -> eps
        if (currHead->next == NULL && currHead->symbol.type == 1 && firstSets[currHead->symbol.symbolID][EPS])
        {
            firstSetRule[EPS] = true;
        }

        return firstSetRule;
    };

    // recursively calculating first set of the non-terminal which is not yet calculated
    ProductionRules *currNonTerminal = fetchNonTerminal(grammar, currSymbol.symbolID);
    bool *tempFirstSet = computeFirstSetNonTerminal(currNonTerminal, grammar, firstSets, isCalculated);
    unionSets(firstSetRule, tempFirstSet);

    while (tempFirstSet[EPS] && currHead->next != NULL)
    {
        currHead = currHead->next;

        if (currHead->symbol.type == 0)
        {
            tempFirstSet = computeFirstSetTerminal(currHead->symbol.symbolID);
        }
        else
        {
            ProductionRules *nextNonTerminal = fetchNonTerminal(grammar, currHead->symbol.symbolID);
            tempFirstSet = computeFirstSetNonTerminal(nextNonTerminal, grammar, firstSets, isCalculated);
        }

        unionSets(firstSetRule, tempFirstSet);
    }

    if (currHead->next == NULL && currHead->symbol.type == 1 && firstSets[currHead->symbol.symbolID][EPS])
    {
        firstSetRule[EPS] = true;
    }

    return firstSetRule;
}

// recursive function for first set of non-terminal
bool *computeFirstSetNonTerminal(ProductionRules *currNonTerminal, Grammar *grammar, bool **firstSets, bool *isCalculated)
{
    bool *firstSet = (_Bool *)(malloc(NUM_TERMINALS * sizeof(_Bool)));

    for (int i = 0; i < NUM_TERMINALS; i++)
        firstSet[i] = false;
    // initialize everything in firstSet; todo

    bool derivesEps = false;

    for (int i = 0; i < currNonTerminal->currSize; i++)
    {
        GrammarRule *rule = &(currNonTerminal->rules[i]);

        bool *tempFirstSet = computeFirstSetRule(rule, grammar, firstSets, isCalculated);
        unionSets(firstSet, tempFirstSet);

        if (tempFirstSet[EPS])
            derivesEps = true;
    }

    if (derivesEps)
        firstSet[EPS] = true;
    firstSets[currNonTerminal->nonTerminalId] = firstSet;
    isCalculated[currNonTerminal->nonTerminalId] = true;
    return firstSet;
}

// debugging function for printing the RHS location of each non_terminal
void printRHS(RHSNonTerminalAppearance **rhs)
{
    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        RHSNonTerminalAppearance *curr = rhs[i];
        printf("%d -> ", i);
        curr = curr->next;
        while (curr != NULL)
        {
            printf("[%d, %d] ", curr->location[0], curr->location[1]);
            curr = curr->next;
        }
        printf("\n");
    }
}

// debugging function for printing first sets and follow sets
void printSets(_Bool *sets)
{
    for (int i = 0; i < NUM_TERMINALS; i++)
    {
        if (sets[i])
        {
            printf(TerminalIDs[i]);
            printf(" ");
        }
    }
}

// debugging function to print firstSets and followSets of all non-terminals
void printAllSets(FirstAndFollow *firstAndFollow)
{
    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        printf("%s -> ", NonTerminalIDs[i]);
        printSets(firstAndFollow->firstSets[i]);
        printf("                    ");
        printSets(firstAndFollow->followSets[i]);
        printf("\n");
    }
}

// debugging function to check validity of first sets and follow sets
void checkValiditySets(FirstAndFollow *firstAndFollow)
{
    bool **firstSet = firstAndFollow->firstSets;
    bool **followSet = firstAndFollow->followSets;
    bool *commonTerminals = (_Bool *)malloc(NUM_TERMINALS * sizeof(_Bool));

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        bool derivesEps = firstSet[i][EPS];
        printf("%d -> ", NonTerminalIDs[i]);

        if (derivesEps)
        {
            for (int j = 0; j < NUM_TERMINALS; j++)
            {
                if (firstSet[i] == followSet[i])
                {
                    commonTerminals[i] = true;
                }
            }
        }
    }
}

// initiating the RHSNonTerminalAppearance datasturcture
RHSNonTerminalAppearance **initialize_RHS_NT_Appearance()
{
    RHSNonTerminalAppearance **rhsNonTerminalAppearance = (RHSNonTerminalAppearance **)malloc(NUM_NON_TERMINALS * sizeof(RHSNonTerminalAppearance *));

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        rhsNonTerminalAppearance[i] = (RHSNonTerminalAppearance *)malloc(sizeof(RHSNonTerminalAppearance));
        rhsNonTerminalAppearance[i]->next = NULL;
        rhsNonTerminalAppearance[i]->tail = rhsNonTerminalAppearance[i];
    }

    return rhsNonTerminalAppearance;
}

// creating a new node
RHSNonTerminalAppearance *createNewNode(int nonTerminalId, int productionNum)
{
    RHSNonTerminalAppearance *newNode = (RHSNonTerminalAppearance *)malloc(sizeof(RHSNonTerminalAppearance));
    int *location = (int *)malloc(2 * sizeof(int));
    // parser_node_count++;
    // parse_tree_memory += 2*sizeof(int) + sizeof(RHSNonTerminalAppearance);
    location[0] = nonTerminalId;
    location[1] = productionNum;
    newNode->location = location;
    newNode->next = NULL;
    newNode->tail = NULL;
    return newNode;
}

// function to populate the RHSNonTerminalApperanace datastructure
void populateRHSAppearance(RHSNonTerminalAppearance **rhsNonTerminalAppearance, Grammar *grammar)
{
    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        ProductionRules *currNonTerminal = &(grammar->productionRules[i]);
        for (int j = 0; j < currNonTerminal->currSize; j++)
        {
            GrammarRule *rules = &(currNonTerminal->rules[j]);
            SymbolLinkedList *list = rules->rightHandSide;

            while (list != NULL)
            {
                if (list->symbol.type == 1)
                {
                    rhsNonTerminalAppearance[list->symbol.symbolID]->tail->next = createNewNode(currNonTerminal->nonTerminalId, j);
                    rhsNonTerminalAppearance[list->symbol.symbolID]->tail = rhsNonTerminalAppearance[list->symbol.symbolID]->tail->next;
                }
                list = list->next;
            }
        }
    }
}

// utility function to check if follow set does not contain any teminal
bool checkAllFalse(bool *followSet)
{
    bool allFalse = true;
    for (int i = 0; i < NUM_TERMINALS; i++)
    {
        if (followSet[i])
            allFalse = false;
    }
    return allFalse;
}

// creating a base follow set for start symbol (program)
bool *createNewBaseFollowSet()
{
    bool *tempFollowSet = (_Bool *)(malloc(NUM_TERMINALS * sizeof(_Bool)));

    for (int i = 0; i < NUM_TERMINALS; i++)
        tempFollowSet[i] = false;

    tempFollowSet[DOLLAR] = true;
    return tempFollowSet;
}

// computing follow set for a rule
bool *computeFollowSetRule(int currNtId, int currRuleNt, GrammarRule *rule, Grammar *grammar, bool **firstSets, bool **followSets, bool *isCalculated, RHSNonTerminalAppearance **rhsNtAppearance, bool *prevCalledNts)
{
    // search for the currNonTerminalLocation
    SymbolLinkedList *rhsSymbolsHead = rule->rightHandSide;

    while (rhsSymbolsHead->symbol.type != 1 || rhsSymbolsHead->symbol.symbolID != currNtId)
    {
        rhsSymbolsHead = rhsSymbolsHead->next;
    }

    bool *followSetRule = (_Bool *)(malloc(NUM_TERMINALS * sizeof(_Bool)));

    for (int i = 0; i < NUM_TERMINALS; i++)
        followSetRule[i] = false;

    SymbolLinkedList *followSymbol = rhsSymbolsHead->next;

    // Handle different cases based on the location
    // When the symbol is at the end of the production
    if (followSymbol == NULL)
    {
        bool *tempFollowSet = (_Bool *)(malloc(NUM_TERMINALS * sizeof(_Bool)));

        for (int i = 0; i < NUM_TERMINALS; i++)
            tempFollowSet[i] = false;

        if (currNtId != currRuleNt)
        {
            tempFollowSet = computeFollowSetNonTerminal(currRuleNt, rhsNtAppearance, grammar, firstSets, followSets, isCalculated, prevCalledNts);
        }

        return tempFollowSet;
    }

    // when the symbol is followed by a terminal
    if (followSymbol->symbol.type == 0)
    {
        return computeFirstSetTerminal(followSymbol->symbol.symbolID);
    }

    bool *tempFollowSet = firstSets[followSymbol->symbol.symbolID];
    unionSets(followSetRule, tempFollowSet);

    while (tempFollowSet[EPS] && followSymbol->next != NULL)
    {
        followSymbol = followSymbol->next;

        if (followSymbol->symbol.type == 0)
        {
            tempFollowSet = computeFirstSetTerminal(followSymbol->symbol.symbolID);
        }
        else
        {
            tempFollowSet = firstSets[followSymbol->symbol.symbolID];
        }

        unionSets(followSetRule, tempFollowSet);
    }

    if (tempFollowSet[EPS] && followSymbol->next == NULL && currNtId != currRuleNt)
    {
        tempFollowSet = computeFollowSetNonTerminal(currRuleNt, rhsNtAppearance, grammar, firstSets, followSets, isCalculated, prevCalledNts);
        unionSets(followSetRule, tempFollowSet);
    }

    return followSetRule;
}

// computing the follow set of a non-terminal
bool *computeFollowSetNonTerminal(int currNtId, RHSNonTerminalAppearance **rhsNtAppearance, Grammar *grammar, bool **firstSets, bool **followSets, bool *isCalculated, bool *prevCalledNts)
{
    if (isCalculated[currNtId] == true && !checkAllFalse(followSets[currNtId]))
    {
        return followSets[currNtId];
    }

    bool *followSet = (_Bool *)(malloc(NUM_TERMINALS * sizeof(_Bool)));
    RHSNonTerminalAppearance *rhsList = rhsNtAppearance[currNtId];

    for (int i = 0; i < NUM_TERMINALS; i++)
        followSet[i] = false;

    if (prevCalledNts[currNtId])
    {
        return followSet;
    }
    else
    {
        prevCalledNts[currNtId] = true;
    }

    // base case for start symbol
    if (currNtId == 0)
    {
        bool *tempFollowSet = createNewBaseFollowSet();
        unionSets(followSet, tempFollowSet);
    }

    // go to production rule
    rhsList = rhsList->next; // Take us to the first occurence

    while (rhsList != NULL)
    {
        int *location = rhsList->location;
        GrammarRule *rule = &(grammar->productionRules[location[0]].rules[location[1]]);
        bool *tempFollowSet = computeFollowSetRule(currNtId, location[0], rule, grammar, firstSets, followSets, isCalculated, rhsNtAppearance, prevCalledNts);
        unionSets(followSet, tempFollowSet);
        rhsList = rhsList->next;
    }

    followSets[currNtId] = followSet;
    isCalculated[currNtId] = true;
    return followSet;
}

// encapsulates the creation of first & follow sets for the grammar
FirstAndFollow *computeFirstAndFollowSets(Grammar *grammar)
{
    FirstAndFollow *firstAndFollowSets = initializeFirstAndFollow();
    bool *isCalculated = (_Bool *)malloc(NUM_NON_TERMINALS * sizeof(_Bool));

    // First Set computation
    for (int i = 0; i < NUM_NON_TERMINALS; i++)
        isCalculated[i] = false;

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        ProductionRules *currNonTerminal = &(grammar->productionRules[i]);
        computeFirstSetNonTerminal(currNonTerminal, grammar, firstAndFollowSets->firstSets, isCalculated);
    }

    // Follow Set computation
    RHSNonTerminalAppearance **rhsNonTerminalAppearance = initialize_RHS_NT_Appearance();
    populateRHSAppearance(rhsNonTerminalAppearance, grammar);

    // printRHS(rhsNonTerminalAppearance);

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
        isCalculated[i] = false;

    bool *prevCalledNts = (_Bool *)malloc(NUM_NON_TERMINALS * sizeof(_Bool));

    // First Set computation
    for (int i = 0; i < NUM_NON_TERMINALS; i++)
        prevCalledNts[i] = false;

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        computeFollowSetNonTerminal(i, rhsNonTerminalAppearance, grammar, firstAndFollowSets->firstSets, firstAndFollowSets->followSets, isCalculated, prevCalledNts);

        for (int i = 0; i < NUM_NON_TERMINALS; i++)
            prevCalledNts[i] = false;
    }

    return firstAndFollowSets;
}

// Parse Table Functions
// initializing parse table
ParseTable initializeParseTable()
{
    ParseTable table = (ParseTable)malloc(NUM_NON_TERMINALS * sizeof(int *));
    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        table[i] = (int *)malloc(NUM_TERMINALS * sizeof(int));

        for (int j = 0; j < NUM_TERMINALS; j++)
        {
            table[i][j] = -1;
        }
    }

    return table;
}

// populating the parse table
ParseTable populateParseTable(FirstAndFollow *faft, ParseTable table, GrammarRule *alpha, int nonTerminalId, int prodNum)
{
    // derives epsilon directly
    if (alpha->isEpsilon)
    {
        for (int i = 0; i < NUM_TERMINALS - 1; i++)
        {
            if (faft->followSets[nonTerminalId][i])
            {
                table[nonTerminalId][i] = prodNum;
            }
        }
    }
    else
    {
        SymbolLinkedList *symbolList = alpha->rightHandSide;
        bool derivesEpsilon = false;
        while (symbolList != NULL)
        {
            if (symbolList->symbol.type == 1)
            {
                int currFirstNonTerminal = symbolList->symbol.symbolID;
                bool *firstSet = faft->firstSets[currFirstNonTerminal];

                for (int i = 0; i < NUM_TERMINALS - 1; i++)
                {
                    if (firstSet[i])
                    {
                        table[nonTerminalId][i] = prodNum;
                    }
                }

                if (firstSet[EPS])
                { // first (alpha) constains eplison
                    symbolList = symbolList->next;
                    derivesEpsilon = true;
                }
                else
                { // first (alpha) but does not contain eplison;
                    symbolList = NULL;
                    derivesEpsilon = false;
                }
            }
            else
            {
                table[nonTerminalId][symbolList->symbol.symbolID] = prodNum;
                symbolList = NULL;
                derivesEpsilon = false;
            }
        }

        if (derivesEpsilon)
        {
            for (int i = 0; i < NUM_TERMINALS - 1; i++)
            {
                if (faft->followSets[nonTerminalId][i])
                {
                    table[nonTerminalId][i] = prodNum;
                }
            }
        }
    }
    return table;
}

// encapsulates parse table creation
ParseTable createParseTable(FirstAndFollow *faft, ParseTable table, Grammar *grammar)
{
    table = initializeParseTable();

    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        ProductionRules *currNonTerminal = &(grammar->productionRules[i]);
        int nonTerminalId = currNonTerminal->nonTerminalId;
        int numProds = currNonTerminal->currSize;

        for (int j = 0; j < numProds; j++)
        {
            GrammarRule *alpha = &(currNonTerminal->rules[j]);

            table = populateParseTable(faft, table, alpha, nonTerminalId, j);
        }
    }

    return table;
}

// helper function to create a new tree node for the parse tree
ParseTreeNode *createNewTreeNode(bool type, int symbolId, int parentSymbolId, char *lexeme, int lineNumber, Value *value)
{
    ParseTreeNode *newNode = (ParseTreeNode *)malloc(sizeof(ParseTreeNode));
    char *nonTerminalLexeme = (char *)malloc(5 * sizeof(char));



    nonTerminalLexeme[0] = '-';
    nonTerminalLexeme[1] = '-';
    nonTerminalLexeme[2] = '-';
    nonTerminalLexeme[3] = '-';
    nonTerminalLexeme[4] = '\0';

    newNode -> type = type;
    newNode -> symbolId = symbolId;
    newNode -> parentSymbolId = parentSymbolId;
    newNode -> lexeme = type ? nonTerminalLexeme : lexeme;
    newNode -> lineNumber = lineNumber;
    newNode -> numberOfChildren = -1;
    newNode -> children = NULL;
    newNode -> value = value;

    return newNode;
}

// initialing the root of the parse tree
ParseTreeRoot *initializeTreeRoot()
{
    ParseTreeRoot *tree = (ParseTreeRoot *)malloc(sizeof(ParseTreeRoot));
    tree->root = createNewTreeNode(true, 0, -1, NULL, -1, NULL);
    
    parser_node_count++;
    parse_tree_memory+=sizeof(ParseTreeNode);

    return tree;
}

// creating a new stack node
Stack *createNewStackNode(bool type, int symbolId, ParseTreeNode *refToParseTree)
{
    Stack *newNode = (Stack *)malloc(sizeof(Stack));

    newNode -> type = type;
    newNode -> symbolId = symbolId;
    newNode -> refToParseTree = refToParseTree;
    newNode -> next = NULL;

    return newNode;
}

// initializing the stact with DOLLAR and start symbol
Stack *initializeStack(ParseTreeNode *ref)
{
    Stack *top;
    Stack *dollarNode = createNewStackNode(false, DOLLAR, NULL);
    Stack *startNode = createNewStackNode(true, 0, ref);

    startNode->next = dollarNode;
    top = startNode;

    return top;
}

// poping the top of the stack
Stack *pop(Stack *top)
{
    Stack *temp = top;
    top = top->next;
    free(temp);
    return top;
}

// pushing a production rule onto the stack in reverse order
Stack *pushProduction(Stack *top, ParseTreeNode **children, int size)
{
    for (int i = size - 1; i >= 0; i--)
    {
        Stack *newNode = createNewStackNode(children[i]->type, children[i]->symbolId, children[i]);
        newNode->next = top;
        top = newNode;
    }
    return top;
}

// populating the children of a node
void populateParseTree(ParseTreeNode *parentRef, ParseTreeNode **children, int size)
{
    parentRef->children = children;
    parentRef->numberOfChildren = size;
}

// updating terminal info in the parsetree upon recieving the lexeme
void updateTerminalInfo(ParseTreeNode *node, char *lexeme, int lineNum, Value *value)
{
    node->lexeme = lexeme;
    node->lineNumber = lineNum;
    node->value = value;
}

// Prasing Input Source Code functions
ParseTreeRoot *parseInputSourceCode(char *tokenFile, ParseTable table, Grammar *grammar, FirstAndFollow *firstFollowSets)
{
    ParseTreeRoot *tree = initializeTreeRoot();
    Stack *top = initializeStack(tree->root);
    FILE *fp = fopen(tokenFile, "r");

    bool isDone = false; // represents correct parsing without errors
    bool hasLexcialError = false;
    bool hasSyntaticError = false;
    initialize_Symbol_Table();

    TokenInfo *currToken = getNextToken(fp);
    while (!isDone)
    {

        if (currToken -> isError) {
            hasLexcialError = true;
            // print lexcial error along with line
            printf("Lexical Error in the line number %d. %s does not match the language specifications \n", currToken->linenum, currToken->lexeme);
            top = pop(top);
            currToken = getNextToken(fp);
        }
        // case 1 - top symbol is dollar
        else if (currToken->tokenId == DOLLAR)
        {
            if (top->symbolId == DOLLAR)
            {
                isDone = true;
                top = pop(top);
                fclose(fp);
            }
            else
            {
                // error handling
                printf("Syntax Error in line number %d \n", currToken->linenum);
                printf("Top of stack type = %d  token = %d \n", top -> type, top -> symbolId);
                break;
            }
        }
        else if (currToken->tokenId == TK_COMMENT)
        {
            currToken = getNextToken(fp);
        }

        // case 2 - stack top is nonTerminal
        else if (top->type)
        {
            // case 1 - match

            if (table[top->symbolId][currToken->tokenId] != -1)
            {
                int prodRuleNum = table[top->symbolId][currToken->tokenId];

                GrammarRule *rule = &(grammar->productionRules[top->symbolId].rules[prodRuleNum]); // we are retrieving rule by symbolId and ruleNumber
                SymbolLinkedList *symbolsList = rule->rightHandSide;

                ParseTreeNode *parentRef = top->refToParseTree;
                parentRef -> ruleNumber = prodRuleNum;

                top = pop(top);

                if (!rule->isEpsilon)
                {
                    ParseTreeNode **children = (ParseTreeNode **)malloc(rule->size * sizeof(ParseTreeNode *));


                    for (int i = 0; i < rule->size; i++)
                    {
                        children[i] = createNewTreeNode(symbolsList->symbol.type, symbolsList->symbol.symbolID, parentRef->symbolId, NULL, -1, NULL);
                        symbolsList = symbolsList->next;
                        parser_node_count++;
                        parse_tree_memory+=sizeof(ParseTreeNode);
                    }

                    top = pushProduction(top, children, rule->size);
                    populateParseTree(parentRef, children, rule->size);
                }
                else
                {
                    ParseTreeNode **children = (ParseTreeNode **)malloc(rule->size * sizeof(ParseTreeNode *));
                    children[0] = createNewTreeNode(symbolsList->symbol.type, symbolsList->symbol.symbolID, parentRef->symbolId, "EPS", -1, NULL);
                    populateParseTree(parentRef, children, rule->size);
                    parser_node_count++;
                        parse_tree_memory+=sizeof(ParseTreeNode);
                }
            }

            // case 2 - error no production exists
            else
            {
                
                printf("Syntatic Error in line number %d. Expecting any of [ ", currToken->linenum);

                for (int i = 0; i < NUM_TERMINALS; i++)
                {
                    if (firstFollowSets->firstSets[top->symbolId][i])
                    {
                        printf("%s ,", TerminalIDs[i]);
                    }
                }

                printf("], but received %s \n", TerminalIDs[currToken->tokenId]);
                

                
                // error recovery panic mode.
                bool *followSet = firstFollowSets -> followSets[top->symbolId];

                while (currToken -> tokenId != DOLLAR && !followSet[currToken -> tokenId]) {
                    currToken = getNextToken(fp);
                }

                // pop stack top. 
                top = pop(top);
                hasSyntaticError = true;

                if (currToken -> tokenId == DOLLAR) 
                    isDone = true;
            }
        }

        // case 3 - stackTop is terminal
        else if (top -> type == 0)
        {
            // case 1 - match
            if (top->symbolId == currToken->tokenId)
            {
                updateTerminalInfo(top->refToParseTree, currToken->lexeme, currToken->linenum, currToken->value);
                top = pop(top);
                currToken = getNextToken(fp);
            }

            // case 2 - error
            else
            {
                // case - 2 input terminal does not match with top of stack
                printf("Syntatic Error in the line number %d. Received %s lexeme. Expecting %s instead of %s \n", currToken->linenum, currToken->lexeme, TerminalIDs[top->symbolId], TerminalIDs[currToken->tokenId]);

                hasSyntaticError = true;
                // getting next token

                // assuming input token to be same as stack top to procede with parsing
                top = pop(top);
            }
        }
    }

    if (!hasLexcialError && !hasSyntaticError)
    {
        printf("\n********************************** \n");
        printf("Source Code parsed successfully \n");
        printf("**********************************\n");
        tree -> parsetree_memory = parse_tree_memory;
        tree -> parsetree_nodecount = parser_node_count;
    }
    else{
        printf("\n************************************* \n");
        printf("Source Code is not syntactically correct. Please Recheck \n");
        printf("*************************************\n");
        return NULL;
    }

    
    
};

// helper to print a tree node in required structure
void printTreeNode(ParseTreeNode *node, FILE *fptr)
{
    // lexemeCurrentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol
    char *yes = (char *)malloc(4 * sizeof(char));
    char *no = (char *)malloc(3 * sizeof(char));

    yes[0] = 'y';
    yes[1] = 'e';
    yes[2] = 's';
    yes[3] = '\0';

    no[0] = 'n';
    no[1] = 'o';
    no[2] = '\0';

    char *lexeme = node -> lexeme;
    int linenumber = node -> lineNumber;
    bool isRealNum = node -> symbolId == REALNUM;
    bool isNum = node -> symbolId == NUM;

    int valueInt = 0;
    float valueFloat = 0;

    if (isNum)
        valueInt = node -> value == NULL ? -1 : node -> value->i;
    if (isRealNum)
    {
        valueFloat = node -> value == NULL ? -1 : node -> value->f;
    }

    char *tokenName = node -> type ? NULL : TerminalIDs[node -> symbolId];
    char *nodeSymbol = node -> type ? NonTerminalIDs[node -> symbolId] : NULL;
    char *parentName = NonTerminalIDs[node -> parentSymbolId];
    char *isLeafNode  =  !node -> type ? yes : no;

    // numbers
    if (isRealNum)
    {
        fprintf(fptr, "%s %d %s %f %s %s \n", lexeme, linenumber, tokenName, valueFloat, parentName, isLeafNode);
    }
    else if (isNum)
    {
        fprintf(fptr, "%s %d %s %d %s %s \n", lexeme, linenumber, tokenName, valueInt, parentName, isLeafNode);
    }
    // terminals
    else if (node->type == 0)
    {
        fprintf(fptr, "%s %d %s %s %s \n", lexeme, linenumber, tokenName, parentName, isLeafNode);
    }
    // nonterminals
    else if (node->type)
    {
        fprintf(fptr, "%s %s %s %s %d \n", lexeme, parentName, isLeafNode, nodeSymbol, node -> ruleNumber);
    }
};

// inorder traversal of the parse tree
void printInorderTraversalToFile(ParseTreeNode *node, FILE *fptr)
{
    if (node->children == NULL)
    {
        printTreeNode(node, fptr);
        return;
    }

    for (int i = 0; i < node->numberOfChildren - 1; i++)
    {
        printInorderTraversalToFile(node->children[i], fptr);
    }

    printTreeNode(node, fptr);

    printInorderTraversalToFile(node->children[node->numberOfChildren - 1], fptr);
};

// printing parse tree to outfile.txt
void printParseTree(ParseTreeRoot *tree, char *outFile)
{
    // opening a file to print the parsetree
    FILE *fptr;
    fptr = fopen(outFile, "w");

    // verifying sucessful opening
    if (fptr == NULL)
    {
        printf("File opening unsucessfull");
        return;
    }

    ParseTreeNode *root = tree->root;
    printInorderTraversalToFile(root, fptr);
};

void printTreeNodeToConsole(ParseTreeNode *node)
{
    // lexemeCurrentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol
    char *yes = (char *)malloc(4 * sizeof(char));
    char *no = (char *)malloc(3 * sizeof(char));

    yes[0] = 'y';
    yes[1] = 'e';
    yes[2] = 's';
    yes[3] = '\0';

    no[0] = 'n';
    no[1] = 'o';
    no[2] = '\0';

    char *lexeme = node -> lexeme;
    int linenumber = node -> lineNumber;
    bool isRealNum = node -> symbolId == REALNUM;
    bool isNum = node -> symbolId == NUM;

    int valueInt = 0;
    float valueFloat = 0;

    if (isNum)
        valueInt = node -> value == NULL ? -1 : node -> value->i;
    if (isRealNum)
    {
        valueFloat = node -> value == NULL ? -1 : node -> value->f;
    }

    char *tokenName = node -> type ? NULL : TerminalIDs[node -> symbolId];
    char *nodeSymbol = node -> type ? NonTerminalIDs[node -> symbolId] : NULL;
    char *parentName = NonTerminalIDs[node -> parentSymbolId];
    char *isLeafNode  =  !node -> type ? yes : no;

    // numbers
    if (isRealNum)
    {
        printf("%s %d %s %f %s %s \n", lexeme, linenumber, tokenName, valueFloat, parentName, isLeafNode);
    }
    else if (isNum)
    {
        printf("%s %d %s %d %s %s \n", lexeme, linenumber, tokenName, valueInt, parentName, isLeafNode);
    }
    // terminals
    else if (node->type == 0)
    {
        printf("%s %d %s %s %s \n", lexeme, linenumber, tokenName, parentName, isLeafNode);
    }
    // nonterminals
    else if (node->type)
    {
        printf("%s %s %s %s %d \n", lexeme, parentName, isLeafNode, nodeSymbol, node -> ruleNumber);
    }
};

void printInorderTraversalToConsole(ParseTreeNode *node)
{
    if (node->children == NULL)
    {
        printTreeNodeToConsole(node);
        return;
    }

    for (int i = 0; i < node->numberOfChildren - 1; i++)
    {
        printInorderTraversalToConsole(node->children[i]);
    }

    printTreeNodeToConsole(node);

    printInorderTraversalToConsole(node->children[node->numberOfChildren - 1]);
};

void printparsetreeconsole(ParseTreeRoot *tree){
    ParseTreeNode* root = tree->root;
    printInorderTraversalToConsole(root);
};

// void print_parsenodecount(){
//     printf("Number of nodes in parse tree: %d",parser_node_count);
// }

// void print_parsememory(){
//     printf("Memory allocated to parse tree: %d",memory);
// }

// int main() {
//     Grammar* grammar;
//     FirstAndFollow* firstAndFollowSets;

//     // storing the grammar of the langugae into the grammar data structure
//     grammar = generateGrammarFromFile("Complete Grammar.txt");

//     // calculating first and follow sets and storing them in firstAndFollowSets data strcture
//     firstAndFollowSets = computeFirstAndFollowSets(grammar);
//     ParseTable table;
//     table = createParseTable(firstAndFollowSets, table, grammar);
//     printAllSets(firstAndFollowSets);
//     ParseTreeRoot* tree = parseInputSourceCode("t1.txt", table, grammar, firstAndFollowSets);
//     printParseTree(tree, "outfile.txt");
// }