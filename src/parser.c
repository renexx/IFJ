/**
 * Project: Implementace překladače imperativního jazyka IFJ18.
 *
 * @brief Implementácia syntaktickej analýzy
 *
 * @author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
 * @author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
 * @author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
 * @author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>
 */

#include "parser.h"


#define GET_TOKEN() do                                                                          \
                    {                                                                           \
                        if ((ret = get_next_token(Pdata->token)) != EXIT_SUCCESS) return ret;   \
                        else                                                                    \
                        {                                                                       \
                            /*printf("\ntoken->type: %d\n", Pdata->token->type);*/                  \
                            /*printf("token->attr: %s\n", Pdata->token->attribute);*/               \
                        }                                                                       \
                    }                                                                           \
                    while (0)                                                                   \


#define CHOOSE_SYMTABLE(_func_) if ((Pdata->current_symtable = func_symtable(Pdata->symtable, _func_)) == NULL) return ERR_INTERNAL


/**
 * <prog> -> <statement> <func> <prog> EOF
 */
int prog(ParserData *Pdata) // TATO FUNKCIA ASI OK
{
    if (Pdata->token->type == TOKEN_EOF)
    {
        return EXIT_SUCCESS;
    }

    int ret, ret2;

    ret = statement(Pdata);
    ret2 = func(Pdata);
    if (ret != EXIT_SUCCESS)
    {
        return ret;
    }
    if (ret2 != EXIT_SUCCESS)
    {
        return ret2;
    }

    GET_TOKEN();
    return prog(Pdata);
}


/**
 * <func> -> DEF ID (<param>) EOL <statement> END EOL
 * <func> -> Ԑ
 */
int func(ParserData *Pdata) // TATO FUNKCIA ASI OK
{
    int ret;

    if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "def") == 0))
    {
        Pdata->FLAG_function_def = true;

        GET_TOKEN();
        if (Pdata->token->type != TOKEN_ID)
        {
            return ERR_PARSER;
        }

        CHOOSE_SYMTABLE(MAIN); // prepneme sa do symtable pre main

        if (symtable_add(Pdata->current_symtable, Pdata->token->attribute) == EXISTUJE)
        {
            return ERR_SEMANTIC_1;
        }
        else symtable_fill(Pdata->current_symtable, Pdata->token->attribute, FUNC, true, 0); // defined == true, lebo def....type == FUNC, lebo def

        CHOOSE_SYMTABLE(Pdata->token->attribute); // prepneme sa do symtable pre funkciu

        char *function_ID = Pdata->token->attribute;

        GenFuncStart(Pdata->token->attribute);

        GET_TOKEN();
        if(Pdata->token->type != TOKEN_LBRACKET)
        {
            return ERR_PARSER;
        }

        Pdata->param_counter = 0;

        GET_TOKEN();
        if ((ret = param(Pdata)) != EXIT_SUCCESS)
        {
            return ret;
        }

        // zratat pocet parametrov a naplnit ich do symtable
        symtable_fill(Pdata->current_symtable, function_ID, FUNC, true, Pdata->param_counter); // neviem....mozno to pojde

        GET_TOKEN();
        if(Pdata->token->type != TOKEN_RBRACKET)
        {
            return ERR_PARSER;
        }

        GET_TOKEN();
        if(Pdata->token->type != TOKEN_EOL)
        {
            return ERR_PARSER;
        }

        GET_TOKEN();
        if ((ret = statement(Pdata)) != EXIT_SUCCESS)
        {
            return ret; // STATEMENT
        }

        // TU NIEKDE RETVAL

        GET_TOKEN();
        if((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "end") == 0))
        {
            return ERR_PARSER;
        }

        Pdata->FLAG_function_def = false;
        CHOOSE_SYMTABLE(MAIN); // prepneme sa do symtable pre main

        GET_TOKEN();
        if(Pdata->token->type != TOKEN_EOL)
        {
            return ERR_PARSER;
        }

        GenFuncEnd(function_ID);

        return EXIT_SUCCESS;
    }

    // <func> -> Ԑ
    else return EXIT_SUCCESS;
}


