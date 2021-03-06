// Group 21 
// Raghava Kasyap Kristipati - 2019A7PS0087P
// K.V.S Preetam             - 2019A7PS0030P
// Shanmukh Chandra Yama     - 2019A7PS0028P
// Uday Dheeraj Nulu         - 2019A7PS0083P
// Yadagiri Shiva Sai Sashank - 2019A7PS0068P

#ifndef LEXICAL_ANALYZER_DEF_H
#define LEXICAL_ANALYZER_DEF_H

#define BLOCK_SIZE 16384
#define LEX_BUFFER_INITIAL_SIZE 16
#define SYMBOL_TABLE_INITIAL_CAPACITY 30

#define NUM_TERMINALS 59
#define EPS 58
#define DOLLAR 57
#define REALNUM 5
#define NUM 4

// data structure for storing the value of number (integer / real number)
typedef union Value {
    int i;
    float f;
 } Value;

// token datasturcture
typedef struct TokenInfo {
    int linenum;
    char* lexeme;
    int tokenId;
    Value* value;
    bool isError;
} TokenInfo;

// symbol table datastructure
typedef struct Symbol_Table_Record {
    TokenInfo* tk_info;
    struct Symbol_Table_Record* nextRecord;
} Symbol_Table_Record;

#endif
