/**
 * Project: Implementace překladače imperativního jazyka IFJ18.
 *
 * @brief Implementácia generátoru kódu
 *
 * @author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
 * @author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
 * @author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
 * @author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>
 */

#include "generator.h"

#define APPEND(_string_) if(AppendString(code, _string_))                      \
                         {                                                     \
                             FreeList();                                       \
                             return false;                                     \
                         }


tList *code;
tList *strList;


int InitList()
{
    if ((code = malloc(sizeof(tList))) == NULL) return ERR_INTERNAL;
    code->First = NULL;

    if ((strList = malloc(sizeof(tList))) == NULL) return ERR_INTERNAL;
    strList->First = NULL;

    return EXIT_SUCCESS;
}


void FreeList()
{
    while (strList->First != NULL)
    {
        tElemPtr old2 = strList->First;
        strList->First = strList->First->ptr_next;

        free(old2->arr);
        free(old2);
    }
    free(strList);

    while (code->First != NULL)
    {
        tElemPtr old = code->First;
        code->First = code->First->ptr_next;
        /*
        int num;
        double dnum;
        if((sscanf(old->arr, "%d", &num) == 1) || (sscanf(old->arr, "%lf", &dnum) == 1)) free(old->arr);
        */
        free(old);
    }
    free(code);
}


int AppendString(tList *list, char *append)
{
    tElemPtr Item = malloc(sizeof(struct tElem));
    if (Item == NULL) return ERR_INTERNAL;

    Item->ptr_next = NULL;
    Item->arr = append;

    if (list->First == NULL) list->First = Item;
    else
    {
        tElemPtr Item2 = list->First;
        while (Item2->ptr_next != NULL) Item2 = Item2->ptr_next;
        Item2->ptr_next = Item;
    }

    return EXIT_SUCCESS;
}


/******************* BUILT IN ********************/
bool DefBInFuncs()
{
    APPEND(FUNCTION_LENGTH);

    APPEND(FUNCTION_SUBSTR);

    APPEND(FUNCTION_CHR);

    APPEND(FUNCTION_ASC);

    return true;
}


bool GenPrint(int pocet,...)
{
    va_list valist;
    va_start(valist, pocet);
    APPEND("WRITE ")
    for(int i = 0; i < pocet; i++)
    {
        GenValue(va_arg(valist, Token));
    }
    APPEND("\n")

    va_end(valist);

    return true;
}

bool GenPrint2(int pocet, Token *Tarray)
{

    for(int i = 0; i < pocet; i++)
    {
        APPEND("WRITE ")
        if(Tarray[i].attribute != NULL)
            GenValue(Tarray[i]);
        APPEND("\n")

    }

    APPEND("\n")

    return true;
}

bool GenInpt(char *Variable, Type_I type)
{
    APPEND("READ LF@")

    APPEND(Variable)

    APPEND(" ")

    switch (type)
    {
        case INT:
            APPEND("int\n")

            break;

        case FLOAT:
            APPEND("float\n")

            break;

        case STRING:
            APPEND("string\n")

            break;

        default:
            return false;
    }

    return true;
}


/******************* CODE GEN ********************/
bool GenHeader()
{
    APPEND(".IFJcode18\n")

    APPEND("DEFVAR GF@inpt\n")

    APPEND("MOVE GF@inpt string@\n")

    APPEND("DEFVAR GF@op1\n")

    APPEND("DEFVAR GF@op2\n")

    APPEND("DEFVAR GF@op3\n")

    APPEND("DEFVAR GF@result\n")

    APPEND("JUMP $main\n")

    return true;
}


bool CodeGenStart()
{
    if(!GenHeader())
        return false;

    /*if(!DefBInFuncs())
        return false;*/

    return true;
}


void CodeGenFlush(FILE *f)
{
    tElemPtr old = code->First;
    while (old != NULL)
    {
        fputs(old->arr, f);
        old = old->ptr_next;
    }
}


/******************* MAIN ********************/
bool GenStartMain()
{
    APPEND("\nLABEL $main\n")

    APPEND("CREATEFRAME\n")

    APPEND("PUSHFRAME\n")

    return true;
}


bool GenEndMain()
{
    APPEND("\nPOPFRAME\n")

    APPEND("CLEARS\n")

    return true;
}


/****************** VARIABLE ******************/
bool GenVarDefaultValue(Type_I type)
{
    switch (type)
    {
        case BOOL:
            APPEND("bool@false")

            break;

        case INT:
            APPEND("int@0")

            break;

        case FLOAT:
            APPEND("float@0.0")

            break;

        case STRING:
            APPEND("string@")

            break;

        default:
            return false;
    }

    return true;
}


