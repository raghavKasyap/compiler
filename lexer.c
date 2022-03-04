#include "lexer.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
const char *keywords[28] = {
    "with",
    "parameters",
    "end",
    "while",
    "union",
    "endunion",
    "definetype",
    "as",
    "type",
    "_main",
    "global",
    "parameter",
    "list",
    "input",
    "output",
    "int",
    "real",
    "endwhile",
    "if",
    "then",
    "endif",
    "read",
    "write",
    "return",
    "call",
    "record",
    "endrecord",
    "else"};

int keyword_ids[28] = {8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 23, 24, 25, 26, 31, 34, 35, 36, 37, 38, 39, 44, 45, 46, 47};

char buff[2][BLOCK_SIZE];
int dfa_state = 1;
int linenum = 1;
int inputexhausted = 0;
int buffload = 0;
int fileended = 0;
int buffcurr = 1;
char *ptr1 = NULL;
char *ptr2 = NULL;
Symbol_Table_Record **Symbol_Table;

// this functions loads code into buffer
int getStream(FILE *fp)
{
    if (inputexhausted)
        return -1;

    size_t noOfChars = fread(buff[buffload], sizeof(char), BLOCK_SIZE / sizeof(char), fp);

    if (noOfChars == 0 || noOfChars < BLOCK_SIZE)
    {
        inputexhausted = 1;
    }

    buffload = 1 - buffload;
    buffcurr = 1 - buffcurr;

    if (!(noOfChars + 1))
    {
        printf("Error: Input Buffers failed to be loaded\n");
        return -1;
    }

    return noOfChars;
}

// Fetch the current character pointed by ptr2
char getnextchar(FILE *fp)
{
    char *lastchar;
    int out;

    if (ptr1 == NULL && ptr2 == NULL)
    {
        out = getStream(fp);

        if (out == -1)
            return EOF;

        ptr1 = buff[buffcurr];
        ptr2 = buff[buffcurr];
        lastchar = ptr2;

        if (*lastchar == '\n')
        {
            linenum++;
        }

        ptr2++;
        return *lastchar;
    }
    else
    {
        lastchar = ptr2;

        if (ptr2 + 1 == buff[buffcurr] + BLOCK_SIZE)
        {
            out = getStream(fp);

            if (out + 1 == 0)
            {
                return EOF;
            }
            ptr2 = buff[buffcurr];
        }
        else if (*ptr2 == EOF)
        {
            fileended = 1;
            return ' ';
        }
        else
        {
            ptr2++;
        }

        if (*lastchar == '\n')
        {
            linenum++;
        }

        return *lastchar;
    }
}

// function that sets the pointer ptr1 to the start of new lexeme
void read_new_token()
{
    ptr1 = ptr2;
};

void retract()
{
    ptr2--;

    if (*ptr2 == '\n')
        linenum--;

    ptr2--;
};

