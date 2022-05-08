/* DrewDotCo, 08/05/2022
 *
 * Basic linked list implementation
 */

#include <malloc.h>

#include "list.h"

#pragma clang diagnostic ignored "-Wpadded"

typedef struct list {
    Item *start;
    Item *end;
    int length;
} List;

typedef struct item {
    Item *previous;
    Item *next;
    void *value;
} Item;

List* new_list(void);
int l_push(List *list, void *item);
void *l_pop(List *list);
void *l_index_rec(Item *current, int n);
void *l_index(List *list, int n);

// Make a new list
// Ignoring error handling bc I don't feel like implementing it
List* new_list()
{
    List *list = malloc(sizeof(list));

    list->length = 0;
    return list;
}

// Add an item to the end of a list
// Returns 1 if error, 0 otherwise
int l_push(List *list, void *item)
{
    // Allocate memory for new item
    Item *new = malloc(sizeof(item));
    if(!new) return 1;

    // List operations: Set new list end, send next value of old end,
    //   set previous of new, increment list length
    new->previous = list->end;
    list->end->next = new;
    list->end = new;
    new->value = item;
    list->length += 1;

    return 0;
}

// Remove an item from the end of a list and return
void *l_pop(List *list)
{
    // list operations blah blah
    Item *item = list->end;
    list->end = item->previous;
    item->previous->next = NULL;
    list->length -= 1;

    void *value = item->value;
    free(item);
    return value;
}

// Recursive list index
// Please avoid using, should be accessed via l_index
void *l_index_rec(Item *current, int n)
{
    if(n == 0) return current->value;

    if(current->next) {
        return l_index_rec(current->next, n - 1);
    } else {
        return NULL;
    }
}

// Get nth value of a list
// Returns null if does not exist
// Could be optimised by searching from back if more than half,
//   but outside the scope of this project
void *l_index(List *list, int n)
{
    if(n >= list->length) return NULL;
    if(n == list->length - 1) return list->end->value; // shortcut to last item

    return l_index_rec(list->start, n);
}

// Free a list
// Will also destroy any items
// TODO
// void free_list(List *list)