bool GenVar(char *Variable)
{
    tElemPtr dako = code->First;
    while(dako != NULL)
    {
        if(strcmp(dako->arr, "DEFVAR LF@") == 0)
        {
            dako = dako->ptr_next;
            if(strcmp(dako->arr, Variable) == 0)
                return true;
        }
        dako = dako->ptr_next;
    }

    APPEND("DEFVAR LF@")

    APPEND(Variable)

    APPEND("\n")

    return true;
}


bool GenAndSetVarToDefault(char *Variable, Type_I type)
{
    APPEND("MOVE LF@")

    APPEND(Variable)

    APPEND(" ")

    if(!GenVarDefaultValue(type))

    APPEND("\n")

    return true;
}

bool GenVarSetValue1(char *variable)
{
    tElemPtr t1 = malloc(sizeof(struct tElem));
    if (t1 == NULL) return false;

    tElemPtr t2 = malloc(sizeof(struct tElem));
    if (t2 == NULL) return false;

    t1 = code->First;

    while (t1->ptr_next->ptr_next->ptr_next->ptr_next != NULL)
        t1 = t1->ptr_next;

    t2->ptr_next = t1->ptr_next;
    t1->ptr_next = t2;

    t1->arr = "\nMOVE LF@";
    t2->arr = variable;

    return true;
}


bool GenVarSetValue2(Token Token)
{
    switch (Token.type)
    {
        case TOKEN_INT:
            APPEND(" int@")

            break;

        case TOKEN_FLOAT:
            APPEND(" float@")

            break;

        case TOKEN_STRING:
            APPEND(" string@")

            break;

        default:
            return false;
    }

    APPEND(Token.attribute)

    APPEND("\n")

    return true;
}


/******************** LABEL *****************/
bool GenLabel(char *label, int label_idx)
{
    char *s = malloc(sizeof(int));
    sprintf(s, "%d", label_idx);

    APPEND("LABEL $")

    APPEND(label)

    APPEND("_")

    APPEND(s)
    if(AppendString(strList, s))
    {
        FreeList();
        return false;
    }

    APPEND("\n")

    return true;
}


/******************** IF ********************/
bool GenIFStart(char *label, int label_idx)
{
    char *s = malloc(sizeof(int));
    sprintf(s, "%d", label_idx);

    APPEND("JUMPIFEQ $")

    APPEND(label);

    APPEND("_")

    APPEND(s)
    if(AppendString(strList, s))
    {
        FreeList();
        return false;
    }

    APPEND(" GF@result bool@false\n")

    return true;
}


bool GenElse(char *label_end, int label_idx_end, char *label_else, int label_idx_else)
{
    char *s = malloc(sizeof(int));
    sprintf(s, "%d", label_idx_end);

    APPEND("JUMP $")

    APPEND(label_end)

    APPEND("_")

    APPEND(s)
    if(AppendString(strList, s))
    {
        FreeList();
        return false;
    }

    APPEND("\n")

    if(!GenLabel(label_else, label_idx_else))
        return false;

    return true;
}


bool GenIFEnd(char *label, int label_idx)
{
    if(!GenLabel(label, label_idx))
        return false;
    return true;
}


/****************** WHILE *****************/
bool GenWhileHead(char *label, int label_idx)
{
    if(!GenLabel(label, label_idx))
        return false;
    return true;
}


bool GenWhileStart(char *label, int label_idx)
{
    char *s = malloc(sizeof(int));
    sprintf(s, "%d", label_idx);

    APPEND("JUMPIFEQ $")

    APPEND(label)

    APPEND("_")

    APPEND(s)
    if(AppendString(strList, s))
    {
        FreeList();
        return false;
    }

    APPEND(" GF@result bool@false");

    APPEND("\n")

    return true;
}


bool GenWhileEnd(char *label_head, int label_idx_head, char *label_end, int label_idx_end)
{
    char *s = malloc(sizeof(int));
    sprintf(s, "%d", label_idx_head);

    APPEND("JUMP $")

    APPEND(label_head)

    APPEND("_")

    APPEND(s)
    if(AppendString(strList, s))
    {
        FreeList();
        return false;
    }

    APPEND("\n")

    if(!GenLabel(label_end, label_idx_end))
        return false;

    return true;
}


/****************** FUNC ******************/
bool GenFuncCall(char *func)
{
    APPEND("CALL $")

    APPEND(func)

    APPEND("\n")

    return true;
}


