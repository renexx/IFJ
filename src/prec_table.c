/**
 * Project: Implementace překladače imperativního jazyka IFJ18.
 *
 * @brief Implementácia precedenčnej anlýzy
 *
 * @author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
 * @author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
 * @author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
 * @author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>
 */

#include "prec_table.h"


char prec_table[TSIZE][TSIZE] = {
/*       +  -  *  /  <  <= >  >= == != (  )  i  $ */
/* +  */{M, M, L, L, M, M, M, M, M, M, L, M, L, M},
/* -  */{M, M, L, L, M, M, M, M, M, M, L, M, L, M},
/* *  */{M, M, M, M, M, M, M, M, M, M, L, M, L, M},
/* /  */{M, M, M, M, M, M, M, M, M, M, L, M, L, M},
/* <  */{L, L, L, L, X, X, X, X, M, M, L, M, L, M},
/* <= */{L, L, L, L, X, X, X, X, M, M, L, M, L, M},
/* >  */{L, L, L, L, X, X, X, X, M, M, L, M, L, M},
/* >= */{L, L, L, L, X, X, X, X, M, M, L, M, L, M},
/* == */{L, L, L, L, L, L, L, L, X, X, L, M, L, M},
/* != */{L, L, L, L, L, L, L, L, X, X, L, M, L, M},
/* (  */{L, L, L, L, L, L, L, L, L, L, L, Q, L, X},
/* )  */{M, M, M, M, M, M, M, M, M, M, X, M, X, M},
/* i  */{M, M, M, M, M, M, M, M, M, M, X, M, X, M},
/* $  */{L, L, L, L, L, L, L, L, L, L, L, X, L, S}
};


PrecT_sym ConvertsToken(Sym_table *table, Token *token, int *type)
{
    *type = UNDEF; // defaultne

    switch (token->type)
    {
        case TOKEN_PLUS:
            return PLUS;

        case TOKEN_MINUS:
            return MINUS;

        case TOKEN_DIV:
            return DIV;

        case TOKEN_MUL:
            return MUL;

        case TOKEN_LTN:
            return LESS;

        case TOKEN_LEQ:
            return LESS_EQ;

        case TOKEN_MTN:
            return MORE;

        case TOKEN_MEQ:
            return MORE_EQ;

        case TOKEN_EQ:
            return EQ;

        case TOKEN_NEQ:
            return NOT_EQ;

        case TOKEN_LBRACKET:
            return LEFT_BR;

        case TOKEN_RBRACKET:
            return RIGHT_BR;

        case TOKEN_INT:
            *type = INT;
            return IDENTIF;

        case TOKEN_FLOAT:
            *type = FLOAT;
            return IDENTIF;

        case TOKEN_STRING:
            *type = STRING;
            return IDENTIF;

        case TOKEN_ID: ;
            sTItem *find_type = symtable_search(table, token->attribute);
            if      (find_type->data->type == INT   )   *type = INT;
            else if (find_type->data->type == FLOAT )   *type = FLOAT;
            else if (find_type->data->type == STRING)   *type = STRING;

            return IDENTIF;

        default:
            return DOLLAR;
    }
}

