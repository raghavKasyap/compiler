// Group 21 
// Raghava Kasyap Kristipati - 2019A7PS0087P
// K.V.S Preetam             - 2019A7PS0030P
// Shanmukh Chandra Yama     - 2019A7PS0028P
// Uday Dheeraj Nulu         - 2019A7PS0083P
// Yadagiri Shiva Sai Sashank - 2019A7PS0068P

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "ast.h"

SymbolTable* initializeSymbolTable(char* scope,SymbolTable* global) {
    SymbolTable* symbolTable = (SymbolTable*) malloc(sizeof(SymbolTable));
    symbolTable -> scope = scope;
    symbolTable -> symbolsHashTable = (SymbolInstance**) malloc(sizeof(SymbolInstance*) * SYMBOL_TABLE_SIZE);
    symbolTable -> globalTable = global;
    symbolTable -> currentOffset = 0;
    return symbolTable;
}

int computeHash(char* name){
    int n = SYMBOL_TABLE_SIZE;
    unsigned long hash = 0;
    int c;

    while (c = *name++) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash % n;
}

void addSymbolToTable(SymbolTable* symbolTable, SymbolInstance* symbol){
    SymbolInstance** hash_Table = symbolTable -> symbolsHashTable;
    char* name = symbol -> name;

    int hash_val = computeHash(name);

    SymbolInstance* symbolList = hash_Table[hash_val];

    if (symbolList == NULL) {
        hash_Table[hash_val] = symbol;
    }

    else {
        while(symbolList -> next){
            symbolList = symbolList -> next;
        }

        symbolList -> next = symbol;
       
    }
     symbolTable -> currentOffset += symbol -> width;
}

SymbolInstance* initializeSymbol(char* name, int offset, SymbolTag symbolTag){
    SymbolInstance* symbol = (SymbolInstance*)malloc(sizeof(SymbolInstance));
    symbol -> name = name;
    symbol -> offset = offset;
    symbol -> symbolTag = symbolTag;
    symbol -> symbolRecord = (SymbolRecord*) malloc (sizeof(SymbolRecord));
    symbol -> next = NULL;
    symbol -> width = 0;

    switch ((symbol -> symbolTag)) {
        case variableIdentifier: {
            VariableIdentifier* varIdentifier = (VariableIdentifier*)malloc(sizeof(VariableIdentifier));
            
            symbol -> symbolRecord -> variableIdentifier = varIdentifier;
            break;
        }

        case functionIdentifier: {
            FunctionIdentifier* funcIdentifier = (FunctionIdentifier *) malloc (sizeof(FunctionIdentifier));
            symbol -> width = 2;
            funcIdentifier -> inputParameters = NULL;
            funcIdentifier -> outputParameters = NULL;
            symbol -> symbolRecord -> functionIdentifier = funcIdentifier;
            break;
        }

        case recordTypeDefinition: {
            RecordTypeDefinition* recType = (RecordTypeDefinition *) malloc(sizeof(RecordTypeDefinition));
            recType -> typeExpression = NULL;
            recType -> alias = NULL;
            symbol -> symbolRecord -> recordTypeDefinition = recType;
            break;
        }

        case unionTypeDefinition: {
            UnionTypeDefinition* recType = (UnionTypeDefinition *) malloc(sizeof(UnionTypeDefinition));
            recType -> typeExpression = NULL;
            recType -> alias = NULL;
            symbol -> symbolRecord -> unionTypeDefinition = recType;
            break;
        }

        case typedef_Definition: {
            TypedefDefinition* typeDefNode = (TypedefDefinition *) malloc (sizeof(TypedefDefinition));
            typeDefNode -> typeExpression = NULL;
            typeDefNode -> isUnion = false;
            typeDefNode -> originalname = NULL;
            symbol -> symbolRecord -> typedefdefinition = typeDefNode;
        }
    }

    return symbol;
}

SymbolInstance* lookUpSymbolTable(char* name, SymbolTable* st) {
    int hash = computeHash(name);
    SymbolInstance* symbolList = st -> symbolsHashTable[hash];

    while(symbolList != NULL && strcmp(name, symbolList -> name) != 0) {
        symbolList = symbolList -> next;
    }
    // what if symbollist is null that is the record was not declared before we must throw an error
    return symbolList;
}

