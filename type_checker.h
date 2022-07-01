// Group 21 
// Raghava Kasyap Kristipati - 2019A7PS0087P
// K.V.S Preetam             - 2019A7PS0030P
// Shanmukh Chandra Yama     - 2019A7PS0028P
// Uday Dheeraj Nulu         - 2019A7PS0083P
// Yadagiri Shiva Sai Sashank - 2019A7PS0068P

#include "type_checkerdef.h"

typedef struct Errorlist{
    char* error;
}Errorlist;

void assignmentCheck(ASTNode* declaration,Errorlist* els);
SymbolInstance* lookupSymbolTable(SymbolTable* lookupTable, char* lexeme, SymbolTag symboltag);
ArithmeticTypeExpression* getArithmeticType(ASTNode* arithmeticExpressionNode, Errorlist* els);