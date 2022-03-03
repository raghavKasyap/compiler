#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "parser.c"
#include "lexer.c"
int main(){
    Grammar* grammar;
    FirstAndFollow* firstAndFollowSets;
    appendlastchar();
    initialize_Symbol_Table();
    FILE *fptr = fopen("t1.txt","r");
    fclose(fptr);
    return 0;
}
int main() {
    
    
    // storing the grammar of the langugae into the grammar data structure
    
    // calculating first and follow sets and storing them in firstAndFollowSets data strcture
   
    printAllSets(firstAndFollowSets);

}
int main(){
    char* sourcefile = "t1.txt";
    appendlastchar(sourcefile);
    initialize_Symbol_Table();
    while(1){
        printf("Press 0 for exit\n");
        printf("Press 1 for clean code without comments\n");
        printf("Press 2 to get all the tokens of the source code from the lexer\n");
        printf("Press 3 to parse the source code\n");
        printf("Press 4 to get the total time taken for parsing\n");
        int choice = 0;
        scanf("%d",&choice);
        if(choice == 0) break;
        else if(choice == 1){
            FILE *fptr1 = fopen("t1.txt","r");
            FILE *fptr2 = fopen("t1-clean.txt","w+");
            removeComments(fptr1,fptr2);
            fclose(fptr1);
            fclose(fptr2);
        }
        else if(choice ==2){
            FILE *fptr3 = fopen("t1.txt","r");
            while(1){
                token_info* tk = getNextToken(fptr3);
                printf("%d ",tk->linenum);
                printf("%s ",tk->lexeme);
                printf("%d ",TerminalIDs[(tk->token_id)]);
                if(tk->lexeme == "EOF") break;
            }
            fclose(fptr3);
        }
        else if(choice ==3){
            Grammar* grammar;
            FirstAndFollow* firstAndFollowSets;
            grammar = generateGrammarFromFile("Complete Grammar.txt");
            firstAndFollowSets = computeFirstAndFollowSets(grammar);
            ParseTable table;
            table = createParseTable(firstAndFollowSets, table, grammar);
            ParseTreeRoot* tree = parseInputSourceCode("outfile.txt", table, grammar, firstAndFollowSets);
            printParseTree(tree, "outfile.txt");
        }
    }
    return 0;
}