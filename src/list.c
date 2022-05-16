/* DrewDotCo, 08/05/2022
 *
 * Basic linked list implementation
 */

/* Notes
 *
 * This isn't particularly well optimised, nor is it an expansive implementation of a double linked list
 * It could be improved further but is outside the scope of this project - I've only implemented what I'm
 *   likely to need during use
 *
 * Recursion is used for iterative list operations because it's tidier than mucking around with pointer
 *   reassignment in a loop and also I like it
 */

#include <malloc.h>

#include "list.h"
#include "dbg.h"

// Make a new list
// Ignoring error handling bc I don't feel like implementing it
List* new_list()
{
    List *list = malloc(sizeof(List));

    list->length = 0;
    list->start = NULL;
    list->end = NULL;
    return list;
}

// Add an item to the end of a list
// Returns 1 if error, 0 otherwise
int l_push(List *list, void *item)
{
    // Allocate memory for new item
    Item *new = malloc(sizeof(Item));
    if(!new) return 1;

    // List operations: Set new list end, send next value of old end,
    //   set previous of new, increment list length
    new->previous = list->end;
    if(list->end) list->end->next = new;
    if(!list->start) list->start = new;
    list->end = new;
    new->value = item;
    new->next = NULL;
    list->length += 1;

    return 0;
}

// Remove an item from the end of a list and return
void *l_pop(List *list)
{
    // list operations blah blah
    Item *item = list->end;
    list->end = item->previous;
    list->length -= 1;

    if(item->previous) {
        // List is not empty
        item->previous->next = NULL;
    } else {
        // List is now empty
        list->start = NULL;
    }

    void *value = item->value;
    free(item);
    return value;
}

// Recursive list index
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
void l_free(List *list)
{
    if(list->start) l_free_rec(list->start);

    free(list);
}

// Recursively free items in a list
void l_free_rec(Item *item)
{
    if(item->next) l_free_rec(item->next);

    free(item);
}
