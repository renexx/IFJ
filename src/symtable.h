/**
 * Project: Implementace překladače imperativního jazyka IFJ18.
 *
 * @brief Hlavičkový súbor pre symtable.h
 *
 * @author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
 * @author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
 * @author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
 * @author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>
 */

#ifndef SYMTABLE_H_
#define SYMTABLE_H_

#define MAX_SYMT_SIZE 401 // prime number.......FIXED MERLIN BUG
#define EXISTUJE -10

#include "headers.h"


typedef enum
{
    UNDEF,         // 0
    INT,           // 1
    FLOAT,         // 2
    STRING,        // 3
    BOOL,          // 4
    FUNC           // 5
} Type_I;


typedef enum
{
    E_PLUS_E,      // 0
    E_MINUS_E,     // 1
    E_MUL_E,       // 2
    E_DIV_E,       // 3
    E_LESS_E,      // 4
    E_LESSEQ_E,    // 5
    E_MORE_E,      // 6
    E_MOREEQ_E,    // 7
    E_EQ_E,        // 8
    E_NOTEQ_E,     // 9
    BR_E_BR,       // 10
    I,             // 11
    NOT_RULE       // 12 nie je to pravidlo, pravidlo neexistuje...navratovz kod, pre funkciu expression 12
} E_rules;


typedef struct htab_data
{
    int type; // ID type
    char *ID; // ID == key cize TOKEN ATTRIBUTE
    bool defined;
    int num_of_params; // pocet parametrov funkcie
} tData;


typedef struct htab_listitem
{
    char *key; // key == ID cize TOKEN ATTRIBUTE
    tData *data;
    struct htab_listitem *ptr_next;
} sTItem;


typedef sTItem *Sym_table[MAX_SYMT_SIZE];


/**
 * @brief Inicializacia symtable
 * @param table - ukazovatel na symtable
 */
void symtable_init(Sym_table *table);


/**
 * funkcia ktora prejde riadok v tabulke na koniec a vrati posledny prvok,
 * v pripade že sa tam už prvok s key nachádza vráti ukazatel na ten prvok a
 * v exist vrati TRUE
 *
 * @brief Prechadzanie a vyhladavanie v tabulke
 * @param table - ukazovatel na symtable
 * @param key - identifikator premennej alebo funkcie
 * @param exist - Ak je TRUE nasiel sa item s Key
 *              - Ak je FALSE nenasiel sa item
 * @return - Ak je exist TRUE vracia ukazatel na item s hodnotou key
 *         - Ak je exist FALSE vracia ukazatel na posledny item na riadku
 */
sTItem *line_cross(Sym_table *table, const char *key, bool *exist);


/**
 * @brief Prida item do tabulky (na koniec)
 * @param table - ukazovatel na symtable
 * @param key - identifikator premennej alebo funkcie
 * @return  - Pri uspechu pridania vracia EXIT_SUCCESS
 *          - Ak prvok s key uz existuje vrati EXISTUJE
 *          - Inac ERR_INTERNAL
 */
int symtable_add(Sym_table *table, char *key);


/**
 * @brief Vyhlada Item v tabulke
 * @param table - ukazovatel na symtable
 * @param key - identifikator premennej alebo funkcie
 * @return  - Pri uspesnom najdeni vracia ukazovatel na Item
 *          - Pri neuspesnom hladani vracia NULL
 *          - Ak nieje tabulka inicializovana alebo key nebol predany spravne vracia ERR_INTERNAL
 */
sTItem *symtable_search(Sym_table *table, const char *key);


/**
 * @brief Vymaze Item z tabulky
 * @param table - ukazovatel na symtable
 * @param key - identifikator premennej alebo funkcie
 * @return - Pri uspesnom odmazani Itemu vracia TRUE, inac FALSE
 *         - Ak nieje tabulka inicializovana alebo key nebol predany spravne vracia ERR_INTERNAL
 */
bool symtable_remove(Sym_table *table, const char *key);


/**
 * @brief Uvolni celu tabulku
 * @param table - ukazovatel na symtable
 */
void symtable_free(Sym_table *table);


/**
 * @brief Naplni Item v tabuke datami
 * @param table - ukazovatel na symtable
 * @param key - identifikator premennej alebo funkcie
 * @param type - ID type
 * @param num_of_params - pocet parametrov vo funkcii
 * @param defined - bool hodnota ci je premmenna alebo funkcia definovana
 * @return - pri uspesnom naplneni Itemu datami vracia TRUE inac FALSE
 */
bool symtable_fill(Sym_table *table, char *key, int type, bool defined, int num_of_params);


//#################################################################################//
/** SYMTABLE do, ktorej sa budu ukladat jednotlive symtables jednotlivych funkcii **/
//#################################################################################//
typedef struct func_sT_listitem
{
    char *function; // ID funckie
    Sym_table *symtable; // symtable tejto funkcie
    struct func_sT_listitem *ptr_next;
} func_sTItem;


typedef func_sTItem *func_Sym_table[MAX_SYMT_SIZE];


void func_symtable_init(func_Sym_table *table);

/**
 * @brief funckia vracia symtable konkretnej funckie, ak takato symtable neexistuje, tak ju vytvory
 */
Sym_table *func_symtable(func_Sym_table *table, char *function);

void func_symtable_free(func_Sym_table *table);
//#################################################################################//
//#################################################################################//

#endif

/*** Koniec súboru symtable.h ***/