// hash function (SDBM) to hash lexemes
int hash_function(char *string)
{
    int n = 100;
    unsigned long hash = 0;
    int c;

    while (c = *string++)
    {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash % n;
};

// Function to initialize Symbol Table with keywords
void initialize_Symbol_Table()
{
    int n = 100;
    Symbol_Table = (Symbol_Table_Record **)malloc(n * sizeof(Symbol_Table_Record *));
    for (int i = 0; i < n; i++)
    {
        Symbol_Table[i] = NULL;
    }
    for (int i = 0; i < 28; i++)
    {
        insertEntry(Symbol_Table, keywords[i], linenum, NULL, keyword_ids[i]);
    }
    return;
};

// Function to lookup a lexeme in the symbol table;
Symbol_Table_Record *lookup(Symbol_Table_Record **Symbol_Table, char *lexeme)
{
    int hash_key = hash_function(lexeme);
    Symbol_Table_Record *helper = Symbol_Table[hash_key];

    while (helper)
    {
        if (strcmp(lexeme, helper->tk_info->lexeme) == 0)
            return helper;

        helper = helper->nextRecord;
    }

    return NULL;
};

// Function to add entry into the symbol table+
TokenInfo *insertEntry(Symbol_Table_Record **Symbol_Table, char *lexeme, int linenum, char *value, int tokenId)
{

    int hash_key = hash_function(lexeme);
    Symbol_Table_Record *lookup_result = lookup(Symbol_Table, lexeme);

    if (lookup_result != NULL)
    {
        lookup_result->tk_info->linenum = linenum;
        return lookup_result->tk_info;
    }

    TokenInfo *tk_info = (TokenInfo *)malloc(sizeof(TokenInfo));
    tk_info = createToken(lexeme, linenum, value, tokenId, false);

    if (Symbol_Table[hash_key] == NULL)
    {
        Symbol_Table[hash_key] = (Symbol_Table_Record *)malloc(sizeof(Symbol_Table_Record));
        Symbol_Table[hash_key]->tk_info = tk_info;
        Symbol_Table[hash_key]->nextRecord = NULL;
    }
    else
    {
        Symbol_Table_Record *newentry = (Symbol_Table_Record *)malloc(sizeof(Symbol_Table_Record));
        Symbol_Table_Record *temp = Symbol_Table[hash_key];
        newentry->tk_info = tk_info;
        newentry->nextRecord = temp;
        Symbol_Table[hash_key] = newentry;
    }

    return tk_info;
};

Value *createNewValue(char *val, int tokenId)
{
    Value *value = (Value *)malloc(sizeof(Value));
    if (tokenId == 4)
    {
        value->i = atoi(val);
        printf("value of number %d \n", value->i);
    }
    else if (tokenId == 5)
    {
        value->f = atof(val);
        printf("value of real number %f \n", value->f);
    }
    return value;
};

// Function create a new Token
TokenInfo *createToken(char *lexeme, int linenum, char *val, int tokenId, bool isError)
{
    TokenInfo *newtoken = (TokenInfo *)malloc(sizeof(TokenInfo));

    newtoken->lexeme = (char *)malloc(40 * sizeof(char)); // lexme size is limited here.
    newtoken->tokenId = tokenId;
    newtoken->lexeme = lexeme;
    newtoken->linenum = linenum;
    newtoken->value = val != NULL ? createNewValue(val, tokenId) : NULL;
    newtoken->isError = isError;

    return newtoken;
};

// utility function to extract a substring between start and end pointers.
char *getsubstring(char *start, char *end)
{
    int n = (end - start) / sizeof(char);
    char *s = (char *)malloc((n + 1) * sizeof(char));
    char *temp = start;
    int i = 0;
    while (temp < end)
    {
        s[i] = *temp;
        ++temp;
        ++i;
    }

    s[i] = '\0';
    read_new_token();
    return s;
}

TokenInfo *getNextToken(FILE *fp)
{
    dfa_state = 1;
    char c;

    TokenInfo *tk_info = (TokenInfo *)malloc(sizeof(TokenInfo));

    while (1)
    {
        c = getnextchar(fp);
        if (fileended)
            return createToken("DOLLAR", linenum, NULL, DOLLAR, false);

        switch (dfa_state)
        {
        // initial state
        case 1:
        {
            if (c == '&')
            {
                dfa_state = 2;
            }
            else if (c == '@')
            {
                dfa_state = 5;
            }
            else if (c == '~')
            {
                dfa_state = 8;
            }
            else if (c == '<')
            {
                dfa_state = 9;
            }
            else if (c == '=')
            {
                dfa_state = 15;
            }
            else if (c == '>')
            {
                dfa_state = 17;
            }
            else if (c == '!')
            {
                dfa_state = 20;
            }
            else if (c == '%')
            {
                dfa_state = 22;
            }
            else if (c == '+')
            {
                dfa_state = 23;
            }
            else if (c == '-')
            {
                dfa_state = 24;
            }
            else if (c == '*')
            {
                dfa_state = 25;
            }
            else if (c == '/')
            {
                dfa_state = 26;
            }
            else if (c == ',')
            {
                dfa_state = 27;
            }
            else if (c == ';')
            {
                dfa_state = 28;
            }
            else if (c == ':')
            {
                dfa_state = 29;
            }
            else if (c == '.')
            {
                dfa_state = 30;
            }
            else if (c == '[')
            {
                dfa_state = 31;
            }
            else if (c == ']')
            {
                dfa_state = 32;
            }
            else if (c == '(')
            {
                dfa_state = 33;
            }
            else if (c == ')')
            {
                dfa_state = 34;
            }
            else if (c == '_')
            {
                dfa_state = 35;
            }
            else if (c >= 'b' && c <= 'd')
            {
                dfa_state = 44;
            }
            else if (c == 'a' || (c >= 'e' && c <= 'z'))
            {
                dfa_state = 48;
            }
            else if (c >= '0' && c <= '9')
            {
                dfa_state = 50;
            }
            else if (c == '#')
            {
                dfa_state = 60;
            }
            else if (c == ' ' || c == '\f' || c == '\r' || c == '\t' || c == '\v')
            {
                ptr1++;
                dfa_state = 1;
            }
            else if (c == '\n')
            {
                ptr1++;
                dfa_state = 1;
            }
            else if (c == '^')
                return createToken("DOLLAR", linenum, NULL, DOLLAR, false);
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 2:
        {
            if (c == '&')
            {
                dfa_state = 3;
            }
            break;
        }
        case 3:
        {
            if (c == '&')
            {
                dfa_state = 4;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 4:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 48);
            dfa_state = 1;
            return tk_info;
            break;
        }
        case 5:
        {
            if (c == '@')
            {
                dfa_state = 6;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 6:
        {
            if (c == '@')
            {
                dfa_state = 7;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 7:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 49);
            dfa_state = 1;
            return tk_info;
        }
        case 8:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 50);
            dfa_state = 1;
            return tk_info;
        }
        case 9:
        {
            if (c == '=')
            {
                dfa_state = 10;
            }
            else if (c == '-')
            {
                dfa_state = 11;
            }
            else
            {
                dfa_state = 14;
            }
            break;
        }
        case 10:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 52);
            dfa_state = 1;
            return tk_info;
        }
        case 11:
        {
            if (c == '-')
            {
                dfa_state = 12;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 12:
        {
            if (c == '-')
            {
                dfa_state = 13;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 13:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 0);
            dfa_state = 1;
            return tk_info;
        }
        case 14:
        {
            retract();
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 51);
            }
            else
            {
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 51);
            }
            dfa_state = 1;
            return tk_info;
        }
        case 15:
        {
            if (c == '=')
            {
                dfa_state = 16;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 16:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 53);
            dfa_state = 1;
            return tk_info;
        }
        case 17:
        {
            if (c == '=')
            {
                dfa_state = 18;
            }
            else
            {
                dfa_state = 19;
            }
            break;
        }
        case 18:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                tk_info = insertEntry(Symbol_Table, lexeme, linenum - 1, NULL, 55);
            }
            else
            {
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 55);
            }
            dfa_state = 1;
            return tk_info;
        }
        case 19:
        {
            retract();
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 54);
            }
            else
            {
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 54);
            }
            dfa_state = 1;
            return tk_info;
        }
        case 20:
        {
            if (c == '=')
            {
                dfa_state = 21;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 21:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 56);
            dfa_state = 1;
            return tk_info;
        }
        case 22:
        {
            while (c != '\n' && c != EOF)
            {
                c = getnextchar(fp);
            }
            // ptr2--;
            read_new_token();
            char *lexeme = "%";
            tk_info = insertEntry(Symbol_Table, lexeme, linenum - 1, NULL, 1);
            dfa_state = 1;
            return tk_info;
        }
        case 23:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 40);
            dfa_state = 1;
            return tk_info;
        }
        case 24:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 41);
            dfa_state = 1;
            return tk_info;
        }
        case 25:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 42);
            dfa_state = 1;
            return tk_info;
        }
        case 26:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 43);
            dfa_state = 1;
            return tk_info;
        }
        case 27:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 27);
            dfa_state = 1;
            return tk_info;
        }
        case 28:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 28);
            dfa_state = 1;
            return tk_info;
        }
        case 29:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 29);
            dfa_state = 1;
            return tk_info;
        }
        case 30:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 30);
            dfa_state = 1;
            return tk_info;
        }
        case 31:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 21);
            dfa_state = 1;
            return tk_info;
        }
        case 32:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 22);
            dfa_state = 1;
            return tk_info;
        }
        case 33:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 32);
            dfa_state = 1;
            return tk_info;
        }
        case 34:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 33);
            dfa_state = 1;
            return tk_info;
        }
        case 35:
        {
            if ((c >= 'a' && c <= 'l') || (c >= 'n' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            {
                dfa_state = 36;
            }
            else if (c == 'm')
            {
                dfa_state = 39;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 36:
        {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            {
                dfa_state = 36;
            }
            else if (c >= '0' && c <= '9')
            {
                dfa_state = 37;
            }
            else
            {
                dfa_state = 38;
            }
            break;
        }
        case 37:
        {
            if (c >= '0' && c <= '9')
            {
                dfa_state = 37;
            }
            else
            {
                dfa_state = 38;
            }
            break;
        }
        case 38:
        {
            retract();
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 6);
            }
            else
            {
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 6);
            }
            dfa_state = 1;
            return tk_info;
        }
        case 39:
        {
            if ((c >= 'b' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
            {
                dfa_state = 36;
            }
            else if (c == 'a')
            {
                dfa_state = 40;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 40:
        {
            if ((c >= 'a' && c < 'h') || (c >= 'A' && c <= 'Z') || (c >= 'j' && c <= 'z') || (c >= '0' && c <= '9'))
            {
                dfa_state = 36;
            }
            else if (c == 'i')
            {
                dfa_state = 41;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 41:
        {
            if ((c >= 'a' && c <= 'm') || (c >= 'o' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
            {
                dfa_state = 36;
            }
            else if (c == 'n')
            {
                dfa_state = 42;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 42:
        {
            if ((c >= 'a' && c <= 'l') || (c >= 'n' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            {
                dfa_state = 36;
            }
            else
            {
                dfa_state = 43;
            }
            break;
        }
        case 43:
        {
            retract();

            char *lexeme = getsubstring(ptr1, ptr2);

            if (*ptr2 == '\n')
            {
                linenum--;

                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 17);
            }
            else
            {
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 17);
            }
            dfa_state = 1;

            return tk_info;
        }
        case 44:
        {
            if (c >= '2' && c <= '7')
            {
                dfa_state = 45;
            }
            else if (c >= 'a' && c <= 'z')
            {
                dfa_state = 48;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 45:
        {
            if (c >= 'b' && c <= 'd')
            {
                dfa_state = 45;
            }
            else if (c >= '2' && c <= '7')
            {
                dfa_state = 46;
            }
            else
            {
                dfa_state = 47;
            }
            break;
        }
        case 46:
        {
            if (c >= '2' && c <= '7')
            {
                dfa_state = 46;
            }
            else
            {
                dfa_state = 47;
            }
            break;
        }
        case 47:
        {
            retract();
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 3);
            }
            else
            {
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 3);
            }
            dfa_state = 1;
            return tk_info;
        }
        case 48:
        {
            if (c >= 'a' && c <= 'z')
            {
                dfa_state = 48;
            }
            else
            {
                dfa_state = 49;
            }
            break;
        }
        case 49:
        {
            retract();
            char *lexeme = getsubstring(ptr1, ptr2);

            Symbol_Table_Record *look_result = lookup(Symbol_Table, lexeme);
            if (look_result != NULL)
            {
                if (*ptr2 == '\n')
                {
                    linenum--;
                }
                look_result->tk_info->linenum = linenum;
                tk_info = look_result->tk_info;
            }
            else
            {
                if (*ptr2 == '\n')
                {
                    linenum--;
                    tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 2);
                }
                else
                {
                    tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 2);
                }
            }
            dfa_state = 1;
            return tk_info;
        }
        case 50:
        {
            if (c >= '0' && c <= '9')
            {
                dfa_state = 50;
            }
            else if (c == '.')
            {
                dfa_state = 51;
            }
            else
            {
                dfa_state = 59;
            }
            break;
        }
        case 51:
        {
            if (c >= '0' && c <= '9')
            {
                dfa_state = 52;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 52:
        {
            if (c >= '0' && c <= '9')
            {
                dfa_state = 53;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 53:
        {
            if (c == 'E')
            {
                dfa_state = 55;
            }
            else
            {
                dfa_state = 54;
            }
            break;
        }
        case 54:
        {
            retract();
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, lexeme, 5);
            }
            else
            {
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, lexeme, 5);
            }
            dfa_state = 1;
            return tk_info;
        }
        case 55:
        {
            if ((c >= '0' && c <= '9'))
            {
                dfa_state = 57;
            }
            else if (c == '+' || c == '-')
            {
                dfa_state = 56;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 56:
        {
            if (c >= '0' && c <= '9')
            {
                dfa_state = 57;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 57:
        {
            if (c >= '0' && c < '9')
            {
                dfa_state = 58;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 58:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                tk_info = insertEntry(Symbol_Table, lexeme, linenum - 1, NULL, 5);
            }
            else
            {
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 5);
            }
            dfa_state = 1;
            return tk_info;
        }
        case 59:
        {
            retract();
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, lexeme, 4);
            }
            else
            {
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, lexeme, 4);
            }
            dfa_state = 1;
            return tk_info;
        }
        case 60:
        {
            if (c >= 'a' && c <= 'z')
            {
                dfa_state = 61;
            }
            else
            {
                dfa_state = 70;
            }
            break;
        }
        case 61:
        {
            if (c >= 'a' && c <= 'z')
            {
                dfa_state = 61;
            }
            else
            {
                dfa_state = 62;
            }
            break;
        }
        case 62:
        {
            retract();
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 7);
            }
            else
            {
                tk_info = insertEntry(Symbol_Table, lexeme, linenum, NULL, 7);
            }
            dfa_state = 1;
            return tk_info;
        }
        case 70:
        {
            ptr2--;
            char *lexeme = getsubstring(ptr1, ptr2);
            if (*ptr2 == '\n')
            {
                linenum--;
            }
            tk_info = createToken(lexeme, linenum, NULL, -1, true);
            return tk_info;
        }
        }
    }
}

void removeComments(FILE *fptr1, FILE *fptr2)
{
    while (!feof(fptr1))
    {
        char c = fgetc(fptr1);
        if (c == '%')
        {
            while (c != '\n')
            {
                c = fgetc(fptr1);
            }
        }
        else
        {
            if (c == '^')
            {
                printf("\n \n");
                return;
            }
            fprintf(fptr2, "%c", c);
            printf("%c", c);
        }
    }
}

// appends an new character to the end of source code
void appendlastchar(char *fileName)
{
    FILE *fptr1 = fopen(fileName, "a");
    fputs("^", fptr1);
    fclose(fptr1);
}

// int main() {
//     appendlastchar("t1.txt");
//     FILE *fptr = fopen("t1.txt", "r");
//     initialize_Symbol_Table();
//     for (int i = 0; i < 300; i++)
//     {
//         TokenInfo *tk = getNextToken(fptr);
//         printf("%d ", tk->linenum);
//         printf("%s ", tk->lexeme);
//         printf("%d ", (tk->tokenId));
//         printf("%d \n", tk->isError);
//         if (tk -> lexeme == "DOLLAR")
//             break;
//     }
//     fclose(fptr);

//     return 0;
// }