int populateRecordTypeExpression(SymbolInstance* recordSymbol, ASTNode* astNode, SymbolTable* st) {
    ConstructedTypeInfoNodes* head = (ConstructedTypeInfoNodes *) malloc(sizeof(ConstructedTypeExpression));
    ConstructedTypeInfoNodes* temp = head;
    
    head -> next = NULL;
    int width = 0;

    for (int i= 1; i < astNode -> numberOfChildren; i++) {
        ASTNode* fieldDefinitionNode = astNode -> children[i];
        ASTNode* fieldTypeNode = fieldDefinitionNode -> children[0];
        ConstructedTypeInfoNodes* currNode = (ConstructedTypeInfoNodes *) malloc(sizeof(ConstructedTypeExpression));
        currNode -> name = fieldDefinitionNode -> children[1] -> astNode -> astFieldIdNode -> fieldName;
        currNode -> next = NULL;

        VariableIdentifier* varIdef = (VariableIdentifier *) malloc (sizeof(VariableIdentifier));
        
        switch (fieldTypeNode -> label) {
            case tkint: {
                varIdef -> type = basic;
                varIdef -> typeExpression = (TypeExpression *) malloc (sizeof(TypeExpression));
                varIdef -> typeExpression -> typeExpressionBasic = (BasicTypeExpression *) malloc (sizeof(BasicTypeExpression));
                varIdef -> typeExpression -> typeExpressionBasic -> currType = INT;
                
                if (recordSymbol -> symbolTag == unionDefinition) {
                    width = width > 2 ? width : 2;
                }
                else {
                    width += 2;
                }  
                break;
            }

            case tkreal: {
                varIdef -> type = basic;
                varIdef -> typeExpression = (TypeExpression *) malloc (sizeof(TypeExpression));
                varIdef -> typeExpression -> typeExpressionBasic = (BasicTypeExpression *) malloc (sizeof(BasicTypeExpression));
                varIdef -> typeExpression -> typeExpressionBasic -> currType = real;
                if (recordSymbol->symbolTag == unionDefinition) {
                    width = width > 4 ? width : 4;
                } else {
                    width += 4;
                }
                break;
            }

            case type_def_ruid: {
                SymbolInstance* retRecordSymbol = lookUpSymbolTable(fieldTypeNode -> astNode -> astRUIDNode -> ruidName, st);
                varIdef -> type = recordOrUnion;
                varIdef -> typeExpression = (TypeExpression *) malloc (sizeof(TypeExpression));
                varIdef -> typeExpression -> typeExpressionConstructed = (ConstructedTypeExpression *) malloc (sizeof (ConstructedTypeExpression));
                varIdef -> typeExpression -> typeExpressionConstructed -> currType = TYPE_DEF;
                varIdef -> typeExpression -> typeExpressionConstructed -> recordName = retRecordSymbol -> name;
                varIdef -> typeExpression -> typeExpressionConstructed -> typeExpression = retRecordSymbol -> symbolRecord -> recordTypeDefinition -> typeExpression;
                if (recordSymbol -> symbolTag == unionTypeDefinition) {
                    width = width > retRecordSymbol -> width ? width : retRecordSymbol -> width;
                } else {
                    width += retRecordSymbol -> width;
                }
                break;
            }

        }

        currNode -> variableInfo = varIdef;
        head -> next = currNode;
        head = head -> next;
    }

    recordSymbol -> symbolRecord -> recordTypeDefinition -> typeExpression = temp -> next;
    return width;
}

