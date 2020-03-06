/**
 * Project: Implementace překladače imperativního jazyka IFJ18.
 *
 * @brief Hlavičkový súbor pre scanner.c
 *
 * @author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
 * @author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
 * @author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
 * @author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>
 */

#ifndef SCANNER_H_
#define SCANNER_H_

#include "headers.h"


//############################ AUTOMAT STATES ################################//
#define STATE_START         1
#define STATE_ID            2
#define STATE_NUMBER        3
#define STATE_STRING        4
#define STATE_EQ            5
#define STATE_NEQ           6
#define STATE_LESS          7
#define STATE_MORE          8
#define STATE_EOL           9
#define STATE_COMMENTARY    10
//############################################################################//


//############################# TOKEN TYPES ##################################//
#define TOKEN_EMPTY         1
#define TOKEN_EOL           2
#define TOKEN_EOF           3
#define TOKEN_ID            4
#define TOKEN_KWORD         5
#define TOKEN_INT           6
#define TOKEN_FLOAT         7
#define TOKEN_STRING        8
#define TOKEN_EQ            9
#define TOKEN_NEQ           10
#define TOKEN_LTN           11
#define TOKEN_MTN           12
#define TOKEN_LEQ           13
#define TOKEN_MEQ           14
#define TOKEN_ASSIGN        15
#define TOKEN_PLUS          16
#define TOKEN_MINUS         17
#define TOKEN_MUL           18
#define TOKEN_DIV           19
#define TOKEN_LBRACKET      20
#define TOKEN_RBRACKET      21
#define TOKEN_COMMA         22
//############################################################################//

typedef struct
{
    int type;
    char *attribute;
} Token;


typedef struct tElem
{
    char *arr;
    struct tElem *ptr_next;
} *tElemPtr;


typedef struct
{
    tElemPtr First;
} tList;


bool set_resources(Token *token);


void clean_resources(Token *token);


bool save_address(char *arr);


int check_block_comment();


int get_next_token(Token *token);

#endif

/*** Koniec súboru scanner.h ***/
