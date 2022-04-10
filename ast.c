#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"

ASTRoot* initializeAST() {
    ASTRoot* ast = (ASTRoot *) malloc (sizeof(ASTRoot));
    return ast;
}

ASTNode* initializeASTNode(bool isLeaf, NodeLabels nodeLabel) {
    ASTNode* newNode = (ASTNode*) malloc(sizeof(ASTNode));
    newNode -> parent = NULL;
    newNode -> children = NULL;
    newNode -> numberOfChildren = 0;
    newNode -> astNode = NULL;
    newNode -> isLeaf = isLeaf;
    newNode -> label = nodeLabel;
    return newNode;
} 

void populateChild(ASTNode* root, ASTNode* child) {
    root -> children = realloc(root, root -> numberOfChildren + 1);
    root -> children[root -> numberOfChildren] = child;
    root -> numberOfChildren++;
    child -> parent = root;
}

void populateChildren(ASTNode* root, ASTNode* child){
    // have to change this.
    int currentChildChildren = root -> currChildrenSize;
    int currentIndex = root->currChildrenSize;
    if(root->numberOfChildren - root -> currChildrenSize < currentChildChildren){
        root = realloc(root, 2*root->numberOfChildren);
        root -> numberOfChildren* = 2;
    }
    for(int i=0 ; i<child->currChildrenSize; i++){
        root[currentIndex] = child[i];
        currentIndex++;
    }
}

