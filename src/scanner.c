/**
 * Project: Implementace překladače imperativního jazyka IFJ18.
 *
 * @brief Implementácia lexikálnej analýzy
 *
 * @author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
 * @author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
 * @author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
 * @author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>
 */

#include "scanner.h"


FILE *sfile;
tList *list;


bool set_resources(Token *token)
{
    sfile = stdin;
    if ((list = malloc(sizeof(tList))) == NULL) return false;
    list->First = NULL;

    token->type = TOKEN_EMPTY;

    return true;
}


void clean_resources(Token *token)
{
    fclose(sfile);

    while (list->First != NULL)
    {
        tElemPtr old = list->First;
        list->First = list->First->ptr_next;
        free(old->arr);
        free(old);
    }
    free(list);
    free(token);
}


bool save_address(char *arr)
{
    tElemPtr Item = malloc(sizeof(struct tElem));
    if (Item == NULL) return false;

    Item->ptr_next = list->First;
    list->First = Item;
    Item->arr = arr;

    return true;
}


int check_block_comment()
{
    int c; // lokalne c do ktoreho citame zo sfile
    char *arr = malloc(sizeof(char) * 7);
    if (arr == NULL) return ERR_INTERNAL;
    memset(arr, 0, 7);
    for (int i = 0; i < 6; i++) arr[i] = getc(sfile);

    if (strcmp(arr, "=begin") == 0)
    {
        c = getc(sfile);
        if (c != '\n' && !isspace(c))
        {
            free(arr);
            return ERR_SCANNER;
        }
        char *arr2 = malloc(sizeof(char) * 5);
        if (arr2 == NULL) return ERR_INTERNAL;
        memset(arr2, 0, 5);
        while (1)
        {
            while (c != '\n') c = getc(sfile);

            c = getc(sfile);
            if (c == -1) // EOF
            {
                free(arr);
                free(arr2);
                return ERR_SCANNER;
            }
            else
            {
                while (c == '\n') c = getc(sfile); // preskocenie riadkov, ktore obsahuju len EOL
                ungetc(c, sfile);
            }

            for (int i = 0; i < 4; i++) arr2[i] = getc(sfile);

            if (strcmp(arr2, "=end") == 0) break;
            else for (int i = 3; i >= 0; i--) ungetc(arr2[i], sfile);
        }
        free(arr2);
        c = getc(sfile);
        if (c != '\n' && !isspace(c))
        {
            free(arr);
            return ERR_SCANNER;
        }
        while (c != '\n') c = getc(sfile);
    }
    else for (int i = 5; i >= 0; i--) ungetc(arr[i], sfile);
    free(arr);

    return EXIT_SUCCESS;
}