SymbolInstance* createVariableSymbol(ASTNode* currParam, ASTNode* currParamDataType, SymbolTable* globalSymbolTable, SymbolTable* currSymbolTable) {
    SymbolInstance* inVar = initializeSymbol(currParam -> astNode -> astIDNode -> IdName, currSymbolTable -> currentOffset, variableIdentifier);

    switch (currParamDataType -> label) {
        case tkint: {
            inVar -> symbolRecord -> variableIdentifier -> type = basic;
            inVar -> symbolRecord -> variableIdentifier -> typeExpression = (TypeExpression *)malloc (sizeof(TypeExpression));
            inVar -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionBasic = (BasicTypeExpression *) malloc (sizeof(BasicTypeExpression));
            inVar -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionBasic -> currType = INT;
            inVar -> width = 2;
            
            break;
        }

        case tkreal: {
            inVar -> symbolRecord -> variableIdentifier -> type = basic;
            inVar -> symbolRecord -> variableIdentifier -> typeExpression = (TypeExpression *)malloc (sizeof(TypeExpression));
            inVar -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionBasic = (BasicTypeExpression *) malloc (sizeof(BasicTypeExpression));
            inVar -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionBasic -> currType = real;
            inVar -> width = 4;
            break;
        }

        case tkrecordruid: {
            SymbolInstance* recordSymbol = lookUpSymbolTable(currParamDataType -> astNode -> astRUIDNode -> ruidName, globalSymbolTable);
            inVar -> symbolRecord -> variableIdentifier -> type = recordOrUnion;
            inVar -> symbolRecord -> variableIdentifier -> typeExpression = (TypeExpression *)malloc (sizeof(TypeExpression));
            inVar -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed = (ConstructedTypeExpression *)malloc (sizeof (ConstructedTypeExpression));
            inVar -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed -> recordName = recordSymbol -> name;
            inVar -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed -> typeExpression = recordSymbol -> symbolRecord -> recordTypeDefinition -> typeExpression;
            inVar -> width = recordSymbol -> width;
            break;
        }

        case tkunionruid: {
            SymbolInstance* unionSymbol = lookUpSymbolTable(currParamDataType -> astNode -> astRUIDNode -> ruidName, globalSymbolTable);
            inVar -> symbolRecord -> variableIdentifier -> type = recordOrUnion;
            inVar -> symbolRecord -> variableIdentifier -> typeExpression = (TypeExpression *)malloc (sizeof(TypeExpression));
            inVar -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed = (ConstructedTypeExpression *) malloc (sizeof (ConstructedTypeExpression));
            inVar -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed -> recordName = unionSymbol -> name;
            inVar -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed -> typeExpression = unionSymbol -> symbolRecord -> recordTypeDefinition -> typeExpression;
            inVar -> width = unionSymbol -> width;
            break;
        }

        case type_def_ruid: {
            SymbolInstance* recordSymbol = lookUpSymbolTable(currParamDataType -> astNode -> astRUIDNode -> ruidName, globalSymbolTable);
            inVar -> symbolRecord -> variableIdentifier -> type = recordOrUnion;
            inVar -> symbolRecord -> variableIdentifier -> typeExpression = (TypeExpression *)malloc (sizeof(TypeExpression));
            inVar -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed = (ConstructedTypeExpression *) malloc (sizeof (ConstructedTypeExpression));
            inVar -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed -> recordName = recordSymbol -> name;
            inVar -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed -> typeExpression = recordSymbol -> symbolRecord -> recordTypeDefinition -> typeExpression;
            inVar -> width = recordSymbol -> width;
            break;
        }
    }

    return inVar;
}

int checkIfTaggedUnion(ASTNode* astNode, SymbolTable* globalSymbolTable) {
    int numUnion = 0; // 0 -> no 1 -> yes 2 -> error
    int numTag = 0;

    for (int i = 1; i < astNode -> numberOfChildren; i++) {
        ASTNode* currFieldType = astNode -> children[i] -> children[0];
        if (currFieldType -> label == type_def_ruid) {
            SymbolInstance* typeDef = lookUpSymbolTable(currFieldType -> astNode -> astRUIDNode -> ruidName, globalSymbolTable);
            if (typeDef == NULL) {
                printf("Using Type which has not been defined \n");
                astNode -> isError = true;
                return 2;
            }

            if (typeDef -> symbolRecord -> typedefdefinition -> isUnion) {
                numUnion++;
            }
        }
        else if (currFieldType -> label == tkint) {
            numTag++;
        }
    }

    if (numUnion == 0){
        return 0;
    }

    if (numTag >= numUnion) {
        return 1;
    }

    astNode -> isError = true;
    return 2;
}