/**
 * <statement> -> IF <expression> THEN EOL <statement> ELSE EOL <statement> END EOL <statement>
 * <statement> -> WHILE <expression> DO EOL <statement> END EOL <statement>
 * <statement> -> ID <starts_w_id> EOL <statement> // ASI MAME SKONTROLOVANE
 * <statement> -> <expression> EOL <statement>
 * <statement> -> <builtin_func> EOL <statement> // TREBA SKONTROLOVAT PARAMETRE VSTAVANEJ FUNKCIE!
 * <statement> -> Ԑ
 */
int statement(ParserData *Pdata) // TATO FUNKCIA ASI OK...NEVIEM...SU TU POCHYBNOSTI
{
    int ret, ret_bfunc;//, ret_exp;
    Pdata->empty_stat = false;

    // <statement> -> IF <expression> THEN EOL <statement> ELSE EOL <statement> END EOL <statement>
    if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "if") == 0))
    {
        GET_TOKEN();
        if ((ret = expression(Pdata->current_symtable, Pdata->token)) != EXIT_SUCCESS)
        {
            return ret;
        }

        //GET_TOKEN(); // po expression je zakazany GET_TOKEN()
        if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "then") == 0))
        {
            GET_TOKEN();
            if (Pdata->token->type != TOKEN_EOL)
            {
                return ERR_PARSER;
            }
            //TU BY MAL BYT IF START
            static int label_idx_else = 0;
            if(GenIFStart("ELSE", label_idx_else) == false)
            {
                return ERR_INTERNAL;
            }


            GET_TOKEN();
            if ((ret = statement(Pdata)) != EXIT_SUCCESS)
            {
                return ret;
            }

            if (Pdata->empty_stat == true)
                Pdata->empty_stat = false;
            else GET_TOKEN();

            if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "else") == 0))
            {
                GET_TOKEN();
                if (Pdata->token->type != TOKEN_EOL)
                {
                    return ERR_PARSER;
                }
                //TU GEN ELSE PART
                static int label_idx_end = 0;
                if(GenElse("END" , label_idx_end,"ELSE", label_idx_else) == false)
                {
                    return ERR_INTERNAL;
                }
                label_idx_else++;

                GET_TOKEN();
                if ((ret = statement(Pdata)) != EXIT_SUCCESS)
                {
                    return ret;
                }

                if (Pdata->empty_stat == true)
                    Pdata->empty_stat = false;
                else GET_TOKEN();

                if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "end") == 0))
                {
                    GET_TOKEN();
                    if (Pdata->token->type != TOKEN_EOL)
                    {
                        return ERR_PARSER;
                    }
                    //TU GEN IF END

                    if(GenIFEnd("END", label_idx_end) == false)
                    {
                        return ERR_INTERNAL;
                    }
                    label_idx_end++;

                    GET_TOKEN();
                    return statement(Pdata);
                }
                else
                {
                    return ERR_PARSER;
                }
            }
            else
            {
                return ERR_PARSER;
            }
        }
        else
        {
            return ERR_PARSER;
        }
    }

    // <statement> -> WHILE <expression> DO EOL <statement> END EOL <statement>
    else if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "while") == 0))
    {
        //tu by mala byt generacia while head
        static int label_idx_whead = 0;
        if(GenWhileHead("WHEAD", label_idx_whead) == false)
        {
            return ERR_INTERNAL;
        }

        GET_TOKEN();
        if ((ret = expression(Pdata->current_symtable, Pdata->token)) != EXIT_SUCCESS)
        {
            return ret;
        }

        //GET_TOKEN(); // po expression je zakazany GET_TOKEN()
        if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "do") == 0))
        {
            GET_TOKEN();
            if (Pdata->token->type != TOKEN_EOL)
            {
                return ERR_PARSER;
            }
            //tu by mala byt while start
            static int label_idx_wend = 0;
            if(GenWhileStart("WEND", label_idx_wend) == false)
            {
                return ERR_INTERNAL;
            }


            GET_TOKEN();
            if ((ret = statement(Pdata)) != EXIT_SUCCESS)
            {
                return ret;
            }

            if (Pdata->empty_stat == true)
                Pdata->empty_stat = false;
            else GET_TOKEN();

            if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "end") == 0)) // end
            {
                GET_TOKEN();
                if (Pdata->token->type != TOKEN_EOL)
                {
                    return ERR_PARSER;
                }
                //while end generacia
                if(GenWhileEnd("WHEAD", label_idx_whead, "WEND", label_idx_wend) == false)
                {
                    return ERR_INTERNAL;
                }
                label_idx_whead++;
                label_idx_wend++;

                GET_TOKEN();
                return statement(Pdata);
            }
            else
            {
                return ERR_PARSER;
            }
        }
        else
        {
            return ERR_PARSER;
        }
    }

    // <statement> -> ID <starts_w_id> EOL <statement>
    else if (Pdata->token->type == TOKEN_ID) // toto ID moze byt aj func aj var
    {
        if (Pdata->FLAG_function_def == true)
        {
            CHOOSE_SYMTABLE(Pdata->token->attribute);
        }
        else
        {
            CHOOSE_SYMTABLE(MAIN);
        }

        if (symtable_add(Pdata->current_symtable, Pdata->token->attribute) != EXISTUJE) // ...nemusi tu byt este ani def. lebo to moze byt init. premennej
            Pdata->FLAG_ID_not_exist = true; // flag neexistovania je true
        else
            Pdata->FLAG_ID_not_exist = false;

        Pdata->init_w_yourself = Pdata->token->attribute; // a = a

        GenVar(Pdata->token->attribute);

        GET_TOKEN();
        if ((ret = starts_w_id(Pdata)) != EXIT_SUCCESS)
        {
            return ret; //KONTROLA ID POKRACUJE v starts_w_id!!!!
        }

        if (Pdata->FLAG_input_wo_br == false)
            GET_TOKEN();

        if (Pdata->FLAG_input_wo_br == true) Pdata->FLAG_input_wo_br = false;

        //if (FLAG_expression == true) FLAG_expression = false;

        if (Pdata->token->type != TOKEN_EOL)
        {
            return ERR_PARSER;
        }

        GET_TOKEN();
        return statement(Pdata);
    }

    // <statement> -> <expression> EOL <statement>
    /*else if ((ret_exp = expression(current_symtable, Pdata->token)) == EXIT_SUCCESS)
    {
        GET_TOKEN();
        if (Pdata->token->type != TOKEN_EOL) return ERR_PARSER;

        GET_TOKEN();
        return statement(Pdata);
    }*/

    // <statement> -> <builtin_func> EOL <statement>
    else if ((ret_bfunc = builtin_func(Pdata)) == EXIT_SUCCESS)
    {
        if (Pdata->FLAG_term_next_empty == false) GET_TOKEN();
        else Pdata->FLAG_term_next_empty = false;
        if (Pdata->token->type != TOKEN_EOL) return ERR_PARSER;

        GET_TOKEN();
        return statement(Pdata);
    }

    // <statement> -> Ԑ
    else
    {
        /*if (ret_exp != EXIT_SUCCESS) return ret_exp;
        if (ret_bfunc != EXIT_SUCCESS) return ret_bfunc;*/
        Pdata->empty_stat = true;

        return EXIT_SUCCESS;
    }
}


