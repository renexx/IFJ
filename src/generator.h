/**
 * Project: Implementace překladače imperativního jazyka IFJ18.
 *
 * @brief Hlavičkový súbor pre generator.c
 *
 * @author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
 * @author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
 * @author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
 * @author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

#include "headers.h"
#include "symtable.h"
#include "scanner.h"


#define FUNCTION_LENGTH                                                         \
    "\nLABEL $len"                                                              \
    "\nPUSHFRAME"                                                               \
    "\nDEFVAR LF@return_value"                                                  \
    "\nSTRLEN LF@return_value LF@0"                                             \
    "\nPOPFRAME"                                                                \
    "\nRETURN\n"


/*RIADOK 43: je si string prazdny nevrati nic*/
/*RIADOK 45: je li index "i" mensi ako 0 == chyba*/
/*RIADOK 47: je li index "i" vacsi ako len_str == chyba*/
/*RIADOK 49: je li index "n" mensi ako 0 == chyba*/
/*RIADOK 53: je li strlen(s)-i mensi ako n == vrati vsetky znaky do konca s od indexu i*/
#define FUNCTION_SUBSTR                                                         \
    "\nLABEL $substr"                                                           \
    "\nPUSHFRAME"                                                               \
    "\nDEFVAR LF@return_value"                                                  \
    "\nDEFVAR LF@len_str"                                                       \
    "\nCREATEFRAME"                                                             \
    "\nDEFVAR TF@0"                                                             \
    "\nMOVE TF@0 LF@0"                                                          \
    "\nCALL $len"                                                               \
    "\nMOVE LF@len_str TF@return_value"                                         \
    "\nDEFVAR LF@ret_cond"                                                      \
    "\nLT LF@ret_cond LF@len_str int@1"                                         \
    "\nJUMPIFEQ $retnic LF@ret_cond bool@true"                                  \
    "\nLT LF@ret_cond LF@1 int@0"                                               \
    "\nJUMPIFEQ $retnil LF@ret_cond bool@true"                                  \
    "\nGT LF@ret_cond LF@1 LF@len_str"                                          \
    "\nJUMPIFEQ $retnil LF@ret_cond bool@true"                                  \
    "\nLT LF@ret_cond LF@2 int@0"                                               \
    "\nJUMPIFEQ $retnil LF@ret_cond bool@true"                                  \
    "\nDEFVAR LF@tmp"                                                           \
    "\nSUB LF@tmp LF@len_str LF@1"                                              \
    "\nGT LF@ret_cond LF@tmp LF@2"                                              \
    "\nJUMPIFEQ $dokonca LF@ret_cond bool@true"                                 \
    "\nMOVE LF@2 LF@tmp"                                                        \
    "\nLABEL $dokonca"                                                          \
    "\nDEFVAR LF@char"                                                          \
    "\nDEFVAR LF@loop"                                                          \
    "\nLABEL $generuj_loop"                                                     \
    "\nGETCHAR LF@char LF@0 LF@1"                                               \
    "\nCONCAT LF@return_value LF@return_value LF@char"                          \
    "\nADD LF@1 LF@1 int@1"                                                     \
    "\nSUB LF@2 LF@2 int@1"                                                     \
    "\nGT LF@loop LF@2 int@0"                                                   \
    "\nJUMPIFEQ $generuj_loop LF@loop bool@true"                                \
    "\nJUMP $retend"                                                            \
    "\nLABEL $retnil"                                                           \
    "\nMOVE LF@return_value nil@nil"                                            \
    "\nJUMP $retend"                                                            \
    "\nLABEL $retnic"                                                           \
    "\nMOVE LF@return_value string@"                                            \
    "\nLABEL $retend"                                                           \
    "\nPOPFRAME"                                                                \
    "\nRETURN\n"