void constructGlobalSymbolTable(ASTNode* astNode, SymbolTable* globalSymbolTable) {
    NodeLabels currNode = astNode -> label;

    if (currNode == recordDefinition) { // record definitions

        ASTNode* recordIdNode = astNode -> children[0]; // retrieving the name TK_RUID_NODE
        char* name = recordIdNode -> astNode -> astRUIDNode -> ruidName; // retrieving the name of record
        SymbolInstance* recordSymbol;

        int isTaggedUnion = checkIfTaggedUnion(astNode, globalSymbolTable);
        if (isTaggedUnion == 1) {
            recordSymbol = initializeSymbol(name, globalSymbolTable -> currentOffset, recordTypeDefinition);
            recordSymbol -> symbolRecord -> recordTypeDefinition -> isTaggedRecord = true;
            astNode -> lookUpTable = globalSymbolTable;
            recordSymbol -> width = populateRecordTypeExpression(recordSymbol, astNode, globalSymbolTable);
            addSymbolToTable(globalSymbolTable, recordSymbol);
        }
        else if (isTaggedUnion == 2) {
            return; // there is an error here.
        }
        else {
            recordSymbol = initializeSymbol(name, globalSymbolTable -> currentOffset, recordTypeDefinition);
            astNode -> lookUpTable = globalSymbolTable;
            recordSymbol -> width = populateRecordTypeExpression(recordSymbol, astNode, globalSymbolTable);
            addSymbolToTable(globalSymbolTable, recordSymbol);
        }        
        
    }

    else if (currNode == unionDefinition) {
        ASTNode* unionIdNode = astNode -> children[0];
        char* name = unionIdNode -> astNode -> astRUIDNode -> ruidName;
        
        SymbolInstance* unionSymbol = initializeSymbol(name, globalSymbolTable -> currentOffset, unionTypeDefinition);
        astNode -> lookUpTable = globalSymbolTable;
        unionSymbol -> width = populateRecordTypeExpression(unionSymbol, astNode, globalSymbolTable);
        addSymbolToTable(globalSymbolTable, unionSymbol);
    }

    else if (currNode == definetypestmt) {
        astNode -> children[2] -> lookUpTable = globalSymbolTable;
        astNode -> children[1] -> lookUpTable = globalSymbolTable;
        if (astNode -> children[0] -> label == record) {
            
            SymbolInstance* recordSymbol = lookUpSymbolTable(astNode -> children[1] -> astNode -> astRUIDNode -> ruidName, globalSymbolTable);
            SymbolInstance* inVar = initializeSymbol(astNode -> children[2] -> astNode -> astRUIDNode -> ruidName, globalSymbolTable -> currentOffset, typedef_Definition);

            // if there is no record Symbol found then have to give an error.

            // logic for filling inVar
            inVar -> symbolRecord -> typedefdefinition -> typeExpression = recordSymbol -> symbolRecord -> recordTypeDefinition -> typeExpression;
            inVar -> symbolRecord -> typedefdefinition -> originalname = recordSymbol -> name;

            AliasList* newNode = (AliasList *) malloc(sizeof(AliasList));
            newNode -> next = NULL;
            newNode -> symbolEntry = inVar;

            if (recordSymbol -> symbolRecord -> recordTypeDefinition -> alias == NULL) {
                recordSymbol -> symbolRecord -> recordTypeDefinition -> alias = newNode;
            }
            else {
                recordSymbol -> symbolRecord ->recordTypeDefinition -> alias -> next = newNode;
                recordSymbol -> symbolRecord ->recordTypeDefinition -> alias = newNode;
            }
            

            addSymbolToTable(globalSymbolTable, inVar);
        }
        else {
            SymbolInstance* recordSymbol = lookUpSymbolTable(astNode -> children[1] -> astNode -> astRUIDNode -> ruidName, globalSymbolTable);
            SymbolInstance* inVar = initializeSymbol(astNode -> children[2] -> astNode -> astRUIDNode -> ruidName, globalSymbolTable -> currentOffset, typedef_Definition);
            
            // if there is no record Symbol found then have to give an error.

            // Logic for filling inVar
            inVar -> symbolRecord -> typedefdefinition -> typeExpression = recordSymbol -> symbolRecord -> unionTypeDefinition -> typeExpression;
            inVar -> symbolRecord -> typedefdefinition -> originalname = recordSymbol -> name;
            inVar -> symbolRecord -> typedefdefinition -> isUnion = true;

            AliasList* newNode = (AliasList *) malloc(sizeof(AliasList));
            newNode -> next = NULL;
            newNode -> symbolEntry = inVar;
           
            if (recordSymbol -> symbolRecord -> unionTypeDefinition -> alias == NULL) {
                recordSymbol -> symbolRecord -> unionTypeDefinition -> alias = newNode;
            }
            else {
                recordSymbol -> symbolRecord ->unionTypeDefinition -> alias -> next = newNode;
                recordSymbol -> symbolRecord ->unionTypeDefinition -> alias = newNode;
            }

            addSymbolToTable(globalSymbolTable, inVar);
        }
    }
    else if (currNode == declaration) {
        ASTNode* currVar = astNode -> children[1];
        ASTNode* currVarDataType = astNode -> children[0];
        ASTNode* globalOrNot = astNode -> children[2];

        SymbolInstance* inVar;
        if (globalOrNot -> astNode -> astglobalNode -> global) {
            inVar = createVariableSymbol(currVar, currVarDataType, globalSymbolTable, globalSymbolTable);
            currVar -> lookUpTable = globalSymbolTable;
            inVar -> symbolRecord -> variableIdentifier -> isGlobal = true;
            addSymbolToTable(globalSymbolTable, inVar);
        }
    }
    else {
        for (int i = 0; i < astNode -> numberOfChildren; i++) {
            constructGlobalSymbolTable(astNode -> children[i], globalSymbolTable);
        }
    }

    return;
}

