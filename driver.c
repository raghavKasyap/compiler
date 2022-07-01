#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
// Group 21 
// Raghava Kasyap Kristipati - 2019A7PS0087P
// K.V.S Preetam             - 2019A7PS0030P
// Shanmukh Chandra Yama     - 2019A7PS0028P
// Uday Dheeraj Nulu         - 2019A7PS0083P
// Yadagiri Shiva Sai Sashank - 2019A7PS0068P

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symbol_table.h"

// function for printing cmd line interface
void printInterface(){
    printf("\nChoose the required option\n\n");
    printf("Press 0 for exit\n");
    printf("Press 1 to get all the tokens of the source code from the lexer\n");
    printf("Press 2 to print parse tree on console\n");
    printf("Press 3 to print AST in preorder traversal\n");
    printf("Press 4 to get the memory and no.of nodes in ast and parse tree \n");
    printf("Press 5 to print Symbol Table Records\n");
    printf("Press 6 to print the details of global variables\n");
    printf("Press 8 to print the records information \n");
}


int main(int argc,char** argv) {
    
    printf("\nLexer and Parser modules have been successfully implemented and worked for all the provided test cases(testcase 1-6). AST works on all giving test cases. Symbol Table also works on given testcases but gives a segmentation fault for the error testcase.\n\n");

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
        
        // // case-2 : 1 for removing tokens
        // else if(choice == 1){
        //     FILE *fptr1 = fopen(sourcefile, "r");
        //     FILE *fptr2 = fopen("Comment_less_file.txt","w+");
            
        //     removeComments(fptr1,fptr2); 
            
        //     fclose(fptr1);
        //     fclose(fptr2);
        // }

        // case-2 : 1 for tokenizing source code & printing to terminal
        else if(choice ==1){
            FILE *fptr1 = fopen(sourcefile, "r"); 

            printTokenInFile(fptr1); 

            fclose(fptr1);
        }

        // case-3 :  2 for parsing the input source code to teminal
        else if(choice ==2){
            // storing the grammar of the langugae into the grammar data structure
            grammar = generateGrammarFromFile("grammar.txt");

            // calculating first and follow sets and storing them in firstAndFollowSets data strcture
            firstAndFollowSets = computeFirstAndFollowSets(grammar);
            
            // creating the parse table
            ParseTable table;
            table = createParseTable(firstAndFollowSets, table, grammar);
            
            // parsing the input source code
            ParseTreeRoot *tree = parseInputSourceCode(sourcefile, table, grammar, firstAndFollowSets);

            // printing the parsetree to console
            if (tree != NULL)
                printparsetreeconsole(tree);
            
        }

        // case 4: 3 for printing the ast to console
        else if(choice == 3){
            printf("\n\n The Preorder traversal of ast nodes : \n");
            grammar = generateGrammarFromFile("grammar.txt");

            // calculating first and follow sets and storing them in firstAndFollowSets data strcture
            firstAndFollowSets = computeFirstAndFollowSets(grammar);
            
            // creating the parse table
            ParseTable table;
            table = createParseTable(firstAndFollowSets, table, grammar);
            
            // parsing the input source code
            ParseTreeRoot *tree = parseInputSourceCode(sourcefile, table, grammar, firstAndFollowSets);


            ASTRoot* ast = buildAST(tree);
            printAST(ast -> root);
            
            //SymbolTable* globalSymbolTable = constructSymbolTable(ast);
        }

        // case 5 : 4 for printing the number of nodes in parse tree and ast tree
        else if(choice ==4){
            grammar = generateGrammarFromFile("grammar.txt");

            // calculating first and follow sets and storing them in firstAndFollowSets data strcture
            firstAndFollowSets = computeFirstAndFollowSets(grammar);
            
            // creating the parse table
            ParseTable table;
            table = createParseTable(firstAndFollowSets, table, grammar);
            
            // parsing the input source code
            ParseTreeRoot *tree = parseInputSourceCode(sourcefile, table, grammar, firstAndFollowSets);

            ASTRoot* ast = buildAST(tree);

            printf("No.of parse tree nodes: %d\n", tree->parsetree_nodecount);
            printf("Amount of memory consumed by parse tree nodes: %d\n", tree->parsetree_memory);

            printf("No.of ast nodes: %d\n", ast->ast_nodes);
            printf("Amount of memory consumed by ast nodes: %d\n", ast->ast_memory);

            float compression_percent =  ((float)(tree->parsetree_memory - ast->ast_memory)/tree->parsetree_memory)*100.0;
            printf("Compression Percentage: %f\n",compression_percent);
        }

        
        // // case-4 :  3 for parsing the input source code
        // else if(choice ==3){
        //     // storing the grammar of the langugae into the grammar data structure
        //     grammar = generateGrammarFromFile("grammar.txt");

        //     // calculating first and follow sets and storing them in firstAndFollowSets data strcture
        //     firstAndFollowSets = computeFirstAndFollowSets(grammar);
                //     // creating the parse table
        //     ParseTable table;
        //     table = createParseTable(firstAndFollowSets, table, grammar);
            
        //     // parsing the input source code
        //     ParseTreeRoot *tree = parseInputSourceCode(sourcefile, table, grammar, firstAndFollowSets);

        //     // printing the parsetree to outfile.txt
        //     if (tree != NULL)
        //         printParseTree(tree,argv[2]);
            
        // }

        // case-5 : 4 for computing time taken for parsing
        // else if(choice == 4){
        //     clock_t start_time, end_time;
        //     double total_CPU_time, total_CPU_time_in_seconds;
            
        //     start_time = clock();

        //     grammar = generateGrammarFromFile("grammar.txt");
        //     firstAndFollowSets = computeFirstAndFollowSets(grammar);
            
        //     ParseTable table;
        //     table = createParseTable(firstAndFollowSets, table, grammar);
            
        //     ParseTreeRoot* tree = parseInputSourceCode(sourcefile, table, grammar, firstAndFollowSets);

        //     end_time = clock();
        //     total_CPU_time = (double) (end_time - start_time);
        //     total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;

        //     printf("\nTotal CPU Time = %.2f clocks \nTotal CPU Time(in secs) = %.3f seconds \n \n", total_CPU_time, total_CPU_time_in_seconds);
        // }
        // option for printing the symbol table records
        else if(choice == 5){
            // storing the grammar of the langugae into the grammar data structure
            grammar = generateGrammarFromFile("grammar.txt");

            // calculating first and follow sets and storing them in firstAndFollowSets data strcture
            firstAndFollowSets = computeFirstAndFollowSets(grammar);
            
            // creating the parse table
            ParseTable table;
            table = createParseTable(firstAndFollowSets, table, grammar);
            
            // parsing the input source code
            ParseTreeRoot *tree = parseInputSourceCode(sourcefile, table, grammar, firstAndFollowSets);

            ASTRoot* ast = buildAST(tree);

            SymbolTable* globalSymbolTable = constructSymbolTable(ast);
            printf("Printing the Symbol Table \n");
            printSymbolTable(globalSymbolTable);
        }

        // option for printing the global variables
        else if(choice == 6){
            // storing the grammar of the langugae into the grammar data structure
            grammar = generateGrammarFromFile("grammar.txt");

            // calculating first and follow sets and storing them in firstAndFollowSets data strcture
            firstAndFollowSets = computeFirstAndFollowSets(grammar);
            
            // creating the parse table
            ParseTable table;
            table = createParseTable(firstAndFollowSets, table, grammar);
            
            // parsing the input source code
            ParseTreeRoot *tree = parseInputSourceCode(sourcefile, table, grammar, firstAndFollowSets);

            ASTRoot* ast = buildAST(tree);

            SymbolTable* globalSymbolTable = constructSymbolTable(ast);
            printf("Printing the Global Variables \n");
            printGlobalVariables(globalSymbolTable);
        }
        else if(choice == 8){
            // storing the grammar of the langugae into the grammar data structure
            grammar = generateGrammarFromFile("grammar.txt");

            // calculating first and follow sets and storing them in firstAndFollowSets data strcture
            firstAndFollowSets = computeFirstAndFollowSets(grammar);
            
            // creating the parse table
            ParseTable table;
            table = createParseTable(firstAndFollowSets, table, grammar);
            
            // parsing the input source code
            ParseTreeRoot *tree = parseInputSourceCode(sourcefile, table, grammar, firstAndFollowSets);

            ASTRoot* ast = buildAST(tree);

            SymbolTable* globalSymbolTable = constructSymbolTable(ast);
            printf("Printing the Record and Union Variables \n");
            printRecordVariables(globalSymbolTable);
        }   
    }
    

    return 0;
}