bool GenFuncParam(char *ParamID, int idx)
{
    char *s = malloc(sizeof(int));
    sprintf(s, "%d", idx);

    APPEND("DEFVAR LF@")

    APPEND(ParamID)

    APPEND("\n")

    APPEND("MOVE LF@")

    APPEND(ParamID)

    APPEND(" LF@")

    APPEND(s)
    if(AppendString(strList, s))
    {
        FreeList();
        return false;
    }

    APPEND("\n")

    return true;
}


bool GenFuncPassParam(Token token, int idx)
{
    APPEND("CREATEFRAME\n")

    return true;

    char *s = malloc(sizeof(int));
    sprintf(s, "%d", idx);

    APPEND("DEFVAR TF@")

    APPEND(s)
    if(AppendString(strList, s))
    {
        FreeList();
        return false;
    }

    APPEND("\n")

    APPEND("MOVE TF@")

    APPEND(s)
    if(AppendString(strList, s))
    {
        FreeList();
        return false;
    }

    APPEND(" ")

    if(!GenValue(token))

    APPEND("\n")

    return true;
}


bool GenFuncConvertPassedParam(Type_I from, Type_I to, int idx) // ___NAZOV___
{
    char *s = malloc(sizeof(int));
    sprintf(s, "%d", idx);

    if (from == FLOAT && to == INT)
    {
        APPEND("FLOAT2INT TF@")

        APPEND(s)
        if(AppendString(strList, s))
        {
            FreeList();
            return false;
        }

        APPEND(" TF@")

        APPEND(s)
        if(AppendString(strList, s))
        {
            FreeList();
            return false;
        }

        APPEND("\n")
    }
    else if(from == INT && to == FLOAT)
    {
        APPEND("INT2FLOAT TF@")

        APPEND(s)
        if(AppendString(strList, s))
        {
            FreeList();
            return false;
        }

        APPEND(" TF@")

        APPEND(s)
        if(AppendString(strList, s))
        {
            FreeList();
            return false;
        }

        APPEND("\n")
    }

    return true;
}


bool GenFuncStart(char *func)
{
    APPEND(func)

    APPEND("\n")

    APPEND("LABEL $")

    APPEND(func)

    APPEND("\n")

    APPEND("PUSHFRAME\n")

    return true;
}


bool GenFuncReturnValueAdjust(char *ValueID, Type_I Type, Type_I ReturnType)
{
    if (Type == INT && ReturnType == FLOAT)
    {
        APPEND("FLOAT2INT TF@return_value TF@return_value\n")
    }
    else if(Type == FLOAT && ReturnType == INT)
    {
        APPEND("INT2FLOAT TF@return_value TF@return_value\n")
    }

    APPEND("MOVE LF@")

    APPEND(ValueID)

    APPEND(" TF@return_value\n")

    return true;
}


bool GenFuncReturnValue(Type_I type)
{
    APPEND("DEFVAR LF@return_value\n")

    APPEND("MOVE LF@return_value ")

    if(!GenVarDefaultValue(type))
        return false;

    APPEND("\n")

    return true;
}


bool GenFuncReturn(char *func)
{
    APPEND("MOVE LF@return_value GF@result\n")

    APPEND("JUMP $")

    APPEND(func)

    APPEND("_return\n")

    return true;
}


bool GenFuncEnd(char *func)
{
    APPEND(func)

    APPEND("\n")

    APPEND("LABEL $")

    APPEND(func)

    APPEND("_return\n")

    APPEND("POPFRAME\n")

    APPEND("RETURN\n")

    return true;
}