/**
 * <starts_w_id> -> = <inicialization>
 * <starts_w_id> -> <function_call>
 */
int starts_w_id(ParserData *Pdata) // TATO FUNKCIA ASI OK
{
    int ret;
    // <starts_w_id> -> = <inicialization>
    if(Pdata->token->type == TOKEN_ASSIGN)
    {
        GET_TOKEN();
        if ((ret = inicialization(Pdata)) != EXIT_SUCCESS)
        {
            return ret;
        }
        return EXIT_SUCCESS;
    }
    else if (Pdata->token->type != TOKEN_ASSIGN)
    {
        // ak neexistuje a nie je ten dalsi token '=', tak je to volanie nedefinovanej func a to je chyba!
        if (Pdata->FLAG_ID_not_exist == true) return ERR_SEMANTIC_1;

        sTItem *check = symtable_search(Pdata->current_symtable, Pdata->token->attribute);
        if (check->data->type != FUNC || check->data->defined != true)
            return ERR_SEMANTIC_1; // ak existuje, kontrolujeme, ci je to funkcia a ci je definovana
        // <starts_w_id> -> <function_call>

        CHOOSE_SYMTABLE(Pdata->token->attribute);

        Pdata->func_call_counter = 0;

        if ((ret = function_call(Pdata)) != EXIT_SUCCESS) return ret; // spocitat pocet paramterov pri volani func MOZNO SKONTROLOVANE

        if (check->data->num_of_params != Pdata->func_call_counter) return ERR_SEMANTIC_3;

        CHOOSE_SYMTABLE(MAIN);
        return EXIT_SUCCESS;
    }
    else return ERR_PARSER;
}


