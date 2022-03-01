#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

Grammar * generateGrammarFromFile() {
    Grammar* grammar = (Grammar *)malloc(NUM_NON_TERMINALS * sizeof(Grammar));

    FILE* fptr = fopen("Complete Grammar.txt", "r");
    
    if (fptr == NULL) {
        printf("Please include the Grammar file");
    }
    else {
        char arr[52];
        string str ="";
        int index=0, lastIndex=-1;
        while(fptr != NULL){
            fscanf(fptr,%s,str);
            if(index==0){
                arr[index] = str;
                index++;
                lastIndex++;
            }
            else{
                if(!strcmp(str,arr[lastIndex])){
                    
                }
                else{
                    arr[index]=str;
                    index++;
                    lastIndex++;
                }
            }
            while(*fptr != "\n"){
                fptr++;
            }
            fptr++;
            str="";

        }
        fclose(fptr);
        i=0;
        while(i<52){
            print(%s,arr[i]);
            i++;
        }

    return grammar;
}


// store the grammar into grammar datastr

int main() {
    Grammar* grammar;
    grammar = generateGrammarFromFile();
}