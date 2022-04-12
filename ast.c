#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "ast.h"

char* astLabel[] = {
    "program",
    "otherfunctions",
    "function",
    "inputpar",
    "parameterlist",
    "mainFunction",
    "stmts",
    "recordDefinition",
    "unionDefinition",
    "tkRuid",
    "fieldDefinitions",
    "moreFields",
    "fieldDefinition",
    "tkFieldId",
    "tkint",
    "tkreal",
    "definetypestmt",
    "record",
    "unionStruct", 
    "declarations",
    "declaration",
    "otherStmts",
    "tkid",
    "tkglobal",
    "singleOrRecordId",
    "returnstmt",
    "anlonalreturn",
    "idlist",
    "null",
    "constructedVariable",
    "funcallstmt",
    "tkfunid",
    "moreExpansions",
    "assignmentStmt",
    "outputparameters",
    "inputparameters",
    "ioStmt",
    "readFunc",
    "writeFunc",
    "iterativeStmt",
    "tknum",
    "tkrnum",
    "booleanExpression",
    "conditionalStmt",
    "tkand",
    "tkor",
    "outputpar",
    "remainingList",
    "tkrecordruid",
    "tkunionruid",
    "type_def_ruid",
    "tklt",
    "tkle",
    "tkeq",
    "tkgt",
    "tkge",
    "tkne",
    "errorCondition",
    "arithmeticExpression",
    "term",
    "expprime",
    "tkplus",
    "tkminus",
    "tkmul",
    "tkdiv",
    "termPrime",
    "typeDefinitions",
    "optionalReturn",
};

ASTRoot* initializeAST() {
    ASTRoot* ast = (ASTRoot *) malloc (sizeof(ASTRoot));
    return ast;
}

ASTNode* initializeASTNode(bool isLeaf, NodeLabels nodeLabel) {
    ASTNode* newNode = (ASTNode*) malloc(sizeof(ASTNode));
    newNode -> parent = NULL;
    newNode -> children = NULL;
    newNode -> numberOfChildren = 0;
    newNode -> astNode = (ASTNodeUnion*)malloc(sizeof(ASTNodeUnion));
    // what to initialize for operator?
    newNode -> isLeaf = isLeaf;
    newNode -> label = nodeLabel;
    return newNode;
} 

void populateChild(ASTNode* root, ASTNode* child) {
    
    root -> children = realloc(root->children , ((root -> numberOfChildren) + 1)*sizeof(struct ASTNode*));
    root -> children[root -> numberOfChildren] = child;
    root -> numberOfChildren++;
    child -> parent = root;
}

void populateChildren(ASTNode* root, ASTNode* child){
    int sizeOfChild = child -> numberOfChildren;
    root -> children = realloc(root -> children, ((root -> numberOfChildren) + sizeOfChild)*sizeof(struct ASTNode*));
    
    for (int i = 0; i < sizeOfChild; i++) {
        root -> children[root -> numberOfChildren + i] = child -> children[i];
        child -> children[i] -> parent =  root;
    }
    
    root -> numberOfChildren += sizeOfChild;
}

Operator fetchoperator(ParseTreeNode* parseNode){
    ParseTreeNode* parsePrecedenceoperatorNode = parseNode -> children[0];
    switch(parsePrecedenceoperatorNode -> symbolId){
        case 44:{ // low precedence order
            switch(parsePrecedenceoperatorNode -> ruleNumber){
                case 0:{
                    return plus;
                }

                case 1:{
                    return minus;
                }
            }
        }

        case 47:{ // high precedence order
            switch(parsePrecedenceoperatorNode -> ruleNumber){
                case 0:{
                    return multiplication;
                }
                
                case 1:{
                    return division;
                }
            }
        }
    }
}

 void printAST(ASTNode* AST){
    printf("%s \n",astLabel[AST->label]);
    if(AST->isLeaf == 1){
        return;
    }
    else{
        for(int i=0;i< AST -> numberOfChildren; i++){
            printAST(AST->children[i]);
        }
    }

}

ASTNode* ASTarithmeticHelper(ParseTreeNode* expprimeParseNode, ASTNode* termNode){
    ASTNode* arithmeticExpressionNode = initializeASTNode(false, arithmeticExpression);

    switch(expprimeParseNode -> ruleNumber){
        case 0:{
            ASTNode* termExpprimeNode = buildASTRecursive(expprimeParseNode -> children[1]);
            arithmeticExpressionNode->op = fetchoperator(expprimeParseNode);

            populateChild(arithmeticExpressionNode, termNode);
            populateChild(arithmeticExpressionNode, termExpprimeNode);

            ASTNode* arithmeticExpressionNodeActual = ASTarithmeticHelper(expprimeParseNode -> children[2], arithmeticExpressionNode);

            return arithmeticExpressionNodeActual;
        }

        case 1:{ // null production
            return termNode;
        }
    }
}