/**
 * <inicialization> -> <builtin_func>
 * <inicialization> -> <function> // treba skontrolovat, ci je definovana SKONTROLOVANE
 * <inicialization> -> <expression>
 * <inicialization> -> <value> // ak bude inicializacia pomocou ID, tak musiem skontrolovat, ci bolo ID definovane SKONTROLOVANE
 * <inicialization> -> NIL
 */
int inicialization(ParserData *Pdata) // TATO FUNKCIA ASI OK
{
    int ret;
    Pdata->FLAG_init = true;
    //int pole[256];
    // <inicialization> -> <builtin_func>
    if ((ret = builtin_func(Pdata)) == EXIT_SUCCESS)
    {
        Pdata->FLAG_init = false;
        return ret;
    }
    // <inicialization> -> <function>
    else if ((ret = function(Pdata)) == EXIT_SUCCESS)
    {
        Pdata->FLAG_init = false;
        return ret;
    }
    // <inicialization> -> <expression>
    else if ((ret = expression(Pdata->current_symtable, Pdata->token)) == EXIT_SUCCESS || ret == 385)
    {
        // POZOOOR! expression pyta o token navyse
        if (ret == 385)
        {
            GenVarSetValue1(Pdata->init_w_yourself);
            ret = EXIT_SUCCESS;
        }


        Pdata->FLAG_input_wo_br = true;
        Pdata->FLAG_init = false;
        return ret;
    }
    // <inicialization> -> <value>
    else if ((ret = value(Pdata)) == EXIT_SUCCESS)
    {
        Pdata->FLAG_init = false;
        return ret;
    }
    // <inicialization> -> nil
    else if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "nil") == 0))
    {
        Pdata->FLAG_init = false;
        return EXIT_SUCCESS;
    }
    else
    {
        return ret;
    }
}


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
int builtin_func(ParserData *Pdata)
{
    int ret;

    // <builtin_func> -> PRINT  <builtin_call>
    if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "print") == 0))
    {
        Pdata->Tarray = (Token *) malloc(sizeof(Token) * 100);
        Pdata->tarrc = 0;

        Pdata->FLAG_print = true;
        GET_TOKEN();

        //TU BY MALO BYT NIEKDE GENEROVANIE KODU
        if ((ret = builtin_call(Pdata)) != EXIT_SUCCESS)
        {
            return ret;
        }

        GenPrint2(Pdata->tarrc, Pdata->Tarray);
        free(Pdata->Tarray);
        Pdata->FLAG_print = false;
        return EXIT_SUCCESS;
    }
    // <builtin_func> -> CHR    <builtin_call>
    else if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "chr") == 0))
    {
        GET_TOKEN();
        //TU BY MALO BYT NIEKDE GENEROVANIE KODU
        if ((ret = builtin_call(Pdata)) != EXIT_SUCCESS) return ret;
        return EXIT_SUCCESS;
    }
     // <builtin_func> -> ORD    <builtin_call>
    else if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "ord") == 0))
    {
        GET_TOKEN();
        //TU BY MALO BYT NIEKDE GENEROVANIE KODU
        if ((ret = builtin_call(Pdata)) != EXIT_SUCCESS) return ret;
        return EXIT_SUCCESS;
    }
    // <builtin_func> -> SUBSTR <builtin_call>
    else if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "substr") == 0))
    {
        GET_TOKEN();
        //TU BY MALO BYT NIEKDE GENEROVANIE KODU
        if ((ret = builtin_call(Pdata)) != EXIT_SUCCESS) return ret;
        return EXIT_SUCCESS;
    }
    // <builtin_func> -> LENGTH <builtin_call>
    else if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "length") == 0))
    {
        GET_TOKEN();
        //TU BY MALO BYT NIEKDE GENEROVANIE KODU
        if ((ret = builtin_call(Pdata)) != EXIT_SUCCESS) return ret;
        return EXIT_SUCCESS;
    }
    // <builtin_func> -> INPUTF <input_call>
    else if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "inputf") == 0))
    {
        GET_TOKEN();
        //TU BY MALO BYT NIEKDE GENEROVANIE KODU
        if ((ret = input_call(Pdata)) != EXIT_SUCCESS) return ret;

        GenInpt(Pdata->init_w_yourself, FLOAT);

        return EXIT_SUCCESS;
    }
    // <builtin_func> -> INPUTS <input_call>
    else if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "inputs") == 0))
    {
        GET_TOKEN();
        //TU BY MALO BYT NIEKDE GENEROVANIE KODU
        if ((ret = input_call(Pdata)) != EXIT_SUCCESS) return ret;

        GenInpt(Pdata->init_w_yourself, STRING);

        return EXIT_SUCCESS;
    }
    // <builtin_func> -> INPUTI <input_call>
    else if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "inputi") == 0))
    {
        GET_TOKEN();
        //TU BY MALO BYT NIEKDE GENEROVANIE KODU
        if ((ret = input_call(Pdata)) != EXIT_SUCCESS) return ret;

        GenInpt(Pdata->init_w_yourself, INT);

        return EXIT_SUCCESS;
    }
    else
    {
        return ERR_PARSER;
    }
}


