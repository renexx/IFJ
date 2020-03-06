/**
 * Project: Implementace překladače imperativního jazyka IFJ18.
 *
 * @brief Implementácia zásobníku pre precedenčnú analýzu
 *
 * @author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
 * @author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
 * @author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
 * @author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>
 */

#include "symstack.h"


void symstack_init(SymStack *stack)
{
    stack->top = NULL;
}


int symstack_push(SymStack *stack, PrecT_sym sym, int type)
{
    SymStackItem *New = (SymStackItem *) malloc(sizeof(SymStackItem));
    if (New == NULL) return ERR_INTERNAL;

    New->sym = sym;
    New->type = type;
    New->ptr_next = stack->top;
    stack->top = New;
    return EXIT_SUCCESS;
}


int symstack_pop(SymStack *stack)
{
    if (stack->top != NULL)
    {
        SymStackItem *tmp = stack->top;
        stack->top = stack->top->ptr_next;
        free(tmp);
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}


int symstack_count(SymStack *stack, PrecT_sym sym)
{
    int counter = 0;
    SymStackItem *ret = stack->top;
    while (ret->sym != sym)
    {
        ret = ret->ptr_next;
        counter++;
        if (ret == NULL) return -1;
    }
    return counter;
}


int symstack_pop_many(SymStack *stack, PrecT_sym sym)
{
    int counter = symstack_count(stack, sym);
    if (counter == -1) return ERR_INTERNAL;

    for (int i = 0; i < counter + 1; i++)
    {
        if (symstack_pop(stack) == EXIT_FAILURE) return ERR_INTERNAL;
    }
    return EXIT_SUCCESS;
}


SymStackItem *symstack_top(SymStack *stack)
{
    return stack->top;
}


SymStackItem *symstack_top_term(SymStack *stack)
{
    SymStackItem *tmp = stack->top;
    while (tmp->sym == EXPRESSION) // preskakujeme neterminalne znaky
    {
        tmp = tmp->ptr_next;
        if (tmp == NULL) return (SymStackItem *) ERR_INTERNAL;
    }
    return tmp;
}


int symstack_post_insert(SymStack *stack, PrecT_sym sym, int type)
{
    SymStackItem *tmp = stack->top;
    SymStackItem *pre_tmp = NULL;

    while (tmp->sym == EXPRESSION) // preskakujeme neterminalne znaky
    {
        pre_tmp = tmp;
        tmp = tmp->ptr_next;
        if (tmp == NULL) return ERR_INTERNAL;
    }
    SymStackItem *new = (SymStackItem *) malloc(sizeof(SymStackItem));
    if (new == NULL) return ERR_INTERNAL;

    new->sym = sym;
    new->type = type;
    if (pre_tmp == NULL)
    {
        stack->top = new;
        new->ptr_next = tmp;
    }
    else
    {
        pre_tmp->ptr_next = new;
        new->ptr_next = tmp;
    }

    return EXIT_SUCCESS;
}


void SymStack_free(SymStack *stack)
{
    while (symstack_pop(stack) == EXIT_SUCCESS);
}


void symstack_print(SymStack *stack)
{
    SymStackItem *top = symstack_top(stack);
    printf("\n ||            || TOP: %d type: %d\n", top->sym, top->type);
    SymStackItem *tmp = stack->top;

    while (tmp != NULL)
    {
        printf(" || %2d type: %d ||\n", tmp->sym, tmp->type);
        tmp = tmp->ptr_next;
    }
    printf(" ****************\n\n");
}

#ifdef TESTING
int main()
{
    SymStack *Syn = (SymStack *) malloc(sizeof(SymStack));
    if (Syn == NULL) return ERR_INTERNAL;
    symstack_init(Syn);
    printf("\n~~~~~~~~~~~~~~~~~~~~\n");
    printf("Inicializacia stacku\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n\n");

    PrecT_sym symbol[14] = {IDENTIF, PLUS, IDENTIF, MUL, IDENTIF, PLUS, LEFT_BR, PLUS, EXPRESSION, LESS, RIGHT_BR, MINUS, IDENTIF, DOLLAR};

    for (int i = 0; i < 14; i++)
    {
        if (symstack_push(Syn, symbol[i], UNDEF) == ERR_INTERNAL)
        {
            printf("CHYBA PUSH\n");
            return 1;
        }
    }

    symstack_print(Syn);

    symstack_pop_many(Syn, LESS);
    symstack_print(Syn);

    printf("\nStack PRED symstack_post_insert:\n\n");
    symstack_print(Syn);
    symstack_post_insert(Syn, LESS, UNDEF);
    printf("\nStack PO symstack_post_insert:\n\n");
    symstack_print(Syn);

    SymStackItem *top_term = symstack_top_term(Syn);
    printf("Stack top terminal: %d type: %d\n", top_term->sym, top_term->type);
    printf("--------------------------------------------------\n\n");

    symstack_pop_many(Syn, LEFT_BR);
    symstack_print(Syn);

    symstack_pop(Syn);
    symstack_print(Syn);

    printf("\n~~~~~~~~~~~\n");
    printf("Free stacku\n");
    printf("~~~~~~~~~~~\n\n");
    SymStack_free(Syn);
    free(Syn);

    return 0;
}
#endif

/*** Koniec súboru symstack.c ***/
