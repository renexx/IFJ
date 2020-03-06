/**
 * Project: Implementace překladače imperativního jazyka IFJ18.
 *
 * @brief Hlavičkový súbor pre prec_table.c
 *
 * @author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
 * @author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
 * @author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
 * @author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>
 */

#ifndef PREC_TABLE_H_
#define PREC_TABLE_H_

#include "headers.h"
#include "symstack.h"
#include "generator.h"


#define M 1        // >
#define L 2        // <
#define Q 3        // =
#define X 0        // error
#define S 42       // success $&$

#define TSIZE 14   // velkost taulky


char prec_table[TSIZE][TSIZE];


/**
 * @brief Funkcia mení token na symbol
 * @param *table tabulka symbolov
 * @param *token vstupny TOKEN
 * @param *type ukazatelom predavany typ terminalneho symbolu
 * @return prislusny terminalny symbol
 */
PrecT_sym ConvertsToken(Sym_table *table, Token *token, int *type);


/**
 * @brief Funkica vracia pravu stranu prislusneho pravidla
 * @param *stack zasobnik
 * @param number je return z funkcie SymStackFind cize pozicia zarazky
 * @return - pravidlo 0-11 alebo NOT_RULE 12
 */
int rules(SymStack *stack, int number);


/**
 * @brief hlavna funkcia precedencnej analyzy
 * @param *table tabulka symbolov
 * @param *token vstupny TOKEN
 * @return prislusny EXIT code
 */
int expression(Sym_table *table, Token *token);

/**
 * Bude sa volat generovanie kodu pravdepodobne.....
 */
int expression_semantics(E_rules rule, Type_I sym1, Type_I sym2, Type_I sym3);

#endif

/*** Koniec súboru prec_table.h ***/