/**
 * <function> -> ID <function_call> EOL
 */
int function(ParserData *Pdata)
{
    int ret;

    if (Pdata->token->type == TOKEN_ID) // inicializacia premennej funkciou
    {
        if (symtable_add(Pdata->current_symtable, Pdata->token->attribute) == EXISTUJE)
        {
            sTItem *check = symtable_search(Pdata->current_symtable, Pdata->token->attribute);
            if (check->data->type != FUNC || check->data->defined != true)
                return ERR_SEMANTIC_1; // ak existuje, kontrolujeme, ci je to funkcia a ci je definovana
        }
        else return ERR_SEMANTIC_1;

        GET_TOKEN();
        if ((ret = function_call(Pdata)) != EXIT_SUCCESS) return ret;

        GET_TOKEN();
        if (Pdata->token->type != TOKEN_EOL) return ERR_PARSER;
        return EXIT_SUCCESS;
    }
    else return ERR_PARSER;
}


/**
 * <function_call> -> (<term_user>)
 * <function_call> -> <term_user>
 */
int function_call(ParserData *Pdata)
{
    int ret;

    // <function_call> -> (<term_user>)
    if (Pdata->token->type == TOKEN_LBRACKET)
    {
        GET_TOKEN();
        if ((ret = term_user(Pdata)) != EXIT_SUCCESS) return ret;

        GET_TOKEN();
        if (Pdata->token->type != TOKEN_RBRACKET) return ERR_PARSER;
        return EXIT_SUCCESS;
    }
    // <function_call> -> <term_user>
    else if ((ret = term_user(Pdata)) != EXIT_SUCCESS) return ret;
    else return ret;
}


/**
 * <builtin_call> -> (<term>)
 * <builtin_call> -> <term>
 */
int builtin_call(ParserData *Pdata)
{
    int ret;

    // <builtin_call> -> (<term>)
    if (Pdata->token->type == TOKEN_LBRACKET)
    {
        GET_TOKEN();
        if ((ret = term(Pdata)) != EXIT_SUCCESS)
        {
            return ret;
        }

        if (Pdata->FLAG_term_next_empty == false) GET_TOKEN();
        else Pdata->FLAG_term_next_empty = false;
        if (Pdata->token->type != TOKEN_RBRACKET)
        {
            return ERR_PARSER;
        }
        return EXIT_SUCCESS;

    }
    // <builtin_call> -> <term>
    if ((ret = term(Pdata)) != EXIT_SUCCESS)
    {
        return ret;
    }
    else return ret;
}


/**
 * <input_call> -> ()
 * <input_call> -> Ԑ
 */
int input_call(ParserData *Pdata)
{
    int ret;

    if(Pdata->token->type == TOKEN_LBRACKET)
    {
        GET_TOKEN();
        Pdata->FLAG_input_wo_br = false;
        if (Pdata->token->type == TOKEN_RBRACKET) return EXIT_SUCCESS;
        else return ERR_PARSER;
    }

    // <input_call -> Ԑ
    else
    {
        Pdata->FLAG_input_wo_br = true;
        return EXIT_SUCCESS;
    }
}


