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

// function for printing cmd line interface
void printInterface(){
    printf("\nChoose the required option\n\n");
    printf("Press 0 for exit\n");
    printf("Press 1 for clean code without comments\n");
    printf("Press 2 to get all the tokens of the source code from the lexer\n");
    printf("Press 3 to parse the source code\n");
    printf("Press 4 to get the total time taken for parsing\n");
}


int main(int argc,char** argv) {
    
    printf("\nLexer and Parser modules have been successfully implemented and worked for all the provided test cases(testcase 1-6).\n\n");

    char* sourcefile = argv[1];

    // appending utility character to the source file
    appendlastchar(sourcefile); 

    //
    initialize_Symbol_Table();

    Grammar* grammar;
    FirstAndFollow* firstAndFollowSets;

    // driver functionality
    while(1){
        printInterface();

        int choice = 0;
        scanf("%d",&choice);

        // case-1 : 0 for exit
        if( choice == 0) 
            break;
        
        // case-2 : 1 for removing tokens
        else if(choice == 1){
            FILE *fptr1 = fopen(sourcefile, "r");
            FILE *fptr2 = fopen("Comment_less_file.txt","w+");
            
            removeComments(fptr1,fptr2); 
            
            fclose(fptr1);
            fclose(fptr2);
        }

        // case-3 : 2 for tokenizing source code & printing to terminal
        else if(choice ==2){
            FILE *fptr1 = fopen(sourcefile, "r"); 

            printTokenInFile(fptr1); 

            fclose(fptr1);
        }

        // case-4 :  3 for parsing the input source code
        else if(choice ==3){
            // storing the grammar of the langugae into the grammar data structure
            grammar = generateGrammarFromFile("grammar.txt");

            // calculating first and follow sets and storing them in firstAndFollowSets data strcture
            firstAndFollowSets = computeFirstAndFollowSets(grammar);
            
            // creating the parse table
            ParseTable table;
            table = createParseTable(firstAndFollowSets, table, grammar);
            
            // parsing the input source code
            ParseTreeRoot *tree = parseInputSourceCode(sourcefile, table, grammar, firstAndFollowSets);

            // printing the parsetree to outfile.txt
            if (tree != NULL)
                printParseTree(tree,argv[2]);
            
        }

        // case-5 : 4 for computing time taken for parsing
        else if(choice == 4){
            clock_t start_time, end_time;
            double total_CPU_time, total_CPU_time_in_seconds;
            
            start_time = clock();

            grammar = generateGrammarFromFile("grammar.txt");
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