SymbolTable* constructSymbolTableRecursive(ASTNode* astNode, SymbolTable* globalSymbolTable, SymbolTable* currSymbolTable){
    NodeLabels currNode = astNode -> label;

    switch (currNode) {
        case program: {
            // for each function
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                ASTNode* currFunction = astNode -> children[i];
                char* functionName = currFunction -> astNode -> astFunctionNode -> functionName;

                SymbolTable* newFunctionTable = initializeSymbolTable(functionName, globalSymbolTable);
                // Updating in ast
                currFunction -> astNode -> astFunctionNode -> functionScope = newFunctionTable;
                currFunction -> lookUpTable = newFunctionTable;
                // update in global symbol table
                SymbolInstance* funcSymbol = initializeSymbol(functionName, 0, functionIdentifier);
                funcSymbol -> symbolRecord -> functionIdentifier -> scopeOfTable = newFunctionTable;
                addSymbolToTable(globalSymbolTable, funcSymbol);

                // recursively construct the symbol table for currFunction
                constructSymbolTableRecursive(currFunction, globalSymbolTable, newFunctionTable);
            }

            break;
        }

        case function: {
            ASTNode* inParas = astNode -> children[0];
            ASTNode* outParas = astNode -> children[1];
            ASTNode* stmtsNode = astNode -> children[2];
            
            inParas -> lookUpTable = currSymbolTable;
            outParas -> lookUpTable = currSymbolTable;
            stmtsNode -> lookUpTable = currSymbolTable;
            constructSymbolTableRecursive(inParas, globalSymbolTable, currSymbolTable);
            constructSymbolTableRecursive(outParas, globalSymbolTable, currSymbolTable);

            for (int i = 2; i < astNode-> numberOfChildren; i++) {
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
            }   
            
            
            break;
        }

        case mainFunction: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                astNode -> children[i] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
            }

            break;
        }

        case inputpar: {
            SymbolInstance* parametersWrapper = initializeSymbol("inputParameters", currSymbolTable -> currentOffset, functionParameters);
            parametersWrapper -> symbolRecord -> functionParameters = (FunctionParameters*) malloc(sizeof(FunctionParameters));
            parametersWrapper -> symbolRecord -> functionParameters -> parameterList = (SymbolInstance**) malloc(sizeof(SymbolInstance *) * (astNode -> numberOfChildren / 2));
            
            for (int i = 1; i < astNode -> numberOfChildren; i += 2) {
                ASTNode* currParam = astNode -> children[i];
                ASTNode* currParamDataType = astNode -> children[i - 1];
                currParam -> lookUpTable = currSymbolTable;

                SymbolInstance* inVar = createVariableSymbol(currParam, currParamDataType, globalSymbolTable, currSymbolTable);

                addSymbolToTable(currSymbolTable, inVar);
                parametersWrapper -> symbolRecord -> functionParameters -> parameterList[i / 2] = inVar;
            }
            break;
        }

        case outputpar: {
            SymbolInstance* parametersWrapper = initializeSymbol("outputParameters", currSymbolTable -> currentOffset, functionParameters);
            parametersWrapper -> symbolRecord -> functionParameters = (FunctionParameters*) malloc(sizeof(FunctionParameters));
            parametersWrapper -> symbolRecord -> functionParameters -> parameterList = (SymbolInstance**) malloc(sizeof(SymbolInstance *) * (astNode -> numberOfChildren / 2));
            
            for (int i = 1; i < astNode -> numberOfChildren; i += 2) {
                ASTNode* currParam = astNode -> children[i];
                ASTNode* currParamDataType = astNode -> children[i - 1];
                currParam -> lookUpTable = currSymbolTable;

                SymbolInstance* inVar = createVariableSymbol(currParam, currParamDataType, globalSymbolTable, currSymbolTable);

                addSymbolToTable(currSymbolTable, inVar);
                parametersWrapper -> symbolRecord -> functionParameters -> parameterList[i / 2] = inVar;
            }
            break;
        }

        case stmts: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                // will assign in the children level
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
            }
            break;
        }

        case typeDefinitions: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                // will assign in the children level
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
            }
            break;
        }

        case recordDefinition: {
            // has to defined globally according to language specifications
            // already updated during the first iteration
            // ASTNode* recordIdNode = astNode -> children[0];
            // char* name = recordIdNode -> astNode -> astRUIDNode -> ruidName;
            
            // SymbolInstance* recordSymbol = initializeSymbol(name, globalSymbolTable -> currentOffset, recordTypeDefinition);
            // astNode -> lookUpTable = globalSymbolTable;
            // recordSymbol -> width = populateRecordTypeExpression(recordSymbol, astNode, globalSymbolTable);
            // addSymbolToTable(globalSymbolTable, recordSymbol);
            break;
        }

        case unionDefinition: {
            ASTNode* unionIdNode = astNode -> children[0];
            char* name = unionIdNode -> astNode -> astRUIDNode -> ruidName;
            
            SymbolInstance* unionSymbol = initializeSymbol(name, globalSymbolTable -> currentOffset, unionTypeDefinition);
            astNode -> lookUpTable = globalSymbolTable;
            unionSymbol -> width = populateRecordTypeExpression(unionSymbol, astNode, globalSymbolTable);
            addSymbolToTable(globalSymbolTable, unionSymbol);
            break;
        }

        // case definetypestmt: {
        //     // add to global table
        //     astNode -> children[2] -> lookUpTable = globalSymbolTable;
        //     astNode -> children[1] -> lookUpTable = globalSymbolTable;
        //     if (astNode -> children[0] -> label == record) {
                
        //         SymbolInstance* recordSymbol = lookUpSymbolTable(astNode -> children[1] -> astNode -> astRUIDNode -> ruidName, globalSymbolTable);
        //         SymbolInstance* inVar = initializeSymbol(astNode -> children[2] -> astNode -> astRUIDNode -> ruidName, globalSymbolTable -> currentOffset, recordTypeDefinition);
        //         // logic for filling inVar
        //         inVar -> symbolRecord -> recordTypeDefinition -> typeExpression = recordSymbol -> symbolRecord -> recordTypeDefinition -> typeExpression;
        //         inVar -> symbolRecord -> recordTypeDefinition -> isAlias = true;
        //         AliasList* newNode = (AliasList *) malloc(sizeof(AliasList));
        //         newNode -> next = NULL;
        //         newNode -> symbolEntry = inVar;
        //         recordSymbol -> symbolRecord ->recordTypeDefinition -> alias -> next = newNode;
        //         recordSymbol -> symbolRecord ->recordTypeDefinition -> alias = newNode;

        //         addSymbolToTable(globalSymbolTable, inVar);
        //     }
        //     else {
        //         SymbolInstance* recordSymbol = lookUpSymbolTable(astNode -> children[1] -> astNode -> astRUIDNode -> ruidName, globalSymbolTable);
        //         SymbolInstance* inVar = initializeSymbol(astNode -> children[2] -> astNode -> astRUIDNode -> ruidName, globalSymbolTable -> currentOffset, unionTypeDefinition);
        //        // Logic for filling inVar
        //         inVar -> symbolRecord -> unionTypeDefinition -> typeExpression = recordSymbol -> symbolRecord -> unionTypeDefinition -> typeExpression;
        //         inVar -> symbolRecord -> unionTypeDefinition -> isAlias = true;

        //         AliasList* newNode = (AliasList *) malloc(sizeof(AliasList));
        //         newNode -> next = NULL;
        //         newNode -> symbolEntry = inVar;
        //         recordSymbol -> symbolRecord ->recordTypeDefinition -> alias -> next = newNode;
        //         recordSymbol -> symbolRecord ->recordTypeDefinition -> alias = newNode;

        //         addSymbolToTable(globalSymbolTable, inVar);
        //     }
        //     break;
        // }

        case declarations: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
            }
            break;
        }

        case declaration: {
            ASTNode* currVar = astNode -> children[1];
            ASTNode* currVarDataType = astNode -> children[0];
            ASTNode* globalOrNot = astNode -> children[2];

            SymbolInstance* inVar;
            if (!(globalOrNot -> astNode -> astglobalNode -> global)) {
                inVar = createVariableSymbol(currVar, currVarDataType, globalSymbolTable, currSymbolTable);
                currVar -> lookUpTable = currSymbolTable;
                addSymbolToTable(currSymbolTable, inVar);
            }
            break;
        }

        case otherStmts: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                astNode -> children[i] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
            }
            break;
        }

        case assignmentStmt: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                astNode -> children[i] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
            }

            break;
        }

        case singleOrRecordId: {
            astNode -> children[0] -> lookUpTable = currSymbolTable; // assigning for the tkid
            break;
        }

        case arithmeticExpression: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                astNode -> children[i] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
                // possible children airthmetic expression singleOrRecordId, tknum, tkrnum all of them are covered.
            }

            break;
        }

        case tknum: {
            break;
        }

        case tkrnum: {
            break;
        }
        
        case tkfunid: {
            break;
        }

        case tkid: {
            break;
        }

        case iterativeStmt: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                astNode -> children[i] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
                // possible children booleanExpression, singleOrRecordId, stmtNode(this is being covered now)
            }

            break;
        }

        case booleanExpression: {

            if (astNode -> numberOfChildren == 3) {
                astNode -> children[0] -> lookUpTable = currSymbolTable;
                astNode -> children[2] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[0], globalSymbolTable, currSymbolTable);
                constructSymbolTableRecursive(astNode -> children[2], globalSymbolTable, currSymbolTable);

            }
            else {
                astNode -> children[0] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[0], globalSymbolTable, currSymbolTable);

            }
            break;
        }

        case conditionalStmt: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                astNode -> children[i] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
                // possible children booleanExpression, stmtNode(this is being covered now)
            }

            break;
        }

        case elseCondition: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                astNode -> children[i] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
                // possible children bstmtNode(this is being covered now)
            }
            break;
        }

        case ioStmt: {
            astNode -> children[1] -> lookUpTable = currSymbolTable;
            constructSymbolTableRecursive(astNode -> children[1], globalSymbolTable, currSymbolTable);
            // possible children are var which is handled
            break;
        }

        case funcallstmt: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                astNode -> children[i] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
                // possible children bstmtNode(this is being covered now)
            }

            break;
        }

        case outputparameters: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                astNode -> children[i] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
                // possible children bstmtNode(this is being covered now)
            }

            break;
        }

        case inputparameters: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                astNode -> children[i] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
                // possible children bstmtNode(this is being covered now)
            }

            break;
        }

        case idlist: {
            for (int i = 0; i < astNode -> numberOfChildren; i++) {
                astNode -> children[i] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
                // possible children tkid
            }

            break;
        }

        case returnstmt: {
             for (int i = 0; i < astNode -> numberOfChildren; i++) {
                astNode -> children[i] -> lookUpTable = currSymbolTable;
                constructSymbolTableRecursive(astNode -> children[i], globalSymbolTable, currSymbolTable);
                // possible children idlist
            }

            break;
        }
    }
}