/**
 * <param> -> <value> <param_next>
 * <param> -> Ԑ
 */
int param(ParserData *Pdata)
{
    // <param> -> <value> <param_next>
    int ret, ret2;

    ret = value(Pdata);
    ret2 = param_next(Pdata);
    if (ret != EXIT_SUCCESS) return ret;
    else if (ret2 != EXIT_SUCCESS) return ret2;

    if (ret == EXIT_SUCCESS) Pdata->param_counter++;

    // <param> -> Ԑ
    return EXIT_SUCCESS;
}


/**
 * <param_next> -> , <value> <param_next>
 * <param_next> -> Ԑ
 */
int param_next(ParserData *Pdata)
{
    int ret;

    // param_next> -> , <value> <param_next>
    if (Pdata->token->type == TOKEN_COMMA)
    {
        GET_TOKEN();
        if ((ret = value(Pdata)) != EXIT_SUCCESS) return ret;

        Pdata->param_counter++;

        GET_TOKEN();
        return param_next(Pdata);
    }

    // <param_next> -> Ԑ
    else return EXIT_SUCCESS;
}


/**
 * <term_user> -> <term>
 * <term_user> -> Ԑ
 */
int term_user(ParserData *Pdata)
{
    int ret;

    // <term_user> -> <term>
    if ((ret = term(Pdata)) != EXIT_SUCCESS) return ret;
    // <term_user> -> Ԑ
    else return EXIT_SUCCESS;
}


/**
 * <term> -> NIL     <term_next>
 * <term> -> <value> <term_next>
 */
int term(ParserData *Pdata)
{
    if (Pdata->FLAG_print == true) Pdata->Tarray[Pdata->tarrc++] = *Pdata->token;

    int ret, ret2;
    if ((Pdata->token->type == TOKEN_KWORD) && (strcmp(Pdata->token->attribute, "nil") == 0))
    {
        Pdata->func_call_counter++;

        GET_TOKEN();
        if ((ret = term_next(Pdata)) != EXIT_SUCCESS) return ret;
        return EXIT_SUCCESS;
    }
    else
    {
        Pdata->FLAG_term = true;

        ret = value(Pdata);
        ret2 = term_next(Pdata);

        Pdata->FLAG_term = false;

        Pdata->func_call_counter++;

        if (ret != EXIT_SUCCESS)
        {
            return ret;
        }
        else if (ret2 != EXIT_SUCCESS) return ret2;
        else if (ret == EXIT_SUCCESS || ret2 == EXIT_SUCCESS) return EXIT_SUCCESS;

        else return ERR_PARSER;
    }
}


/**
 * <term_next> -> , <term>
 * <term_next> -> Ԑ
 */
int term_next(ParserData *Pdata)
{
    int ret;

    GET_TOKEN();
    if(Pdata->token->type == TOKEN_COMMA)
    {
        GET_TOKEN();
        if ((ret = term(Pdata)) != EXIT_SUCCESS) return ret;
        if (Pdata->FLAG_term_next_empty == false) Pdata->FLAG_term_next_empty = false;
        return EXIT_SUCCESS;
    }

    else
    {
        Pdata->FLAG_term_next_empty = true;
        return EXIT_SUCCESS;
    }
}


/**
 * <value> -> INTEGER
 * <value> -> FLOAT
 * <value> -> STRING
 * <value> -> ID
 */
