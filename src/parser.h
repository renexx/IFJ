/**
 * Project: Implementace překladače imperativního jazyka IFJ18.
 *
 * @brief Hlavičkový súbor pre parser.c
 *
 * @author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
 * @author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
 * @author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
 * @author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "headers.h"
#include "prec_table.h"


#define MAIN "VLeseJe42StromovACislo42SaNerovna0" // UNIQUE ID pre main funkciu


typedef struct
{
    func_Sym_table *symtable;
    Token *token;

    bool FLAG_ID_not_exist; // flag neexistovania ID v konkretnej symtable
    bool FLAG_function_def; // flag definovania funkcie
    bool FLAG_init;         // flag inicializacie
    bool FLAG_term;         // flag term?????
    bool FLAG_input_wo_br;  // input bez zatvoriek
    bool empty_stat;
    bool FLAG_term_next_empty;
    bool FLAG_print;

    Token *Tarray;
    int tarrc;

    int param_counter;      // counter parametrov func pri jej definovani
    int func_call_counter;  // counter parametrov func pri jej volani

    char *init_w_yourself;  // zapamatame si sem token atribut pri a = a

    Sym_table *current_symtable;
} ParserData;

/**
 * <prog> -> <statement> <func> <prog>
 */
int prog(ParserData *Pdata);

/**
 * <func> -> DEF ID (<param>) EOL <statement> END EOL
 * <func> -> Ԑ
 */
int func(ParserData *Pdata);

/**
 * <statement> -> IF <expression> THEN EOL <statement> ELSE EOL <statement> END EOL <statement>
 * <statement> -> WHILE <expression> DO EOL <statement> END EOL <statement>
 * <statement> -> ID <starts_w_id> EOL <statement>
 * <statement> -> <expression> EOL <statement>
 * <statement> -> <builtin_func> EOL <statement>
 * <statement> -> Ԑ
 */
int statement(ParserData *Pdata);

/**
 * <starts_w_id> -> = <inicialization>
 * <starts_w_id> -> <function_call>
 */
int starts_w_id(ParserData *Pdata);

/**
 * <inicialization> -> <builtin_func>
 * <inicialization> -> <function> // treba skontrolovat, ci je definovana SKONTROLOVANE
 * <inicialization> -> <value>
 * <inicialization> -> NIL
 * <inicialization> -> <expression>
 */
int inicialization(ParserData *Pdata);

/**
 * <builtin_func> -> PRINT  <builtin_call>
 * <builtin_func> -> CHR    <builtin_call>
 * <builtin_func> -> ORD    <builtin_call>
 * <builtin_func> -> SUBSTR <builtin_call>
 * <builtin_func> -> LENGTH <builtin_call>
 * <builtin_func> -> INPUTF <input_call>
 * <builtin_func> -> INPUTS <input_call>
 * <builtin_func> -> INPUTI <input_call>
 */
int builtin_func(ParserData *Pdata);

/**
 * <function> -> ID <function_call> EOL
 */
int function(ParserData *Pdata);

/**
 * <function_call> -> <term_user>
 * <function_call> -> (<term_user>)
 */
int function_call(ParserData *Pdata);

/**
 * <builtin_call> -> <term>
 * <builtin_call> -> (<term>)
 */
int builtin_call(ParserData *Pdata);

/**
 * <input_call> -> ()
 * <input_call> -> Ԑ
 */
int input_call(ParserData *Pdata);

/**
 * <param> -> <value> <param_next>
 * <param> -> Ԑ
 */
int param(ParserData *Pdata);

/**
 * <param_next> -> , <value> <param_next>
 * <param_next> -> Ԑ
 */
int param_next(ParserData *Pdata);

/**
 * <term_user> -> <term>
 * <term_user> -> Ԑ
 */
int term_user(ParserData *Pdata);

/**
 * <term> -> <value> <term_next>
 * <term> -> NIL     <term_next>
 * <term> -> ID      <term_next>
 */
int term(ParserData *Pdata);

/**
 * <term_next> -> , <term>
 * <term_next> -> Ԑ
 */
int term_next(ParserData *Pdata);

/**
 * <value> -> INTEGER
 * <value> -> FLOAT
 * <value> -> STRING
 */
int value(ParserData *Pdata);

#endif

/*** Koniec súboru parser.h ***/
