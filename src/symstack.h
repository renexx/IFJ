/**
 * Project: Implementace překladače imperativního jazyka IFJ18.
 *
 * @brief Hlavičkový súbor pre symstack.c
 *
 * @author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
 * @author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
 * @author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
 * @author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>
 */

#ifndef SYMSTACK_H_
#define SYMSTACK_H_

#include "headers.h"


#define EXPRESSION 15   //mozno 12

typedef enum
{
    PLUS,          // +                         0  term
    MINUS,         // -                         1  term
    MUL,           // *                         2  term
    DIV,           // /                         3  term
    LESS,          // <                         4  term
    LESS_EQ,       // <=                        5  term
    MORE,          // >                         6  term
    MORE_EQ,       // >=                        7  term
    EQ,            // ==                        8  term
    NOT_EQ,        // !=                        9  term
    LEFT_BR,       // (                         10 term
    RIGHT_BR,      // )                         11 term
    IDENTIF,       // i = int, float, string    12 term
    DOLLAR,        // $                         13 term
    NLESS          // riadiaci znak <, ktory sa vklada na Stack
} PrecT_sym;


typedef struct symstack_item
{
    PrecT_sym sym;
    int type;
    struct symstack_item *ptr_next;
} SymStackItem;


typedef struct symstack
{
    SymStackItem *top;
} SymStack;


void symstack_init(SymStack *stack); // Inicializacia zásobnika


int symstack_push(SymStack *stack, PrecT_sym sym, int type); //pushnutie hodnoty na zásobnik


int symstack_pop(SymStack *stack); // popnutie hodnoty na zásobnik


int symstack_pop_many(SymStack *stack, PrecT_sym sym); // popnutie hodnoty na zásobnik


SymStackItem *symstack_top(SymStack *stack); // vratenie vrcholu (top) zásobnika


SymStackItem *symstack_top_term(SymStack *stack);


int symstack_count(SymStack *stack, PrecT_sym sym);


int symstack_post_insert(SymStack *stack, PrecT_sym sym, int type); //vlozi prvok za najdeny prvok


void SymStack_free(SymStack *stack); // zrušenie celeho zasobinka


void symstack_print(SymStack *stack);

#endif

/*** Koniec súboru symstack.h ***/