int value(ParserData *Pdata)
{
    if (Pdata->token->type == TOKEN_INT)
    {
        //TODO neviem, zeby daka generacia
        return EXIT_SUCCESS;
    }
    else if (Pdata->token->type == TOKEN_FLOAT)
    {
        //TODO neviem, zeby daka generacia
        return EXIT_SUCCESS;
    }
    else if (Pdata->token->type == TOKEN_STRING)
    {
        //TODO neviem, zeby daka generacia

        return EXIT_SUCCESS;
    }
    else if (Pdata->token->type == TOKEN_ID)
    {
        // inicializacia pomocou ID, treba ceknut, ci je defined a ci je v tabulke...este sa moze robit a = a, tak treba ceknut, ci to nie je nahodou tak
        if (Pdata->FLAG_init == true)
        {
            if (strcmp(Pdata->init_w_yourself, Pdata->token->attribute) == 0) // a = a
            {
                symtable_add(Pdata->current_symtable, Pdata->token->attribute);
                symtable_fill(Pdata->current_symtable, Pdata->token->attribute, UNDEF, true, 0);
                // vygenerovat, ze a = NIL
            }
            else if (symtable_add(Pdata->current_symtable, Pdata->token->attribute) != EXISTUJE) return ERR_SEMANTIC_1;
            else
            {
                sTItem *check = symtable_search(Pdata->current_symtable, Pdata->token->attribute);
                if ((check->data->type != INT && check->data->type != FLOAT && check->data->type != STRING) || check->data->defined != true)
                    return ERR_SEMANTIC_1; // ak existuje, kontrolujeme, ci to je int || float || string a ci je d
            }
        }
        // Definicia funkcie - parameter ID
        if (Pdata->FLAG_function_def == true)
        {
            if (symtable_add(Pdata->current_symtable, Pdata->token->attribute) == EXISTUJE) return ERR_SEMANTIC_1;
            else symtable_fill(Pdata->current_symtable, Pdata->token->attribute, UNDEF, true, 0); // zatial nepozname datovy typ, preto UNDEF a defined, je true

            //GenFuncParam(); // tu daco volaj
        }

        if (Pdata->FLAG_term == true)
        {
            if (symtable_add(Pdata->current_symtable, Pdata->token->attribute) == EXISTUJE)
            {
                /* TOTO ESTE POZRI!!!!
                sTItem *check = symtable_search(current_symtable, Pdata->token->attribute);
                if ((check->data->type != INT && check->data->type != FLOAT && check->data->type != STRING) || check->data->defined != true)
                    return ERR_SEMANTIC_1; // ak existuje, kontrolujeme, ci to je int || float || string a ci je definovany
                */
            }
            else return ERR_SEMANTIC_1;
        }

        return EXIT_SUCCESS;
    }
    else return ERR_PARSER;
}


int main(int argc, char **argv)
{
    if (argc != 1)
    {
        fprintf(stderr, "main(): ERR bad args\n");
        return ERR_INTERNAL;
    }

    ParserData *Pdata = (ParserData *) malloc(sizeof(ParserData));
    if (Pdata == NULL) return ERR_INTERNAL;

    Pdata->symtable = (func_Sym_table *) malloc(sizeof(func_Sym_table) * MAX_SYMT_SIZE);
    if (Pdata->symtable == NULL)
    {
        free(Pdata);
        return ERR_INTERNAL;
    }

    func_symtable_init(Pdata->symtable);
    Pdata->current_symtable = func_symtable(Pdata->symtable, MAIN); // vytvorime symtable pre main()
    if (Pdata->current_symtable == NULL) return ERR_INTERNAL;

    Pdata->token = (Token *) malloc(sizeof(Token));
    if (Pdata->token == NULL)
    {
        free(Pdata->symtable);
        free(Pdata);
        return ERR_INTERNAL;
    }

    if (set_resources(Pdata->token) == false) return ERR_INTERNAL;

    int ret = InitList();
    if (ret != EXIT_SUCCESS) return ret;

    Pdata->FLAG_input_wo_br = false;  // input bez zatvoriek
    Pdata->empty_stat = false;
    Pdata->FLAG_term_next_empty = false;
    Pdata->FLAG_print = false;

    //TODO something
    GET_TOKEN();
    if (CodeGenStart() == false) return ERR_INTERNAL;
    if (GenStartMain() == false) return ERR_INTERNAL;

    ret = prog(Pdata);
    if (ret != EXIT_SUCCESS) return ret;

    if (GenPop() == false) return ERR_INTERNAL;
    if (GenEndMain() == false) return ERR_INTERNAL;
    CodeGenFlush(stdout);

    FreeList();

    clean_resources(Pdata->token);
    func_symtable_free(Pdata->symtable);
    free(Pdata->symtable);
    free(Pdata);

    return EXIT_SUCCESS;
}

/*** Koniec súboru parser.c ***/