SymbolTable* constructSymbolTable(ASTRoot* ast) {
    // Plan
    // For each function we have 
    // we are initializing the global symbol table
    SymbolTable* globalSymbolTable = initializeSymbolTable("global", NULL);
    constructGlobalSymbolTable(ast -> root, globalSymbolTable);
    constructSymbolTableRecursive(ast -> root, globalSymbolTable, globalSymbolTable);
    
    return globalSymbolTable;
}

void printTypeInfoFunction(SymbolInstance* symbol) {
    printf("Type - Function Name \n");
    printf("Type Expression - ");
    SymbolInstance* inputPars = symbol -> symbolRecord -> functionIdentifier -> inputParameters;
    SymbolInstance* outputPars = symbol -> symbolRecord -> functionIdentifier -> outputParameters;
    
    printf("( ");
    while(inputPars) {
        if (inputPars -> symbolRecord -> variableIdentifier -> type == basic) {
            if (!(inputPars -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionBasic -> currType)) {
                printf("INT * ");
            }
            else {
                printf("REAL *");
            }
        }
        else {
            printf("%s * ", inputPars -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed -> recordName);
        }
        inputPars = inputPars -> next;
    }

    printf(" -> ");

    while(outputPars) {
        if (outputPars -> symbolRecord -> variableIdentifier -> type == basic) {
            if (!(outputPars -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionBasic -> currType)) {
                printf("INT * ");
            }
            else {
                printf("REAL *");
            }
        }
        else {
            printf("%s * ", outputPars -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed -> recordName);
        }
        outputPars = outputPars -> next;
    }

    printf(") \n");

    printf("Width - 2 \n");
    printf("isGlobal - Global \n");
    printf("Offset - %d\n", symbol -> offset);
}

