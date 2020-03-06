/**
 * Project: Implementace překladače imperativního jazyka IFJ18.
 *
 * @brief Hlavičkový súbor obsahujúci chybové kódy
 *
 * @author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
 * @author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
 * @author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
 * @author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>
 */

#ifndef ERROR_H_
#define ERROR_H_

//################################ ERRORS ####################################//
#define ERR_SCANNER     1  // chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému)
#define ERR_PARSER      2  // chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu)
#define ERR_SEMANTIC_1  3  // sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefinici funkce/proměnné, atp.
#define ERR_SEMANTIC_2  4  // sémantická/běhová chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech
#define ERR_SEMANTIC_3  5  // sémantická chyba v programu – špatný počet parametrů u volání funkce
#define ERR_SEMANTIC_4  6  // ostatní sémantické chyby
#define ERR_ZERO_DIV    9  // běhová chyba dělení nulou
#define ERR_INTERNAL    99 // interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti, atp.)
//############################################################################//

#endif

/*** Koniec súboru error.h ***/