int get_next_token(Token *token)
{
    if (sfile == NULL) return ERR_INTERNAL;

    int c; // Znaky zo súboru budu skenované do tejto premennej
    int automat_state = STATE_START;

    //token->type = TOKEN_EMPTY;
    token->attribute = NULL;

    static bool b = true;
    if (b == true)
    {
        int check_bc1 = check_block_comment();
        if (check_bc1 != EXIT_SUCCESS) return check_bc1; //skontrolujeme, ci hned prvy riadok suboru nie je blokovy koment
        b = false;
    }

    while(42)
    {
        c = getc(sfile);

        switch (automat_state)
        {
            case STATE_START: //////////////////////////////////////////////////

                if      (c == '\n')                             automat_state = STATE_EOL;
                else if (isspace(c))                            automat_state = STATE_START;
                else if (c == '#')                              automat_state = STATE_COMMENTARY;
                else if ((c >= 'a' && c <= 'z') || c == '_')    automat_state = STATE_ID;
                else if (isdigit(c))                            automat_state = STATE_NUMBER;
                else if (c == '"')                              automat_state = STATE_STRING;
                else if (c == '<')                              automat_state = STATE_LESS;
                else if (c == '>')                              automat_state = STATE_MORE;
                else if (c == '=')                              automat_state = STATE_EQ;
                else if (c == '!')                              automat_state = STATE_NEQ;
                else if (c == '+')
                {
                    token->type = TOKEN_PLUS;
                    return EXIT_SUCCESS;
                }
                else if (c == '-')
                {
                    token->type = TOKEN_MINUS;
                    return EXIT_SUCCESS;
                }
                else if (c == '*')
                {
                    token->type = TOKEN_MUL;
                    return EXIT_SUCCESS;
                }
                else if (c == '/')
                {
                    token->type = TOKEN_DIV;
                    return EXIT_SUCCESS;
                }
                else if (c == '(')
                {
                    token->type = TOKEN_LBRACKET;
                    return EXIT_SUCCESS;
                }
                else if (c == ')')
                {
                    token->type = TOKEN_RBRACKET;
                    return EXIT_SUCCESS;
                }
                else if (c == ',')
                {
                    token->type = TOKEN_COMMA;
                    return EXIT_SUCCESS;
                }
                else if (c == -1)
                {
                    token->type = TOKEN_EOF;
                    return EXIT_SUCCESS;
                }
                else return ERR_SCANNER;

                if (!isspace(c)) ungetc(c, sfile);

                break; /////////////////////////////////////////////////////////
            case STATE_ID: ; ///////////////////////////////////////////////////

                char *id_arr = malloc(sizeof(char) * 256);
                if (id_arr == NULL) return ERR_INTERNAL;
                memset(id_arr, 0, 256);
                int id_counter = 0;

                id_arr[id_counter] = c;
                id_counter++;

                c = getc(sfile);
                while (isalpha(c) || isdigit(c) || c == '_')
                {
                    id_arr[id_counter] = c;
                    id_counter++;
                    c = getc(sfile);
                }

                if (c == '!' || c == '?')
                {
                    id_arr[id_counter] = c;
                    token->type = TOKEN_ID;
                }
                else
                {
                    ungetc(c, sfile);
                    if ((strcmp(id_arr, "def"   ) == 0)     || \
                        (strcmp(id_arr, "do"    ) == 0)     || \
                        (strcmp(id_arr, "else"  ) == 0)     || \
                        (strcmp(id_arr, "end"   ) == 0)     || \
                        (strcmp(id_arr, "if"    ) == 0)     || \
                        (strcmp(id_arr, "not"   ) == 0)     || \
                        (strcmp(id_arr, "nil"   ) == 0)     || \
                        (strcmp(id_arr, "then"  ) == 0)     || \
                        (strcmp(id_arr, "while" ) == 0)     || \
                        (strcmp(id_arr, "inputs") == 0)     || \
                        (strcmp(id_arr, "inputi") == 0)     || \
                        (strcmp(id_arr, "inputf") == 0)     || \
                        (strcmp(id_arr, "print" ) == 0)     || \
                        (strcmp(id_arr, "length") == 0)     || \
                        (strcmp(id_arr, "substr") == 0)     || \
                        (strcmp(id_arr, "ord"   ) == 0)     || \
                        (strcmp(id_arr, "chr"   ) == 0))    token->type = TOKEN_KWORD;
                    else                                    token->type = TOKEN_ID;
                }
                token->attribute = id_arr;

                if (save_address(id_arr) == false)
                {
                    free(id_arr);
                    return ERR_INTERNAL;
                }

                return EXIT_SUCCESS;

                break; /////////////////////////////////////////////////////////
            case STATE_NUMBER: ; ///////////////////////////////////////////////

                char *num_arr = malloc(sizeof(char) * 256);
                if (num_arr == NULL) return ERR_INTERNAL;
                memset(num_arr, 0, 256);
                int num_counter = 0;

                num_arr[num_counter] = c;
                num_counter++;

                c = getc(sfile);
                while (isdigit(c))
                {
                    num_arr[num_counter] = c;
                    num_counter++;
                    c = getc(sfile);
                }

                if      (c == '.')             goto STATE_FLOAT;
                else if (c == 'e' || c == 'E') goto STATE_EXP;
                else
                {
                    ungetc(c, sfile);
                    token->type = TOKEN_INT;
                    token->attribute = num_arr;

                    if (save_address(num_arr) == false)
                    {
                        free(num_arr);
                        return ERR_INTERNAL;
                    }
                }

                return EXIT_SUCCESS;

                break; /////////////////////////////////////////////////////////
            STATE_FLOAT: ///////////////////////////////////////////////////////

                num_arr[num_counter] = c;
                num_counter++;

                c = getc(sfile);
                if (!isdigit(c))
                {
                    free(num_arr);
                    return ERR_SCANNER;
                }

                while (isdigit(c))
                {
                    num_arr[num_counter] = c;
                    num_counter++;
                    c = getc(sfile);
                }

                if (c == 'e' || c == 'E') goto STATE_EXP;
                else
                {
                    ungetc(c, sfile);
                    token->type = TOKEN_FLOAT;
                    token->attribute = num_arr;

                    if (save_address(num_arr) == false)
                    {
                        free(num_arr);
                        return ERR_INTERNAL;
                    }
                }

                return EXIT_SUCCESS;

            ////////////////////////////////////////////////////////////////////
            STATE_EXP: /////////////////////////////////////////////////////////

                num_arr[num_counter] = c;
                num_counter++;

                c = getc(sfile);
                if (c != '+' && c != '-' && !isdigit(c))
                {
                    free(num_arr);
                    return ERR_SCANNER;
                }

                if (c == '+' || c == '-')
                {
                    num_arr[num_counter] = c;
                    num_counter++;
                    c = getc(sfile);
                    if (!isdigit(c))
                    {
                        free(num_arr);
                        return ERR_SCANNER;
                    }
                }

                while (isdigit(c))
                {
                    num_arr[num_counter] = c;
                    num_counter++;
                    c = getc(sfile);
                }
                ungetc(c, sfile);
                token->type = TOKEN_FLOAT;
                token->attribute = num_arr;

                if (save_address(num_arr) == false)
                {
                    free(num_arr);
                    return ERR_INTERNAL;
                }

                return EXIT_SUCCESS;

            ////////////////////////////////////////////////////////////////////
            case STATE_STRING: ; ///////////////////////////////////////////////

                char *str_arr = malloc(sizeof(char) * 1024);
                if (str_arr == NULL) return ERR_INTERNAL;
                memset(str_arr, 0, 1024);
                int str_counter = 0;

                c = getc(sfile);
                while (1)
                {
                    if (c >= 32 && c <=255 && c != '"' && c != '\\')
                    {
                        str_arr[str_counter] = c;
                        str_counter++;
                    }
                    else if (c == '\\')
                    {
                        str_arr[str_counter] = c;
                        str_counter++;

                        c = getc(sfile);
                        if (c == 't' || c == 's' || c == 'n' || c == '"' || c == '\\')
                        {
                            str_arr[str_counter] = c;
                            str_counter++;
                        }
                        else if (c == 'x')
                        {
                            str_arr[str_counter] = c;
                            str_counter++;

                            c = getc(sfile);
                            if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
                            {
                                str_arr[str_counter] = c;
                                str_counter++;

                                c = getc(sfile);
                                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
                                {
                                    str_arr[str_counter] = c;
                                    str_counter++;
                                }
                                else ungetc(c, sfile);
                            }
                            else
                            {
                                free(str_arr);
                                return ERR_SCANNER;
                            }
                        }
                        else
                        {
                            free(str_arr);
                            return ERR_SCANNER;
                        }
                    }
                    else if (c == '"') break;
                    else
                    {
                        free(str_arr);
                        return ERR_SCANNER;
                    }
                    c = getc(sfile);
                }
                token->type = TOKEN_STRING;
                if (str_counter != 0) token->attribute = str_arr;

                if (save_address(str_arr) == false)
                {
                    free(str_arr);
                    return ERR_INTERNAL;
                }

                return EXIT_SUCCESS;

                break; /////////////////////////////////////////////////////////
            case STATE_EOL: ////////////////////////////////////////////////////

                ungetc(c, sfile);
                int check_bc2 = check_block_comment();
                if (check_bc2 != EXIT_SUCCESS) return check_bc2;

                if (token->type != TOKEN_EMPTY && token->type != TOKEN_EOL)
                {
                    token->type = TOKEN_EOL;
                    return EXIT_SUCCESS;
                }
                else automat_state = STATE_START;

                break; /////////////////////////////////////////////////////////
            case STATE_COMMENTARY: /////////////////////////////////////////////

                while (c != '\n') c = getc(sfile);
                ungetc(c, sfile);
                automat_state = STATE_START;

                break; /////////////////////////////////////////////////////////
            case STATE_EQ: /////////////////////////////////////////////////////

                c = getc(sfile);
                if (c == '=') token->type = TOKEN_EQ;
                else
                {
                    ungetc(c, sfile);
                    token->type = TOKEN_ASSIGN;
                }
                return EXIT_SUCCESS;

                break; /////////////////////////////////////////////////////////
            case STATE_NEQ: ////////////////////////////////////////////////////

                c = getc(sfile);
                if (c == '=') token->type = TOKEN_NEQ;
                else return ERR_SCANNER;
                return EXIT_SUCCESS;

                break; /////////////////////////////////////////////////////////
            case STATE_LESS: ///////////////////////////////////////////////////

                c = getc(sfile);
                if (c == '=') token->type = TOKEN_LEQ;
                else
                {
                    ungetc(c, sfile);
                    token->type = TOKEN_LTN;
                }
                return EXIT_SUCCESS;

                break; /////////////////////////////////////////////////////////
            case STATE_MORE: ///////////////////////////////////////////////////

                c = getc(sfile);
                if (c == '=') token->type = TOKEN_MEQ;
                else
                {
                    ungetc(c, sfile);
                    token->type = TOKEN_MTN;
                }
                return EXIT_SUCCESS;

                break; /////////////////////////////////////////////////////////
            default: ///////////////////////////////////////////////////////////

                return ERR_SCANNER;

                break; /////////////////////////////////////////////////////////
        }
    }
}