/******************* OTHERS ********************/
bool GenValue(Token token)
{
    if(token.type == TOKEN_STRING)
    {
        tList *tmp_string; // ALOKACIA A INICIALIZACIA LISTU tmp_string ///////////////////////////////////////////////#
        if ((tmp_string = malloc(sizeof(tList))) == NULL) return ERR_INTERNAL;                                       //#
        tmp_string->First = NULL; /////////////////////////////////////////////////////////////////////////////////////#

        unsigned int length = strlen(token.attribute);
        char *esc_string;

        for (int i = 0; i < length; i++)
        {
            if (token.attribute[i] <= 32 || token.attribute[i] == 34 || token.attribute[i] == 35)
            {
                esc_string = malloc(5);
                sprintf(esc_string, "\\%03d", token.attribute[i]);
                if(AppendString(strList, esc_string))
                {
                    FreeList();
                    return false;
                }
            }
            else if (token.attribute[i] == 92)
            {
                if(token.attribute[i + 1] == 'n')
                {
                    esc_string = malloc(5);
                    sprintf(esc_string, "\\%03d", 10);
                    if(AppendString(strList, esc_string))
                    {
                        FreeList();
                        return false;
                    }
                    i++;
                }
                else if(token.attribute[i + 1] == 's')
                {
                    esc_string = malloc(5);
                    sprintf(esc_string, "\\%03d", 32);
                    if(AppendString(strList, esc_string))
                    {
                        FreeList();
                        return false;
                    }
                    i++;
                }
                else
                {
                    esc_string = malloc(5);
                    sprintf(esc_string, "\\%03d", token.attribute[i]);
                    if(AppendString(strList, esc_string))
                    {
                        FreeList();
                        return false;
                    }
                }
            }
            else if (isprint(token.attribute[i]))
            {
                esc_string = malloc(2);
                esc_string[0] = token.attribute[i];
                esc_string[1] = '\0';
                if(AppendString(strList, esc_string))
                {
                    FreeList();
                    return false;
                }
            }
            else
            {
                esc_string = malloc(5);
                if (token.attribute[i] < 100) sprintf(esc_string, "\\%03d", token.attribute[i]);
                else                          sprintf(esc_string, "\\%3d", token.attribute[i]);
                if(AppendString(strList, esc_string))
                {
                    FreeList();
                    return false;
                }
            }

            if(AppendString(tmp_string, esc_string)) // AK SA APPEND NEPODARIL TAK TREBA VYCISTIT tmp_string a vratit false //#
            {                                                                                                               //#
                while (tmp_string->First != NULL)                                                                           //#
                {                                                                                                           //#
                    tElemPtr old = tmp_string->First;                                                                       //#
                    tmp_string->First = tmp_string->First->ptr_next;                                                        //#
                    free(old);                                                                                              //#
                }                                                                                                           //#
                free(tmp_string);                                                                                           //#
                return false;                                                                                               //#
            } ////////////////////////////////////////////////////////////////////////////////////////////////////////////////#
        }

        APPEND("string@")

        while (tmp_string->First != NULL) // TREBA APPENDNUT KAZDY PRVOK LISTU tmp_string do LISTU code a VYCISTIT tmp_string //#
        {                                                                                                                     //#
            tElemPtr old = tmp_string->First;                                                                                 //#
                                                                                                                              //#
            APPEND(old->arr)                                                                                                  //#
                                                                                                                              //#
            tmp_string->First = tmp_string->First->ptr_next;                                                                  //#
            free(old);                                                                                                        //#
        }                                                                                                                     //#
        free(tmp_string); //////////////////////////////////////////////////////////////////////////////////////////////////////#

        return true;
    }
    else
    {

        switch (token.type)
        {
            case TOKEN_INT:
                APPEND("int@")
                char *term_str_i = malloc(sizeof(int));

                int int_retype;
                if(sscanf(token.attribute, "%d", &int_retype) != 1) return false; // konverzia char * na int
                sprintf(term_str_i, "%d", int_retype);

                APPEND(term_str_i)
                if(AppendString(strList, term_str_i))
                {
                    FreeList();
                    return false;
                }

                break;

            case TOKEN_FLOAT:
                APPEND("float@")
                char *term_str_f = malloc(sizeof(double));

                double float_retype;
                if(sscanf(token.attribute, "%lf", &float_retype) != 1) return false; // konverzia char * na double
                sprintf(term_str_f, "%a", float_retype);

                APPEND(term_str_f)
                if(AppendString(strList, term_str_f))
                {
                    FreeList();
                    return false;
                }

                break;

            case TOKEN_ID:
                APPEND("LF@")

                APPEND(token.attribute)

                break;

            default:
                return false;
        }
    }

    return true;
}


bool GenPush(Token token)
{
    APPEND("PUSHS ")

    if(!GenValue(token))
        return false;

    APPEND("\n")

    return true;
}


bool GenPop()
{
    tElemPtr tmp_6 = code->First;
    tElemPtr tmp = tmp_6->ptr_next->ptr_next->ptr_next->ptr_next->ptr_next->ptr_next;

    while (tmp != NULL)
    {
        if (!strcmp(tmp->arr, "ADDS\n") || !strcmp(tmp->arr, "SUBS\n") || !strcmp(tmp->arr, "MULS\n") || !strcmp(tmp->arr, "DIVS\n") || !strcmp(tmp->arr, "IDIVS\n"))
        {
            tElemPtr Item = malloc(sizeof(struct tElem));
            if (Item == NULL) return false;

            tElemPtr Item2 = malloc(sizeof(struct tElem));
            if (Item == NULL) return false;

            tElemPtr Item3 = malloc(sizeof(struct tElem));
            if (Item == NULL) return false;

            Item->ptr_next = Item2;
            Item2->ptr_next = Item3;
            Item3->ptr_next = tmp->ptr_next;
            tmp->ptr_next = Item;

            Item->arr = "POPS LF@";
            Item2->arr = tmp_6->arr;
            Item3->arr = "\n";
        }

        if (!strcmp(tmp->arr, "LTS\n") || !strcmp(tmp->arr, "GTS\n") || !strcmp(tmp->arr, "EQS\n"))
        {
            tElemPtr Item = malloc(sizeof(struct tElem));
            if (Item == NULL) return false;

            Item->ptr_next = tmp->ptr_next;
            tmp->ptr_next = Item;

            Item->arr = "POPS GF@result\n";
        }

        tmp = tmp->ptr_next;
        tmp_6 = tmp_6->ptr_next;
    }
    return true;
}


