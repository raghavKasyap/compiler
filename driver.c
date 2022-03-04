#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "lexer.h"
#include "parser.h"


int main() {
    char* sourcefile = "t1.txt";
    appendlastchar(sourcefile);
    initialize_Symbol_Table();
    Grammar* grammar;
    FirstAndFollow* firstAndFollowSets;
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
            printTokenInFile(sourcefile);
        }
        else if(choice ==3){
            // storing the grammar of the langugae into the grammar data structure
            grammar = generateGrammarFromFile("Complete Grammar.txt");

            // calculating first and follow sets and storing them in firstAndFollowSets data strcture
            firstAndFollowSets = computeFirstAndFollowSets(grammar);
            ParseTable table;
            table = createParseTable(firstAndFollowSets, table, grammar);
            
            ParseTreeRoot *tree = parseInputSourceCode(sourcefile, table, grammar, firstAndFollowSets);
            
            printf("working");
            printParseTree(tree, "outfile.txt");
        }
        else if(choice == 4){
            clock_t start_time, end_time;
            double total_CPU_time, total_CPU_time_in_seconds;
            start_time = clock();


            grammar = generateGrammarFromFile("Complete Grammar.txt");
            firstAndFollowSets = computeFirstAndFollowSets(grammar);
            ParseTable table;
            table = createParseTable(firstAndFollowSets, table, grammar);
            ParseTreeRoot* tree = parseInputSourceCode(sourcefile, table, grammar, firstAndFollowSets);

            end_time = clock();
            total_CPU_time = (double) (end_time - start_time);
            total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
            printf("\nTotal CPU Time = %.2f clocks \nTotal CPU Time(in secs) = %.3f seconds \n \n", total_CPU_time, total_CPU_time_in_seconds);
        }
    }
    return 0;
}