ASTNode* buildASTRecursive(ParseTreeNode* currNode) {
    int ruleNumber = currNode -> ruleNumber;
    int symbolId = currNode -> symbolId;
    
    switch (symbolId) {
        case 0: { // program 
            ASTNode* programNode = initializeASTNode(false, program);
            ASTNode* otherFunctionsList = buildASTRecursive(currNode -> children[0], program);
            ASTNode* mainFunctionNode = buildASTRecursive(currNode -> children[1], program);

            populateChildren(programNode, otherFunctionsList);
            populateChildren(programNode, mainFunctionNode);
            free(otherFunctionsList);
            return programNode;
        }
        
        case 1: { // main function
            ASTNode* mainFunctionNode = initializeASTNode(false, function);
            // main function should have access to global scope.

            ASTNode* stmts = buildASTRecursive(currNode -> children[1]); // building the ast subtree for stmts
            
            populateChildren(mainFunctionNode, stmts);
            free(stmts); // removing the stmts nodes as it is no longer required
        }

        case 2: {  // otherfunctions
            ASTNode* otherfunctions = initializeASTNode(false, otherfunctions);
            ASTNode* function = buildASTRecursive(currNode->children[0]);
            ASTNode* otherfunctions_1 = buildASTRecursive(currNode->children[1]);

            populateChild(otherfunctions, function);
            populateChildren(otherfunctions, otherfunctions_1);
        }

        case 3: { // stmts
            ASTNode* stmtsNode = initializeASTNode(false, stmts);
            ASTNode* typeDefinitionsNode = buildASTRecursive(currNode -> children[0]);
            ASTNode* declarationsNode = buildASTRecursive(currNode -> children[1]);
            ASTNode* otherStmtsNode = buildASTRecursive(currNode -> children[2]);
            ASTNode* returnStmtsNode = buildASTRecursive(currNode -> children[3]);

            // adding these nodes to the stmtsNode as children nodes
        }

        case 12: { // typedefinitionsNode;
            switch (currNode -> ruleNumber) {
                ASTNode* typedefinitionsNode = initializeAST(false, typeDefinition);
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

        case 17: { // actualOrRedefined variable
            switch (currNode -> ruleNumber) {
                case 0: { // typedefinition
                    return buildASTRecursive(currNode -> children[0]);
                }

                case 1: { // definetypestmt
                    return buildASTRecursive(currNode -> children[1]);
                }
            }
        }

        case 18: { // typedefinion
        
            switch (currNode -> ruleNumber) {
                case 0: {  // record definition
                    ASTNode* recordTypeDefinitionNode = initializeAST(false, recordDefinition);
                    ASTNode* TK_RUID_Node = initializeAST(true, tkRuid);
                    
                    TK_RUID_Node -> astNode -> astRUIDNo.ruidName = currNode -> children[1] -> lexeme;
                    populateChild(recordTypeDefinitionNode, TK_RUID_Node);
                    
                    ASTNode* fieldDefinitionsList = buildASTRecursive(currNode -> children[2]);
                    populateChildren(recordTypeDefinitionNode, fieldDefinitionsList);
                    free(fieldDefinitionsList);
                    
                    return recordTypeDefinitionNode;
                }

                case 1: {  // union definition
                    ASTNode* unionTypeDefinitionNode = initializeAST(false, unionDefinition);
                    ASTNode* TK_RUID_Node = initializeAST(true, tkRuid);
                    
                    TK_RUID_Node -> astNode -> astRUIDNo.ruidName = currNode -> children[1] -> lexeme;
                    populateChild(unionTypeDefinitionNode, TK_RUID_Node);
                    
                    ASTNode* fieldDefinitionsList = buildASTRecursive(currNode -> children[2]);
                    populateChildren(unionTypeDefinitionNode, fieldDefinitionsList);
                    free(fieldDefinitionsList);

                    return unionTypeDefinitionNode;
                }
            }
        }
        
        case 19: { // definetype statement
            ASTNode* definetypestmtNode = initializeAST(false, definetypestmt);
            ASTNode* A = buildASTRecursive(currNode -> children[1]);
            ASTNode* TK_RUID_NODE1 = initializeAST(true, tkRuid);
            ASTNode* TK_RUID_NODE2 = initializeAST(true, tkRuid);

            TK_RUID_NODE1 -> astNode -> astRUIDNode.ruidName = currNode -> children[2] ->lexeme;
            TK_RUID_NODE2 -> astNode -> astRUIDNode.ruidName = currNode -> children[4] ->lexeme;

            populateChild(definetypestmtNode, A);
            populateChild(definetypestmtNode, TK_RUID_NODE1);
            populateChild(definetypestmtNode, TK_RUID_NODE2);

        } 
        
        case 20: { // fieldDefinitions
            ASTNode* fieldDefinitionsNode = initializeAST(false, fieldDefinitions);
            ASTNode* fieldDefinition_1 = buildASTRecursive(currNode -> children[0]);
            ASTNode* fieldDefinition_2 = buildASTRecursive(currNode -> children[1]);
            
            populateChild(fieldDefinitionsNode, fieldDefinition_1);
            populateChild(fieldDefinitionsNode, fieldDefinition_2);

            ASTNode* moreFieldsNode = buildASTRecursive(currNode -> children[2]);
            populateChildren(fieldDefinitionsNode, moreFieldsNode);
            free(moreFieldsNode);
        }

        case 21: {  // fieldDefinition
            ASTNode* fieldDefinitionNode = initializeASTNode(false, fieldDefinition);
            ASTNode* fieldTypeNode = buildASTRecursive(currNode -> children[1]);
            ASTNode* TK_FIELDID_Node = initializeASTNode(true, tkFieldId);
            TK_FIELDID_Node -> astNode -> astFieldIdNode.fieldName = currNode -> children[3] -> lexeme;

            populateChild(fieldDefinitionNode, fieldTypeNode);
            populateChild(fieldDefinitionNode, TK_FIELDID_Node);
            return fieldDefinitionNode;
        }

        case 22: { // moreFields
            switch(currNode -> ruleNumber) {
                ASTNode* moreFieldsNode = initializeAST(false, moreFields);
                
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
                    ASTNode* TK_RUID_Node = initializeAST(true, tkRuid);
                    
                    TK_RUID_Node -> astNode -> astRUIDNo.ruidName = currNode -> children[1] -> lexeme;
                    return TK_RUID_Node;
                }
            }
        }
        
        case 10: { // primitveDataType
            switch (currNode -> ruleNumber) {
                case 0: { // INT_TYPE
                    return initializeAST(true, tkint);
                }

                case 1: { // REAL_TYPE
                    return initializeAST(true, tkreal);
                }
            }
        }
        
        case 52: { //a
            switch(currNode -> ruleNumber){
                case 0: {
                    return initializeASTNode(true, record);
                }
                case 1: {
                    return initializeASTNode(true, union);
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