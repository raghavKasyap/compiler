#ifndef AST_H
#define AST_H

#include "astDef.h"

ASTRoot* buildAST(ParseTreeRoot* parseTree);
ASTNode* buildASTRecursive(ParseTreeNode* currNode);
ASTRoot* initializeAST();
ASTNode* initializeASTNode(bool isLeaf, NodeLabels nodeLabel);
void populateChild(ASTNode* root, ASTNode* child);
void populateChildren(ASTNode* root, ASTNode* child);
char fetchoperator(ParseTreeNode* parseNode);
ASTNode* ASTarithmeticHelper(ParseTreeNode* expprimeParseNode, ASTNode* termNode);
ASTNode* ASTtermHelper(ParseTreeNode* termprimeParseNode, ASTNode* factorNode);

#endif