int rules(SymStack *stack, int number)
{
    if (number != 1 && number != 3) return ERR_PARSER;
    SymStackItem *temp = stack->top;
    PrecT_sym sym1, sym2, sym3;
    Type_I ssym1, ssym2, ssym3; // pre semanticku kontrolu
    int counter = 0;

    int ret;


    if (counter < number) // pravidlo 11: I; counter == 0, number == 1 || 3
    {
        sym1 = temp->sym;
        ssym1 = temp->type;
        temp = temp->ptr_next;
        counter++;

        if (counter < number) // pravidlo 0-10; counter == 1, number == 1 || 3
        {
            sym2 = temp->sym;
            ssym2 = temp->type;
            temp = temp->ptr_next;
            counter++;

            if (counter < number) // counter == 2, number == 3
            {
                sym3 = temp->sym;
                ssym3 = temp->type;
            }
        }
    }

    switch (number)
    {
        case 1:
            if (sym1 == IDENTIF)
            {
                ret = expression_semantics(I, ssym1, ssym2, ssym3);
                if (ret != EXIT_SUCCESS)
                {
                    return ret;
                }
                //return I;
                return EXIT_SUCCESS;
            }
            else
            {
                return NOT_RULE;
            }

        case 3:
            if (sym1 == LEFT_BR && (sym2 == IDENTIF || sym2 == EXPRESSION) && sym3 == RIGHT_BR)
            {
                ret = expression_semantics(BR_E_BR, ssym1, ssym2, ssym3);
                if (ret != EXIT_SUCCESS)
                {
                    return ret;
                }
                //return BR_E_BR;
                return EXIT_SUCCESS;
            }
            else if ((sym1 == IDENTIF || sym1 == EXPRESSION) && (sym3 == IDENTIF || sym3 == EXPRESSION))
            {
                if (sym2 == PLUS)
                {
                    ret = expression_semantics(E_PLUS_E, ssym1, ssym2, ssym3);
                    if (ret != EXIT_SUCCESS)
                    {
                        return ret;
                    }
                    //return E_PLUS_E;
                    return EXIT_SUCCESS;
                }
                else if (sym2 == MINUS)
                {
                    ret = expression_semantics(E_MINUS_E, ssym1, ssym2, ssym3);
                    if (ret != EXIT_SUCCESS)
                    {
                        return ret;
                    }
                    //return E_MINUS_E;
                    return EXIT_SUCCESS;
                }
                else if (sym2 == MUL)
                {
                    ret = expression_semantics(E_MUL_E, ssym1, ssym2, ssym3);
                    if (ret != EXIT_SUCCESS)
                    {
                        return ret;
                    }
                    //return E_MUL_E;
                    return EXIT_SUCCESS;
                }
                else if (sym2 == DIV)
                {
                    ret = expression_semantics(E_DIV_E, ssym1, ssym2, ssym3);
                    if (ret != EXIT_SUCCESS)
                    {
                        return ret;
                    }
                    //return E_DIV_E;
                    return EXIT_SUCCESS;
                }
                else if (sym2 == LESS)
                {
                    ret = expression_semantics(E_LESS_E, ssym1, ssym2, ssym3);
                    if (ret != EXIT_SUCCESS)
                    {
                        return ret;
                    }
                    //return E_LESS_E;
                    return EXIT_SUCCESS;
                }
                else if (sym2 == LESS_EQ)
                {
                    ret = expression_semantics(E_LESSEQ_E, ssym1, ssym2, ssym3);
                    if (ret != EXIT_SUCCESS)
                    {
                        return ret;
                    }
                    //return E_LESSEQ_E;
                    return EXIT_SUCCESS;
                }
                else if (sym2 == MORE)
                {
                    ret = expression_semantics(E_MORE_E, ssym1, ssym2, ssym3);
                    if (ret != EXIT_SUCCESS)
                    {
                        return ret;
                    }
                    //return E_MORE_E;
                    return EXIT_SUCCESS;
                }
                else if (sym2 == MORE_EQ)
                {
                    ret = expression_semantics(E_MOREEQ_E, ssym1, ssym2, ssym3);
                    if (ret != EXIT_SUCCESS)
                    {
                        return ret;
                    }
                    //return E_MOREEQ_E;
                    return EXIT_SUCCESS;
                }
                else if (sym2 == EQ)
                {
                    ret = expression_semantics(E_EQ_E, ssym1, ssym2, ssym3);
                    if (ret != EXIT_SUCCESS)
                    {
                        return ret;
                    }
                    //return E_EQ_E;
                    return EXIT_SUCCESS;
                }
                else if (sym2 == NOT_EQ)
                {
                    ret = expression_semantics(E_NOTEQ_E, ssym1, ssym2, ssym3);
                    if (ret != EXIT_SUCCESS)
                    {
                        return ret;
                    }
                    //return E_NOTEQ_E;
                    return EXIT_SUCCESS;
                }
                else
                {
                    return NOT_RULE;
                }
            }
            else
            {
                return NOT_RULE;
            }

        default:
            return NOT_RULE;
    }
}


int expression(Sym_table *table, Token *token)
{
    Token Told = *token;
    SymStack *Stack = (SymStack *) malloc(sizeof(SymStack));
    if (Stack == NULL) return ERR_INTERNAL;
    symstack_init(Stack);
    symstack_push(Stack, DOLLAR, UNDEF);

    int c = 0;

    int ret;
    int type; // ak bude PrecT_sym == IDENTIF tak pouzijeme na rozlisenie int, float, string

    PrecT_sym a = ConvertsToken(table, token, &type);
    SymStackItem *top_term = symstack_top_term(Stack);
    PrecT_sym b = top_term->sym;

    while (1)
    {
        char character = prec_table[b][a];
        Token pushT = *token;

        if (character == S) break;
        else if (character == Q)
        {
            symstack_push(Stack, a, type);
            //if (top_term->sym == LEFT_BR && top_term->ptr_next->ptr_next->sym == DOLLAR && a == RIGHT_BR) break;
        }
        else if (character == M)
        {
            int number = symstack_count(Stack, NLESS);
            ret = rules(Stack, number);
            if (ret != EXIT_SUCCESS) return ret;

            symstack_pop_many(Stack, NLESS);
            symstack_push(Stack, EXPRESSION, type);
        }
        else if (character == L)
        {
            if ((ret = get_next_token(token)) != EXIT_SUCCESS)
                return ret;
            c++;
            if (c == 1 && token->type == TOKEN_EOL)
            {
                GenVarSetValue2(Told);
                free(Stack);
                return 385;
            }

            symstack_post_insert(Stack, NLESS, type);
            symstack_push(Stack, a, type);

            if (a == IDENTIF) GenPush(pushT);
        }
        else
        {
            fprintf(stderr, "\nZNAK x, ERROR\n");
            return ERR_PARSER;
        }

        if (character != M && character != L)
        {
            if ((ret = get_next_token(token)) != EXIT_SUCCESS)
                return ret;

        }

        a = ConvertsToken(table, token, &type);
        top_term = symstack_top_term(Stack);
        b = top_term->sym;
    }
    /*
    int number = symstack_count(Stack, LESS);
    ret = rules(Stack, number);
    if (ret == ERR_PARSER) return ret;

    symstack_print(Stack);
    symstack_pop_many(Stack, LESS);*/

    free(Stack);
    return EXIT_SUCCESS;
}


