// Group 21 
// Raghava Kasyap Kristipati - 2019A7PS0087P
// K.V.S Preetam             - 2019A7PS0030P
// Shanmukh Chandra Yama     - 2019A7PS0028P
// Uday Dheeraj Nulu         - 2019A7PS0083P
// Yadagiri Shiva Sai Sashank - 2019A7PS0068P

#ifndef AST_H
#define AST_H

#include "astDef.h"

ASTRoot* buildAST(ParseTreeRoot* parseTree);
ASTNode* buildASTRecursive(ParseTreeNode* currNode);
ASTRoot* initializeAST();
ASTNode* initializeASTNode(bool isLeaf, NodeLabels nodeLabel);
void populateChild(ASTNode* root, ASTNode* child);
void populateChildren(ASTNode* root, ASTNode* child);
Operator fetchoperator(ParseTreeNode* parseNode);
ASTNode* ASTarithmeticHelper(ParseTreeNode* expprimeParseNode, ASTNode* termNode);
ASTNode* ASTtermHelper(ParseTreeNode* termprimeParseNode, ASTNode* factorNode);
void printAST(ASTNode* AST);
void printASTtoConsole();
void get_astmemory();
void get_astnodecount();
#endif