void printTypeInfoVar(SymbolInstance* symbol) {
    printf("Type - Variable Identifier");
    printf("Type Expression - ");

    if (symbol -> symbolRecord -> variableIdentifier -> type == basic) {
        if (!(symbol -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionBasic -> currType)) {
            printf("INT\n");
            printf("Width - 2\n");
        }
        else {
            printf("REAL\n");
            printf("Width - 4\n");
        }
    }
    else {
        printf("%s\n", symbol -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed -> recordName);
        printf("Width - %d \n", symbol -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed -> width);
    }

    char* isGlobal = symbol -> symbolRecord -> variableIdentifier -> isGlobal ? "Global" : "----";
    printf("isGlobal - %s \n", isGlobal);
    printf("Offset - %d \n", symbol -> offset);
}

void printTypeInfoRecord(SymbolInstance* symbol) {
    printf("Type - Record");
    printf("Type Expression - ");

    ConstructedTypeInfoNodes* tpe = symbol -> symbolRecord -> recordTypeDefinition -> typeExpression;

    while(tpe) {
        printf("( %s, ", tpe -> name);
        if (tpe -> variableInfo -> type == basic) {
            if (!(tpe -> variableInfo -> typeExpression -> typeExpressionBasic -> currType)) {
                printf("INT), ");
            }
            else {
                printf("REAL), ");
            }
        }
        else {
            printf("%s), ", tpe -> variableInfo -> typeExpression -> typeExpressionConstructed -> recordName);
        }
        tpe = tpe -> next;
    }

    printf("\n");
    printf("isGlobal - Global \n");
    printf("Width - %d \n", symbol -> width);
    printf("Offset - %d \n", symbol -> offset);
}

