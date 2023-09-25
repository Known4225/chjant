#ifndef LIST_H
#define LIST_H
/* it blows my mind that this didn't have an include guard until now */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
21.04.23:
unitype list, supports a variety of types

access items of a list:
list -> data[0]

access length of list:
list -> length

list functions:

create list:
list_t list = list_init();

access items of a list (as a void pointer):
void *item = list_item(list, [index]);

append to list:
list_append(list, (unitype) [data], 'i');

insert to list
list_insert(list, [index], (unitype) [data], 'i');

pop from list:
list_pop(list);

delete index from list:
list_delete(list, [index]);

delete range of indices from list (deletes from indexMin to indexMax - 1):
list_delete_range(list, [indexMin], [indexMax]);

find element in the list (must specify type):
list_find(list, [element], 'i');

count how many elements are in the list (must specify type):
list_count(list, [elements], 'i');

find and delete element in the list (must specify type):
list_remove(list, [element], 'i');

copy one list to another
list_copy(source, dest);

delete all the elements of a list
list_clear(list);

print the list:
list_print(list);

free the list (when done using):
list_free(list);
*/

struct list_f; // so basically im a good programmer
typedef struct list_f list_t;

typedef union { // supported types
    int i;
    float f;
    double d;
    char c;
    char* s;
    void* p;
    list_t* r;
    long long l;
    short h;
    bool b;
} unitype;

struct list_f {
    unsigned int length;
    unsigned int realLength;
    char *type;
    unitype *data;
};

list_t* list_init(); // initialise a list

void* list_item(list_t *list, int index); // accesses an item of the list as a void pointer

void list_append(list_t *list, unitype data, char type); // append to list, must specify type

void list_clear(list_t *list);

unitype list_pop(list_t *list); // pops the last item of the list off and returns it

unitype list_delete(list_t *list, int index); // deletes the item at list[index] of the list and returns it

void list_delete_range(list_t* list, int indexMin, int indexMax); // deletes many items from the list spanning from [indexMin] to [indexMax - 1]

int unitype_check_equal (unitype item1, unitype item2, char typeItem1, char typeItem2); // checks if two unitype items are equal

int list_find(list_t *list, unitype item, char type); // returns the index of the first instance of the item in the list, returns -1 if not found (python)

int list_index(list_t *list, unitype item, char type); // duplicate of list_find

int list_count(list_t *list, unitype item, char type); // counts how many instances of an item is found in the list

int list_remove(list_t *list, unitype item, char type); // deletes the first instance of the item from the list, returns the index the item was at, returns -1 and doesn't modify the list if not found (python but without ValueError)

void unitype_print(unitype item, char type); // prints a unitype item

void list_copy(list_t *src, list_t *dest); // copies one list to another (duplicates strings or pointers)

void list_print(list_t *list); // prints the list (like python would)

void list_print_emb(list_t *list); // prints the list but without closing \n

void list_print_type(list_t *list); // prints the types of the list

void list_free_lite(list_t *list); // frees the list's data but not the list itself

void list_free(list_t *list); // frees the data used by the list

#endif