#define FUNCTION_ASC                                                            \
    "\nLABEL $asc"                                                              \
    "\nPUSHFRAME"                                                               \
    "\nDEFVAR LF@return_value"                                                  \
    "\nMOVE LF@return_value nil@nil"                                            \
    "\nDEFVAR LF@length"                                                        \
    "\nLT LF@length LF@1 int@0"                                                 \
    "\nJUMPIFEQ $ascend LF@length bool@true"                                    \
    "\nDEFVAR LF@length_str"                                                    \
    "\nCREATEFRAME"                                                             \
    "\nDEFVAR TF@0"                                                             \
    "\nMOVE TF@0 LF@0"                                                          \
    "\nCALL $length"                                                            \
    "\nMOVE LF@length_str TF@return_value"                                      \
    "\nSUB LF@length_str LF@length_str int@1"                                   \
    "\nGT LF@length LF@1 LF@length_str"                                         \
    "\nJUMPIFEQ $ascend LF@length bool@true"                                    \
    "\nSTRI2INT LF@return_value LF@0 LF@1"                                      \
    "\nLABEL $ascend"                                                           \
    "\nPOPFRAME"                                                                \
    "\nRETURN\n"


#define FUNCTION_CHR                                                            \
    "\nLABEL $chr"                                                              \
    "\nPUSHFRAME"                                                               \
    "\nDEFVAR LF@return_value"                                                  \
    "\nDEFVAR LF@range"                                                         \
    "\nLT LF@range LF@0 int@0"                                                  \
    "\nJUMPIFEQ $chrerrorend LF@range bool@true"                                \
    "\nGT LF@range LF@0 int@255"                                                \
    "\nJUMPIFEQ $chrerrorend LF@range bool@true"                                \
    "\nINT2CHAR LF@return_value LF@0"                                           \
    "\nJUMP $chrend"                                                            \
    "\nLABEL $chrerrorend"                                                      \
    "\nMOVE LF@return_value int@58"                                             \
    "\nLABEL $chrend"                                                           \
    "\nPOPFRAME"                                                                \
    "\nRETURN\n"


int InitList();


void FreeList();


int AppendString(tList *list, char *append);


/******************* BUILT IN ********************/
bool DefBInFuncs();

bool GenPrint(int pocet,...);

bool GenPrint2(int pocet, Token *Tarray);

bool GenInpt(char *Variable, Type_I type);


/******************* CODE GEN ********************/
bool GenHeader();

bool CodeGenStart();

void CodeGenFlush(FILE *f);


/******************* MAIN ********************/
bool GenStartMain();

bool GenEndMain();


/****************** VARIABLE ******************/
bool GenVarDefaultValue(Type_I type);

bool GenVar(char *Variable);

bool GenAndSetVarToDefault(char *Variable, Type_I type);

bool GenVarSetValue1(char *variable);

bool GenVarSetValue2(Token Token);

/******************** LABEL *****************/
bool GenLabel(char *func, int label_idx);


/******************** IF ********************/
bool GenIFStart(char *func, int label_idx);

bool GenElse(char *label_end, int label_idx_end, char *label_else, int label_idx_else);

bool GenIFEnd(char *func, int label_idx);


/****************** WHILE *****************/
bool GenWhileHead(char *func, int label_idx);

bool GenWhileStart(char *func, int label_idx);

bool GenWhileEnd(char *label_head, int label_idx_head, char *label_end, int label_idx_end);


/****************** FUNC ******************/
bool GenFuncCall(char *func);

bool GenFuncParam(char *ParamID, int idx);

bool GenFuncPassParam(Token token, int idx);

bool GenFuncConvertPassedParam(Type_I from, Type_I to, int idx); // ___NAZOV___

bool GenFuncStart(char *func);

bool GenFuncReturnValueAdjust(char *ValueID, Type_I Type, Type_I ReturnType);

bool GenFuncReturnValue(Type_I type);

bool GenFuncReturn(char *func);

bool GenFuncEnd(char *func);


/******************* OTHERS ********************/
bool GenValue(Token token);

bool GenPush(Token token);

bool GenPop();

bool GenStackOperation(E_rules rule);

bool GenConcat();

bool GenSaveExpressionResult(char *Variable, Type_I ReturnType, Type_I Type); //___NAZOV___

bool GenStackOP1ToFloat();

bool GenStackOP1ToInteger();

bool GenStackOP2ToFloat();

bool GenStackOP2ToInteger();

#endif

/*** Koniec súboru generator.h ***/