void printTypeInfoUnion(SymbolInstance* symbol) {
    printf("Type - Record");
    printf("Type Expression - ");

    ConstructedTypeInfoNodes* tpe = symbol -> symbolRecord -> unionTypeDefinition -> typeExpression;

    while(tpe) {
        printf("( %s, ", tpe -> name);
        if (tpe -> variableInfo -> type == basic) {
            if (!(tpe -> variableInfo -> typeExpression -> typeExpressionBasic -> currType)) {
                printf("INT), ");
            }
            else {
                printf("REAL), ");
            }
        }
        else {
            printf("%s), ", tpe -> variableInfo -> typeExpression -> typeExpressionConstructed -> recordName);
        }
        tpe = tpe -> next;
    }

    printf("\n");
    printf("isGlobal - Global \n");
    printf("Width - %d \n", symbol -> width);
    printf("Offset - %d \n", symbol -> offset);
}

void printSymbolTable(SymbolTable* currSymbolTable) {
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        SymbolInstance* hash_list = currSymbolTable -> symbolsHashTable[i];
        while(hash_list){
            printf("Symbol name: %s, Scope: %s \n", hash_list -> name, currSymbolTable -> scope);
            if(hash_list -> symbolTag == functionIdentifier){
                printTypeInfoFunction(hash_list);
                printSymbolTable(hash_list -> symbolRecord -> functionIdentifier ->scopeOfTable);
            }
            else if (hash_list -> symbolTag == recordTypeDefinition) {
                printTypeInfoRecord(hash_list);
            }
            else if (hash_list -> symbolTag == unionTypeDefinition) {
                printTypeInfoUnion(hash_list);
            }
            else if (hash_list -> symbolTag == variableIdentifier) {
                printTypeInfoVar(hash_list);
            }
            printf("====================================== \n");
            hash_list = hash_list -> next;
        }
    }
}

void printGlobalVariables(SymbolTable* currSymbolTable) {
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        SymbolInstance* hash_list = currSymbolTable->symbolsHashTable[i];
        while (hash_list) {
            printf("Symbol name: %s, Scope: %s \n", hash_list->name,
                   currSymbolTable->scope);
            if (hash_list->symbolTag == functionIdentifier) {
                printTypeInfoFunction(hash_list);
            } else if (hash_list->symbolTag == recordTypeDefinition) {
                printTypeInfoRecord(hash_list);
            } else if (hash_list->symbolTag == unionTypeDefinition) {
                printTypeInfoUnion(hash_list);
            } else if (hash_list->symbolTag == variableIdentifier) {
                printTypeInfoVar(hash_list);
            }
            printf("====================================== \n");
            hash_list = hash_list->next;
        }
    }
}

void printRecordVariables(SymbolTable* currSymbolTable) {
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        SymbolInstance* hash_list = currSymbolTable->symbolsHashTable[i];
        while (hash_list) {
            
            if (hash_list->symbolTag == recordTypeDefinition) {
                printf("Symbol name: %s, Scope: %s \n", hash_list->name, currSymbolTable->scope);
                printTypeInfoRecord(hash_list);
            } else if (hash_list->symbolTag == unionTypeDefinition) {
                printf("Symbol name: %s, Scope: %s \n", hash_list->name, currSymbolTable->scope);
                printTypeInfoUnion(hash_list);
            }

            printf("====================================== \n");
            hash_list = hash_list->next;
        }
    }
}