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
    printf("%s \n", astLabel[AST->label]);
    if (AST->isLeaf == 1) {
        return;
    }
    else {
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
            
            
            ASTNode* otherFunctionsList = buildASTRecursive(currNode -> children[0]);
            
            ASTNode* mainFunctionNode = buildASTRecursive(currNode -> children[1]);
            
            populateChildren(programNode, otherFunctionsList);
            
            populateChildren(programNode, mainFunctionNode);
            
            free(otherFunctionsList);
            return programNode;
            
        }
        
        case 2: { // main function
            
            ASTNode* mainFunctionNode = initializeASTNode(false, function);
            // main function should have access to global scope.

            ASTNode* stmts = buildASTRecursive(currNode -> children[1]); // building the ast subtree for stmts
            
            populateChildren(mainFunctionNode, stmts);
            
            free(stmts); // removing the stmts nodes as it is no longer required
            
            return mainFunctionNode;
        }

        case 1: {  // otherfunctions
            
            ASTNode* otherfunctionsNode = initializeASTNode(false, otherfunctions);
            switch(currNode -> ruleNumber){
                case 0:{
                    
                    ASTNode* function = buildASTRecursive(currNode->children[0]);
                    
                    ASTNode* otherfunctions_1 = buildASTRecursive(currNode->children[1]);
                    
                    populateChild(otherfunctionsNode, function);
                    
                    populateChildren(otherfunctionsNode, otherfunctions_1);
                    
                    
                    return otherfunctionsNode;
                }

                case 1:{
                    return otherfunctionsNode;
                }
            }

            
            ASTNode* function = buildASTRecursive(currNode->children[0]);
            
            ASTNode* otherfunctions_1 = buildASTRecursive(currNode->children[1]);
            
            populateChild(otherfunctionsNode, function);
            
            populateChildren(otherfunctionsNode, otherfunctions_1);
            
            
            return otherfunctionsNode;
        }

        case 3: { // stmts
            ASTNode* stmtsNode = initializeASTNode(false, stmts);
            
            ASTNode* typeDefinitionsNode = buildASTRecursive(currNode -> children[0]);
            
            ASTNode* declarationsNode = buildASTRecursive(currNode -> children[1]);
            
            ASTNode* otherStmtsNode = buildASTRecursive(currNode -> children[2]);
            
            ASTNode* returnStmtsNode = buildASTRecursive(currNode -> children[3]);
            

            // adding these nodes to the stmtsNode as children nodes
            populateChild(stmtsNode, typeDefinitionsNode);
            
            populateChild(stmtsNode, declarationsNode);
            
            populateChild(stmtsNode, otherStmtsNode);
            
            populateChild(stmtsNode, returnStmtsNode);
            

            return stmtsNode;
        }

        case 4:{ // function
            
            ASTNode* functionNode = initializeASTNode(false, function);
            
            ASTNode* TK_FUNID_Node = initializeASTNode(true, tkfunid);
            
            
            TK_FUNID_Node -> astNode -> astFunctionNode = (ASTFunctionNode *) malloc(sizeof(ASTFunctionNode));
            TK_FUNID_Node -> astNode -> astFunctionNode -> functionName = currNode -> children[0]->lexeme;
            
            ASTNode* input_par_Node = buildASTRecursive(currNode -> children[1]);
            
            ASTNode* output_par_Node = buildASTRecursive(currNode -> children[2]);
            
            ASTNode* stmtsNode = buildASTRecursive(currNode -> children[4]);
            

            populateChild(functionNode, TK_FUNID_Node);
            
            populateChild(functionNode, input_par_Node);
            
            populateChild(functionNode, output_par_Node);
            
            populateChildren(functionNode, stmtsNode);
            

            free(stmtsNode);

            return functionNode;
        }

        case 5:{ //input_par
            ASTNode* input_par_Node = initializeASTNode(false, inputpar);
            
            ASTNode* parameter_list_Node = buildASTRecursive(currNode -> children[4]);
            
            populateChildren(input_par_Node, parameter_list_Node);
            
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
            
            ASTNode* datatypeNode = buildASTRecursive(currNode -> children[0]);
            
            ASTNode* TK_ID_Node = initializeASTNode(true, tkid);
            
            TK_ID_Node->astNode->astIDNode = (AST_TK_ID*)malloc(sizeof(AST_TK_ID));
            TK_ID_Node->astNode->astIDNode->IdName = currNode -> children[1]->lexeme;
            
            ASTNode* remainingListNode = buildASTRecursive(currNode -> children[2]);
            

            populateChild(parameterListNode, datatypeNode);
            
            populateChild(parameterListNode, TK_ID_Node);
            
            populateChildren(parameterListNode, remainingListNode);
            
            free(remainingListNode);
            
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
            
            switch (currNode -> ruleNumber) {
                
                case 0: { // rule 0 which derives records and unions
                    
                    ASTNode* actualOrRedefined = buildASTRecursive(currNode -> children[0]);
                    
                    ASTNode* otherTypeDefinitions = buildASTRecursive(currNode -> children[1]);
                    
                    // add the first typedefinition 
                    populateChild(typedefinitionsNode, actualOrRedefined);
                    
                    // add all the below defined definitions
                    populateChildren(typedefinitionsNode, otherTypeDefinitions);
                    
                    free(otherTypeDefinitions);

                    return typedefinitionsNode;
                }

                case 1: {  // eps production
                    
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
            
            switch (currNode -> ruleNumber) {
                case 0: {
                    
                    ASTNode* stmtNode = buildASTRecursive(currNode->children[0]);
                    
                    ASTNode* otherStmtsList = buildASTRecursive(currNode->children[1]);
                    

                    populateChild(otherStmtsNode, stmtNode);
                    
                    populateChildren(otherStmtsNode, otherStmtsList);
                    
                    free(otherStmtsList);
                    return otherStmtsNode;
                }

                case 1: {
                    
                    return otherStmtsNode;
                }
            }
            
        }
        
        case 16: { //returnstmt
            
            ASTNode* returnstmtNode = initializeASTNode(false, returnstmt);
            ASTNode* optionalreturnNode = buildASTRecursive(currNode -> children[1]);
            

            populateChildren(returnstmtNode, optionalreturnNode);
            

            return returnstmtNode;
        }

        case 17: { // actualOrRedefined variable
            
            switch (currNode -> ruleNumber) {
                case 0: { // typedefinition
                     
                    return buildASTRecursive(currNode -> children[0]);
                }

                case 1: { // definetypestmt
                    
                    return buildASTRecursive(currNode -> children[0]);
                }
            }
            
        }

        case 18: { // typedefinition
            
            switch (currNode -> ruleNumber) {
                case 0: {  // record definition
                    
                    ASTNode* recordTypeDefinitionNode = initializeASTNode(false, recordDefinition);
                    ASTNode* TK_RUID_Node = initializeASTNode(true, tkRuid);

                    TK_RUID_Node -> astNode -> astRUIDNode = (AST_TK_RUID*)malloc(sizeof(AST_TK_RUID));
                    TK_RUID_Node -> astNode -> astRUIDNode->ruidName = currNode -> children[1] -> lexeme;
                    populateChild(recordTypeDefinitionNode, TK_RUID_Node);
                    

                    ASTNode* fieldDefinitionsList = buildASTRecursive(currNode -> children[2]);
                    
                    populateChildren(recordTypeDefinitionNode, fieldDefinitionsList);
                    
                    free(fieldDefinitionsList);
                    
                    return recordTypeDefinitionNode;
                }

                case 1: {  // union definition
                    
                    ASTNode* unionTypeDefinitionNode = initializeASTNode(false, unionDefinition);
                    ASTNode* TK_RUID_Node = initializeASTNode(true, tkRuid);
                    
                    TK_RUID_Node -> astNode -> astRUIDNode = (AST_TK_RUID*)malloc(sizeof(AST_TK_RUID));
                    TK_RUID_Node -> astNode -> astRUIDNode->ruidName = currNode -> children[1] -> lexeme;
                    populateChild(unionTypeDefinitionNode, TK_RUID_Node);
                    
                    ASTNode* fieldDefinitionsList = buildASTRecursive(currNode -> children[2]);
                    
                    populateChildren(unionTypeDefinitionNode, fieldDefinitionsList);
                    free(fieldDefinitionsList);

                    return unionTypeDefinitionNode;
                }
            }
            
        }
        
        case 19: { // definetype statement
            
            ASTNode* definetypestmtNode = initializeASTNode(false, definetypestmt);
            
            ASTNode* A = buildASTRecursive(currNode -> children[1]);
            ASTNode* TK_RUID_NODE1 = initializeASTNode(true, tkRuid);
            ASTNode* TK_RUID_NODE2 = initializeASTNode(true, tkRuid);

            
            TK_RUID_NODE1 -> astNode -> astRUIDNode = (AST_TK_RUID*)malloc(sizeof(AST_TK_RUID));
            TK_RUID_NODE2 -> astNode -> astRUIDNode = (AST_TK_RUID*)malloc(sizeof(AST_TK_RUID));

            

            TK_RUID_NODE1 -> astNode -> astRUIDNode->ruidName = currNode -> children[2] ->lexeme;
            TK_RUID_NODE2 -> astNode -> astRUIDNode->ruidName = currNode -> children[4] ->lexeme;

                        

            populateChild(definetypestmtNode, A);
            populateChild(definetypestmtNode, TK_RUID_NODE1);
            populateChild(definetypestmtNode, TK_RUID_NODE2);

                        

            return definetypestmtNode;
        } 
        
        case 20: { // fieldDefinitions
            
            ASTNode* fieldDefinitionsNode = initializeASTNode(false, fieldDefinitions);
            ASTNode* fieldDefinition_1 = buildASTRecursive(currNode -> children[0]);
            
            ASTNode* fieldDefinition_2 = buildASTRecursive(currNode -> children[1]);
            
            populateChild(fieldDefinitionsNode, fieldDefinition_1);
            populateChild(fieldDefinitionsNode, fieldDefinition_2);

            ASTNode* moreFieldsNode = buildASTRecursive(currNode -> children[2]);
            
            populateChildren(fieldDefinitionsNode, moreFieldsNode);
            
            free(moreFieldsNode);
            return fieldDefinitionsNode;
        }

        case 21: {  // fieldDefinition
            
            ASTNode* fieldDefinitionNode = initializeASTNode(false, fieldDefinition);
            ASTNode* fieldTypeNode = buildASTRecursive(currNode -> children[1]);
            
            ASTNode* TK_FIELDID_Node = initializeASTNode(true, tkFieldId);
            TK_FIELDID_Node -> astNode -> astFieldIdNode = (AST_TK_FIELDID*) malloc(sizeof(AST_TK_FIELDID));
            TK_FIELDID_Node -> astNode -> astFieldIdNode->fieldName = currNode -> children[3] -> lexeme;
            
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
                    
                    return buildASTRecursive(currNode -> children[0]);
                }

                case 1: { // ruid type
                    ASTNode* TK_RUID_Node = initializeASTNode(true, tkRuid);
                    
                    
                    TK_RUID_Node -> astNode -> astRUIDNode = (AST_TK_RUID*)malloc(sizeof(AST_TK_RUID));
                    TK_RUID_Node -> astNode -> astRUIDNode->ruidName = currNode -> children[0] -> lexeme;
                    
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
            
            switch(currNode -> ruleNumber) {
                case 0: { // assignment stmt
                    
                    return buildASTRecursive(currNode -> children[0]);
                }

                case 1: { // iterative stmt
                    
                    return buildASTRecursive(currNode -> children[0]);
                }

                case 2: { // coinditional stmt
                    
                    return buildASTRecursive(currNode -> children[0]);
                }

                case 3: { // io stmt
                    
                    return buildASTRecursive(currNode -> children[0]);
                }
                
                case 4: { // funcCallStmt
                    
                    return buildASTRecursive(currNode -> children[0]);
                }
            }
            
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
            
            ASTNode* ioStmtNode = initializeASTNode(false, ioStmt);
            ASTNode* readOrWrite;
            
            switch (currNode -> ruleNumber) {
                case 0: {
                    
                    readOrWrite = initializeASTNode(true, readFunc);
                }

                case 1: {
                    
                    readOrWrite = initializeASTNode(true, writeFunc);
                }
            }
            
            ASTNode* varNode = buildASTRecursive(currNode -> children[2]);
            
            populateChild(ioStmtNode, readOrWrite);
            
            populateChild(ioStmtNode, varNode);
            

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