bool GenStackOperation(E_rules rule)
{
    switch (rule)
    {
        case E_PLUS_E:
            APPEND("ADDS\n")

            break;

        case E_MINUS_E:
            APPEND("SUBS\n")

            break;

        case E_MUL_E:
            APPEND("MULS\n")

            break;

        case E_DIV_E:
            APPEND("DIVS\n")

            break;

        case E_EQ_E:
            APPEND("EQS\n")

            break;

        case E_NOTEQ_E:
            APPEND("EQS\n")

            APPEND("NOTS\n")

            break;

        case E_LESSEQ_E:
            APPEND("POPS GF@op1\n")

            APPEND("POPS GF@op2\n")

            APPEND("PUSHS GF@op2\n")

            APPEND("PUSHS GF@op1\n")

            APPEND("LTS\n")

            APPEND("PUSHS GF@op2\n")

            APPEND("PUSHS GF@op1\n")

            APPEND("EQS\n")

            APPEND("ORS\n")

            break;

        case E_LESS_E:
            APPEND("LTS\n")

            break;

        case E_MOREEQ_E:
            APPEND("POPS GF@op1\n")

            APPEND("POPS GF@op2\n")

            APPEND("PUSHS GF@op2\n")

            APPEND("PUSHS GF@op1\n")

            APPEND("GTS\n")

            APPEND("PUSHS GF@op2\n")

            APPEND("PUSHS GF@op1\n")

            APPEND("EQS\n")

            APPEND("ORS\n")

            break;

        case E_MORE_E:
            APPEND("GTS\n")

            break;

        default:
            break;
    }

    return true;
}


bool GenConcat()
{
    APPEND("POPS GF@op3\n")

    APPEND("POPS GF@op2\n")

    APPEND("CONCAT GF@op1 GF@op2 GF@op3\n")

    APPEND("PUSHS GF@op1\n")

    return true;
}


bool GenSaveExpressionResult(char *Variable, Type_I ReturnType, Type_I Type) //___NAZOV___
{
    if (Type == INT && ReturnType == FLOAT)
    {
        APPEND("FLOAT2INT\n")
    }
    else if(Type == FLOAT && ReturnType == INT)
    {
        APPEND("INT2FLOATS\n")
    }

    APPEND("POPS ")

    APPEND(Variable)

    APPEND("\n")

    return true;
}


bool GenStackOP1ToFloat()
{
    APPEND("INT2FLOATS\n")

    return true;
}


bool GenStackOP1ToInteger()
{
    APPEND("FLOAT2INTS\n")

    return true;
}


bool GenStackOP2ToFloat()
{
    APPEND("POPS GF@op1\n")

    APPEND("INT2FLOATS\n")

    APPEND("PUSHS GF@op1\n")

    return true;
}


bool GenStackOP2ToInteger()
{
    APPEND("POPS GF@op1\n")

    APPEND("FLOAT2INTS\n")

    APPEND("PUSHS GF@op1\n")

    return true;
}

#ifdef TESTING
int main()
{
    int ret = InitList();
    if (ret != EXIT_SUCCESS) return ret;

    GenIFStart("IFSTART", 1);
    GenIFEnd("IFEND", 1);

    GenIFStart("IFSTART", 2);
    GenIFEnd("IFEND", 2);

    Token *token = malloc(sizeof(Token));
    if (token == NULL) return ERR_INTERNAL;

    token->type = TOKEN_STRING;
    char *attr = "Ahoj\s#\t\n\"Sve'te\s\\hahah\xAAgaaaaa";
    token->attribute = attr;

    GenValue(*token);
    APPEND("\n")
    GenPrint2(5, "asd", "123", "l","k","m");
    CodeGenFlush(stdout);

    printf("\n");
    FreeList();
    free(token);
    return 0;
}
#endif

/*** Koniec súboru generator.c ***/
