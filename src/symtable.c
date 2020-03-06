/**
 * Project: Implementace překladače imperativního jazyka IFJ18.
 *
 * @brief Implementácia tabuľky symbolov
 *
 * @author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
 * @author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
 * @author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
 * @author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>
 */

#include "symtable.h"


unsigned long hash_fun(const char *key)
{
    int retval = 1;
    int keylen = strlen(key);
    for (int i = 0; i < keylen; i++) retval = retval + key[i];
    return (retval % MAX_SYMT_SIZE);
}


void symtable_init(Sym_table *table)
{
    if (table == NULL) return;
    memset(*table, 0, sizeof(struct htab_listitem) * MAX_SYMT_SIZE);
}


sTItem *line_cross(Sym_table *table, const char *key, bool *exist)
{
    *exist = false;
    sTItem *Item;
    sTItem *Last_Item = NULL;
    //nemusime overovať key nakoľko je overený v každej func skor ako je zavolaná táto
    Item = (*table)[hash_fun(key)];
    while (Item != NULL)
    {
        if (strcmp(Item->key, key) == 0) //ak sa zhoduju kluce
        {
            *exist = true;
            return Item;
        }

        Last_Item = Item;
        Item = Item->ptr_next;
    }
    return Last_Item;
}


int symtable_add(Sym_table *table, char *key)
{
    if (table == NULL || key == NULL)   return ERR_INTERNAL;

    unsigned long idx = hash_fun(key);                      //získame index do tabulky
    bool exist = false;
    sTItem *Item = line_cross(table, key, &exist);           //prejdeme cely riadok tabulky až na koniec (pridavame prvok na koniec)
    if(exist)                                               //Prvok s danym key uz existuje
    {
        return EXISTUJE;
    }

    sTItem *New_Item = (sTItem *) malloc(sizeof(sTItem));   //allokujeme si miesto pre novy item
    if (New_Item == NULL)   return ERR_INTERNAL;

    New_Item->key = (char *) malloc((strlen(key) + 1) * sizeof(char));
    if (New_Item->key == NULL)
    {
        free(New_Item);
        return ERR_INTERNAL;
    }

    New_Item->data = (tData *) malloc(sizeof(struct htab_data));
    if (New_Item->data == NULL)
    {
        free(New_Item->key);
        free(New_Item);
        return ERR_INTERNAL;
    }

    strcpy(New_Item->key, key);
    New_Item->data->type = UNDEF;
    New_Item->data->ID = key;
    New_Item->data->defined = false;
    New_Item->data->num_of_params = 0;
    New_Item->ptr_next = NULL;

    if      (Item == NULL && !exist) (*table)[idx] = New_Item; //Pridame prvok na zaciatok riadku
    else                            Item->ptr_next = New_Item; //Pridame prvok na koniec riadku

    return EXIT_SUCCESS;
}


sTItem *symtable_search(Sym_table *table, const char *key)
{
    if (table == NULL || key == NULL) return (sTItem *)ERR_INTERNAL;

    bool exist = false;
    sTItem *Item = line_cross(table, key, &exist);
    if (exist) return Item;
    else       return NULL;
}


bool symtable_remove(Sym_table *table, const char *key)
{
    if (table == NULL || key == NULL) return false;

    unsigned long idx = hash_fun(key);
    sTItem *Item = (*table)[idx];
    sTItem *Last_Item = NULL;

    while (Item != NULL)
    {
        if (strcmp(Item->key, key) == 0)
        {
            if (Last_Item == NULL) (*table)[idx] = (*table)[idx]->ptr_next; //prva iteracia
            else                   Last_Item->ptr_next = Item->ptr_next;

            free(Item->key);
            free(Item->data);
            free(Item);
            return true;
        }
        Last_Item = Item;
        Item = Item->ptr_next;
    }

    return false;
}


void symtable_free(Sym_table *table)
{
    if (table != NULL)
    {
        sTItem *Next_Item = NULL;
        for (unsigned int i = 0; i < MAX_SYMT_SIZE; i++)
        {
            sTItem *Item = (*table)[i];
            while (Item != NULL)
            {
                Next_Item = Item->ptr_next;
                free(Item->key);
                free(Item->data);
                free(Item);
                Item = Next_Item;
            }
            (*table)[i] = 0;
        }
    }
}


bool symtable_fill(Sym_table *table, char *key, int type, bool defined, int num_of_params)
{
    bool exist = false;
    sTItem *tmp = line_cross(table, key, &exist);

    if (exist)
    {
        tmp->data->type = type;
        tmp->data->ID = key;
        tmp->data->defined = defined;
        tmp->data->num_of_params = num_of_params;
        return true;
    }
    else return false;
}