#ifdef TESTING
int main(int argc, char **argv)
{
    if (argc != 1)
    {
        fprintf(stderr, "main(): ERR bad args\n");
        return ERR_INTERNAL;
    }

    FILE *write = stdout;

    Token *token = malloc(sizeof(Token));
    if (token == NULL) return ERR_INTERNAL;

    if (set_resources(token) == false) return ERR_INTERNAL;

    while (1)
    {
        int ret = get_next_token(token);
        if (ret != EXIT_SUCCESS)
        {
            fprintf(stderr, "main(): ERR\n");

            clean_resources(token);
            fclose(write);
            return ret;
        }

        if (token->type == TOKEN_INT)
        {
            int inum;
            sscanf(token->attribute, "%d", &inum);
            fprintf(write, "TYPE: %2d    ATTR: %d\n", token->type, inum);
        }
        else if (token->type == TOKEN_FLOAT)
        {
            double dnum;
            sscanf(token->attribute, "%lf", &dnum);
            fprintf(write, "TYPE: %2d    ATTR: %lf\n", token->type, dnum);
        }
        else fprintf(write, "TYPE: %2d    ATTR: %s\n", token->type, token->attribute);

        if (token->type == TOKEN_EOF) break;
    }

    clean_resources(token);
    fclose(write);

    return EXIT_SUCCESS;
}
#endif

/*** Koniec súboru scanner.c ***/