int expression_semantics(E_rules rule, Type_I sym1, Type_I sym2, Type_I sym3)
{
    /* TOTO NIKDE NEPOUZIVAS
    bool retype_sym1_int = false;
    bool retype_sym3_int = false;
    bool retype_sym1_float = false;
    bool retype_sym3_float = false;
    */
    int result = UNDEF; // defaultne???


    if (rule == E_PLUS_E)
    {
        //konkatenacia
        if (sym1 == STRING && sym3 == STRING)   GenConcat();
        if (sym1 == INT    && sym3 == INT   )   result = INT;
        if (sym1 == FLOAT  && sym3 == FLOAT )   result = FLOAT;
        if (sym1 == INT    && sym3 == FLOAT )   GenStackOP1ToFloat();
        if (sym1 == FLOAT  && sym3 == INT   )   GenStackOP2ToFloat();

        if (!(sym1 == STRING && sym3 == STRING))
            GenStackOperation(rule);
    }

    else if (rule == E_MINUS_E || rule == E_MUL_E || rule == E_DIV_E)
    {
        if (sym1 == STRING || sym3 == STRING)
        {
            return ERR_SEMANTIC_2;
        }
        if (sym1 == INT    && sym3 == INT   )   result = INT;
        if (sym1 == FLOAT  && sym3 == FLOAT )   result = FLOAT;
        if (sym1 == INT    && sym3 == FLOAT )   GenStackOP1ToFloat();
        if (sym1 == FLOAT  && sym3 == INT   )   GenStackOP2ToFloat();
        GenStackOperation(rule);
    }

    else if (rule == E_MOREEQ_E || rule == E_MORE_E || rule == E_LESS_E || rule == E_LESSEQ_E)
    {
        if (sym1 == INT   && sym3 == FLOAT)     GenStackOP1ToFloat();
        if (sym1 == FLOAT && sym3 == INT  )     GenStackOP2ToFloat();

        if ((sym1 == INT   && sym3 == STRING) || (sym1 == STRING && sym3 == INT))
        {
            return ERR_SEMANTIC_2;
        }
        if ((sym1 == FLOAT && sym3 == STRING) || (sym1 == STRING && sym3 == FLOAT))
        {
            return ERR_SEMANTIC_2;
        }
        if (sym1 == STRING && sym3 == STRING)
        {
            return ERR_SEMANTIC_2;
        }
        GenStackOperation(rule);
    }

    else if (rule == E_EQ_E || rule == E_NOTEQ_E)
    {
        if (sym1 == INT    && sym3 == FLOAT )   GenStackOP1ToFloat();
        if (sym1 == FLOAT  && sym3 == INT   )   GenStackOP2ToFloat();

        if ((sym1 == INT   && sym3 == STRING) || (sym1 == STRING && sym3 == INT)  )
        {
            return ERR_SEMANTIC_2;
        }
        if ((sym1 == FLOAT && sym3 == STRING) || (sym1 == STRING && sym3 == FLOAT))
        {
            return ERR_SEMANTIC_2;
        }
        GenStackOperation(rule);
    }

    else if (rule == BR_E_BR)
    {
        if (sym2 == STRING)
        {
            return ERR_SEMANTIC_2;
        }
        if (sym2 == INT   )   result = INT;
        if (sym2 == FLOAT )   result = FLOAT;
    }

    else if (rule == I)
    {
        if (sym1 == STRING)
        {
            return ERR_SEMANTIC_2;
        }
        if (sym1 == INT   )   result = INT;
        if (sym1 == FLOAT )   result = FLOAT;
    }

    /** IBA KVOLI PREKLADU ZATIAL */
    if (result == UNDEF);

    return EXIT_SUCCESS;
}

/*** Koniec prec_table.c ***/