//#################################################################################//
/** SYMTABLE do, ktorej sa budu ukladat jednotlive symtables jednotlivych funkcii **/
//#################################################################################//
void func_symtable_init(func_Sym_table *table)
{
    if (table == NULL) return;
    memset(*table, 0, sizeof(struct func_sT_listitem) * MAX_SYMT_SIZE);
}


Sym_table *func_symtable(func_Sym_table *table, char *function)
{
    if (table == NULL || function == NULL) return NULL;

    func_sTItem *Item = (*table)[hash_fun(function)];
    while (Item != NULL)
    {
        if (strcmp(Item->function, function) == 0) return Item->symtable;
        Item = Item->ptr_next;
    }

    // Vytvorime novy prvok a alokujeme mu pamat
    func_sTItem *New_Item = (func_sTItem *) malloc(sizeof(func_sTItem));
    if (New_Item == NULL) return NULL;

    New_Item->function = (char *) malloc((strlen(function) + 1) * sizeof(char));
    if (New_Item->function == NULL)
    {
        free(New_Item);
        return NULL;
    }

    New_Item->symtable = (Sym_table *) malloc(sizeof(Sym_table) * MAX_SYMT_SIZE);
    if (New_Item->symtable == NULL)
    {
        free(New_Item->function);
        free(New_Item);
        return NULL;
    }

    // Naplnime novy prvok a inicializujeme jeho symtable
    strcpy(New_Item->function, function);
    symtable_init(New_Item->symtable);
    New_Item->ptr_next = NULL;

    if (Item == NULL) (*table)[hash_fun(function)] = New_Item;
    else              Item->ptr_next = New_Item;

    return New_Item->symtable;
}


void func_symtable_free(func_Sym_table *table)
{
    if (table == NULL) return;

    for (int i = 0; i < MAX_SYMT_SIZE; i++)
    {
        func_sTItem *Item = (*table)[i];
        while (Item != NULL)
        {
            func_sTItem *Old_Item = Item;
            free(Old_Item->function);
            symtable_free(Old_Item->symtable);
            free(Old_Item->symtable);
            free(Old_Item);
            Item = Item->ptr_next;
        }
        (*table)[i] = 0;
    }
}
//#################################################################################//
//#################################################################################//


#ifdef TESTING
void vypis(sTItem *New)
{
    printf("KEY: %s\n", New->key);
    printf("DATA:\n\tTYPE\t%u\n", New->data->type);
    printf("\tID\t%s\n", New->data->ID);
    printf("\tDEFINED\t%s\n", New->data->defined ? "TRUE" : "FALSE");
    printf("-------------------------\n");

}


int main()
{
    Sym_table *table = (Sym_table *) malloc(sizeof(Sym_table) * MAX_SYMT_SIZE);
    symtable_init(table);

    int New = symtable_add(table, "Skuska");

    if (New == ERR_INTERNAL)
    {
      printf("CHYBA\n");
      return -1;
    }

    sTItem *vyp_prve = symtable_search(table,"Skuska");
    printf("\n\n");
    vypis(vyp_prve);

    printf("\nIDEM PRIDAT TEN ISTY ITEM:\n");
    int New2 = symtable_add(table, "Skuska");
    printf("     %s NA MNA\n", New2 == EXISTUJE ? "PRISLI" : "NEPRISLI");
    printf("\n-------------------------\n");

    //-----------------------------------------------------------------------------------------------------------------//
    #ifdef CELKOVY_VYPIS
    char *kluce[10] = {"PRVY", "DRUHY", "TRETI", "STVRTY", "PIATY", "SIESTY", "SIEDMI", "OSMI", "DEVIATY", "DESIATY"};
    int pole[10];
    sTItem *vyp_pole[10];
    for (int i = 0; i < 10; i++)
    {

      pole[i] = symtable_add(table, kluce[i]);
      if (pole[i] == ERR_INTERNAL)
      {
          printf("CHYBA %d\n",i);
          return -1 * i;
      }

      vyp_pole[i] = symtable_search(table, kluce[i]);
      vypis(vyp_pole[i]);
    }

    #endif
    //-----------------------------------------------------------------------------------------------------------------//

    int New_podobny = symtable_add(table, "aksukS");
    if (New_podobny == ERR_INTERNAL)
    {
      printf("CHYBA\n");
      return -1;
    }

    sTItem *vyp_n_p = symtable_search(table, "aksukS");
    vypis(vyp_n_p);

    symtable_fill(table, "aksukS", TOKEN_KWORD, true);

    vypis(vyp_prve->ptr_next);
    vypis(vyp_prve);

    bool odmazal = symtable_remove(table, vyp_prve->key);
    printf("VYMAZAL: %s\n", odmazal ? "TRUE" : "FALSE");

    symtable_free(table);
    free(table);

    return 0;
}
#endif

/*** Koniec súboru symtable.c ***/