ASTNode* ASTtermHelper(ParseTreeNode* termprimeParseNode, ASTNode* factorNode){
    ASTNode* termNode = initializeASTNode(false, arithmeticExpression);

    switch(termprimeParseNode -> ruleNumber){
        case 0:{
            ASTNode* factortermprimeNode = buildASTRecursive(termprimeParseNode -> children[1]);
            termNode->op = fetchoperator(termprimeParseNode);

            populateChild(termNode, factorNode);
            populateChild(termNode, factortermprimeNode);

            ASTNode* termNodeActual = ASTtermHelper(termprimeParseNode -> children[2], termNode);

            return termNodeActual;
        }

        case 1:{ // null production
            return factorNode;
        }
    }
}

ASTNode* buildASTRecursive(ParseTreeNode* currNode) {
    int ruleNumber = currNode -> ruleNumber;
    int symbolId = currNode -> symbolId;
    
    switch (symbolId) {
        case 0: { // program 
            
            ASTNode* programNode = initializeASTNode(false, program);
            printf("prgm1\n");
            printf("Symbol id of otherfunclist is %d \n", currNode -> children[0]->symbolId);
            ASTNode* otherFunctionsList = buildASTRecursive(currNode -> children[0]);
            printf("prgrm2\n");
            ASTNode* mainFunctionNode = buildASTRecursive(currNode -> children[1]);
            printf("prgm3 \n");
            populateChildren(programNode, otherFunctionsList);
            printf("prgm4 \n");
            populateChildren(programNode, mainFunctionNode);
            printf("prgm 5 \n");
            free(otherFunctionsList);
            return programNode;
            
        }
        
        case 2: { // main function
            printf("main 0 \n");
            ASTNode* mainFunctionNode = initializeASTNode(false, function);
            // main function should have access to global scope.

            ASTNode* stmts = buildASTRecursive(currNode -> children[1]); // building the ast subtree for stmts
            printf("main 1\n");
            populateChildren(mainFunctionNode, stmts);
            printf("main 2 \n");
            free(stmts); // removing the stmts nodes as it is no longer required
            
            return mainFunctionNode;
        }

        case 1: {  // otherfunctions
            printf("othrfncs1\n");
            ASTNode* otherfunctionsNode = initializeASTNode(false, otherfunctions);
            switch(currNode -> ruleNumber){
                case 0:{
                    printf("otherfnctions2\n");
                    ASTNode* function = buildASTRecursive(currNode->children[0]);
                    printf("otherfunctions3 \n");
                    ASTNode* otherfunctions_1 = buildASTRecursive(currNode->children[1]);
                    printf("otherfunctions4 \n");
                    populateChild(otherfunctionsNode, function);
                    printf("otherfunction5 \n");
                    populateChildren(otherfunctionsNode, otherfunctions_1);
                    printf("otherfunctions6 \n");
                    
                    return otherfunctionsNode;
                }

                case 1:{
                    return otherfunctionsNode;
                }
            }

            printf("otherfnctions2\n");
            ASTNode* function = buildASTRecursive(currNode->children[0]);
            printf("otherfunctions3 \n");
            ASTNode* otherfunctions_1 = buildASTRecursive(currNode->children[1]);
            printf("otherfunctions4 \n");
            populateChild(otherfunctionsNode, function);
            printf("otherfunction5 \n");
            populateChildren(otherfunctionsNode, otherfunctions_1);
            printf("otherfunctions6 \n");
            
            return otherfunctionsNode;
        }

        case 3: { // stmts
            ASTNode* stmtsNode = initializeASTNode(false, stmts);
            printf("stmts 0 \n");
            ASTNode* typeDefinitionsNode = buildASTRecursive(currNode -> children[0]);
            printf("stmts 1 \n");
            ASTNode* declarationsNode = buildASTRecursive(currNode -> children[1]);
            printf("stmts 2 \n");
            ASTNode* otherStmtsNode = buildASTRecursive(currNode -> children[2]);
            printf("stmts 3 \n");
            ASTNode* returnStmtsNode = buildASTRecursive(currNode -> children[3]);
            printf("stmts 4 \n");

            // adding these nodes to the stmtsNode as children nodes
            populateChild(stmtsNode, typeDefinitionsNode);
            printf("stmts 5 \n");
            populateChild(stmtsNode, declarationsNode);
            printf("stmts 6 \n");
            populateChild(stmtsNode, otherStmtsNode);
            printf("stmts 7 \n");
            populateChild(stmtsNode, returnStmtsNode);
            printf("stmts 8 \n");

            return stmtsNode;
        }

        case 4:{ // function
            printf("fnc1\n");
            ASTNode* functionNode = initializeASTNode(false, function);
            printf("fnc2\n");
            ASTNode* TK_FUNID_Node = initializeASTNode(true, tkfunid);
            printf("fnc3\n");
            printf("fnc3.1\n");
            TK_FUNID_Node -> astNode -> astFunctionNode = (ASTFunctionNode *) malloc(sizeof(ASTFunctionNode));
            TK_FUNID_Node -> astNode -> astFunctionNode -> functionName = currNode -> children[0]->lexeme;
            printf("fnc3.5\n");
            ASTNode* input_par_Node = buildASTRecursive(currNode -> children[1]);
            printf("fnc4\n");
            ASTNode* output_par_Node = buildASTRecursive(currNode -> children[2]);
            printf("func5 \n");
            ASTNode* stmtsNode = buildASTRecursive(currNode -> children[4]);
            printf("fun6 /n");

            populateChild(functionNode, TK_FUNID_Node);
            printf("fun7 \n");
            populateChild(functionNode, input_par_Node);
            printf("fun8 \n");
            populateChild(functionNode, output_par_Node);
            printf("fun9 \n");
            populateChildren(functionNode, stmtsNode);
            printf("fun10 \n");

            free(stmtsNode);

            return functionNode;
        }

        case 5:{ //input_par
            ASTNode* input_par_Node = initializeASTNode(false, inputpar);
            printf("intput par1 \n");
            ASTNode* parameter_list_Node = buildASTRecursive(currNode -> children[4]);
            printf("intput par2 \n");
            populateChildren(input_par_Node, parameter_list_Node);
            printf("input par3 \n");
            free(parameter_list_Node);
            
            return input_par_Node;
            
        }

        case 6: { // output_par
            ASTNode* outputParNode = initializeASTNode(false, outputpar);
            ASTNode* parameterListNode = buildASTRecursive(currNode -> children[4]);

            populateChildren(outputParNode, parameterListNode);
            free(parameterListNode);
            
            return  outputParNode;
        }

        case 7:{ //parameterlist
            ASTNode* parameterListNode = initializeASTNode(false, parameterlist);
            printf("parameterlist1 \n");
            ASTNode* datatypeNode = buildASTRecursive(currNode -> children[0]);
            printf("parameterlist2 \n");
            ASTNode* TK_ID_Node = initializeASTNode(true, tkid);
            printf("parameterlist3 \n");
            TK_ID_Node->astNode->astIDNode = (AST_TK_ID*)malloc(sizeof(AST_TK_ID));
            TK_ID_Node->astNode->astIDNode->IdName = currNode -> children[1]->lexeme;
            printf("parameter list4 \n");
            ASTNode* remainingListNode = buildASTRecursive(currNode -> children[2]);
            printf("parameter list5 \n");

            populateChild(parameterListNode, datatypeNode);
            printf("parameter list6 \n");
            populateChild(parameterListNode, TK_ID_Node);
            printf("parameter list7\n");
            populateChildren(parameterListNode, remainingListNode);
            printf("parameter list8\n");
            free(remainingListNode);
            printf("parameter list9\n");
            return parameterListNode;
        }

        case 8: { //datatype
            return buildASTRecursive(currNode -> children[0]); // same for both rules
        }

        case 9: { // remainingList
            switch(currNode -> ruleNumber) {
                case 0: {
                    return buildASTRecursive(currNode -> children[1]);
                }

                case 1: {
                    return initializeASTNode(true, null);
                }
            }
            
        }
        
        case 10: {  // primitveDataType
            printf("primitive data type \n");
            switch (currNode->ruleNumber) {
                case 0: {  // INT_TYPE
                    return initializeASTNode(true, tkint);
                }

                case 1: {  // REAL_TYPE
                    return initializeASTNode(true, tkreal);
                }
            }
        }

        case 11:{ // constructiveDataType
            printf("constructed data type \n");
            switch(currNode -> ruleNumber){
                case 0:{
                    return initializeASTNode(true, tkrecordruid);
                }
                case 1:{
                    return initializeASTNode(true, tkunionruid);
                }
                case 2:{
                    return initializeASTNode(true, type_def_ruid);
                }
            }
        }

        case 12: { // typedefinitionsNode;
            ASTNode* typedefinitionsNode = initializeASTNode(false, typeDefinitions);
            printf("typedefinitions 0 \n");
            switch (currNode -> ruleNumber) {
                
                case 0: { // rule 0 which derives records and unions
                    printf("typedefintions 0.5\n");
                    ASTNode* actualOrRedefined = buildASTRecursive(currNode -> children[0]);
                    printf("typedefinitions 1 \n");
                    ASTNode* otherTypeDefinitions = buildASTRecursive(currNode -> children[1]);
                    printf("typedefinitions 2 \n");
                    // add the first typedefinition 
                    populateChild(typedefinitionsNode, actualOrRedefined);
                    printf("typedefinitions 3 \n");
                    // add all the below defined definitions
                    populateChildren(typedefinitionsNode, otherTypeDefinitions);
                    printf("typedefinitions 4 \n");
                    free(otherTypeDefinitions);

                    return typedefinitionsNode;
                }

                case 1: {  // eps production
                    printf("typedefinitions 5 \n");
                    return typedefinitionsNode;
                }
            }
        }

        case 13: { // declarationsNode
            ASTNode* declarationsNode = initializeASTNode(false, declarations);
            switch(currNode -> ruleNumber){
                case 0:{
                    ASTNode* declaration = buildASTRecursive(currNode -> children[0]);
                    ASTNode* declarationsList = buildASTRecursive(currNode -> children[1]);
                    
                    populateChild(declarationsNode, declaration);
                    populateChildren(declarationsNode, declarationsList);
                    
                    free(declarationsList);
                    return declarationsNode;
                }
                case 1:{
                    return declarationsNode;
                }
            }
        }
        
        case 14: { // declaration
            ASTNode* declarationNode = initializeASTNode(false, declaration);
            ASTNode* datatypeNode = buildASTRecursive(currNode -> children[1]);
            ASTNode* TK_ID_Node = initializeASTNode(true, tkid);
            TK_ID_Node->astNode->astIDNode = (AST_TK_ID*)malloc(sizeof(AST_TK_ID));
            TK_ID_Node -> astNode -> astIDNode->IdName = currNode -> children[3] -> lexeme;
            ASTNode* global_or_not_Node = buildASTRecursive( currNode -> children[4]);
            
            populateChildren(declarationNode, datatypeNode);
            populateChild(declarationNode, TK_ID_Node);
            populateChild(declarationNode, global_or_not_Node);
            free(datatypeNode);

            return declarationNode;
            
        }

        case 15: { // otherstmts
            ASTNode* otherStmtsNode = initializeASTNode(false, otherStmts);
            printf("otherstmts 0 \n");
            switch (currNode -> ruleNumber) {
                case 0: {
                    printf("otherstms 0.5 \n");
                    ASTNode* stmtNode = buildASTRecursive(currNode->children[0]);
                    printf("otherstmts 1 \n");
                    ASTNode* otherStmtsList = buildASTRecursive(currNode->children[1]);
                    printf("otherstmts 2 \n");

                    populateChild(otherStmtsNode, stmtNode);
                    printf("otherstmts 3 \n");
                    populateChildren(otherStmtsNode, otherStmtsList);
                    printf("otherstmts 4 \n");
                    free(otherStmtsList);
                    return otherStmtsNode;
                }

                case 1: {
                    printf("otherstmts 5 \n");
                    return otherStmtsNode;
                }
            }
            printf("otherstmts 6\n");
        }
        
        case 16: { //returnstmt
            printf("returnstmt 0\n");
            ASTNode* returnstmtNode = initializeASTNode(false, returnstmt);
            ASTNode* optionalreturnNode = buildASTRecursive(currNode -> children[1]);
            printf("returnstmt 1");

            populateChildren(returnstmtNode, optionalreturnNode);
            printf("returnstmt2");

            return returnstmtNode;
        }

        case 17: { // actualOrRedefined variable
            printf("actualorRedefined 0 \n");
            switch (currNode -> ruleNumber) {
                case 0: { // typedefinition
                     printf("actualorRedefined 1 \n");
                    return buildASTRecursive(currNode -> children[0]);
                }

                case 1: { // definetypestmt
                    printf("actualorRedefined 2 \n");
                    return buildASTRecursive(currNode -> children[0]);
                }
            }
            printf("actualorRedefined 3 \n");
        }

        case 18: { // typedefinition
            printf("typedefinition 0 \n");
            switch (currNode -> ruleNumber) {
                case 0: {  // record definition
                    printf("typedefinition 1 \n");
                    ASTNode* recordTypeDefinitionNode = initializeASTNode(false, recordDefinition);
                    ASTNode* TK_RUID_Node = initializeASTNode(true, tkRuid);

                    TK_RUID_Node -> astNode -> astRUIDNode = (AST_TK_RUID*)malloc(sizeof(AST_TK_RUID));
                    TK_RUID_Node -> astNode -> astRUIDNode->ruidName = currNode -> children[1] -> lexeme;
                    populateChild(recordTypeDefinitionNode, TK_RUID_Node);
                    printf("typedefinition 2 \n");

                    ASTNode* fieldDefinitionsList = buildASTRecursive(currNode -> children[2]);
                    printf("typedefinition 3 \n");
                    populateChildren(recordTypeDefinitionNode, fieldDefinitionsList);
                    printf("typedefinition 4 \n");
                    free(fieldDefinitionsList);
                    
                    return recordTypeDefinitionNode;
                }

                case 1: {  // union definition
                    printf("typedefinition 5 \n");
                    ASTNode* unionTypeDefinitionNode = initializeASTNode(false, unionDefinition);
                    ASTNode* TK_RUID_Node = initializeASTNode(true, tkRuid);
                    
                    TK_RUID_Node -> astNode -> astRUIDNode = (AST_TK_RUID*)malloc(sizeof(AST_TK_RUID));
                    TK_RUID_Node -> astNode -> astRUIDNode->ruidName = currNode -> children[1] -> lexeme;
                    populateChild(unionTypeDefinitionNode, TK_RUID_Node);
                    
                    ASTNode* fieldDefinitionsList = buildASTRecursive(currNode -> children[2]);
                    printf("typedefinition 6 \n");
                    populateChildren(unionTypeDefinitionNode, fieldDefinitionsList);
                    free(fieldDefinitionsList);

                    return unionTypeDefinitionNode;
                }
            }
            printf("typedefinition 7 \n");
        }
        
        case 19: { // definetype statement
            printf("definetype stmt 0 \n");
            ASTNode* definetypestmtNode = initializeASTNode(false, definetypestmt);
            printf("definetype stmt 1 \n");
            ASTNode* A = buildASTRecursive(currNode -> children[1]);
            ASTNode* TK_RUID_NODE1 = initializeASTNode(true, tkRuid);
            ASTNode* TK_RUID_NODE2 = initializeASTNode(true, tkRuid);

            printf("definetype stmt 1 . 5\n");
            TK_RUID_NODE1 -> astNode -> astRUIDNode = (AST_TK_RUID*)malloc(sizeof(AST_TK_RUID));
            TK_RUID_NODE2 -> astNode -> astRUIDNode = (AST_TK_RUID*)malloc(sizeof(AST_TK_RUID));

            printf("definetype stmt 2 \n");

            TK_RUID_NODE1 -> astNode -> astRUIDNode->ruidName = currNode -> children[2] ->lexeme;
            TK_RUID_NODE2 -> astNode -> astRUIDNode->ruidName = currNode -> children[4] ->lexeme;

                        printf("definetype stmt 3 \n");

            populateChild(definetypestmtNode, A);
            populateChild(definetypestmtNode, TK_RUID_NODE1);
            populateChild(definetypestmtNode, TK_RUID_NODE2);

                        printf("definetype stmt 4 \n");

            return definetypestmtNode;
        } 
        
        case 20: { // fieldDefinitions
            printf("fileddefinitions 0 \n");
            ASTNode* fieldDefinitionsNode = initializeASTNode(false, fieldDefinitions);
            ASTNode* fieldDefinition_1 = buildASTRecursive(currNode -> children[0]);
            printf("fileddefinitions 1 \n");
            ASTNode* fieldDefinition_2 = buildASTRecursive(currNode -> children[1]);
            printf("fileddefinitions 2 \n");
            populateChild(fieldDefinitionsNode, fieldDefinition_1);
            populateChild(fieldDefinitionsNode, fieldDefinition_2);

            ASTNode* moreFieldsNode = buildASTRecursive(currNode -> children[2]);
            printf("fileddefinitions 3 \n");
            populateChildren(fieldDefinitionsNode, moreFieldsNode);
            printf("fielddefinitions 4\n");
            free(moreFieldsNode);
            return fieldDefinitionsNode;
        }

        case 21: {  // fieldDefinition
            printf("fileddefinition 0 \n");
            ASTNode* fieldDefinitionNode = initializeASTNode(false, fieldDefinition);
            ASTNode* fieldTypeNode = buildASTRecursive(currNode -> children[1]);
            printf("fileddefinition 1 \n");
            ASTNode* TK_FIELDID_Node = initializeASTNode(true, tkFieldId);
            TK_FIELDID_Node -> astNode -> astFieldIdNode = (AST_TK_FIELDID*) malloc(sizeof(AST_TK_FIELDID));
            TK_FIELDID_Node -> astNode -> astFieldIdNode->fieldName = currNode -> children[3] -> lexeme;
            printf("fileddefinition 2 \n");
            populateChild(fieldDefinitionNode, fieldTypeNode);
            populateChild(fieldDefinitionNode, TK_FIELDID_Node);
            return fieldDefinitionNode;
        }

        case 22: { // moreFields
            ASTNode* moreFieldsNode = initializeASTNode(false, moreFields);
            switch(currNode -> ruleNumber) {
                
                case 0: {
                    ASTNode* fieldDefinitionNode = buildASTRecursive(currNode->children[0]);
                    ASTNode* moreFieldsList = buildASTRecursive(currNode->children[1]);

                    populateChild(moreFieldsNode, fieldDefinitionNode);
                    populateChildren(moreFieldsNode, moreFieldsList);
                    free(moreFieldsList);
                }

                case 1: {
                    return moreFieldsNode;
                }
            }
        }

        case 23: { // fieldType
            switch (currNode -> ruleNumber) {
                case 0: { // primitive datatype
                    printf("fieldType 0 \n");
                    return buildASTRecursive(currNode -> children[0]);
                }

                case 1: { // ruid type
                    ASTNode* TK_RUID_Node = initializeASTNode(true, tkRuid);
                    
                    printf("fieldType 1 \n");
                    TK_RUID_Node -> astNode -> astRUIDNode = (AST_TK_RUID*)malloc(sizeof(AST_TK_RUID));
                    TK_RUID_Node -> astNode -> astRUIDNode->ruidName = currNode -> children[0] -> lexeme;
                    printf("fieldTYpe 2 \n");
                    return TK_RUID_Node;
                }
            }
        }

        case 24: {  // global_or_not
            ASTNode* TK_Global_Node = initializeASTNode(true, tkglobal);
            TK_Global_Node->astNode->astglobalNode = (AST_TK_GLOBAL*)malloc(sizeof(AST_TK_GLOBAL));
            switch (currNode->ruleNumber) {
                
                case 0: {
                    TK_Global_Node->astNode->astglobalNode->global = true;
                    return TK_Global_Node;
                }
                case 1: {
                    TK_Global_Node->astNode->astglobalNode->global = false;
                    return TK_Global_Node;
                }
            }
        }
        
        case 25: { // stmt
            printf("stmt 0 \n");
            switch(currNode -> ruleNumber) {
                case 0: { // assignment stmt
                    printf("stmt 1 \n");
                    return buildASTRecursive(currNode -> children[0]);
                }

                case 1: { // iterative stmt
                    printf("stmt 2 \n");
                    return buildASTRecursive(currNode -> children[0]);
                }

                case 2: { // coinditional stmt
                    printf("stmt 3 \n");
                    return buildASTRecursive(currNode -> children[0]);
                }

                case 3: { // io stmt
                    printf("stmt 4 \n");
                    return buildASTRecursive(currNode -> children[0]);
                }
                
                case 4: { // funcCallStmt
                    printf("stmt 6 \n");
                    return buildASTRecursive(currNode -> children[0]);
                }
            }
            printf("stmt 7 \n");
        }

        case 26: { // assignment stmt
            ASTNode* assignmentStmtNode = initializeASTNode(false, assignmentStmt);
            ASTNode* singleOrRecordIdNode = buildASTRecursive(currNode -> children[0]);
            ASTNode* arithmeticExpNode = buildASTRecursive(currNode -> children[2]);

            populateChild(assignmentStmtNode, singleOrRecordIdNode);
            populateChild(assignmentStmtNode, arithmeticExpNode);
            
            return assignmentStmtNode;
        }
               
        case 27: { // iterative
            ASTNode* iterativeNode = initializeASTNode(false, iterativeStmt);
            ASTNode* booleanExpressionNode = buildASTRecursive(currNode->children[2]);
            ASTNode* stmtNode = buildASTRecursive(currNode->children[4]);
            ASTNode* otherstmtsNode = buildASTRecursive(currNode->children[5]);

            populateChild(iterativeNode, booleanExpressionNode);
            populateChild(iterativeNode, stmtNode);
            populateChildren(iterativeNode, otherstmtsNode);

            return iterativeNode;
        }

        case 28: { // conditionalStmt
            ASTNode* conditionalStmtNode = initializeASTNode(false, conditionalStmt); // this should point to new scope
            ASTNode* booleanExpressionNode = buildASTRecursive(currNode -> children[2]);
            ASTNode* stmtNode = buildASTRecursive(currNode -> children[5]);
            ASTNode* otherStmtsList = buildASTRecursive(currNode -> children[6]);
            ASTNode* elseConditionStmt = buildASTRecursive(currNode -> children[7]);

            populateChild(conditionalStmtNode, booleanExpressionNode);
            populateChild(conditionalStmtNode, stmtNode);
            populateChildren(conditionalStmtNode, otherStmtsList);
            populateChild(conditionalStmtNode, elseConditionStmt);
            
            free(otherStmtsList);
            return conditionalStmtNode;
        }
        
        case 29: { // iostmt
            printf("iostmt 0 \n");
            ASTNode* ioStmtNode = initializeASTNode(false, ioStmt);
            ASTNode* readOrWrite;
            printf("iostmt 0.5 \n");
            switch (currNode -> ruleNumber) {
                case 0: {
                    printf("iostmt 1 \n");
                    readOrWrite = initializeASTNode(true, readFunc);
                }

                case 1: {
                    printf("iostmt 2 \n");
                    readOrWrite = initializeASTNode(true, writeFunc);
                }
            }
            printf("iostmt 3 \n");
            ASTNode* varNode = buildASTRecursive(currNode -> children[2]);
            printf("iostmt 4\n");
            populateChild(ioStmtNode, readOrWrite);
            printf("iostmt 5 \n");
            populateChild(ioStmtNode, varNode);
            printf("iostmt 6 \n");

            return ioStmtNode;
        }

        case 30:{ //funcall stmt
            ASTNode* funcallstmtNode = initializeASTNode(false, funcallstmt);
            ASTNode* outputparameters = buildASTRecursive(currNode->children[0]);
            ASTNode* TK_FUNID_NODE = initializeASTNode(true, tkfunid);
            TK_FUNID_NODE->astNode->astFunIdNode = (AST_TK_FUNID*)malloc(sizeof(AST_TK_FUNID));
            TK_FUNID_NODE->astNode->astFunIdNode->funDefinition = NULL;
            ASTNode* inputparameters = buildASTRecursive( currNode-> children[5]);
            
            populateChild(funcallstmtNode, outputparameters);
            populateChild(funcallstmtNode, TK_FUNID_NODE);
            populateChild(funcallstmtNode, inputparameters);

            return funcallstmtNode;

        }

        case 31: { // singleOrRecordId
            ASTNode* singleOrRecordIdNode = initializeASTNode(false, singleOrRecordId);
            
            ASTNode* TK_ID_Node = initializeASTNode(true, tkid);
            TK_ID_Node -> astNode -> astIDNode = (AST_TK_ID*) malloc(sizeof(AST_TK_ID));
            TK_ID_Node -> astNode -> astIDNode->IdName = currNode -> children[0] -> lexeme;

            populateChild(singleOrRecordIdNode, TK_ID_Node);

            ASTNode* constructedVariableNode = buildASTRecursive(currNode -> children[1]);
            populateChild(singleOrRecordIdNode, constructedVariableNode);

            return singleOrRecordIdNode;
        }

        case 32: { // arithmetic expression
            ASTNode* termNode = buildASTRecursive(currNode -> children[0]);
            ASTNode* arithmeticExpressionNode = ASTarithmeticHelper(currNode ->children[1], termNode);
            return arithmeticExpressionNode;
        }
        
        case 33: { // constructedVariable
            ASTNode* constructedVariableNode = initializeASTNode(false, constructedVariable);
            switch (currNode -> ruleNumber) {
                
                case 0: {
                    ASTNode* oneExpansionNode = buildASTRecursive(currNode -> children[0]);
                    ASTNode* moreExpansionsNode = buildASTRecursive(currNode -> children[1]);

                    populateChild(constructedVariableNode, oneExpansionNode);
                    populateChildren(constructedVariableNode, moreExpansionsNode);
                    free(moreExpansionsNode);
                    return constructedVariableNode;
                }

                case 1: {
                    return constructedVariableNode;
                }
            }
        }
        
        case 34: { // oneExpansion
            ASTNode* TK_FIELDID_Node = initializeASTNode(true, tkFieldId);
            TK_FIELDID_Node -> astNode -> astFieldIdNode = (AST_TK_FIELDID*) malloc(sizeof(AST_TK_FIELDID));
            TK_FIELDID_Node -> astNode -> astFieldIdNode->fieldName = currNode -> children[1] -> lexeme;
            return TK_FIELDID_Node;
        }

        case 35: { // moreExpansion
            ASTNode* moreExpansionsNode = initializeASTNode(false, moreExpansions);
            
            if (currNode -> ruleNumber == 0) {
                ASTNode* oneExpansionNode = buildASTRecursive(currNode->children[0]);
                ASTNode* moreExpansionsList = buildASTRecursive(currNode->children[1]);

                populateChild(moreExpansionsNode, oneExpansionNode);
                populateChildren(moreExpansionsNode, moreExpansionsList);
                free(moreExpansionsList);
            }
            
            return moreExpansionsNode;
        }

        case 36:{ //outputParameters
            ASTNode* outputparametersNode = initializeASTNode(false, outputparameters);
            switch(currNode -> ruleNumber){
                case 0:{
                    ASTNode* idList = buildASTRecursive(currNode->children[1]);

                    populateChildren(outputparametersNode,idList);
                   
                    return outputparametersNode;
                }
                case 1:{
                    ASTNode* Null = initializeASTNode(true, null);
                   
                    populateChild(outputparametersNode, Null);

                    return outputparametersNode;
                }
            }
        }

        case 37:{ //inputParameters
            ASTNode* inputparametersNode = initializeASTNode(false, inputparameters);
            ASTNode* idList = buildASTRecursive(currNode->children[1]);
            
            populateChildren(inputparametersNode, idList);

            return inputparametersNode;
        }

        case 38:{ //idlist
            ASTNode* idListNode = initializeASTNode(false, idlist);
            ASTNode* TK_ID_NODE = initializeASTNode(true, tkid);
            TK_ID_NODE->astNode->astIDNode = (AST_TK_ID*) malloc(sizeof(AST_TK_ID));
            TK_ID_NODE->astNode->astIDNode->IdName =  currNode -> children[0]->lexeme;
            ASTNode* more_ids = buildASTRecursive( currNode -> children[1]);

            populateChild(idListNode,TK_ID_NODE);
            populateChildren(idListNode, more_ids);

            free(idListNode);

            return idListNode;
        }
         
        case 39:{//booleanExpression
            ASTNode* booleanExpressionNode = initializeASTNode(false, booleanExpression);
            switch(currNode -> ruleNumber){
                case 0:{

                    ASTNode* booleanExpression_1 = buildASTRecursive(currNode->children[1]);
                    ASTNode* logicalopNode = buildASTRecursive(currNode->children[3]);
                    ASTNode* booleanExpression_2 = buildASTRecursive(currNode->children[5]);

                    populateChild(booleanExpressionNode,booleanExpression_1);
                    populateChild(booleanExpressionNode, logicalopNode);
                    populateChild(booleanExpressionNode, booleanExpression_2);

                    return booleanExpressionNode;
                }
                
                case 1:{
                    ASTNode* var_1 = buildASTRecursive(currNode -> children[0]);
                    ASTNode* relationalopNode = buildASTRecursive(currNode -> children[1]);
                    ASTNode* var_2 = buildASTRecursive(currNode -> children[2]);

                    populateChild(booleanExpressionNode, var_1);
                    populateChild(booleanExpressionNode, relationalopNode);
                    populateChild(booleanExpressionNode, var_2);
                    
                    return booleanExpressionNode;
                }
                
                case 2:{
                    ASTNode* booleanExpression_3 = buildASTRecursive(currNode->children[2]);
                    ASTNode* notNode = initializeASTNode(true, tknot);
                    
                    populateChild(booleanExpressionNode, booleanExpression_3);
                    populateChild(booleanExpression_3, notNode);
                    

                    return booleanExpressionNode;
                }
            }
            
        }       
        
        case 40: { // elsecondition
            ASTNode* elseConditionNode = initializeASTNode(false, errorCondition);
            switch (currNode -> ruleNumber) {
                
                case 0: {
                    ASTNode* stmtNode = buildASTRecursive(currNode -> children[1]);
                    ASTNode* otherStmtsNode = buildASTRecursive(currNode -> children[2]);
                    // this will also have its own scope.
                    populateChild(elseConditionNode, stmtNode);
                    populateChildren(elseConditionNode, otherStmtsNode);
                    free (otherStmtsNode);

                    return elseConditionNode;
                }

                case 1: {
                    return elseConditionNode;
                }
            }
        }
        
        case 41: { // var
            switch (currNode->ruleNumber) {
                case 0: { // singleorrecordid
                    return buildASTRecursive(currNode->children[0]);
                }

                case 1: {
                    ASTNode* TK_NUM_Node = initializeASTNode(true, tknum);
                    // have to create a node to store the value of tknum
                    return TK_NUM_Node;
                }

                case 2: {
                    ASTNode* TK_RNUM_Node = initializeASTNode(true, tkrnum);
                    // have to create an node to store the value of tkrnum
                    return TK_RNUM_Node;
                }
            }
        }

        case 42:{ // term
            ASTNode* factorNode = buildASTRecursive(currNode -> children[0]);
            ASTNode* termNode = ASTtermHelper(currNode -> children[1], factorNode);


            return termNode;
        }

       /* case 43: { //expPrime
            ASTNode* expprimeNode = initializeASTNode(false, expprime);
            switch(currNode -> ruleNumber){
                case 0:{
                    ASTNode* lowprecedenceoperatorNode = buildASTRecursive(currNode -> children [0]);
                    ASTNode* termNode = buildASTRecursive(currNode -> children[1]);
                    ASTNode* expprime_1 = buildASTRecursive(currNode -> children[2]);

                    populateChild(expprimeNode, lowprecedenceoperatorNode);
                    populateChild(expprimeNode, termNode);
                    populateChildren(expprimeNode, expprime_1);

                    free(expprime_1);

                    return expprimeNode;
                }
                
                case 1:{
                    return initializeASTNode(true, null);
                }
            }
        }

        case 44: { //lowprecedenceoperator
            switch(currNode -> ruleNumber){
                case 0:{
                    return initializeASTNode(true, tkplus);
                }
                case 1: {
                    return initializeASTNode(true, tkminus);
                }
            }

        }*/
        
        case 45: { //factor
            switch(currNode -> ruleNumber){
                case 0:{
                    return buildASTRecursive(currNode -> children[1]);
                }
                case 1:{
                    return buildASTRecursive(currNode -> children[0]);
                }
            }

        }

       /* case 46: { //termPrime
            ASTNode* termPrimeNode = intializeASTNode(false, termPrime);
            switch(currNode -> ruleNumber){
                case 0:{
                    ASTNode* highPrecedenceOperatorNode = buildASTRecursive( currNode -> children[0]);
                    ASTNode* factorNode = buildASTRecursive( currNode -> children[1]);
                    ASTNode* termPrime_1 = buildASTRecursive( currNode -> children[2]);

                    populateChild(termPrimeNode, highPrecedenceOperatorNode);
                    populateChild(termPrimeNode, factorNode);
                    populateChildren(termPrimeNode, termPrime_1);

                    free(termPrime_1);
                    
                    return termPrimeNode;
                }
            }

        }

        case 47:{ //highprecedenceoperator
            switch (currNode->ruleNumber) {
                case 0: {
                    return initializeASTNode(true, tkmul);
                }
                case 1: {
                    return initializeASTNode(true, tkdiv);
                }
            }
        }*/
        
        case 48: { //logicalOp
            switch(currNode -> ruleNumber){
                case 0:{
                    return initializeASTNode(true, tkand);
                }
                case 1:{
                    return initializeASTNode(true, tkor);
                }
            }
        }
        
        case 49:{ // relationalOp
            switch(currNode -> ruleNumber){
                case 0:{
                    return initializeASTNode(true, tklt);
                }
                case 1:{
                    return initializeASTNode(true, tkle);
                }
                case 2:{
                    return initializeASTNode(true, tkeq);
                }
                case 3:{
                    return initializeASTNode(true, tkgt);
                }
                case 4:{
                    return initializeASTNode(true, tkge);
                }
                case 5:{
                    return initializeASTNode(true, tkne);
                }
            }
        }
            
        case 50: {// optionalReturn

            ASTNode* optionalReturnNode = initializeASTNode(false, optionalReturn);
            switch(currNode -> ruleNumber){
                case 0:{
                    ASTNode* idList = buildASTRecursive(currNode -> children[1]);

                    populateChildren(optionalReturnNode, idList);
                    return optionalReturnNode;

                }
                case 1:{
                    return optionalReturnNode;
                }
            }
        }

        case 51:{ //moreids
            switch(currNode -> ruleNumber){
                case 0:{
                    return buildASTRecursive(currNode -> children[1]);
                }
                case 1:{
                    ASTNode* Null= initializeASTNode(true, null);
                    return Null;
                }
            }
        }
        
        case 52: { //a
            switch(currNode -> ruleNumber){
                case 0: {
                    return initializeASTNode(true, record);
                }
                case 1: {
                    return initializeASTNode(true, unionStruct);
                }
            }
        }
    }
}

ASTRoot* buildAST(ParseTreeRoot* parseTree) {
    ParseTreeNode* root = parseTree -> root;
    ASTRoot* ast = initializeAST();
    
    ast -> root =  buildASTRecursive(root);
    return ast;
}